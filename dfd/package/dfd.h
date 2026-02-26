#ifndef DFD_H
#define DFD_H

#include <SDL3/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int				DfD_init(void);
void *			DfD_createWindow(const char *title, int w, int h);
int				DfD_set_image(void * win, uint32_t * image);
int				DfD_clear(void * win);
int				DfD_update(void * win);
SDL_WindowID	DfD_get_window_id(void * win);
int				DfD_pool_event(void * event);
int				super_main_1(void);
int				super_main_0(void);

#endif
