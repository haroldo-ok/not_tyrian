#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "SMSlib.h"
#include "PSGlib.h"
#include "gfx.h"

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
	
	SMS_useFirstHalfTilesforSprites(true);
	SMS_setSpriteMode(SPRITEMODE_TALL);

	SMS_loadSpritePalette(player__palette__bin);
	SMS_loadPSGaidencompressedTiles(player__tiles__psgcompr, 2);
	SMS_setClippingWindow(0, 0, 255, 192);
	SMS_displayOn();
	
	while (true) {
		joy = SMS_getKeysStatus();
		
		if (joy & PORT_A_KEY_LEFT) {
			x--;
		}
		
		if (joy & PORT_A_KEY_RIGHT) {
			x++;
		}
		
		SMS_initSprites();

		draw_ship(8, 8, 2, 30);
		draw_ship(8, 40, 8, 30);
		draw_ship(8, 72, 14, 30);

		draw_ship(x, 160, 14, 30);

		SMS_finalizeSprites();

		SMS_waitForVBlank();
		SMS_copySpritestoSAT();
	}

}

SMS_EMBED_SEGA_ROM_HEADER(9999,0); // code 9999 hopefully free, here this means 'homebrew'
SMS_EMBED_SDSC_HEADER(0,1, 2016,3,20, "Haroldo-OK\\2016", "Not Tyrian",
  "A very basic shoot-em-up.\n"
  "Built using devkitSMS & SMSlib - https://github.com/sverx/devkitSMS");
