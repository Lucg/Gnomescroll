cdef extern from "./animations/animations.hpp" namespace "Animations":
    void block_crumble_animation(float x, float y, float z, int n)

def block_crumble(pos, int n):
    cdef float x,y,z
    x,y,z = pos
    block_crumble_animation(x,y,z, n)
