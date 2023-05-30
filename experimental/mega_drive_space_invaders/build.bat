SET SGDK_PATH=C:\Users\memor\Desktop\sgdk180\
rmdir /s/q %cd%\out
%SGDK_PATH%\bin\make -f %SGDK_PATH%\makefile.gen
#pause
%cd%\GensKModv0.7.3\gens.exe %cd%/out/rom.bin