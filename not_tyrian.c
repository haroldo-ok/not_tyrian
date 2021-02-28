#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "SMSlib.h"
#include "PSGlib.h"
#include "gfx.h"

#define FOR_EACH_ENEMY(enm) enm = enemies; for (int i = 6; i; i--, enm++)
#define FOR_EACH_SHOT(sht) sht = shots; for (int i = 6; i; i--, sht++)
#define MAX_X (256 - 24)
#define MIDDLE_X (MAX_X >> 1)

typedef struct enemy {
	unsigned char type;
	unsigned char movement;
	unsigned char frame;
	int x, y;
	int spd_x;
	int dir_x;
	int min_x, max_x;
} enemy;

typedef struct shot {
	int x, y;
	bool active;
} shot;

const unsigned char base_tile_indexes[] = { 2, 8, 14, 20, 26 };
const unsigned char uship_tile_indexes[] = { 64, 70, 76, 82, 88, 94, 100, 106 };
const unsigned char enemy_tile_indexes[] = { 160, 166, 172, 178, 184, 178, 172, 166, 160 };

enemy enemies[6];
struct wave {
	unsigned char type;
	unsigned char movement;
	int x;
	unsigned char remaining;
} wave;

shot shots[6];
char shot_delay;

int level_timer;

bool boss_mode;
struct boss {
	int x, y;
	int target_x, target_y;	
	int change_delay;
} boss;

void draw_ship(unsigned char x, unsigned char y, unsigned char base_tile, unsigned char line_incr) {
	SMS_addSprite(x, y, base_tile);
	SMS_addSprite(x + 8, y, base_tile + 2);
	SMS_addSprite(x + 16, y, base_tile + 4);

	base_tile += line_incr;
	y += 16;
	SMS_addSprite(x, y, base_tile);
	SMS_addSprite(x + 8, y, base_tile + 2);
	SMS_addSprite(x + 16, y, base_tile + 4);
}

void draw_tile(unsigned char x, unsigned char y, unsigned int base_tile) {
	unsigned int tiles[4];
	int n = base_tile;
	
	tiles[0] = n++;
	tiles[1] = n++;
	tiles[2] = n++;
	tiles[3] = n++;
	
	SMS_loadTileMapArea(x, y, tiles, 2, 2);
}

void draw_tiles() {
	for (char i = 0; i != 16; i++) {
		for (char j = 0; j != 14; j++) {
			draw_tile(i << 1, j << 1, 256);
		}
	}
}

void initialize() {	
	SMS_useFirstHalfTilesforSprites(true);
	SMS_setSpriteMode(SPRITEMODE_TALL);

	SMS_displayOff();
}

void initialize_gameplay() {			
	SMS_zeroBGPalette();

	SMS_setBGScrollX(0);
	SMS_setBGScrollY(0);
	
	SMS_loadPSGaidencompressedTiles(player__tiles__psgcompr, 2);
	SMS_loadPSGaidencompressedTiles(u_fighter__tiles__psgcompr, 64);
	SMS_loadPSGaidencompressedTiles(enemy__tiles__psgcompr, 160);
	SMS_loadPSGaidencompressedTiles(shot__tiles__psgcompr, 224);
	SMS_loadPSGaidencompressedTiles(tileset__tiles__psgcompr, 256);
	SMS_setClippingWindow(0, 0, 255, 192);

	draw_tiles();
	
	SMS_setBGPaletteColor(0, 0x04);	
	SMS_loadSpritePalette(player__palette__bin);
	SMS_loadBGPalette(tileset__palette__bin);

	SMS_displayOn();

	boss_mode = false;
	level_timer = 2048;
}

void move_boss() {
	if (!boss.change_delay) {
		boss.target_x = abs(rand()) % (256 - 113);
		boss.target_y = (abs(rand()) % (192 - 105));
		boss.change_delay = 128;
	}
	boss.change_delay--;
	
	if (boss.x > boss.target_x) {
		boss.x--;
	} else if (boss.x < boss.target_x) {
		boss.x++;
	}

	if (boss.y > boss.target_y) {
		boss.y--;
	} else if (boss.y < boss.target_y) {
		boss.y++;
	}
}

void draw_boss() {
	if (!boss_mode) return;
	
	SMS_setBGScrollX(boss.x);
	SMS_setBGScrollY(223 - boss.y);
}

