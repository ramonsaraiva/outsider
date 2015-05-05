#include <string.h>
#include <stdbool.h>
#include <Windows.h>
#include <TlHelp32.h>

#include "process.h"

int process_init(process* p, const char* window_name)
{
	memset(p, 0, sizeof(process));
	if (!process_hook(p, window_name))
		return 1;
	if (!process_get_base_address(p))
		return 2;
	return 0;
}

bool process_hook(process* p, const char* window_name)
{
	p->window_handler = FindWindow(NULL, window_name);

	if (!p->window_handler)
		return false;

	GetWindowThreadProcessId(p->window_handler, &p->pid);
	p->handler = OpenProcess(PROCESS_ALL_ACCESS, FALSE, p->pid);

	if (!p->handler)
		return false;

	return true;
}

unsigned int process_get_base_address(process* p)
{
	MODULEENTRY32 module;
	module.dwSize = sizeof(MODULEENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, p->pid);

	if (snapshot == INVALID_HANDLE_VALUE)
		return -1;

	if (!Module32First(snapshot, &module))
		return -1;

	CloseHandle(snapshot);
	p->base_address = module.modBaseAddr;
	return p->base_address;
}

bool process_read(process* p, unsigned long address, void* buffer, unsigned long buffer_size)
{
	return ReadProcessMemory(p->handler, (LPCVOID) address, buffer, buffer_size, NULL);
}

unsigned int process_read_from_offsets(process* p, const unsigned long* offsets, const int offsets_count, void* buffer, unsigned long buffer_size)
{
	unsigned long current_address;
	int i;

	current_address = p->base_address;

	for (i = 0; i < offsets_count - 1; i++)
	{
		process_read(p, current_address + offsets[i], &current_address, 4);
	}

	process_read(p, current_address + offsets[offsets_count - 1], buffer, buffer_size);

	return current_address + offsets[offsets_count - 1];
}

bool process_write(process* p, unsigned long address, void* buffer, unsigned long buffer_size)
{
	return WriteProcessMemory(p->handler, (LPVOID) address, (LPCVOID) buffer, buffer_size, NULL);
}

int process_create_window(process* p)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return 1;

	if (!(p->graphics.window = SDL_CreateWindowFrom(p->window_handler)))
		return 2;

	SDL_GetWindowSize(p->graphics.window, &p->graphics.width, &p->graphics.height);

	p->graphics.screen = SDL_GetWindowSurface(p->graphics.window);
	p->graphics.renderer = SDL_CreateRenderer(p->graphics.window, -1, SDL_RENDERER_ACCELERATED);

	if (!p->graphics.renderer)
		return 3;

	SDL_SetColorKey(p->graphics.screen, SDL_TRUE, SDL_MapRGB(p->graphics.screen->format, 255, 0, 255));
	//SDL_SetSurfaceAlphaMod(screen, 255);

	return 0;
}

void process_keyboard_hook(process* p, HOOKPROC proc)
{
	p->keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, proc, 0, 0);
}

void process_keyboard_unhook(process* p)
{
	UnhookWindowsHookEx(p->keyboard_hook);
}

void process_peek_message(process* p)
{
	while (PeekMessage(&p->msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&p->msg);
		DispatchMessage(&p->msg);
	}
}
