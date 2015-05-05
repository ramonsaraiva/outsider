#include <Windows.h>
#include <TlHelp32.h>

#include "process.h"
#include "outsider.h"

/*
 * camera
 */
void outsider_resolve_camera(process* p)
{
	process_read(p, p->base_address, &cam.base_address, 4);
}

void outsider_read_camera(process* p)
{
	process_read(p, cam.base_address + 0x68, &cam.x, 4);
	process_read(p, cam.base_address + 0x68 + 0x04, &cam.y, 4);
}

void outsider_write_camera(process* p)
{
	process_write(p, cam.base_address + 0x68, &cam.x, 4);
	process_write(p, cam.base_address + 0x68 + 0x04, &cam.y, 4);
}

/*
 * player
 */
void outsider_resolve_players(process* p, player* players, const int player_count)
{
	int i;
	unsigned long iter = 0;

	for (i = 0; i < player_count; i++)
	{
		const unsigned long offsets[] = {0xE55910, 0x58, 0x10};
		process_read_from_offsets(p, &offsets, 3, &players[i].base_address, 4);
		players[i].base_address += iter;
		iter += 0x260;
	}
}

void outsider_read_player(process* p, player* player)
{
	process_read(p, player->base_address + 0x1E8, &player->x, 4);
	process_read(p, player->base_address + 0x1E8 - 0x08, &player->y, 4);
	
	if (!player->flying)
		process_read(p, player->base_address + 0x1E8 - 0x08 - 0x1DC, &player->z, 4);
}

void outsider_read_players(process* p, player* players, const int player_count)
{
	int i;

	for (i = 0; i < player_count; i++)
		outsider_read_player(p, &players[i]);
}

void outsider_write_player(process* p, player* player)
{
	process_write(p, player->base_address + 0x1E8, &player->x, 4);
	process_write(p, player->base_address + 0x1E8 - 0x08, &player->y, 4);
	process_write(p, player->base_address + 0x1E8 - 0x08 - 0x1DC, &player->z, 4);
}

void outsider_generate_colors(player* players, const int player_count)
{
	int i;
	int j;

	for (i = 0; i < player_count; i++)
		for (j = 0; j < 3; j++)
			players[i].color[j] = rand() % 200;
}
