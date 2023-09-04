gcc softpipe.c -c -g -o softpipe.o
gcc sput.c -c -g -o sput.o
gcc main.win32.c -c -g -o main.win32.o

gcc softpipe.o sput.o main.win32.o -lgdi32 -o softpipe.win32.exe
softpipe.win32.exe