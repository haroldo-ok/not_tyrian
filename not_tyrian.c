#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "SMSlib.h"
#include "PSGlib.h"
#include "gfx.h"

#define FOR_EACH_ENEMY(enm) enm = enemies; for (int i = 6; i; i--, enm++)
#define MAX_X (256 - 24)

typedef struct enemy {
	unsigned char type;
	unsigned char frame;
	int x, y;
	int spd_x;
} enemy;

const unsigned char base_tile_indexes[] = { 2, 8, 14, 20, 26 };
const unsigned char uship_tile_indexes[] = { 64, 70, 76, 82, 88, 94, 100, 106 };
const unsigned char enemy_tile_indexes[] = { 160, 166, 172, 178, 184, 178, 172, 166, 160 };

enemy enemies[6];
struct wave {
	unsigned char type;
	int x;
	unsigned char remaining;
} wave;

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

void initialize() {
	SMS_useFirstHalfTilesforSprites(true);
	SMS_setSpriteMode(SPRITEMODE_TALL);

	SMS_setBGPaletteColor(0, 0x04);	
	SMS_loadSpritePalette(player__palette__bin);
	SMS_loadPSGaidencompressedTiles(player__tiles__psgcompr, 2);
	SMS_loadPSGaidencompressedTiles(u_fighter__tiles__psgcompr, 64);
	SMS_loadPSGaidencompressedTiles(enemy__tiles__psgcompr, 160);
	SMS_setClippingWindow(0, 0, 255, 192);
	SMS_displayOn();
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
	wave.remaining = ((rand() >> 2) % 0x03) + 4;
	wave.type = ((rand() >> 2) & 0x01) + 1;
	wave.x = rand() % MAX_X;
}

void move_enemies() {
	enemy *enm;

	FOR_EACH_ENEMY(enm) {			
		enm->spd_x = (enm->y + 24) >> 6;
		
		enm->x += enm->spd_x;
		if (enm->x < 0) {
			enm->x = 0;
			enm->spd_x = -enm->spd_x;
		} else if (enm->x > MAX_X) {
			enm->x = MAX_X;
			enm->spd_x = -enm->spd_x;
		}
		
		enm->y++;
		if (enm->y > 192) {			
			if (!wave.remaining) {
				next_wave();
			}
		
			enm->type = wave.type;
			enm->x = wave.x;
			enm->y = -24;
			enm->frame = rand() % (8 << 2);
			
			wave.remaining--;
		}
	}
}

void draw_enemies() {
	enemy *enm;

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

void main(void) {
	unsigned char y = 160;
	unsigned char x = 0;
	unsigned char joy = 0;
	int tilt = 0;
	
	initialize();

	clear_enemies();

	while (true) {
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
				
		move_enemies();
				
		SMS_initSprites();

		draw_ship(x, y, base_tile_indexes[(tilt + (2 << 2)) >> 2], 30);
		
		draw_enemies();
		
		SMS_finalizeSprites();

		SMS_waitForVBlank();
		SMS_copySpritestoSAT();
	}

}

SMS_EMBED_SEGA_ROM_HEADER(9999,0); // code 9999 hopefully free, here this means 'homebrew'
SMS_EMBED_SDSC_HEADER(0,2, 2021,2,21, "Haroldo-OK\\2016", "Not Tyrian",
  "A very basic shoot-em-up.\n"
  "Built using devkitSMS & SMSlib - https://github.com/sverx/devkitSMS");