void initialize_boss() {
	unsigned int row[32];	
	unsigned int *o = boss__tilemap__bin;	
	
	boss.x = (256 - 113) >> 1;
	boss.y = 0;
	boss.change_delay = 0;
	level_timer = 1024;
	
	SMS_zeroBGPalette();
	SMS_loadPSGaidencompressedTiles(boss__tiles__psgcompr, 256);
	
	for (unsigned char y = 0; y != 28; y++) {
		unsigned int *d = row;
		for (unsigned char x = 0; x != 32; x++) {
			*d = *o + 256;
			SMS_loadTileMap(0, y, row, 64);
			o++; d++;
		}		
	}
	
	boss_mode = true;
	draw_boss();

	SMS_loadBGPalette(boss__palette__bin);		
}

void clear_enemies() {
	enemy *enm;
	
	wave.remaining = 0;

	FOR_EACH_ENEMY(enm) {
		enm->type = 0;
		enm->x = 0;
		enm->y = i << 5;
		//enm->spd_x = 2;
	}
}

void next_wave() {
	if (wave.type) {
		// Pause between attacks
		wave.type = 0;
		wave.remaining = ((rand() >> 4) % 0x03) + 1;
	} else {
		// Actual enemies
		wave.type = ((rand() >> 4) & 0x01) + 1;
		wave.remaining = ((rand() >> 4) & 0x03) + 4;
	}
	wave.x = rand() % MAX_X;
	wave.movement = (rand() >> 4) & 0x01;
}

void move_enemies() {
	enemy *enm;

	FOR_EACH_ENEMY(enm) {			
		if (enm->movement) {			
			enm->spd_x = (enm->y + 24) >> 6;
			if (enm->dir_x < 0) {
				enm->spd_x = -enm->spd_x;
			}
		}
		
		enm->x += enm->spd_x;
		
		if (enm->x < enm->min_x) {
			enm->x = enm->min_x;
			enm->spd_x = -enm->spd_x;
		} else if (enm->x > enm->max_x) {
			enm->x = enm->max_x;
			enm->spd_x = -enm->spd_x;
		}
		
		enm->y++;
		if (enm->y > 192) {			
			if (!wave.remaining) {
				next_wave();
			}
		
			enm->type = wave.type;
			enm->movement = wave.movement;
			enm->x = wave.x;
			enm->y = -24;
			enm->frame = rand() % (8 << 2);
			enm->dir_x = enm->x > MIDDLE_X ? -1 : 1;
			
			if (wave.movement) {
				enm->min_x = 0;
				enm->max_x = MAX_X;
			} else if (!wave.movement) {
				if (wave.x > MIDDLE_X) {
					enm->max_x = wave.x;
					enm->min_x = enm->max_x - MIDDLE_X;
				} else {
					enm->min_x = wave.x;
					enm->max_x = enm->min_x + MIDDLE_X;
				}
				
				if (wave.remaining & 0x01) {
					enm->x = enm->min_x;
					enm->spd_x = 2;
				} else {
					enm->x = enm->max_x;
					enm->spd_x = -2;
				}
			}
			
			wave.remaining--;
		}
	}
}

void draw_enemies() {
	enemy *enm;

	// Do not draw enemies in boss mode
	if (boss_mode) return;
	
	FOR_EACH_ENEMY(enm) {
		if (enm->type == 1) {
			enm->frame++;
			draw_ship(enm->x, enm->y, uship_tile_indexes[(enm->frame >> 2) & 0x07], 48);
		} else if (enm->type == 2) {
			enm->frame++;
			if ((enm->frame >> 2) > 8) enm->frame = 0;
			draw_ship(enm->x, enm->y, enemy_tile_indexes[(enm->frame >> 2)], 30);
		}
	}
}

void clear_shots() {
	shot *sht;

	FOR_EACH_SHOT(sht) {
		sht->x = 0;
		sht->y = 0;
		sht->active = false;
	}
	
	shot_delay = 0;
}

void move_shots() {
	shot *sht;

	if (shot_delay) shot_delay--;
	
	FOR_EACH_SHOT(sht) {
		sht->y -= 3;
		if (sht->y < -16) sht->active = false;
	}	
}

void draw_shots() {
	shot *sht;

	FOR_EACH_SHOT(sht) {
		if (sht->active) {
			SMS_addSprite(sht->x, sht->y, 224);
			SMS_addSprite(sht->x + 8, sht->y, 226);
		}
	}
}

