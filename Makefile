SHELL=C:/Windows/System32/cmd.exe
CC	= sdcc -mz80

BINS	= not_tyrian.sms


all:	$(BINS)

gfx.c: gfx/font.fnt gfx/skull.txt
	folder2c gfx gfx

not_tyrian.rel: gfx.c

%.rel:	%.c
	$(CC) -c --std-sdcc99 $< -o $@

%.sms:	%.ihx
	ihx2sms $< $@

not_tyrian.ihx:	SMSlib/crt0_sms.rel not_tyrian.rel SMSlib/SMSlib.lib gfx.rel
	$(CC) --no-std-crt0 --data-loc 0xC000 -o not_tyrian.ihx $^

clean:
	rm -f *.ihx *.lk *.lst *.map *.noi *.rel *.sym *.asm *.sms gfx.*
