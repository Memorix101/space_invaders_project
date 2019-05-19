@path=.\devkitadv\bin

@REM Clean binaries
@cls
@del *.o
@del *.elf
@del *.gba

@REM Compile game
gcc -c -O3 main.c

@REM link to smgbalib
gcc -o main.elf main.o smgbalib.a

@REM Translate executable to GBA rom format
objcopy -O binary main.elf space-invaders.gba

@REM Clean binaries
@del *.o
@del *.elf
