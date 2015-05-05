/*
 * outsider exploiter
 * made with love
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

player players[PLAYER_C];
int cmode;
bool done;

bool init(process* p);
void quit(process* p);

void update(process* p);
void flush(process* p);
void print_positions();
void draw_map(process* p);

LRESULT CALLBACK keyboard_proc(int nc, WPARAM wp, LPARAM lp);

int main(int argc, char** argv)
{
	process p;

	srand(time(NULL));

	if (!init(&p))
	{
		quit(&p);
		return 0;
	}

	printf("[*] processing camera data.. \n");

	outsider_resolve_camera(&p);

	printf("[*] processing players data.. \n");

	outsider_resolve_players(&p, players, PLAYER_C);
	outsider_generate_colors(players, PLAYER_C);

	update(&p);
	print_positions();

	printf("[*] loading map.. \n");

	done = false;

	for (;;)
	{
		if (done)
			break;

		process_peek_message(&p);

		update(&p);

		//SDL_SetRenderDrawColor(p.graphics.renderer, 255, 0, 255, 255);
		//SDL_RenderClear(p.graphics.renderer);

		draw_map(&p);

		SDL_RenderPresent(p.graphics.renderer);

		flush(&p);
		//SDL_Delay(10);
	}

	quit(&p);
	return 0;
}

bool init(process* p)
{
	int process_status;

	process_status = process_init(p, "outsider");
	if (process_status)
	{
		if (process_status == 1)
			printf("[*] ERROR: could not hook outsider process\n");
		else if (process_status == 2)
			printf("[*] ERROR: could not resolve the process base address\n");

		return false;
	}

	printf("[*] process hooked, base address: %x\n", p->base_address);

	process_status = process_create_window(p);

	if (process_status)
	{
		if (process_status == 1)
			printf("[*] ERROR: could not initialize SDL\n");
		else if (process_status == 2)
			printf("[*] ERROR: could not attach SDL window to ousider window\n");
		else if (process_status == 3)
			printf("[*] ERROR: failed to create SDL window renderer\n");

		return false;
	}

	process_keyboard_hook(p, keyboard_proc);

	return true;
}

void quit(process* p)
{
	SDL_FreeSurface(p->graphics.screen);
	SDL_DestroyWindow(p->graphics.window);
	SDL_Quit();

	process_keyboard_unhook(p);
}

LRESULT CALLBACK keyboard_proc(int nc, WPARAM wp, LPARAM lp)
{
	PKBDLLHOOKSTRUCT p;

	if (nc == HC_ACTION)
	{
		if (wp == WM_KEYDOWN)
		{
			p = (PKBDLLHOOKSTRUCT) lp;

			switch (p->vkCode)
			{

			case 0x50: // P
				print_positions();
				break;

			case 0x51: // Q
				done = true;
				break;

			}
		}
	}

	return CallNextHookEx(NULL, nc, wp, lp);
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

void draw_map(process* p)
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
	
	map_size = p->graphics.width * 0.1;

	player_rect_size = map_size * 0.1;

	SDL_Rect rectm = {0, 0, map_size, map_size};
	SDL_SetRenderDrawColor(p->graphics.renderer, 0x00, 0xFF, 0xFF, 0xFF);
	SDL_RenderFillRect(p->graphics.renderer, &rectm);

	for (i = 0; i < PLAYER_C; i++)
	{
		// ((max limit - min limit) * (player x - min_x) / (max_x - min_x)) + min limit 
		pmap_x = ((map_size - 0) * (players[i].x - min_x) / (max_x - min_x)) + 0;
		pmap_y = ((map_size - 0) * (players[i].y - min_y) / (max_y - min_y)) + 0;

		if (i == ME)
		{
			SDL_Rect rectb = {map_size - pmap_x - player_rect_size / 2 * 1.2, map_size - pmap_y - player_rect_size / 2 * 1.2, player_rect_size * 1.2, player_rect_size * 1.2};
			SDL_SetRenderDrawColor(p->graphics.renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderFillRect(p->graphics.renderer, &rectb);
		}

		SDL_Rect rect = {map_size - pmap_x - player_rect_size / 2, map_size - pmap_y - player_rect_size / 2, player_rect_size, player_rect_size};
		SDL_SetRenderDrawColor(p->graphics.renderer, players[i].color[0], players[i].color[1], players[i].color[2], 0xFF);
		SDL_RenderFillRect(p->graphics.renderer, &rect);
	}
}

void update(process* p)
{
	outsider_read_players(p, &players, PLAYER_C);

	if (cmode == 0)
		outsider_read_camera(p);
}

void flush(process* p)
{
	outsider_write_player(p, &players[ME]);

	if (cmode == 1)
		outsider_write_camera(p);
}
