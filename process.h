#ifndef PROCESS_H
#define PROCESS_H	1

#include <stdbool.h>

struct _process
{
	HWND window_handler;
	HANDLE handler;
	unsigned long pid;
	unsigned long base_address;
};

typedef struct _process process;

int process_init(process* p, const char* window_name);
bool process_hook(process* p, const char* window_name);
unsigned int process_get_base_address(process* p);

bool process_read(process* p, unsigned long address, void* buffer, unsigned long buffer_size);
unsigned int process_read_from_offsets(process* p, const unsigned long* offsets, const int offsets_count, void* buffer, unsigned long buffer_size);

bool process_write(process* p, unsigned long address, void* buffer, unsigned long buffer_size);

#endif
