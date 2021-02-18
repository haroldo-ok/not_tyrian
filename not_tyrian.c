#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "SMSlib.h"
#include "PSGlib.h"
#include "gfx.h"

const unsigned char base_tile_indexes[] = { 2, 8, 14, 20, 26 };
const unsigned char uship_tile_indexes[] = { 64, 70, 76, 82, 88, 94, 100, 106 };
const unsigned char enemy_tile_indexes[] = { 160, 166, 172, 178, 184, 178, 172, 166, 160 };

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

void main(void) {
	unsigned char x = 0;
	unsigned char joy = 0;
	int tilt = 0;
	unsigned char uship_frame = 0;
	unsigned char enemy_frame = 0;
	
	SMS_useFirstHalfTilesforSprites(true);
	SMS_setSpriteMode(SPRITEMODE_TALL);

	SMS_setBGPaletteColor(0, 0x04);	
	SMS_loadSpritePalette(player__palette__bin);
	SMS_loadPSGaidencompressedTiles(player__tiles__psgcompr, 2);
	SMS_loadPSGaidencompressedTiles(u_fighter__tiles__psgcompr, 64);
	SMS_loadPSGaidencompressedTiles(enemy__tiles__psgcompr, 160);
	SMS_setClippingWindow(0, 0, 255, 192);
	SMS_displayOn();
	
	while (true) {
		joy = SMS_getKeysStatus();
		
		if (joy & PORT_A_KEY_LEFT) {
			if (x) x -= 2;
			if (tilt > -(2 << 2) + 1) tilt--;
		} else if (joy & PORT_A_KEY_RIGHT) {
			if (x < (256 - 24)) x += 2;
			if (tilt < (2 << 2) - 1) tilt++;
		} else {
			if (tilt < 0) tilt++;
			if (tilt > 0) tilt--;
		}
		
		uship_frame++;
		enemy_frame++;
		if ((enemy_frame >> 2) > 8) enemy_frame = 0;
		
		SMS_initSprites();

		draw_ship(48, 8, uship_tile_indexes[(uship_frame >> 2) & 0x07], 48);
		draw_ship(256 - 48 - 24, 8, enemy_tile_indexes[(enemy_frame >> 2)], 30);

		draw_ship(x, 160, base_tile_indexes[(tilt + (2 << 2)) >> 2], 30);

		SMS_finalizeSprites();

		SMS_waitForVBlank();
		SMS_copySpritestoSAT();
	}

}

SMS_EMBED_SEGA_ROM_HEADER(9999,0); // code 9999 hopefully free, here this means 'homebrew'
SMS_EMBED_SDSC_HEADER(0,1, 2016,3,20, "Haroldo-OK\\2016", "Not Tyrian",
  "A very basic shoot-em-up.\n"
  "Built using devkitSMS & SMSlib - https://github.com/sverx/devkitSMS");
