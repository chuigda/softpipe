gcc softpipe.c -c -g -o softpipe.o -Wall -Wextra
gcc sputnikw.c -c -g -o sputnik.o -Wall -Wextra
gcc demo.c -c -g -o demo.o -Wall -Wextra

gcc softpipe.o sputnik.o demo.o -lgdi32 -o demo.exe
demo.exe
