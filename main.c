#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "chip8.h"
#include "chip8.c"

void render_screen(SDL_Window *window, SDL_Renderer *renderer, SDL_Surface *surface, SDL_Texture *texture, struct chip8 *cpu);
int init_sdl(SDL_Window **window, SDL_Renderer **renderer, SDL_Surface **surface, SDL_Texture **texture);


int main(int argc, char *argv[]) {
	int res;
	struct chip8 processor;

    SDL_Window *window = NULL;
    SDL_Surface *surface = NULL; 
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;

    res = 0;
    if ((res = init_sdl(&window, &renderer, &surface, &texture)) != EXIT_SUCCESS) {
    	exit(res);
    }



	init(&processor);
	printf("1\n");
	loadrom(&processor, "games/MAZE");
	printf("2\n");
	while(!done) {
		printf("%d\n", processor.opcode);
        /* check for press */
        while (SDL_PollEvent(&e)) {
            done |= (e.type == SDL_QUIT);
            if (e.type == SDL_KEYDOWN) {
                key_down = handle_key_down(e.key.keysym.sym);
            } else if (e.type == SDL_KEYUP) {
                handle_key_up(e.key.keysym.sym);
            }
        }

		docycle(&processor);

		if(processor.drawFlag == 1) {
			render_screen(window, renderer, surface, texture, &processor);
			processor.drawFlag = 0;
		}
	}
/*	SDL_DestroyWindow(window); */
	SDL_Quit();
	return(EXIT_SUCCESS);
}

void render_screen(SDL_Window *window, SDL_Renderer *renderer, SDL_Surface *surface, SDL_Texture *texture, struct chip8 *cpu) {
    SDL_UpdateTexture(texture, NULL, cpu->gfx, 64 * sizeof(uint32_t)); /* pitch is the number of bytes from the start of one row to the next--and since we have a linear RGBA buffer in this example, it's just 640 times 4 (r,g,b,a). */
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

int init_sdl(SDL_Window **window, SDL_Renderer **renderer, SDL_Surface **surface, SDL_Texture **texture) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return(EXIT_FAILURE);
    }

    *window = SDL_CreateWindow("Chip8",
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               SCREEN_WIDTH,
                               SCREEN_HEIGHT,
                               SDL_WINDOW_SHOWN);

    if (*window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return(EXIT_FAILURE);
    }

    *renderer = SDL_CreateRenderer(*window, -1, 0);
    if (*renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return(EXIT_FAILURE);
    }

    *surface = SDL_GetWindowSurface(*window);
    *texture = SDL_CreateTexture(*renderer,
                                 SDL_PIXELFORMAT_ARGB8888,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 SCREEN_WIDTH, SCREEN_HEIGHT);
    if (*texture == NULL) {
        printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return(EXIT_FAILURE);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_RenderSetLogicalSize(*renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetWindowSize(*window, 640, 320);

    SDL_SetRenderDrawColor(*renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(*renderer);
    SDL_RenderPresent(*renderer);

    return(EXIT_SUCCESS);
}

