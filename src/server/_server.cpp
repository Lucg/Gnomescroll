

#include "stdafx.h" 
//precompiled headers

#ifndef __MSVC__
    #ifdef _MSC_VER
        #define __MSVC__ 1
    #endif
#endif

//_getcwd()/_wgetcwd() Gets the current working directory
//_chdir()/_wchdir() Sets the current working directory

#ifdef __MSVC__
    #include <direct.h>
    #define getcwd _getcwd
    #define chdir _chdir
#else
    #include <unistd.h>
#endif

#ifdef __MSVC__
    #include <windows.h> 
    #include <winbase.h> // usleep function
    #include <tchar.h>

    #include <stdio.h>
    #include <stdlib.h>
    #include <stdint.h>
 
    #include <string.h>
    #include <math.h>
#endif

#ifdef __GNUC__
#endif

#include "_c_lib.cpp"
#include "_main.cpp"
#include <common/lua/lua.hpp>

//int main(int argc, char** argv)
int _tmain(int argc, _TCHAR* argv[])
{
    #if PRODUCTION
    printf("Production build\n");
    #else
    printf("Debug build\n");
    #endif

    #ifdef __APPLE__
    set_working_directory_to_Resources();
    #endif

#ifdef __MSVC__
    chdir("C:/dc_mmo/server");
#endif
    // set output unbuffered
    setvbuf(stdout, NULL, _IONBF, 0);   // NULL,0 are ignored when _IONBF enabled
    
    // program accepts one option: path to lua settings file
    // will use a default value if not provided
    if (argc > 1) LUA::set_options_file( (char*) argv[1] );

    Main::init();
    int ret = Main::run();


    //Sleep(10000);
    return ret;
}