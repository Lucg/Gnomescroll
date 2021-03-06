/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

int _set_resolution(int xres, int yres, int fullscreen);

int VersionMismatchBox(int local_version, int server_version);

int init_video();
void _del_video();
int _swap_buffers();

void close_SDL();

void save_screenshot();

int check_gl_error();
int check_gl_error(const char* filename, const int line_no);

#if PRODUCTION
# define CHECK_GL_ERROR()
#else
# define CHECK_GL_ERROR() check_gl_error(__FILE__, __LINE__)
#endif

extern int _xres;
extern int _yres;
extern float _xresf;
extern float _yresf;
extern int _fullscreen;
