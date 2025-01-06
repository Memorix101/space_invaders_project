mkdir cd_root
elf2bin space-invaders-dc.elf
scramble space-invaders-dc.bin cd_root/1ST_READ.BIN
makeip -v IP.BIN
makedisc space-invaders-dreamcast.cdi cd_root IP.BIN space-invaders-dreamcast