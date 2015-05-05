#ifndef OUTSIDER_H
#define OUTSIDER_H	1

struct _camera
{
	unsigned long base_address; // 0xE49EF4
	float x; // base + 0x68
	float y; // x + 0x04
};

struct _player
{
	unsigned long base_address; // 0xE55910, 0x58, 0x10, i * 0x260
	float x; // base + 0x1E8
	float y; // x - 0x08
	float z; // y - 0x1DC
	float stamina;

	bool flying;
	unsigned int color[3];
};

typedef struct _camera camera;
typedef struct _player player;

/*
 * camera
 */
camera cam;

void outsider_resolve_camera(process* p);
void outsider_read_camera(process* p);
void outsider_write_camera(process* p);

/*
 * player
 */
void outsider_resolve_players(process* p, player* players, int player_count);
void outsider_read_player(process* p, player* player);
void outsider_read_players(process* p, player* players, const int player_count);
void outsider_write_player(process* p, player* player);
void outsider_generate_colors(player* players, const int player_count);

#endif
