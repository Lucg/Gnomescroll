#include "t_map_class.hpp"

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <t_map/_interface.hpp>
#include <t_map/common/constants.hpp>
#include <physics/quadrant.hpp>

namespace t_map
{


    static const MAP_ELEMENT NO_MAP_ELEMENT = {{{0}}};
    
    //moved to constants
    //const int TERRAIN_MAP_HEIGHT_BIT_MASK = ~(TERRAIN_MAP_HEIGHT-1);
    //const int TERRAIN_MAP_WIDTH_BIT_MASK = ~(512-1); //assumes map size of 512

    /*
        Constructors
    */
    MAP_CHUNK::MAP_CHUNK(int _xpos, int _ypos)
    {
        GS_ASSERT( (_xpos % 16) == 0 && (_ypos % 16) == 0 );
        #if DC_CLIENT
            needs_update = false;
        #endif
        xpos = _xpos;
        ypos = _ypos;
        for(int i=0; i<TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH;i++) top_block[i] = 0;
        chunk_index = (ypos / 16)*(MAP_WIDTH/16) + (xpos / 16);
        chunk_item_container.chunk_index = chunk_index;
        memset(e, 0, TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*TERRAIN_MAP_HEIGHT*sizeof(struct MAP_ELEMENT) );
    }


    Terrain_map::Terrain_map(int _xdim, int _ydim)
    {
        xdim = _xdim; 
        ydim = _xdim;
        xchunk_dim = MAP_CHUNK_XDIM; 
        ychunk_dim = MAP_CHUNK_YDIM;

        chunk = new MAP_CHUNK*[xchunk_dim*ychunk_dim];

        #if DC_CLIENT
        for(int i=0; i<xchunk_dim*ychunk_dim; i++) chunk[i] = NULL;
        #endif

        #if DC_SERVER
        for(int i=0; i<xchunk_dim; i++) 
        for(int j=0; j<ychunk_dim; j++)
            chunk[i] = new MAP_CHUNK(16*i, 16*j);
        #endif

        #if DC_CLIENT
        for (int i=0; i<MAP_WIDTH*MAP_HEIGHT; column_heights[i++] = 0);
        this->height_changed = false;
        for (int i=0; i<MAP_CHUNK_YDIM*MAP_CHUNK_XDIM; chunk_heights_status[i++] = CHUNK_HEIGHT_UNSET);
        #endif
    }

    Terrain_map::~Terrain_map()
    {
        for(int i=0; i < xchunk_dim*ychunk_dim; i++)
        {
            if(chunk[i] != NULL) delete chunk[i];
        }
        delete[] chunk;
    }


/*
    Get Methods
*/


    int MAP_CHUNK::get_block(int x, int y, int z)
    {
        return e[ (z<<8)+((y&15)<<4)+(x&15) ].block;
    }

    struct MAP_ELEMENT MAP_CHUNK::get_element(int x, int y, int z)
    {
        return e[ (z<<8)+((y&15)<<4)+(x&15) ];
    }

    struct MAP_ELEMENT Terrain_map::get_element(int x, int y, int z)
    {
    #if T_MAP_GET_OPTIMIZED

        if( (z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0)
            return NO_MAP_ELEMENT;

        x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
        y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

        class MAP_CHUNK* c;

        c = chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
        if(c == NULL) return NO_MAP_ELEMENT;
        return c->e[ (z<<8)+((y&15)<<4)+(x&15) ];
    #else

        if( z >= TERRAIN_MAP_HEIGHT || z < 0 ) return NO_MAP_ELEMENT;

        class MAP_CHUNK* c;
        {
            int xchunk = (x >> 4);
            int ychunk = (y >> 4);
            c = chunk[ MAP_CHUNK_XDIM*ychunk + xchunk ];
            if( c == NULL ) return NO_MAP_ELEMENT;
        }

        int xi = x & 15; //bit mask
        int yi = y & 15; //bit mask

        return c->e[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*z+ TERRAIN_CHUNK_WIDTH*yi + xi];
    #endif
    }

#if DC_CLIENT
    void Terrain_map::set_update(int x, int y)
    {
        //printf("set update: %i %i \n", x,y);

        x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
        y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

        class MAP_CHUNK* c = chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
        if( c != NULL ) c->needs_update = true;
    }
#endif

/*
    Set Methods 
*/


