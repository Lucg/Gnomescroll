#pragma once

// branch prediction
#ifdef __MSVC__
# define likely  (x) !!(x)
# define unlikely(x) !!(x)
#else
# define likely  (x) __builtin_expect(!!(x), 1)
# define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#define GS_MAX(a,b) ((a) > (b)) ? (a) : (b);
#define GS_MIN(a,b) ((a) < (b)) ? (a) : (b);

#ifdef __MSVC__
# define fmin(x,y) ((x) < (y)) ? (x) : (y)
# define fmax(x,y) ((x) > (y)) ? (x) : (y)
#endif

#define ASSERT_VALID_ITEM_TYPE(item_type) GS_ASSERT((item_type) >= 0 && (item_type) < MAX_ITEM_TYPES)
#define IF_INVALID_ITEM_TYPE(item_type) if (unlikely((item_type) < 0 || (item_type) >= MAX_ITEM_TYPES))

#define ASSERT_VALID_MECH_TYPE(mech_type) GS_ASSERT((mech_type) >= 0 && (mech_type) < MAX_MECHS && (mech_type) != NULL_MECH_TYPE)
#define IF_INVALID_MECH_TYPE(mech_type) if (unlikely((mech_type) < 0 || (mech_type) >= MAX_MECHS || (mech_type) == NULL_MECH_TYPE))

#define ASSERT_VALID_CONTAINER_TYPE(container_type) GS_ASSERT((container_type) >= 0 && (container_type) < MAX_CONTAINER_TYPES)
#define IF_INVALID_CONTAINER_TYPE(container_type) if (unlikely((container_type) < 0 || (container_type) >= MAX_CONTAINER_TYPES))

#define ASSERT_VALID_CONTAINER_ID(container_id) GS_ASSERT((container_id) >= 0 && (container_id) < MAX_CONTAINERS)
#define IS_VALID_CONTAINER_ID(container_id) ((container_id) >= 0 && (container_id) < MAX_CONTAINERS) 
#define IF_INVALID_CONTAINER_ID(container_id) if (unlikely(!IS_VALID_CONTAINER_ID((container_id))))

#define ASSERT_VALID_SPAWNER_ID(spawner_id) GS_ASSERT((spawner_id) == BASE_SPAWN_ID || ((spawner_id) >= 0 && (spawner_id) < MAX_SPAWNERS));
#define IF_INVALID_SPAWNER_ID(spawner_id) if (unlikely((spawner_id) != BASE_SPAWN_ID && ((spawner_id) < 0 || (spawner_id) >= MAX_SPAWNERS)))

#define ASSERT_VALID_CUBE_ID(cube_id) GS_ASSERT((cube_id) >= 0 && (cube_id) < MAX_CUBES);
#define IF_INVALID_CUBE_ID(cube_id) if (unlikely((cube_id) < 0 || (cube_id) >= MAX_CUBES))

#define ASSERT_VALID_OBJECT_TYPE(object_type) GS_ASSERT((object_type) >= 0 && (object_type) < MAX_OBJECT_TYPES);
#define IF_INVALID_OBJECT_TYPE(object_type) if (unlikely((object_type) < 0 || (object_type) >= MAX_OBJECT_TYPES))

#define GS_ABORT() \
    do \
    { \
        printf("GS_ABORT error: %s, line %d function: %s\n", __FILE__, __LINE__, __FUNCTION__); \
        print_trace(); \
        exit(1); \
    } while(0);

// macro stringification
#define GS_STR2(x) #x
#define GS_STR(x) GS_STR2(x)

// __GNUG__
// The GNU C++ compiler defines this. Testing it is equivalent to testing (__GNUC__ && __cplusplus). 
// preprocessor macros 
// http://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html

#ifdef __GNUC__
# define OPTIMIZED __attribute__((optimize("-O3")))
# define STATIC_OPTIMIZED __attribute__((optimize("-O3"))) static
# define INLINE __attribute__((__always_inline__)) inline 
# define STATIC_INLINE __attribute__((__always_inline__)) static inline 
# define INLINE_OPTIMIZED __attribute__((always_inline, optimize("-O3"))) inline
# define STATIC_INLINE_OPTIMIZED __attribute__((always_inline, optimize("-O3"))) static inline 
#endif

#ifdef __MSVC__
# define OPTIMIZED
# define STATIC_OPTIMIZED static
# define INLINE_OPTIMIZED __forceinline
# define STATIC_INLINE_OPTIMIZED __forceinline
# define INLINE __forceinline
# define STATIC_INLINE __foreceinline static
#endif
