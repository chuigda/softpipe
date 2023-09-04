gcc softpipe.c -c -g -o softpipe.o
gcc sputnik.c -c -g -o sputnik.o
gcc main.win32.c -c -g -o main.win32.o

gcc softpipe.o sputnik.o main.win32.o -lgdi32 -o softpipe.win32.exe
softpipe.win32.exe