void fire_shot(unsigned char x, int y) {
	if (shot_delay) {
		return;
	}
	
	shot *sht;	
	FOR_EACH_SHOT(sht) {
		if (!sht->active) {
			sht->x = x + 4;
			sht->y = y - 12;
			sht->active = true;
			shot_delay = 15;
			return;
		}
	}
}

void title_screen() {
	unsigned char joy = 0;
	int y = 24 << 2;
	
	SMS_displayOff();
	
	SMS_loadBGPalette(title__palette__bin);
	SMS_loadPSGaidencompressedTiles(title__tiles__psgcompr, 0);
	SMS_loadTileMap(0, 0, title__tilemap__bin, title__tilemap__bin_size);
	
	SMS_displayOn();

	while (!(joy & (PORT_A_KEY_1 | PORT_A_KEY_2))) {
		if (y) {
			y--;
			SMS_setBGScrollY(224 - (y >> 2));
		}
		
		joy = SMS_getKeysStatus();
		SMS_waitForVBlank();
	}
}

void draw_player(unsigned char x, unsigned char y, int tilt) {
	draw_ship(x, y, base_tile_indexes[(tilt + (2 << 2)) >> 2], 30);
}

void level_end(unsigned char x, unsigned char y) {
	int prev_y[32];
	char phase = 2;
	
	prev_y[0] = y;
	for (int i = 31; i; i--) {
		prev_y[i] = -32;
	}
	
	while (phase) {
		for (int i = 31; i; i--) {
			prev_y[i] = prev_y[i - 1];
		}
	
		if (phase == 2) {
			prev_y[0] += 3;
			if (prev_y[0] > (192 - 24)) {
				phase = 1;
			}
		} else {
			prev_y[0] -= 3;
			if (prev_y[0] < -64) {
				prev_y[0] = 0;
				phase = 0;
			}
		}			
		
		SMS_initSprites();
		
		for (int i = 0; i < 32; i += 4) {
			draw_player(x, prev_y[i], 0);
		}
		
		SMS_finalizeSprites();
		
		SMS_waitForVBlank();
		SMS_copySpritestoSAT();
	}
}

void main(void) {
	unsigned char x = 0;
	unsigned char y = 160;
	unsigned char joy = 0;
	unsigned char scroll_y = 0;
	int tilt = 0;
	
	title_screen();

	initialize();
	initialize_gameplay();
	
	clear_enemies();
	clear_shots();
		
	while (true) {
		if (!level_timer) {
			if (boss_mode) {
				level_end(x, y);
				initialize_gameplay();
			} else {
				initialize_boss();
			}
		}
		level_timer--;
		
		joy = SMS_getKeysStatus();
		
		if (joy & PORT_A_KEY_LEFT) {
			if (x) x -= 2;
			if (tilt > -(2 << 2) + 1) tilt--;
		} else if (joy & PORT_A_KEY_RIGHT) {
			if (x < MAX_X) x += 2;
			if (tilt < (2 << 2) - 1) tilt++;
		} else {
			if (tilt < 0) tilt++;
			if (tilt > 0) tilt--;
		}
		
		if (joy & PORT_A_KEY_UP) {
			if (y) y -= 2;
		} else if (joy & PORT_A_KEY_DOWN) {
			if (y < (192 - 32)) y += 2;
		}
		
		if (joy & (PORT_A_KEY_1 | PORT_A_KEY_2)) {
			fire_shot(x, y);
		}

		move_shots();				
		move_enemies();
		move_boss();
		
		scroll_y--;
		if (scroll_y > 223) scroll_y = 223;
			
				
		SMS_initSprites();

		draw_player(x, y, tilt);
		
		draw_enemies();
		draw_shots();
		
		SMS_finalizeSprites();

		SMS_waitForVBlank();
		SMS_copySpritestoSAT();
		
		if (!boss_mode) {
			SMS_setBGScrollY(scroll_y);
		}
		draw_boss();
	}

}

SMS_EMBED_SEGA_ROM_HEADER(9999,0); // code 9999 hopefully free, here this means 'homebrew'
SMS_EMBED_SDSC_HEADER(0,2, 2021,2,25, "Haroldo-OK\\2021", "Not Tyrian",
  "A very basic shoot-em-up.\n"
  "Built using devkitSMS & SMSlib - https://github.com/sverx/devkitSMS");
