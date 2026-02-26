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

int DfD_set_image(void * win, uint32_t * image)
{
	int					pitch;
	void *				rawPixels;
	DfD_win_data_t *	dfd_win = win;

	// yes need check ret value
	if (!SDL_LockTexture(dfd_win->texture, NULL, &rawPixels, &pitch))
	{
		return DFD_FAILURE;
	}
	
	memcpy(rawPixels, image, dfd_win->w * dfd_win->h * 4);
	SDL_UnlockTexture(dfd_win->texture);

	return DFD_SUCCESS;
}

//                                                                                       |
// --------------------------------------------------------------------------------------|
//                                                                                       |

int DfD_clear(void * win)
{
	DfD_win_data_t * dfd_win = win;

	if (!SDL_RenderClear(dfd_win->renderer))
	{
		return DFD_FAILURE;
	}

	return DFD_SUCCESS;
}

//                                                                                       |
// --------------------------------------------------------------------------------------|
//                                                                                       |

int DfD_update(void * win)
{
	DfD_win_data_t * dfd_win = win;

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

//                                                                                       |
// --------------------------------------------------------------------------------------|
//                                                                                       |

// FOR TEST
int super_main_1(void)
{
	int			width = 1600;
	int			height = 900;
	void *		dfd_win;
	uint32_t *	img;

	img = malloc(width * height * sizeof(uint32_t));
	if (!img)
	{
		printf("exit from malloc\n");
		return DFD_FAILURE;
	}

	// if (DfD_init() != DFD_SUCCESS)
	// {
	// 	printf("exit from DfD_init\n");
	// 	free(img);
	// 	return DFD_FAILURE;
	// }

	dfd_win = DfD_createWindow("test_window", width, height);
	if (!dfd_win)
	{
		printf("exit from DfD_createWindow\n");
		free(img);
		return DFD_FAILURE;
	}

	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			img[y * width + x] = 0xFF;
			img[y * width + x] |= (0xFF & y) << 8;
			img[y * width + x] |= (0xFF & x) << 24;
		}
	}

	if (DfD_set_image(dfd_win, img) != DFD_SUCCESS)
	{
		printf("exit from DfD_set_image\n");
		free(img);
		return DFD_FAILURE;
	}

	if (DfD_update(dfd_win) != DFD_SUCCESS)
	{
		printf("exit from DfD_update\n");
		free(img);
		return DFD_FAILURE;
	}

	// while (1)
	// {
	// 	SDL_Delay(15);
	// }

	SDL_Event event;
	int running = 1;
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				running = 0;
			}
			printf("event :\n");
			printf("--> type: %d\n", event.type);
		}

		SDL_Delay(15);
	}

	free(img);
	return DFD_SUCCESS;
}


//                                                                                       |
// --------------------------------------------------------------------------------------|
//                                                                                       |

// FOR EXAMPLE
int super_main_0(void)
{
	// ok
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		printf("Ошибка инициализации SDL: %s\n", SDL_GetError());
		return DFD_FAILURE;
	}

	int w = 1600, h = 900;

	SDL_Window * window = SDL_CreateWindow("you window", w, h, 0);
	if (!window) {
		printf("Ошибка создания окна: %s\n", SDL_GetError());
		SDL_Quit();
		return DFD_FAILURE;
	}

	SDL_Renderer * renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer) {
		printf("Ошибка создания рендера: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return DFD_FAILURE;
	}

	SDL_Texture * texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		w, 
		h
	);
	if (!texture) {
		printf("Ошибка создания текстуры: %s\n", SDL_GetError());
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return DFD_FAILURE;
	}


	// yes need check ret value
	void *texPixels;
	int pitch;
	SDL_LockTexture(texture, NULL, &texPixels, &pitch);

	pitch /= 4;
	uint32_t *p = (uint32_t*)texPixels;
	for (size_t y = 0; y < h; ++y)
	{
		for (size_t x = 0; x < w; ++x)
		{
			p[y * pitch + x] = 0xFF;
			p[y * pitch + x] |= (0xFF & y) << 8;
			p[y * pitch + x] |= (0xFF & x) << 16;
		}
	}

	SDL_UnlockTexture(texture);

	SDL_RenderClear(renderer);

	SDL_RenderTexture(renderer, texture, NULL, NULL);

	SDL_RenderPresent(renderer);


	SDL_Event event;
	int running = 1;
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				running = 0;
			}
		}
		SDL_Delay(16);
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return DFD_SUCCESS;
}

//                                                                                       |
// --------------------------------------------------------------------------------------|
//                                                                                       |

// int main(int argc, const char ** argv)
// {
// 	if (super_main_1() != DFD_SUCCESS)
// 	{
// 		return DFD_FAILURE;
// 	}
// }