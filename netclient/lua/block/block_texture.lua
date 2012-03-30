

--[[
1> Block Properties
2> Block Hud Information
3< Block Texture Information
]]

bl_ffi = require("ffi")
bl_ffi.cdef[[
void set_cube_side_texture(int id, int side, int tex_id);
void set_cube_hud(int pos, int cube_id, int tex_id);

int LUA_load_cube_texture_sheet(char* filename);
void LUA_blit_cube_texture(int sheet_id, int source_x, int source_y, int dest_index);

void LUA_blit_cube_texture;
void LUA_save_cube_texture();

void load_cube_texture_sprite_sheet(char*, int pos);
void load_hud_texture(char*, int pos);

void LUA_set_block_name(int id, char* name, int length);
]]

prefix = "media/sprites/";


function load_texture_sheet(filename)
  print("load_texture_sheet: ", filename);
  local str = bl_ffi.new("char[128]");
  bl_ffi.copy(str, prefix .. filename ..".png");
  return bl_ffi.C.LUA_load_cube_texture_sheet(str);
end


-- Spritesheet loader
spritesheet_name_to_id = {};
spritesheet_id_to_name = {};

function register_spritesheet(spritesheet)
  if(spritesheet_name_to_id[spritesheet] ) then 
    return  spritesheet_name_to_id[spritesheet]
  end

  local id = load_texture_sheet(spritesheet);
  spritesheet_name_to_id[spritesheet] = id;
  spritesheet_id_to_name[id] = spritesheet;

  print("textured sheet loaded: " .. spritesheet .. " id= " .. id )
  return id
end


-- texture loader

texture_id_counter = 0;
texture_id_table = {};

texture_look_up_table = {}


---register a texture on a spritesheet
function register_texture(spritesheet, xpos, ypos)
  --local sindex = register_spritesheet(spritesheet);
  sindex = spritesheet
  local index = string.format("%s_%i_%i", spritesheet, xpos, ypos)

  if(texture_look_up_table[index]) then
    print("register texture: index found: " .. index .. " id= " .. texture_look_up_table[index])
    return texture_look_up_table[index]
  end

  local id = texture_id_counter;
  print("registered texture: " .. index .. " id=" .. id);

  bl_ffi.C.LUA_blit_cube_texture(sindex, xpos, ypos, id)

  texture_look_up_table[index] = id;
  texture_id_counter = texture_id_counter + 1;
  return id;
end

---not used right now ---
---registers a n individual file
function register_individual_texture(sprite)
  local sindex = register_spritesheet(spritesheet);
  local index = string.format("%s", sprite);

  if(texture_look_up_table[index]) then
    return texture_look_up_table[index]
  end

  local id = texture_id_counter;
  texture_look_up_table[index] = id;
  texture_id_counter = texture_id_counter + 1;
  return id;
end


--- texture stuff

texture_aliases = {}

function texture_alias(spritesheet, xpos, ypos)
  return register_texture(spritesheet, xpos, ypos)
end



function blit_textures()
  for id, tex_sheet in pairs(texture_id_table) do

  end
end