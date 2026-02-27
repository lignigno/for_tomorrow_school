#include "dfd.h"

#define DFD_SUCCESS 0
#define DFD_FAILURE 1
#define MAX_WINDOWS 42

typedef struct
{
	int				w;
	int				h;
	SDL_WindowID	id;
	SDL_Window *	window;
	SDL_Renderer *	renderer;
	SDL_Texture *	texture;
} DfD_win_data_t;

typedef struct
{
	int					exist;
	DfD_win_data_t *	windows[MAX_WINDOWS];
} dfd_win_arr_t;

dfd_win_arr_t	win_arr;

// __________________________________________________________________________________FUNCS

void some_print(void)
{
	printf("\033[1;38;2;255;0;128mHello bitches\033[0m\n");
}

//                                                                                       |
// --------------------------------------------------------------------------------------|
//                                                                                       |

void DfD_cleanup(void)
{
	for (size_t i = 0; i < MAX_WINDOWS; ++i)
	{
		if (win_arr.windows[i] != NULL)
		{
			if (win_arr.windows[i]->texture)
			{
				SDL_DestroyTexture(win_arr.windows[i]->texture);
			}
			if (win_arr.windows[i]->renderer)
			{
				SDL_DestroyRenderer(win_arr.windows[i]->renderer);
			}
			if (win_arr.windows[i]->window) 
			{
				SDL_DestroyWindow(win_arr.windows[i]->window);
			}

			free(win_arr.windows[i]);
			win_arr.windows[i] = NULL;
		}
	}
	
	SDL_Quit();
}

//                                                                                       |
// --------------------------------------------------------------------------------------|
//                                                                                       |

int DfD_init(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 1) {
		return DFD_FAILURE;
	}

	win_arr.exist = 0;
	for (size_t i = 0; i < MAX_WINDOWS; ++i)
	{
		win_arr.windows[i] = NULL;
	}

	atexit(DfD_cleanup);

	return DFD_SUCCESS;
}

//                                                                                       |
// --------------------------------------------------------------------------------------|
//                                                                                       |

void * DfD_createWindow(const char *title, int w, int h)
{
	DfD_win_data_t * new_win = NULL;

	new_win = (DfD_win_data_t *)malloc(sizeof(DfD_win_data_t));
	if (!new_win)
	{
		goto ret_1;
	}

	new_win->window = SDL_CreateWindow(title, w, h, 0);
	if (!new_win->window)
	{
		goto ret_2;
	}
	new_win->id = SDL_GetWindowID(new_win->window);
	if (new_win->id == 0)
	{
		goto ret_3;
	}

	new_win->renderer = SDL_CreateRenderer(new_win->window, NULL);
	if (!new_win->renderer)
	{
		goto ret_3;
	}
	if (!SDL_SetRenderVSync(new_win->renderer, 1))
	{
		goto ret_4;
	}
	if (!SDL_SetRenderDrawColor(new_win->renderer, 0,0,0,0))
	{
		goto ret_4;
	}

	new_win->texture = SDL_CreateTexture(
		new_win->renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		w, 
		h
	);
	if (!new_win->texture)
	{
		goto ret_4;
	}
	new_win->w = w;
	new_win->h = h;

	for (size_t i = 0; i < MAX_WINDOWS; ++i)
	{
		if (win_arr.windows[i] == NULL)
		{
			win_arr.windows[win_arr.exist++] = new_win;
			break;
		}
	}

	return (void *)new_win;

ret_4:
	SDL_DestroyRenderer(new_win->renderer);
ret_3:
	SDL_DestroyWindow(new_win->window);
ret_2:
	free(new_win);
ret_1:

	return NULL;
}

//                                                                                       |
// --------------------------------------------------------------------------------------|
//                                                                                       |

// int DfD_set_image(void * win, uint32_t * image)
// {
// 	int					pitch;
// 	void *				rawPixels;
// 	DfD_win_data_t *	dfd_win = win;

// 	// yes need check ret value
// 	if (!SDL_LockTexture(dfd_win->texture, NULL, &rawPixels, &pitch))
// 	{
// 		return DFD_FAILURE;
// 	}

// 	memcpy(rawPixels, image, dfd_win->w * dfd_win->h * 4);
// 	SDL_UnlockTexture(dfd_win->texture);

// 	return DFD_SUCCESS;
// }

//                                                                                       |
// --------------------------------------------------------------------------------------|
//                                                                                       |

int DfD_update(void * win, uint32_t * image)
{
	int					pitch;
	void *				rawPixels;
	DfD_win_data_t * dfd_win = win;

	if (!SDL_LockTexture(dfd_win->texture, NULL, &rawPixels, &pitch))
	{
		return DFD_FAILURE;
	}

	memcpy(rawPixels, image, dfd_win->w * dfd_win->h * 4);
	SDL_UnlockTexture(dfd_win->texture);

	if (!SDL_RenderTexture(dfd_win->renderer, dfd_win->texture, NULL, NULL))
	{
		return DFD_FAILURE;
	}
	if (!SDL_RenderPresent(dfd_win->renderer))
	{
		return DFD_FAILURE;
	}

	return DFD_SUCCESS;
}

//                                                                                       |
// --------------------------------------------------------------------------------------|
//                                                                                       |

SDL_WindowID DfD_get_window_id(void * win)
{
	DfD_win_data_t * dfd_win = win;

	return dfd_win->id;
}

//                                                                                       |
// --------------------------------------------------------------------------------------|
//                                                                                       |

int DfD_pool_event(void * event)
{
	if (SDL_PollEvent((SDL_Event *)event))
	{
		return DFD_SUCCESS;
	}

	return DFD_FAILURE;
}
