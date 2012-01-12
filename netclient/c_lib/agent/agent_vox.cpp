#include "agent_vox.hpp"

VoxBody agent_vox_dat;

Agent_vox::Agent_vox(Agent_state* a)
:
a(a)
{
    this->init_parts();
}

void Agent_vox::init_parts() {
    // create each vox part from agent_vox_dat conf
    int i;
    int x,y,z;
    VoxPart *vp;
    float size = agent_vox_dat.vox_size;
    for (i=0; i<AGENT_PART_NUM; i++) {
        printf("part %d\n", i);
        vp = agent_vox_dat.vox_part[i];
        x = vp->dimension.x;
        y = vp->dimension.y;
        z = vp->dimension.z;
        printf("init part\n");
        this->vv[i].init(x,y,z,size);
        printf("setting axis\n");
        this->vv[i].set_unit_axis();
        printf("registering\n");
        ClientState::voxel_render_list.register_voxel_volume(&(this->vv[i]));
    }
    printf("setup all parts\n");
    ClientState::voxel_render_list.update_vertex_buffer_object();
    printf("updated vertex buff\n");
}

void Agent_vox::set_draw(bool draw) {
    int i;
    for (i=0; i<AGENT_PART_NUM; i++) {
        this->vv[i].draw = draw;
    }
}

void Agent_vox::update() {

    float x,y,z,theta,phi;
    x = this->a->s.x;
    y = this->a->s.y;
    z = this->a->s.z;
    theta = this->a->s.theta;
    phi = this->a->s.phi;
    
    struct Vector forward;
    struct Vector forward_tmp;
    this->forward(&forward, theta);

    Vector up = {0.0f,0.0f,1.0f};
    Vector up_tmp = up;

    VoxPart* vp;
    float ax,ay,az;
    int i;
    for (i=0; i<AGENT_PART_NUM; i++) {
        vp = agent_vox_dat.vox_part[i];
        ax = vp->anchor.x;
        ay = vp->anchor.y;
        az = vp->anchor.z;
        this->vv[i].set_center(x+ax,y+ay,z+az); // add vox config offsets
        
        if (i == AGENT_PART_HEAD) {
            this->vv[i].set_rotated_unit_axis(phi, 0.0f, theta);
        } else {
            // add vox conf adjustments
            forward_tmp = forward;
            forward_tmp.x += vp->rotation.fx;
            forward_tmp.y += vp->rotation.fy;
            forward_tmp.z += vp->rotation.fz;
            normalize_vector(&forward_tmp);

            up_tmp = up;
            up_tmp.x += vp->rotation.nx;
            up_tmp.y += vp->rotation.ny;
            up_tmp.z += vp->rotation.nz;
            normalize_vector(&up_tmp);
            this->vv[i].set_axis(&forward_tmp, &up);
        }
    }
}

void Agent_vox::right(Vector* f, float theta) {
    f->x = cos(theta * PI + PI/2); 
    f->y = sin(theta * PI + PI/2);
    f->z = 0.0f;
    normalize_vector(f);
}

void Agent_vox::forward(Vector* f, float theta) {
    f->x = cos(theta * PI);
    f->y = sin(theta * PI);
    f->z = 0.0f;
    normalize_vector(f);
}

void Agent_vox::look(Vector* f, float theta, float phi) {
    f->x = cos(theta * PI) * cos(phi * PI);
    f->y = sin(theta * PI) * cos(phi * PI);
    f->z = sin(phi);
    normalize_vector(f);
}

Agent_vox::~Agent_vox() {
    int i;
    for (i=0; i<AGENT_PART_NUM; i++) {
        ClientState::voxel_render_list.unregister_voxel_volume(&(this->vv[i]));
    }
}
