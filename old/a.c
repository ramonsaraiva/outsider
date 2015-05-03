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

#define PLAYER_C 12
#define ME 0

#define WIDTH 300
#define HEIGHT 300

typedef struct _proc
{
	HWND w;
	unsigned long pid;
	HANDLE h;
	unsigned long b;
} proc;

typedef struct _player
{
	int i;
	unsigned int pos_offsets[3];
	unsigned int xa;
	unsigned int ya;
	unsigned int za;
	float x;
	float y;
	float z;
	float stamina;
} player;

typedef struct _camera
{
	unsigned int xa;
	unsigned int ya;
	float x;
	float y;
} camera;

/*
 * offsets
 */

const unsigned int	PLAYER_POS_OFFSETS[3]	= {0x1E8, 0x1E0, 0x04};
const unsigned int	PLAYER_POS_ITER			= 0x260;
const unsigned int	PLAYER_POS_PATH[]		= {0xE55910, 0x58, 0x10, 0x0};
const int			PLAYER_POS_PATH_C		= 4;

const unsigned int	CAMERA_X_OFFSETS[]		= {0xE49EF4, 0x68};
const unsigned int	CAMERA_Y_OFFSETS[]		= {0xE49EF4, 0x6C};
const int			CAMERA_OFFSETS_C		= 2;

/*
 * global stuff
 */

int win_width;
int win_height;

camera cam;
player players[PLAYER_C];
int player_colors[PLAYER_C][3];
int cmode; // 0 player 1 cam
int fly = 0; 

/*
 * retarded function to get the process base memory address
 * dunno why the hell i can't get it easier
 */
bool pba(proc* p);

/*
 * wrote this to go through all offsets needed to reach the data
 * and then setting its value somewhere
 *
 * parameters are: process, array of offsets, offsets count, output, output size
 */
unsigned int fv(proc* p, const unsigned long* of, const int ofs, void* o, unsigned long s);

/*
 * update the player offsets according to the offset
 * for example, if you are player 1, you have 1 * OFFSET_DIFF
 * without it, we can mess with other players memory data
 */
void process_players_offsets();

/*
 * resolve all addresses to read/write player data
 */
void process_players_addr(proc* p);

/*
 * resolve all addresses to read/write camera data
 */
void process_cam_addr(proc* p);

/*
 * randomize one color for each player to be
 * displayed on the map :~)
 */
void process_players_colors();

/*
 * update data reading from memory
 */
void update(proc* p);

/*
 * flush local data to memory
 */
void flush(proc* p);

/*
 * this won't work as expected
 * need to see how to deal with colliders :{
 * maybe exploiting the 'z' value?!
 */
void teleport(char k);

/*
 * output all player positions
 */
void print_positions();

/*
 * draws the map showing where all players are 8OP
 * the cool stuff about it is that i am accidently
 * drawing the monsters cuz they are stored in the same
 * offsets as the players LOL
 */
void draw_map(SDL_Renderer* renderer);

