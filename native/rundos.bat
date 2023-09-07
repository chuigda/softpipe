gcc softpipe.c -c -g -o softpipe.o -Wall -Wextra
gcc sputnikd.c -c -g -o sputnik.o -Wall -Wextra
gcc demo.c -c -g -o demo.o -Wall -Wextra

gcc softpipe.o sputnik.o demo.o -lm -o demo.exe
demo.exe
