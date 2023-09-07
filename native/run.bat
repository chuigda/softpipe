gcc softpipe.c -c -g -o softpipe.o
gcc sputnikw.c -c -g -o sputnik.o
gcc demo.c -c -g -o demo.o

gcc softpipe.o sputnik.o demo.o -lgdi32 -o demo.exe
demo.exe

