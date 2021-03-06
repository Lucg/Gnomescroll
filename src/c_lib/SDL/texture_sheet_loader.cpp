/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "texture_sheet_loader.hpp"

#if DC_SERVER
# error Do not include this file in the server
#endif

#include <common/macros.hpp>
#include <SDL/texture_loader.hpp>
#include <t_map/config/textures.hpp>
#include <t_map/glsl/shader.hpp>
#include <animations/sprite_voxelizer.hpp>

namespace TextureSheetLoader
{

TextureSheetLoader::TextureSheetLoader(size_t tile_size) :
    tile_size(tile_size), tiles_wide(16), tiles_high(16),
    surface_num(0), tile_num(0), format(GL_BGRA),
    texture(0), greyscale_texture(0), mag_filter(GL_NEAREST)
{
    this->tile_meta = (struct TileMeta*)calloc(this->tiles_wide*this->tiles_high, sizeof(struct TileMeta));
    this->surface_meta = (struct SurfaceMeta*)calloc(MAX_SURFACES, sizeof(struct SurfaceMeta));

    this->width = this->tiles_wide*this->tile_size;
    this->height = this->tiles_high*this->tile_size;

    this->surface = create_surface_from_nothing(this->width, this->height);
    this->greyscale_surface = create_surface_from_nothing(this->width, this->height);
    this->texture_stack = (Uint32*)calloc(4*this->width*this->height, sizeof(char));

    this->pixels = (Color*)calloc(this->width*this->height, sizeof(Color));

    this->format = get_texture_format(surface);
}

TextureSheetLoader::~TextureSheetLoader()
{
    if (this->surface_meta != NULL)
        for (size_t i=0; i<MAX_SURFACES; i++)
            if (this->surface_meta[i].surface != NULL)
                SDL_FreeSurface(this->surface_meta[i].surface);
    free(this->surface_meta);
    free(this->tile_meta);
    free(this->texture_stack);
    free(this->pixels);
    if (this->surface != NULL) SDL_FreeSurface(this->surface);
    if (this->greyscale_surface != NULL) SDL_FreeSurface(this->greyscale_surface);
    if (this->texture != 0) glDeleteTextures(1, &this->texture);
    if (this->greyscale_texture != 0) glDeleteTextures(1, &this->greyscale_texture);
}

SpriteSheet TextureSheetLoader::load_texture(const char* filename)
{
    return this->load_texture(filename, this->tile_size);
}

SpriteSheet TextureSheetLoader::load_texture(const char* filename, size_t tile_size)
{
    IF_ASSERT(!tile_size || this->tile_size % tile_size) return NULL_SPRITE_SHEET;

    for (size_t i=0; i<this->surface_num; i++)
        if (strcmp(this->surface_meta[i].filename, filename) == 0)
            return SpriteSheet(i);

    SDL_Surface* s = create_surface_from_file(filename);
    IF_ASSERT(s == NULL) return NULL_SPRITE_SHEET;

    SpriteSheet index = this->load_texture_from_surface(s);

    if (index < 0) SDL_FreeSurface(s);
    GS_ASSERT(this->surface_meta[index].filename[0] == '\0');

    this->surface_meta[index].tile_size = tile_size;
    strncpy(this->surface_meta[index].filename, filename, GS_FN_MAX+1);
    this->surface_meta[index].filename[GS_FN_MAX] = '\0';

    return index;
}

SpriteSheet TextureSheetLoader::load_texture_from_surface(struct SDL_Surface* surface, SpriteSheet sheet_id)
{
    IF_ASSERT(surface == NULL) return NULL_SPRITE_SHEET;
    IF_ASSERT(sheet_id == NULL_SPRITE_SHEET) return NULL_SPRITE_SHEET;
    IF_ASSERT(sheet_id >= (SpriteSheet)MAX_SURFACES) return NULL_SPRITE_SHEET;

    this->surface_meta[sheet_id].surface = surface;
    return sheet_id;
}

SpriteSheet TextureSheetLoader::load_texture_from_surface(struct SDL_Surface* surface)
{
    SpriteSheet index = this->load_texture_from_surface(surface, (SpriteSheet)this->surface_num);
    this->surface_num++;
    return index;
}

bool TextureSheetLoader::blit_meta(size_t meta_index)
{
    IF_ASSERT(meta_index >= this->tile_num) return false;

    struct TileMeta tile_meta = this->tile_meta[meta_index];

    // sanity checks
    GS_ASSERT(tile_meta.sheet_id != NULL_SPRITE_SHEET);
    if (tile_meta.sheet_id == NULL_SPRITE_SHEET) return NULL_SPRITE;
    GS_ASSERT(tile_meta.sheet_id < (SpriteSheet)surface_num);
    if (tile_meta.sheet_id >= (SpriteSheet)surface_num) return NULL_SPRITE;

    // get surface
    SDL_Surface* s = this->surface_meta[tile_meta.sheet_id].surface;
    IF_ASSERT(s == NULL) return false;

    // compute pixel scaling
    size_t scale = this->tile_size / this->surface_meta[tile_meta.sheet_id].tile_size;
    //if (scale != 1) printf("SCALING SHEET: %s by %u\n", this->surface_meta[tile_meta.sheet_id].filename, (unsigned)scale);

    // check that tiles are in bounds
    IF_ASSERT(tile_meta.xpos*tile_size >= (size_t)s->w || tile_meta.ypos*tile_size >= (size_t)s->h)
        return false;

    // lock surfaces
    int s_lock = SDL_MUSTLOCK(s);
    int c_lock = SDL_MUSTLOCK(this->surface);

    if (s_lock) SDL_LockSurface(s);
    if (c_lock) SDL_LockSurface(this->surface);

    // alias pixel buffers
    Uint32* stack_pixels = (Uint32*)this->texture_stack;
    Uint32* sheet_pixels = (Uint32*)this->surface->pixels;

    // sprite coordinate index into destination pixel buffers
    size_t index = (meta_index % this->tiles_wide) + this->tiles_wide * (meta_index / this->tiles_wide);
    size_t dest_x = (index % this->tiles_wide) * this->tile_size;
    size_t dest_y = (index / this->tiles_wide) * this->tile_size;

    // copy sprite icon pixels from source pixel buffer to
    // sheet and stack buffers and unpacked pixel array
    size_t surface_tile_size = this->surface_meta[tile_meta.sheet_id].tile_size;
    for (size_t i=0; i<surface_tile_size; i++)
    for (size_t j=0; j<surface_tile_size; j++)
    {
        size_t pix_index = s->w * (surface_tile_size * tile_meta.ypos + j) + (surface_tile_size * tile_meta.xpos + i);

        // convert source pixel to final format
        Uint32 pix = ((Uint32*)s->pixels)[pix_index];
        unsigned char r,g,b,a;
        SDL_GetRGBA(pix, s->format, &r, &g, &b, &a);
        pix = SDL_MapRGBA(this->surface->format, r,g,b,a);

        // copy to surface stack & sheet
        for (size_t m=0; m<scale; m++)
        for (size_t n=0; n<scale; n++)
        {
            size_t stack_index = this->tile_size * this->tile_size * index;
            stack_index += ((j * scale + n) * this->tile_size + (i * scale  + m));
            stack_pixels[stack_index] = pix;

            size_t sheet_index = this->width * (dest_y + (j * scale + n)) + (dest_x + (i * scale + m));
            sheet_pixels[sheet_index] = pix;

            // copy to raw pixel buffer
            size_t pixel_index = meta_index * this->tile_size * this->tile_size;
            pixel_index += (this->tile_size - (j + n) - 1) * this->tile_size + (this->tile_size - (i + m) - 1);
            this->pixels[pixel_index] = Color(r,g,b,a);
        }
    }

    // unlock
    if (s_lock) SDL_UnlockSurface(s);
    if (c_lock) SDL_UnlockSurface(this->surface);

    return true;
}

//blit to sheet or return texture id
int TextureSheetLoader::blit(SpriteSheet sheet_id, size_t source_x, size_t source_y)
{
    IF_ASSERT(sheet_id == NULL_SPRITE_SHEET) return NULL_SPRITE;
    IF_ASSERT(this->tile_num > 0xFF) return NULL_SPRITE;
    IF_ASSERT(source_x < 1) return NULL_SPRITE;
    IF_ASSERT(source_y < 1) return NULL_SPRITE;

    // decrement x,y because arguments should be 1-indexed
    source_x--;
    source_y--;

    //check to see if already loaded
    for (size_t i=0; i<this->tile_num; i++)
        if (tile_meta[i].sheet_id == sheet_id &&
            tile_meta[i].xpos == source_x &&
            tile_meta[i].ypos == source_y)
        {
            return i;
        }

    // record metadata
    size_t index = this->tile_num;
    tile_meta[index].sheet_id = sheet_id;
    tile_meta[index].xpos = source_x;
    tile_meta[index].ypos = source_y;
    this->tile_num++;

    if (this->blit_meta(index))
        return index;
    else
        return NULL_SPRITE;
}

void TextureSheetLoader::generate_greyscale()
{
    static float gamma_correction[256];

    for (int i=0; i<256; i++)
    {
        float intensity = ((float) i) / 255.0f;
        gamma_correction[i] = powf(intensity, 1.0f/2.2f);
    }

    int c_lock = SDL_MUSTLOCK(surface);
    int s_lock = SDL_MUSTLOCK(greyscale_surface);

    if (c_lock) SDL_LockSurface(surface);
    if (s_lock) SDL_LockSurface(greyscale_surface);

    Uint8 r,g,b,a;

    for (size_t x=0; x<this->width; x++)
    for (size_t y=0; y<this->height; y++)
    {
        size_t index = y*this->width + x;
        GS_ASSERT(index < (size_t)greyscale_surface->w*greyscale_surface->h);
        Uint32 pix = ((Uint32*)surface->pixels)[index];
        SDL_GetRGBA(pix, surface->format, &r, &g, &b, &a);

        float avg = (gamma_correction[r] + gamma_correction[g] + gamma_correction[b]) / 3.0f;
        avg = powf(avg, 2.2f);
        GS_ASSERT(avg >= 0.0f && avg <= 1.0f);

        unsigned char g = (unsigned char)(255.0f*avg);

        ((Uint32*)greyscale_surface->pixels)[index] = SDL_MapRGBA(greyscale_surface->format, g,g,g,a);
    }

    if (c_lock) SDL_UnlockSurface(surface);
    if (s_lock) SDL_UnlockSurface(greyscale_surface);

    create_texture_from_surface(item_texture_sheet_loader->greyscale_surface,
        &this->greyscale_texture, this->mag_filter);
}

void TextureSheetLoader::generate_texture()
{
    create_texture_from_surface(this->surface, &this->texture, this->mag_filter);
}

void TextureSheetLoader::reload()
{
    // remove existing loaded surfaces
    for (size_t i=0; i<this->surface_num; i++)
    {
        IF_ASSERT(this->surface_meta[i].surface == NULL) continue;
        SDL_FreeSurface(this->surface_meta[i].surface);
        this->surface_meta[i].surface = NULL;
    }

    // regenerate surfaces from filenames
    for (size_t i=0; i<this->surface_num; i++)
    {
        printf("Reloading texture file: %s\n", this->surface_meta[i].filename);
        SDL_Surface* s = create_surface_from_file(this->surface_meta[i].filename);
        IF_ASSERT(s == NULL)
        {
            this->surface_num = i;
            break;
        }

        SpriteSheet index = this->load_texture_from_surface(s, (SpriteSheet)i);
        IF_ASSERT(index != (SpriteSheet)i)
        {
            this->surface_num = i;
            SDL_FreeSurface(s);
            break;
        }
    }

    // wipe out main surfaces
    MALLOX(SDL_Surface*, surfaces, 2);
    surfaces[0] = this->surface;
    surfaces[1] = this->greyscale_surface;
    for (int k=0; k<2; k++)
    {
        SDL_Surface* surface = surfaces[k];
        int lock = SDL_MUSTLOCK(surface);
        if (lock) SDL_LockSurface(surface);
        for (int i=0; i<this->surface->w*this->surface->h; i++)
            ((Uint32*)surface->pixels)[i] = SDL_MapRGBA(surface->format, 0,0,0,0);
        if (lock) SDL_UnlockSurface(surface);
    }

    // re-blit from tile tile_meta
    for (size_t i=0; i<this->tile_num; i++) this->blit_meta(i);

    // destroy existing textures
    if (this->texture != 0) glDeleteTextures(1, &this->texture);
    if (this->greyscale_texture != 0) glDeleteTextures(1, &this->greyscale_texture);

    // regenerate textures
    this->generate_texture();
    this->generate_greyscale();
}

const Color* TextureSheetLoader::get_sprite_pixels(int sprite_id) const
{
    IF_ASSERT(sprite_id < 0 || sprite_id >= (int)this->tile_num) return NULL;
    size_t index = sprite_id * this->tile_size * this->tile_size;
    return &this->pixels[index];
}

void TextureSheetLoader::save_texture(const char* name)
{
    IF_ASSERT(name == NULL) return;
    const char fmt[] = SCREENSHOT_PATH "%s.png";
    const size_t len = sizeof(fmt) + strlen(name);
    char* filename = (char*)malloc(len);
    size_t could = snprintf(filename, len, fmt, name);
    GS_ASSERT(could < len);
    filename[len-1] = '\0';
    save_surface_to_png(this->surface, filename);
    free(filename);
}

class TextureSheetLoader* cube_texture_sheet_loader = NULL;
class TextureSheetLoader* item_texture_sheet_loader = NULL;
class TextureSheetLoader* badge_texture_sheet_loader = NULL;

void init()
{
    GS_ASSERT(cube_texture_sheet_loader == NULL);
    GS_ASSERT(item_texture_sheet_loader == NULL);
    GS_ASSERT(badge_texture_sheet_loader == NULL);
    cube_texture_sheet_loader = new TextureSheetLoader(32);        //pixel size for cube textures
    item_texture_sheet_loader = new TextureSheetLoader(16);
    item_texture_sheet_loader->mag_filter = GL_NEAREST;
    badge_texture_sheet_loader = new TextureSheetLoader(16);
}

void init_item_texture()
{
    IF_ASSERT(item_texture_sheet_loader == NULL) return;
    item_texture_sheet_loader->generate_texture();
}

void init_greyscale()
{
    item_texture_sheet_loader->generate_greyscale();
}

void teardown()
{
    delete cube_texture_sheet_loader;
    delete item_texture_sheet_loader;
    delete badge_texture_sheet_loader;
}

SpriteSheet load_cube_texture_sheet(const char* filename)
{
    return cube_texture_sheet_loader->load_texture(filename);
}

int blit_cube_texture(SpriteSheet sheet_id, int source_x, int source_y)
{
    return cube_texture_sheet_loader->blit(sheet_id, source_x, source_y);
}

void save_cube_texture()
{
    save_surface_to_png(cube_texture_sheet_loader->surface, SCREENSHOT_PATH "cubes.png");
}

SpriteSheet cube_texture_alias(const char* filename)
{
    return cube_texture_sheet_loader->load_texture(filename);
}

SpriteSheet cube_texture_alias(const char* filename, size_t tile_size)
{
    return cube_texture_sheet_loader->load_texture(filename, tile_size);
}

//Item API
SpriteSheet item_texture_alias(const char* filename)
{
    return item_texture_sheet_loader->load_texture(filename);
}

SpriteSheet load_item_texture(struct SDL_Surface* surface)
{
    return item_texture_sheet_loader->load_texture_from_surface(surface);
}

int blit_item_texture(SpriteSheet sheet_id, int source_x, int source_y)
{
    return item_texture_sheet_loader->blit(sheet_id, source_x, source_y);
}

void save_item_texture()
{
    IF_ASSERT(item_texture_sheet_loader->surface == NULL) return;
    IF_ASSERT(item_texture_sheet_loader->greyscale_surface == NULL) return;

    save_surface_to_png(item_texture_sheet_loader->surface,
        SCREENSHOT_PATH "items.png");

    save_surface_to_png(item_texture_sheet_loader->greyscale_surface,
        SCREENSHOT_PATH "greyscale_items.png");
}

// badge api
void save_badge_texture()
{
    IF_ASSERT(badge_texture_sheet_loader->surface == NULL) return;

    save_surface_to_png(badge_texture_sheet_loader->surface,
        SCREENSHOT_PATH "badges.png");
}

SpriteSheet badge_texture_alias(const char* filename)
{
    return badge_texture_sheet_loader->load_texture(filename);
}

void init_badge_texture()
{
    badge_texture_sheet_loader->generate_texture();
}

void reload_texture_sheets()
{
    printf("Reloading texture sheets\n");
    cube_texture_sheet_loader->reload();
    t_map::blit_block_item_sheet();
    item_texture_sheet_loader->reload();
    Animations::load_sprite_voxelizer();
    t_map::reload_textures();
    badge_texture_sheet_loader->reload();
}

}   // TextureSheetLoader
