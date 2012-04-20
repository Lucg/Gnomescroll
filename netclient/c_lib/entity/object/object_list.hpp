#pragma once

namespace Objects
{

//forward decl
class Object;

/* Management */

const int MAX_OBJECT_TYPES = 256;
class ObjectList
{
    private:
        // keep a counters for individual object indices
        int* indices;
        int* maximums;
        Object*** objects;
        
        int get_free_id(ObjectType type);

    public:

        void set_object_id(Object* object, int id);

        int count(ObjectType type);
        int max(ObjectType type);
        bool full(ObjectType type);

        Object* get(ObjectType type, int id);
        void destroy(ObjectType type, int id);
        Object* create(ObjectType type);
        Object* create(ObjectType type, int id);
        void set_object_max(ObjectType type, int max);
        void init();

        void tick();    // physics tick
        void update();  // draw state update
        void harvest(); // kill
        
    ~ObjectList();

    ObjectList()
    : indices(NULL), maximums(NULL), objects(NULL)
    {}

};

} // Objects
