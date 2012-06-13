#pragma once 

/*
 * Objects managed by DynamicObjectList must support the minimum interface:
 *
 * void draw();
 * void client_tick();
 * void server_tick();
 */
 
//#include <common/common.hpp>
//#include <physics/common.hpp>

#define OBJECT_LIST_DEBUG 0

template <class Object_state, int max_n=1024, int HARD_MAX=0>   // 0 means unlimited
class DynamicObjectList
{
    private:
        //#if PRODUCTION
        //const char* name() { return "Object"; }
        //#else
        virtual const char* name() = 0;
        //#endif

    protected:
        int id_c;
        
    public:
        static const int n_max_base = max_n;
        int n_max;
        int num;

        Object_state** a;

        Object_state* get(int id);
        Object_state* create();         //object auto id
        Object_state* create(int id);   //create object with id
        
        Object_state* get_or_create(int id);

        void resize(int new_size);

        bool contains(int id);
        bool full();

        int get_free_id();
        
        void destroy(int _id);

        void where();
        void print_members();

        DynamicObjectList(); //default constructor
        virtual ~DynamicObjectList(); //default deconstructor
};

template <class Object_state, int max_n, int HARD_MAX> 
DynamicObjectList<Object_state, max_n, HARD_MAX>::DynamicObjectList()
:
id_c(0),
n_max(max_n),
num(0)
{
    this->a = (Object_state**)calloc(max_n, sizeof(Object_state*));
    //where();
}

template <class Object_state, int max_n, int HARD_MAX> 
DynamicObjectList<Object_state, max_n, HARD_MAX>::~DynamicObjectList()
{
    if (a != NULL)
    {
        for (int i=0; i<this->n_max; i++)
        {
            if (this->a[i] != NULL)
                delete this->a[i];
        }
        free(this->a);
    }
}

template <class Object_state, int max_n, int HARD_MAX>
void DynamicObjectList<Object_state, max_n, HARD_MAX>::where()
{
    printf("%s_list pointer is %p\n", name(), this);
}

template <class Object_state, int max_n, int HARD_MAX>
Object_state* DynamicObjectList<Object_state, max_n, HARD_MAX>::get(int id)
{
    //where();
    if((id < 0) || (id >= n_max)) {
        //printf("%s id error: id=%i\n", name() ,id);
        //if (id != NO_AGENT) // TODO 
        //    print_trace();
        return NULL;
    } 
    if(a[id] == NULL) {
        //printf("%s get error: object is null, id=%i\n",name(), id);
        //this->print_members();
        return NULL;
    }
    return a[id];
}

template <class Object_state, int max_n, int HARD_MAX>
void DynamicObjectList<Object_state, max_n, HARD_MAX>::print_members() {
    int i;
    printf("%s members:\n", name());
    for (i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        printf("%d\n", i);
    }
}

template <class Object_state, int max_n, int HARD_MAX>
int DynamicObjectList<Object_state, max_n, HARD_MAX>::get_free_id()
{
    int i;
    int id;
    for (i=0; i<n_max; i++)
    {
        id = (i + id_c) % n_max;
        if (a[id] == NULL) break;
    }
    if (i == n_max)
    {
        printf("%s_list Error: no free ids found\n", name());
        return -1;
    }
    return id;
}

template <class Object_state, int max_n, int HARD_MAX>
void DynamicObjectList<Object_state, max_n, HARD_MAX>::resize(int new_size)
{
    //GS_ASSERT(HARD_MAX == 0 || new_size < HARD_MAX);
    if (HARD_MAX && new_size >= HARD_MAX) new_size = HARD_MAX-1;
    //GS_ASSERT(this->n_max < new_size);
    if (this->n_max == new_size) return;
    printf("Resizing %s list from %d to %d\n", this->name(), this->n_max, new_size);
    int old_size = this->n_max;
    this->a = (Object_state**)realloc(this->a, new_size * sizeof(Object_state**));
    GS_ASSERT(this->a != NULL);
    if (this->a == NULL) return;
    // initialize to NULL
    for (int i=old_size; i<new_size; this->a[i++] = NULL);
    this->n_max = new_size;
}

template <class Object_state, int max_n, int HARD_MAX>
Object_state* DynamicObjectList<Object_state, max_n, HARD_MAX>::create()
{
    //where();
    GS_ASSERT(n_max > 0);
    if (n_max <= 0) return NULL;
    int i;
    int id=0;
    for(i=0; i<n_max;i++)
    {
        id = (i+id_c)%n_max;
        if(a[id] == NULL) break;
    }
    if (i==n_max)
    {
        id = n_max; // save next id
        int new_size = this->n_max + n_max_base;
        this->resize(new_size);
    }
    a[id] = new Object_state(id);
    num++;
    id_c = id+1;
    return a[id];
}

template <class Object_state, int max_n, int HARD_MAX>
Object_state* DynamicObjectList<Object_state, max_n, HARD_MAX>::create(int id)
{
    //where();
    if (id >= this->n_max)
    {   // need to resize
        int new_size = ((id/this->n_max_base) + 1) * this->n_max_base;
        this->resize(new_size);
    }
    if(a[id] == NULL) {
        a[id] = new Object_state(id);
        num++;
        return a[id];
    } else {
        printf("%s_list: Cannot Create object from id; id is in use: %i\n", name(), id);
        return NULL;
    }
}

template <class Object_state, int max_n, int HARD_MAX>
Object_state* DynamicObjectList<Object_state, max_n, HARD_MAX>::get_or_create(int id)
{
    //where();
    Object_state* obj = NULL;
    if (id < this->n_max) obj = a[id];
    if (obj == NULL) {
        obj = create(id);
    }
    return obj;
}

template <class Object_state, int max_n, int HARD_MAX>
bool DynamicObjectList<Object_state, max_n, HARD_MAX>::contains(int id) {
    //where();
    Object_state* obj = NULL;
    if (id < this->n_max) obj = a[id];
    if (obj == NULL) {
        return false;
    }
    return true;
}

template <class Object_state, int max_n, int HARD_MAX>
void DynamicObjectList<Object_state, max_n, HARD_MAX>::destroy(int id)
{
    //where();
    if(id >= this->n_max || a[id]==NULL)
    {
        #if DC_SERVER || (!PRODUCTION || PRODUCTION_DEV)
        printf("%s_list: Cannot delete object %d: object is null\n", name(), id);
        #endif
        return;
    }
    delete a[id];
    a[id] = NULL;
    num--;
    //printf("%s_list: Deleted object %i\n",name(), id);
}
 
template <class Object_state, int max_n, int HARD_MAX>
bool DynamicObjectList<Object_state, max_n, HARD_MAX>::full()
{
    GS_ASSERT(this->num <= max_n);
    return (this->num >= max_n);
}
