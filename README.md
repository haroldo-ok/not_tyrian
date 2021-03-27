Not Tyrian
==========

As it stands now, it's mostly a tech demo, using sprites from Tyrian.

Basically, I figured out that, since most of Tyrian player and enemy sprites are 24 pixels wide, and most of the shots are 16 pixels wide,
it should be possible to show a single enemy, plus the player, plus a single shot on a single scanline without flickering issues;
so as long as as the enemies move at the same speed, are spawned at intervals that prevent them from overlapping their Y cooordinates and are
restricted to only move horizontally after being spawned, and the same restrictions are applied to the shots, it should be possible to use them on a
Sega Master System without resizing, only reducing the color count. This is an experiment to test how it would look like. 

Originally made for the [SMS Power! devkitSMS Coding Competition 2016][1], then further expanded for the [SMS Power! Coding Competition 2020][2].

[1]: http://www.smspower.org/forums/16184-SMSPowerDevkitSMSCodingCompetition2016
[2]: https://www.smspower.org/forums/18393-CodingCompetition2021NotTyrianByHaroldoop
