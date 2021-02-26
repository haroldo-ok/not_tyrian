D:\Util\bmp2tile-0.5\BMP2Tile.exe gfx\png\player.png -noremovedupes -8x16 -palsms -fullpalette -savetiles "gfx\player (tiles).psgcompr" -savepalette "gfx\player (palette).bin"
D:\Util\bmp2tile-0.5\BMP2Tile.exe gfx\png\u_fighter.png -noremovedupes -8x16 -savetiles "gfx\u_fighter (tiles).psgcompr"
D:\Util\bmp2tile-0.5\BMP2Tile.exe gfx\png\enemy.png -noremovedupes -8x16 -savetiles "gfx\enemy (tiles).psgcompr"
D:\Util\bmp2tile-0.5\BMP2Tile.exe gfx\png\shot.png -noremovedupes -8x16 -savetiles "gfx\shot (tiles).psgcompr"
D:\Util\bmp2tile-0.5\BMP2Tile.exe gfx\png\tileset.png -noremovedupes -palsms -fullpalette -savetiles "gfx\tileset (tiles).psgcompr" -savepalette "gfx\tileset (palette).bin"
folder2c gfx gfx
sdcc -c -mz80 --peep-file peep-rules.txt gfx.c
sdcc -c -mz80 --peep-file peep-rules.txt not_tyrian.c
sdcc -o not_tyrian.ihx -mz80 --no-std-crt0 --data-loc 0xC000 crt0_sms.rel not_tyrian.rel gfx.rel SMSlib.lib PSGlib.rel
ihx2sms not_tyrian.ihx not_tyrian.sms