    void MAP_CHUNK::set_element(int x, int y, int z, struct MAP_ELEMENT element)
    {
        e[ (z<<8)+((y&15)<<4)+(x&15) ] = element;
    }


    void Terrain_map::set_element(int x, int y, int z, struct MAP_ELEMENT element)
    {
        //printf("set_element: %i %i %i %i \n", x,y,element.block);

    #if T_MAP_SET_OPTIMIZED
        if( (z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return;

        x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
        y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

        class MAP_CHUNK* c;
        c = chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
        if( c != NULL )
        {
            c = new MAP_CHUNK( x & ~15, y & ~15);
            chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ] = c;
        }
        c->e[ (z << 8)+ ((y & 15) <<4) + (x & 15)] = element;

        #if DC_CLIENT
            c->needs_update = true; 

            if((x & 15) == 0)  set_update(x-1,y);
            if((x & 15) == 15) set_update(x+1,y);
            if((y & 15) == 0)  set_update(x,y-1);
            if((y & 15) == 15) set_update(x,y+1);
        #endif
    #else

        if( z >= TERRAIN_MAP_HEIGHT || z < 0 ) return;

        x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
        y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

        class MAP_CHUNK* c;
        {
            int xchunk = (x >> 4);
            int ychunk = (y >> 4);
    
            c = chunk[ MAP_CHUNK_XDIM*ychunk + xchunk ];
            if( c == NULL )
            {
                c = new MAP_CHUNK( x & ~15, y & ~15);
                chunk[ MAP_CHUNK_XDIM*ychunk + xchunk ] = c;
            }
        }

        int xi = x & 15; //bit mask
        int yi = y & 15; //bit mask

        c->e[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*z+ TERRAIN_CHUNK_WIDTH*yi + xi] = element;

        #if DC_CLIENT
            c->needs_update = true; 

            if((x & 15) == 0)  set_update(x-1,y);
            if((x & 15) == 15) set_update(x+1,y);
            if((y & 15) == 0)  set_update(x,y-1);
            if((y & 15) == 15) set_update(x,y+1);
        #endif

    #endif
    }

    int Terrain_map::get_damage(int x, int y, int z)
    {
    #if T_MAP_SET_OPTIMIZED

        if( (z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0 ) return -2; // an error

        x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
        y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

        class MAP_CHUNK* c;
        c = chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
        if( c != NULL ) return -3;

        struct MAP_ELEMENT* e = &c->e[ (z<<8)+((y&15)<<4)+(x&15) ];

        return e->damage;

    #else
        if( z >= TERRAIN_MAP_HEIGHT || z < 0 ) return -2;

        x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
        y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

        class MAP_CHUNK* c;
        {
            int xchunk = (x >> 4);
            int ychunk = (y >> 4);

            c = chunk[ MAP_CHUNK_XDIM*ychunk + xchunk ];

            if( c == NULL ) return -3;
        }

        int xi = x & 15; //bit mask
        int yi = y & 15; //bit mask
        struct MAP_ELEMENT* e =  &c->e[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*z+ TERRAIN_CHUNK_WIDTH*yi + xi];
        
        return e->damage;
    #endif
    }

    int Terrain_map::apply_damage(int x, int y, int z, int dmg)
    {
        //printf("set: %i %i %i %i \n", x,y,element.block);
    #if T_MAP_SET_OPTIMIZED
        if (dmg <= 0) return -4;

        if( (z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0 ) return -2; // an error

        x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
        y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

        class MAP_CHUNK* c;
        c = chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
        if( c != NULL ) return -3;

        struct MAP_ELEMENT* e = &c->e[ (z<<8)+((y&15)<<4)+(x&15) ];

        if(e->block == 0) return -1;
        if (maxDamage(e->block) == 255) return -5;
        e->damage += dmg;
        if(e->damage >= maxDamage(e->block) ) 
        {
            #if DC_SERVER
            if(isItemContainer(e->block))
                destroy_item_container_block(x,y,z);
            #endif
            // destroy block
            *e = NO_MAP_ELEMENT; 
            
            #if DC_CLIENT
                c->needs_update = true; 

                if((x & 15) == 0)  set_update(x-1,y);
                if((x & 15) == 15) set_update(x+1,y);
                if((y & 15) == 0)  set_update(x,y-1);
                if((y & 15) == 15) set_update(x,y+1);
            #endif

            return 0;
        } 
        else 
        {
            return e->damage;
        }

    #else

        if (dmg <= 0) return -4;
        if( z >= TERRAIN_MAP_HEIGHT || z < 0 ) return -2;

        x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
        y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

        class MAP_CHUNK* c;
        {
            int xchunk = (x >> 4);
            int ychunk = (y >> 4);
            c = chunk[ MAP_CHUNK_XDIM*ychunk + xchunk ];

            if( c == NULL ) return -3;
        }

        int xi = x & 15; //bit mask
        int yi = y & 15; //bit mask

        struct MAP_ELEMENT* e =  &c->e[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*z+ TERRAIN_CHUNK_WIDTH*yi + xi];
        
        if(e->block == 0) return -1;
        if (maxDamage(e->block) == 255) return -5;
        e->damage += dmg;
        if(e->damage >= maxDamage(e->block) ) 
        {
            #if DC_SERVER
            if(isItemContainer(e->block))
                destroy_item_container_block(x,y,z);
            #endif
            // destroy block
            *e = NO_MAP_ELEMENT; 

            #if DC_CLIENT
                c->needs_update = true; 

                if((x & 15) == 0)  set_update(x-1,y);
                if((x & 15) == 15) set_update(x+1,y);
                if((y & 15) == 0)  set_update(x,y-1);
                if((y & 15) == 15) set_update(x,y+1);
            #endif

            return 0;
        } 
        else 
        {
            return e->damage;
        }

    #endif
    }

    int Terrain_map::apply_damage(int x, int y, int z, int dmg, int* block_type)
    {
        //printf("set: %i %i %i %i \n", x,y,element.block);
    #if T_MAP_SET_OPTIMIZED
        if (dmg <= 0) return -4;

        if( (z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0 ) return -2; // an error
        
        x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
        y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

        class MAP_CHUNK* c;
        c = chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
        if( c != NULL ) return -3;

        struct MAP_ELEMENT* e = &c->e[ (z<<8)+((y&15)<<4)+(x&15) ];

        if(e->block == 0) return -1;
        if (maxDamage(e->block) == 255) return -5;
        e->damage += dmg;
        if(e->damage >= maxDamage(e->block) ) 
        {
            #if DC_SERVER
            if(isItemContainer(e->block))
                destroy_item_container_block(x,y,z);
            #endif
            // destroy block
            *e = NO_MAP_ELEMENT; 
            
            #if DC_CLIENT
                c->needs_update = true; 

                if((x & 15) == 0)  set_update(x-1,y);
                if((x & 15) == 15) set_update(x+1,y);
                if((y & 15) == 0)  set_update(x,y-1);
                if((y & 15) == 15) set_update(x,y+1);

            #endif

            return 0;
        } 
        else 
        {
            return e->damage;
        }

    #else

        if (dmg <= 0) return -4;

        if( z >= TERRAIN_MAP_HEIGHT || z < 0 ) return -2;

        class MAP_CHUNK* c;
        {
            int xchunk = (x >> 4);
            int ychunk = (y >> 4);
    
            c = chunk[ MAP_CHUNK_XDIM*ychunk + xchunk ];

            if( c == NULL ) return -3;
        }

        int xi = x & 15; //bit mask
        int yi = y & 15; //bit mask

        struct MAP_ELEMENT* e =  &c->e[TERRAIN_CHUNK_WIDTH*TERRAIN_CHUNK_WIDTH*z+ TERRAIN_CHUNK_WIDTH*yi + xi];
        
        *block_type = e->block;

        if(e->block == 0) return -1;
        if (maxDamage(e->block) == 255) return -5;
        e->damage += dmg;
        if(e->damage >= maxDamage(e->block) ) 
        {
            #if DC_SERVER
            if(isItemContainer(e->block))
                destroy_item_container_block(x,y,z);
            #endif
            // destroy block
            *e = NO_MAP_ELEMENT; 

            #if DC_CLIENT
                c->needs_update = true; 

                if((x & 15) == 0)  set_update(x-1,y);
                if((x & 15) == 15) set_update(x+1,y);
                if((y & 15) == 0)  set_update(x,y-1);
                if((y & 15) == 15) set_update(x,y+1);
            #endif

            return 0;
        } 
        else 
        {
            return e->damage;
        }

    #endif
    }


    #if DC_CLIENT
    void Terrain_map::reset_heights_read()
    {   // call when heights are done being read
        this->height_changed = false;
        // only toggle CHANGED chunks, we dont want to switch UNSET->UNCHANGED
        for (int i=0; i<MAP_CHUNK_YDIM*MAP_CHUNK_XDIM; i++)
			if (chunk_heights_status[i] == CHUNK_HEIGHT_CHANGED)
				chunk_heights_status[i] = CHUNK_HEIGHT_UNCHANGED;
    }
    
    void Terrain_map::chunk_received(int cx, int cy)
    {   // update chunk/column heights based on this chunk
        int highest = -1;
        int h;
        int xoff = cx * TERRAIN_CHUNK_WIDTH;
        int yoff = cy * TERRAIN_CHUNK_WIDTH;
        MAP_CHUNK* m = this->chunk[cx + MAP_CHUNK_XDIM*cy];
        int x,y;

        for (int i=0; i<TERRAIN_CHUNK_WIDTH; i++)
            for (int j=0; j<TERRAIN_CHUNK_WIDTH; j++)
            {
                x = xoff+i;
                y = yoff+j;

                for (h=map_dim.z-1; h>=0; h--)
                    if (isSolid(m->get_block(x,y,h))) break;
                h += 1;
                
                if (h > highest) highest = h;
                this->column_heights[x + y*MAP_WIDTH] = h;
            }
            
        this->chunk_heights_status[cx + cy*MAP_CHUNK_XDIM] = CHUNK_HEIGHT_CHANGED;
        this->height_changed = true;

        const static int MASK = (512/16)-1; //chunk width

        int CX;
        int CY;

        CX = (cx+1) & MASK;
        CY = cy;
        if(chunk[ MAP_CHUNK_XDIM*CY + CX ] != NULL)
            chunk[ MAP_CHUNK_XDIM*CY + CX ]->needs_update = true;

        CX = (cx-1) & MASK;
        CY = cy;
        if(chunk[ MAP_CHUNK_XDIM*CY + CX ] != NULL)
            chunk[ MAP_CHUNK_XDIM*CY + CX ]->needs_update = true;

        CX = cx;
        CY = (cy+1) & MASK;
        if(chunk[ MAP_CHUNK_XDIM*CY + CX ] != NULL)
            chunk[ MAP_CHUNK_XDIM*CY + CX ]->needs_update = true;

        CX = cx;
        CY = (cy-1) & MASK;
        if(chunk[ MAP_CHUNK_XDIM*CY + CX ] != NULL)
            chunk[ MAP_CHUNK_XDIM*CY + CX ]->needs_update = true;

    }
    
    inline unsigned char Terrain_map::get_cached_height(int x, int y)
    {
        x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
        y &= TERRAIN_MAP_WIDTH_BIT_MASK2;
        return this->column_heights[x + y*MAP_WIDTH];
    }

    void Terrain_map::update_heights(int x, int y, int z, int val)
    {
        z += 1; // heights are not 0 indexed;
        int new_h = -1;
        int h = this->column_heights[x + y*MAP_WIDTH];
        if (val != 0)
        {   // setting higher block
            if (z > h)
                new_h = z;
        }
        else
        {
            if (z >= h) // deleting top block
                    new_h = get_highest_solid_block(x,y,z) + 1;
        }

        if (new_h == -1) return; // no change in height

        this->column_heights[x + y*MAP_WIDTH] = new_h;

        int cx = x / TERRAIN_CHUNK_WIDTH;   // truncate
        int cy = y / TERRAIN_CHUNK_WIDTH;
        this->chunk_heights_status[cx + cy*MAP_CHUNK_XDIM] = CHUNK_HEIGHT_CHANGED;
        this->height_changed = true;
    }
    #endif

/*
    Block Methodss
*/
    int Terrain_map::get_block(int x, int y, int z)
    {

        if( (z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0 ) return 0;

        x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
        y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

        class MAP_CHUNK* c = chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
        if(c == NULL) return 0;
        return c->e[ (z<<8)+((y&15)<<4)+(x&15) ].block;
    }

    void Terrain_map::set_block(int x, int y, int z, int value)
    {
        if ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return;

        x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
        y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

        struct MAP_ELEMENT element = {{{value, 0,0,0}}};
        set_element(x,y,z, element);
        #if DC_CLIENT
        update_heights(x,y,z,value);
        #endif
    }

}
