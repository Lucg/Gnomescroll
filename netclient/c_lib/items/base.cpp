#include "base.hpp"

VoxBody base_vox_dat;

void Base_vox::init_parts(VoxBody* vox_dat) {
    printf("Base vox\n");
    // create each vox part from vox_dat conf
    int i;
    int x,y,z;
    VoxPart *vp;
    Voxel_volume* vv;
    float size = vox_dat->vox_size;
    for (i=0; i<this->n_parts; i++) {
        vp = vox_dat->vox_part[i];
        x = vp->dimension.x;
        y = vp->dimension.y;
        z = vp->dimension.z;

        vv = &(this->vv[i]);

        vv->init(x,y,z,size);
        vv->set_unit_axis();
        vv->set_hitscan_properties(this->a->id, this->a->type, i);

        #ifdef DC_CLIENT
        unsigned char r,g,b,a;
        int j;
        int ix,iy,iz;
        for (j=0; j<vp->colors.n; j++) {
            ix = vp->colors.index[j][0];
            iy = vp->colors.index[j][1];
            iz = vp->colors.index[j][2];
            r = vp->colors.rgba[j][0];
            g = vp->colors.rgba[j][1];
            b = vp->colors.rgba[j][2];
            a = vp->colors.rgba[j][3];

            if (vp->colors.team
            && r == vp->colors.team_r
            && g == vp->colors.team_g
            && b == vp->colors.team_b)
            {
                ClientState::get_team_color(this->a->team, &r, &g, &b);
            }

            vv->set_color(ix, iy, iz, r,g,b,a);
        }

        ClientState::voxel_render_list.register_voxel_volume(vv);
        #endif
        STATE::voxel_hitscan_list.register_voxel_volume(vv);
    }
    #ifdef DC_CLIENT
    ClientState::voxel_render_list.update_vertex_buffer_object();
    #endif
}

void Base::set_position(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->vox->update(&base_vox_dat);
}

//void Base::set_color(unsigned char r, unsigned char g, unsigned char b) {
    //this->r = r;
    //this->g = g;
    //this->b = b;
//}

Base::Base()
:
theta(0), phi(0),
team(0),
type(OBJ_TYPE_BASE)
{
    this->vox = new Base_vox(this, &base_vox_dat);
    #ifdef DC_CLIENT
    this->vox->set_draw(true);
    #endif
}

Base::~Base()
{
    if (this->vox != NULL) free(this->vox);
}
