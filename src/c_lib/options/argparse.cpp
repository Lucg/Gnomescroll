/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "argparse.hpp"

#include <stdlib.h>
#include <string.h>

#include <errno.h>
//extern int errno;

namespace Options
{

/* Globals */

// table of option values
static int n_options = 0;
static void** option_values = NULL;
static char** option_names = NULL;

// table of functions that coerce a string into correct type
typedef void (*coercion)(int, char*);
static coercion* coercion_methods = NULL;

/* Table lookup */

static int get_option_index(char* name)
{
    for (int i=0; i<n_options; i++)
        if (!strcmp(name, option_names[i])) return i;
    return -1;
}

/* Setter entry point */

static int set_option_from_str(char* name, char* val)
{
    int opt_index = get_option_index(name);
    if (opt_index < 0) return opt_index;
    coercion_methods[opt_index](opt_index, val); // array of (void)(coerce)(str*)
    return opt_index;
}

/* Option Setters */

static void set_option_float(int key, float opt)
{
    *((float*)(option_values[key])) = opt;
}

static void set_option_uint(int key, unsigned int opt)
{
    *((unsigned int*)(option_values[key])) = opt;
}

static void set_option_int(int key, int opt)
{
    *((int*)(option_values[key])) = opt;
}

static void set_option_str(int key, char* opt)
{
    char* dest = *((char**)(option_values[key]));
    strncpy(dest, opt, ARG_STRING_MAX+1);
    dest[ARG_STRING_MAX] = '\0';
}

static void set_option_bool(int key, bool opt)
{
    *((bool*)(option_values[key])) = opt;
}

/* Coercions */

static void coerce_option_float(int key, char* val)
{
    errno = 0;
    char* end = NULL;
    float opt = (float)strtod(val, &end);
    GS_ASSERT(errno == 0);
    GS_ASSERT(val[0] != '\0'); // valid string test
    GS_ASSERT(end != val);
    set_option_float(key, opt);
}

static void coerce_option_uint(int key, char* val)
{
    errno = 0;
    char* end = (char*)"";
    unsigned int opt = (unsigned int)strtoul(val, &end, 10);
    GS_ASSERT(errno == 0);
    GS_ASSERT(val[0] != '\0' && end[0] == '\0'); // valid string test
    set_option_uint(key, opt);
}

static void coerce_option_int(int key, char* val)
{
    int opt = atoi(val);
    set_option_int(key, opt);
}

static void coerce_option_bool(int key, char* val)
{
    bool opt = true;
    if (!strcmp("false", val) || !strcmp("0", val))
        opt = false;
    set_option_bool(key, opt);
}

static void coerce_option_str(int key, char* val)
{
    set_option_str(key, val);
}

/* Registration */

static void add_option_name(int index, const char* name)
{
    unsigned int len = strlen(name);
    GS_ASSERT(len <= (unsigned int)ARG_NAME_MAX);
    if (len > (unsigned int)ARG_NAME_MAX) len = ARG_NAME_MAX;
    option_names[n_options] = (char*)calloc(len+1, sizeof(char));
    strncpy(option_names[n_options], name, len);
}

void register_uint_option(const char* key, unsigned int* val)
{
    GS_ASSERT(n_options < MAX_OPTIONS);
    add_option_name(n_options, key);
    option_values[n_options] = (void*)val;
    coercion_methods[n_options] = &coerce_option_uint;

    n_options++;
}

void register_int_option(const char* key, int* val)
{
    GS_ASSERT(n_options < MAX_OPTIONS);

    add_option_name(n_options, key);
    option_values[n_options] = (void*)val;
    coercion_methods[n_options] = &coerce_option_int;

    n_options++;
}

void register_float_option(const char* key, float* val)
{
    GS_ASSERT(n_options < MAX_OPTIONS);

    add_option_name(n_options, key);
    option_values[n_options] = (void*)val;
    coercion_methods[n_options] = &coerce_option_float;

    n_options++;
}

void register_bool_option(const char* key, bool* val)
{
    GS_ASSERT(n_options < MAX_OPTIONS);

    add_option_name(n_options, key);
    option_values[n_options] = (void*)val;
    coercion_methods[n_options] = &coerce_option_bool;

    n_options++;
}

void register_string_option(const char* key, char** addr, const char* val)
{
    *addr = (char*)calloc(ARG_STRING_MAX+1, sizeof(char));

    GS_ASSERT(n_options < MAX_OPTIONS);

    strncpy(*addr, val, ARG_STRING_MAX+1);
    (*addr)[ARG_STRING_MAX] = '\0';

    add_option_name(n_options, key);
    option_values[n_options] = (void*)addr;
    coercion_methods[n_options] = &coerce_option_str;

    n_options++;
}

/* Public Interface */

int parse_args(int argc, const char* argv[])
{
    if (argc < 2) return 0;

    char argname[ARG_NAME_MAX+1] = {'\0'};
    char argstr[ARG_STRING_MAX+1] = {'\0'};

    // skip the first argument if it is the settings filename
    int start = 1;
    if (argv[1][0] != '-') start = 2;

    int n=0;
    for (int i=start; i<argc; i++)
    {
        const char* str = argv[i];
        IF_ASSERT(str[0] == '\0') continue;
        if (str[0] != '-')
        {
            printf("Invalid argument style: %s\n", str);
            continue;
        }

        size_t j=0,k=0,m=0;
        char c;
        while ((c = str[j++]) != '\0' && c == '-'); // skip all -
        j--;
        while ((c = str[j++]) != '\0' && c != '=' && k < ARG_NAME_MAX)
            argname[k++] = c;

        if (k == 0)
        {
            printf("Missing argument name. %s\n", argname);
            continue;
        }

        if (k == ARG_NAME_MAX && (c != '=' || c != '\0'))
        {
            printf("Argument name is too long. Error arg: %s\n", str);
            continue;
        }

        argname[k] = '\0';

        if (c != '=')
        {
            if (!strcmp(argname, "help"))
                printf("\n\nUsage: ./run [lua settings file] --key=value\n\n\n");
            else
                printf("Invalid argument style: %s\n", str);
            continue;
        }

        while ((c = str[j++]) != '\0' && m < ARG_STRING_MAX)
            argstr[m++] = c;

        if (m == 0)
        {
            printf("Missing argument value. %s\n", str);
            continue;
        }

        if (m == ARG_STRING_MAX && c != '\0')
        {
            printf("Argument value is too long. Error arg: %s\n", str);
            continue;
        }

        argstr[m] = '\0';

        // now need to coerce argstr to correct type
        int ret = set_option_from_str(argname, argstr);
        if (ret < 0)
        {
            printf("Argument %s unrecognized\n", argname);
            continue;
        }

        n++;
    }
    return n;
}

/* Init/Teardown */

void init_option_tables()
{
    option_values = (void**)calloc(MAX_OPTIONS, sizeof(void*));
    option_names = (char**)calloc(MAX_OPTIONS, sizeof(char*));
    coercion_methods = (coercion*)calloc(MAX_OPTIONS, sizeof(coercion));
}

void teardown_option_tables()
{
    if (option_values != NULL)
        for (int i=0; i<MAX_OPTIONS; i++)
            if (coercion_methods[i] == &coerce_option_str)
                free(*((char**)(option_values[i])));
    free(option_values);
    if (option_names != NULL)
        for (int i=0; i<MAX_OPTIONS; i++)
            free(option_names[i]);
    free(option_names);
    free(coercion_methods);
}

}   // Options
