/*
 * THE GIL'Z NIGHTMARE
 * made with love
 * by mon
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <Windows.h>
#include <TlHelp32.h>
#include "SDL2/SDL.h"

#include "process.h"
#include "outsider.h"

#define PLAYER_C 12
#define ME 0

#define WIDTH 300
#define HEIGHT 300

int win_width;
int win_height;

player players[PLAYER_C];
int cmode; // 0 player 1 cam

void update(process* p);
void flush(process* p);
void print_positions();
void draw_map(SDL_Renderer* renderer);

int main(int argc, char** argv)
{
	process p;
	int i;
	int process_status;
	bool quit = false;

	SDL_Window* win;
	SDL_Surface* screen;
	SDL_Renderer* renderer;
	SDL_Event e;

	srand(time(NULL));

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("[*] ERROR: could not initialize SDL\n");
	}

	process_status = process_init(&p, "outsider");
	if (process_status)
	{
		if (process_status == 1)
			printf("[*] ERROR: could not hook outsider process\n");
		else if (process_status == 2)
			printf("[*] ERROR: could not resolve the process base address\n");
		return 1;
	}

	printf("[*] process hooked\n");
	printf("[*] querying process module info..\n");

	if (!p.base_address)
	{
		printf("[*] ERROR: could not get process base address\n");
		return 1;
	}

	printf("[*] process base address => %x\n", p.base_address);

	if (!(win = SDL_CreateWindow("7H3 G1LZ N1GH7M4R3", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN)))
	{
		SDL_Quit();
		return 1;
	}

	/*
	if (!(win = SDL_CreateWindowFrom(p.window_handler)))
	{
		SDL_Quit();
		return 1;
	}
	*/

	//SDL_SetWindowGrab(win, SDL_TRUE);
	
	SDL_GetWindowSize(win, &win_width, &win_height);

	printf("[*] created sdl window from process (%d, %d)\n", win_width, win_height);

	screen = SDL_GetWindowSurface(win);
	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

	SDL_SetColorKey(screen, SDL_TRUE, SDL_MapRGB(screen->format, 255, 0, 255));
	//SDL_SetSurfaceAlphaMod(screen, 255);

	if (!renderer)
	{
		SDL_Quit();
		return 1;
	}

	printf("[*] processing players data.. \n");

	outsider_resolve_players(&p, players, PLAYER_C);
	outsider_generate_colors(players, PLAYER_C);
	outsider_read_players(&p, players, PLAYER_C);

	print_positions();

	printf("[*] loading map.. \n");

	/*
	 * ugliest switch EVER
	 */
	for (;;)
	{
		if (quit)
			break;

		SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
		SDL_RenderClear(renderer);

		draw_map(renderer);

		SDL_RenderPresent(renderer);

		update(&p);

		while (SDL_PollEvent(&e))
		{
			switch(e.type)
			{

			case SDL_KEYDOWN:
				break;
			case SDL_KEYUP:
				switch (e.key.keysym.sym)
				{

				case SDLK_h:
					if (!cmode)
						players[ME].x += 10;
					else
						cam.x -= 500;
					break;
				case SDLK_j:
					if (!cmode)
						players[ME].y -= 10;
					else
						cam.y -= 500;
					break;
				case SDLK_k:
					if (!cmode)
						players[ME].y += 10;
					else
						cam.y += 500;
					break;
				case SDLK_l:
					if (!cmode)
						players[ME].x -= 10;
					else
						cam.x += 500;
					break;
				case SDLK_u:
					players[ME].z -= 2;
					break;
				case SDLK_p:
					players[ME].z += 2;
					break;
				case SDLK_m:
					print_positions();
					break;
				case SDLK_x:
					cmode = (cmode == 1) ? 0 : 1;
					break;
				case SDLK_f:
					players[ME].flying = players[ME].flying ? false : true;
					break;
				case SDLK_q:
					quit = true;
					break;
				default:
					break;

				}
				break;
			default:
				break;

			}
		}

		flush(&p);
		SDL_Delay(10);
	}

	SDL_FreeSurface(screen);
	SDL_DestroyWindow(win);

	SDL_Quit();

	return 0;
}

void print_positions()
{
	int i;
	printf("\n");
	for (i = 0; i < PLAYER_C; i++)
	{
		printf("[*] p[%d] => [%f/%f/%f]\n", i, players[i].x, players[i].y, players[i].z);
	}
	printf("\n");
}

void draw_map(SDL_Renderer* renderer)
{
	float max_x;
	float max_y;
	float min_x;
	float min_y;
	float border_x;
	float border_y;
	float pmap_x;
	float pmap_y;
	int map_size;
	int player_rect_size;
	int i;

	/*
	 * dont't have a fucking idea what are the correct map borders
	 * even GIL does not know that.........
	 */
	max_x = 130;
	max_y = 1200;
	min_x = -10;
	min_y = 950;
	
	map_size = win_width * 0.1;

	player_rect_size = map_size * 0.1;

	SDL_Rect rectm = {0, 0, map_size, map_size};
	SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0xFF, 0xFF);
	SDL_RenderFillRect(renderer, &rectm);

	for (i = 0; i < PLAYER_C; i++)
	{
		// ((max limit - min limit) * (player x - min_x) / (max_x - min_x)) + min limit 
		pmap_x = ((map_size - 0) * (players[i].x - min_x) / (max_x - min_x)) + 0;
		pmap_y = ((map_size - 0) * (players[i].y - min_y) / (max_y - min_y)) + 0;

		if (i == ME)
		{
			SDL_Rect rectb = {map_size - pmap_x - player_rect_size / 2 * 1.2, map_size - pmap_y - player_rect_size / 2 * 1.2, player_rect_size * 1.2, player_rect_size * 1.2};
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderFillRect(renderer, &rectb);
		}

		SDL_Rect rect = {map_size - pmap_x - player_rect_size / 2, map_size - pmap_y - player_rect_size / 2, player_rect_size, player_rect_size};
		SDL_SetRenderDrawColor(renderer, players[i].color[0], players[i].color[1], players[i].color[2], 0xFF);
		SDL_RenderFillRect(renderer, &rect);
	}
}

void update(process* p)
{
	outsider_read_players(p, &players, PLAYER_C);

	/*
	if (cmode == 0)
	{
		process_read(&p, cam.xa, &cam.x, sizeof(float));
		process_read(&p, cam.ya, &cam.y, sizeof(float));
	}
	*/
}

void flush(process* p)
{
	outsider_write_player(p, &players[ME]);

	/*
	if (cmode == 1)
	{
		WriteProcessMemory(p->handler, (LPVOID) cam.xa, (LPCVOID) &cam.x, sizeof(float), NULL);
		WriteProcessMemory(p->handler, (LPVOID) cam.ya, (LPCVOID) &cam.y, sizeof(float), NULL);
	}
	*/
}
