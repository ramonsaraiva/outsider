#ifndef PROCESS_H
#define PROCESS_H	1

#include <stdbool.h>
#include <SDL2/SDL.h>

struct _sdl_window
{
	SDL_Window* window;
	SDL_Surface* screen;
	SDL_Renderer* renderer;

	int width;
	int height;
};

typedef struct _sdl_window sdl_window;

struct _process
{
	HWND window_handler;
	HANDLE handler;
	unsigned long pid;
	unsigned long base_address;

	HHOOK keyboard_hook;
	MSG msg;

	sdl_window graphics;
};

typedef struct _process process;

int process_init(process* p, const char* window_name);
bool process_hook(process* p, const char* window_name);
unsigned int process_get_base_address(process* p);

bool process_read(process* p, unsigned long address, void* buffer, unsigned long buffer_size);
unsigned int process_read_from_offsets(process* p, const unsigned long* offsets, const int offsets_count, void* buffer, unsigned long buffer_size);

bool process_write(process* p, unsigned long address, void* buffer, unsigned long buffer_size);

int process_create_window(process* p);

void process_keyboard_hook(process* p, HOOKPROC proc);
void process_keyboard_unhook(process* p);
void process_peek_message(process *p);

#endif
