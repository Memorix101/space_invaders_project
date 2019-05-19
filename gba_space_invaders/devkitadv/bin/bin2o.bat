@echo off
rem by Jason Wilkins (fenix AT io DOT com)
rem I place this in the public domain.

echo SECTIONS { %1 : { *(.data) } } > bin2otmp.ls

objcopy -I binary -O elf32-little %2 bin2otmp.o

ld -T bin2otmp.ls bin2otmp.o -o %3

del bin2otmp.ls
del bin2otmp.o