int main(int argc, char** argv)
{
	proc p;
	int i;
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

	p.w = FindWindow(0, "outsider");

	if (p.w == 0)
	{
		printf("[*] ERROR: outsider window not found\n");
		return 1;
	}

	GetWindowThreadProcessId(p.w, &p.pid);
	printf("[*] found outsider with pid => %lu\n", p.pid);

	p.h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, p.pid);

	if (!p.h)
	{
		printf("[*] ERROR: could not hook outsider process\n");
		return 1;
	}

	printf("[*] process hooked\n");
	printf("[*] querying process module info..\n");

	if (!pba(&p))
	{
		printf("[*] ERROR: could not get process base address\n");
		return 1;
	}

	printf("[*] process base address => %x\n", p.b);


	/*
	if (!(win = SDL_CreateWindow("7H3 G1LZ N1GH7M4R3", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN)))
	{
		SDL_Quit();
		return 1;
	}
	*/

	if (!(win = SDL_CreateWindowFrom(p.w)))
	{
		SDL_Quit();
		return 1;
	}

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

	process_players_offsets();
	process_players_addr(&p);
	process_cam_addr(&p);
	process_players_colors();

	print_positions();

	printf("[*] loading map.. \n");

	/*
	 * ugliest switch EVER
	 */
	for (;;)
	{
		if (quit)
			break;

		//printf("my stamina => %d\n", players[ME].stamina);
		

		/* HDC STUFF
		HDC hdc = GetDC(p.w);

		RECT r = {0, 0, 100, 100};
		HBRUSH br = CreateSolidBrush(RGB(0, 2550, 0));

		FillRect(hdc, &r, br);

		InvalidateRect(p.w, &r, FALSE);

		ReleaseDC(p.w, hdc);

		*/

		SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
		SDL_RenderClear(renderer);

		SDL_BlitSurface(screen, NULL, screen, NULL);

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
					fly = (fly == 1) ? 0 : 1;
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

void teleport(char k)
{
	int p = atoi(&k);
	players[ME].x = players[p].x;
	players[ME].y = players[p].y;
	printf("[@] teleporting to player %d\n", p);
}

void print_positions()
{
	int i;
	printf("\n");
	for (i = 0; i < PLAYER_C; i++)
	{
		printf("[*] p[%d] (%x/%x) => [%f/%f]\n", i, players[i].pos_offsets[0], players[i].pos_offsets[1], players[i].x, players[i].y);
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
	 * thought about picking up players min/max X & Y positions
	 * to calculate some map based on it, but it is better just
	 * guessing the map limits and applying some limit formula
	 *
	max_x = 0;
	max_y = 0;
	min_x = 999999.0;
	min_y = 999999.0;
	for (i = 0; i < PLAYER_C; i++)
	{
		if (players[i].x > max_x)
			max_x = players[i].x;
		if (players[i].y > max_y)
			max_y = players[i].y;

		if (players[i].x < min_x)
			min_x = players[i].x;
		if (players[i].y < min_y)
			min_y = players[i].y;
	}
	*/

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
		/* ((max limit - min limit) * (player x - min_x) / (max_x - min_x)) + min limit */
		pmap_x = ((map_size - 0) * (players[i].x - min_x) / (max_x - min_x)) + 0;
		pmap_y = ((map_size - 0) * (players[i].y - min_y) / (max_y - min_y)) + 0;

		if (i == ME)
		{
			SDL_Rect rectb = {map_size - pmap_x - player_rect_size / 2 * 1.2, map_size - pmap_y - player_rect_size / 2 * 1.2, player_rect_size * 1.2, player_rect_size * 1.2};
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderFillRect(renderer, &rectb);
		}

		SDL_Rect rect = {map_size - pmap_x - player_rect_size / 2, map_size - pmap_y - player_rect_size / 2, player_rect_size, player_rect_size};
		SDL_SetRenderDrawColor(renderer, player_colors[i][0], player_colors[i][1], player_colors[i][2], 0xFF);
		SDL_RenderFillRect(renderer, &rect);
	}
}

bool pba(proc* p)
{
	MODULEENTRY32 me;
	me.dwSize = sizeof(MODULEENTRY32);
	HANDLE ss = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, p->pid);

	if (ss == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	if (!Module32First(ss, &me))
	{
		return false;
	}

	CloseHandle(ss);
	p->b = (DWORD) me.modBaseAddr;
	return true;
}

unsigned int fv(proc* p, const unsigned long* of, const int ofs, void* o, unsigned long s)
{
	unsigned int a;
	int i;

	a = p->b;
	for (i = 0; i < ofs - 1; i++)
	{
		//printf("[!!] swimming through.. %x and summing %x\n", a, of[i]);
		ReadProcessMemory(p->h, (LPCVOID) (a + of[i]), &a, 4, NULL);
	}

	ReadProcessMemory(p->h, (LPCVOID) (a + of[ofs - 1]), o, s, NULL);
	return (a + of[ofs - 1]);
}

void process_players_offsets()
{
	unsigned int xof;
	unsigned int yof;
	unsigned int zof;
	int i;

	xof = PLAYER_POS_OFFSETS[0];
	yof = PLAYER_POS_OFFSETS[1];
	zof = PLAYER_POS_OFFSETS[2];
	for (i = 0; i < PLAYER_C; i++)
	{
		players[i].pos_offsets[0] = xof;
		players[i].pos_offsets[1] = yof;
		players[i].pos_offsets[2] = zof;

		xof += PLAYER_POS_ITER;
		yof += PLAYER_POS_ITER;
		zof += PLAYER_POS_ITER;
	}
}

void process_players_addr(proc* p)
{
	unsigned int pos_offsets[PLAYER_POS_PATH_C];
	int i;

	memcpy(&pos_offsets, &PLAYER_POS_PATH, PLAYER_POS_PATH_C * 4);
	process_players_offsets();

	for (i = 0; i < PLAYER_C; i++)
	{
		pos_offsets[PLAYER_POS_PATH_C - 1] = players[i].pos_offsets[0];
		players[i].xa = fv(p, &pos_offsets, PLAYER_POS_PATH_C, &players[i].x, sizeof(float));

		pos_offsets[PLAYER_POS_PATH_C - 1] = players[i].pos_offsets[1];
		players[i].ya = fv(p, &pos_offsets, PLAYER_POS_PATH_C, &players[i].y, sizeof(float));

		pos_offsets[PLAYER_POS_PATH_C - 1] = players[i].pos_offsets[2];
		players[i].za = fv(p, &pos_offsets, PLAYER_POS_PATH_C, &players[i].z, sizeof(float));

	}
}

void process_cam_addr(proc* p)
{
	cam.xa = fv(p, &CAMERA_X_OFFSETS, CAMERA_OFFSETS_C, &cam.x, sizeof(float));
	cam.ya = fv(p, &CAMERA_Y_OFFSETS, CAMERA_OFFSETS_C, &cam.y, sizeof(float));
}

void process_players_colors()
{
	int i;
	int j;

	for (i = 0; i < PLAYER_C; i++)
		for (j = 0; j < 3; j++)
			player_colors[i][j] = rand() % 200;
}

void update(proc* p)
{
	int i = 0;
	for (i = 0; i < PLAYER_C; i++)
	{
		ReadProcessMemory(p->h, (LPCVOID) players[i].xa, &players[i].x, sizeof(float), NULL);
		ReadProcessMemory(p->h, (LPCVOID) players[i].ya, &players[i].y, sizeof(float), NULL);
		if (!fly)
			ReadProcessMemory(p->h, (LPCVOID) players[i].za, &players[i].z, sizeof(float), NULL);
		//gonna do it sometime
		//ReadProcessMemory(p->h, (LPCVOID) (players[i].xa - 0x8CD8030), &players[i].stamina, 4, NULL);
	}

	if (cmode == 0)
	{
		ReadProcessMemory(p->h, (LPCVOID) cam.xa, &cam.x, sizeof(float), NULL);
		ReadProcessMemory(p->h, (LPCVOID) cam.ya, &cam.y, sizeof(float), NULL);
	}
}

void flush(proc* p)
{
	WriteProcessMemory(p->h, (LPVOID) players[ME].xa, (LPCVOID) &players[ME].x, sizeof(float), NULL);
	WriteProcessMemory(p->h, (LPVOID) players[ME].ya, (LPCVOID) &players[ME].y, sizeof(float), NULL);
	WriteProcessMemory(p->h, (LPVOID) players[ME].za, (LPCVOID) &players[ME].z, sizeof(float), NULL);

	if (cmode == 1)
	{
		WriteProcessMemory(p->h, (LPVOID) cam.xa, (LPCVOID) &cam.x, sizeof(float), NULL);
		WriteProcessMemory(p->h, (LPVOID) cam.ya, (LPCVOID) &cam.y, sizeof(float), NULL);
	}
}
