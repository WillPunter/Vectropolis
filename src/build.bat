@echo off
echo Building...
windres res.rc -o resource
gcc main.c resource -mwindows -lgdi32 -lxaudio2_8 -lole32 -o ./../build/Vectropolis
echo Completed
pause