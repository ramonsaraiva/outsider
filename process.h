#ifndef PROCESS_H
#define PROCESS_H	1

#include <stdbool.h>

struct _process
{
	HWND window_handler;
	HANDLE handler;
	unsigned long pid;
	unsigned long base_address;

	HHOOK keyboard_hook;
	MSG msg;

	HDC hdc;
};

typedef struct _process process;

int process_init(process* p, const char* window_name);
bool process_hook(process* p, const char* window_name);
unsigned int process_get_base_address(process* p);

bool process_read(process* p, unsigned long address, void* buffer, unsigned long buffer_size);
unsigned int process_read_from_offsets(process* p, const unsigned long* offsets, const int offsets_count, void* buffer, unsigned long buffer_size);

bool process_write(process* p, unsigned long address, void* buffer, unsigned long buffer_size);

void process_keyboard_hook(process* p, HOOKPROC proc);
void process_keyboard_unhook(process* p);
void process_peek_message(process *p);

void process_get_window_size(process* p, int* width, int* height);

#endif
