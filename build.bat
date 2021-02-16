folder2c gfx gfx
sdcc -c -mz80 --peep-file peep-rules.txt not_tyrian.c
sdcc -o not_tyrian.ihx -mz80 --no-std-crt0 --data-loc 0xC000 crt0_sms.rel not_tyrian.rel SMSlib.lib PSGlib.rel
ihx2sms not_tyrian.ihx not_tyrian.sms