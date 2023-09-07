gcc softpipe.c -c -g -o softpipe.o -Wall -Wextra
pause

gcc sputnikd.c -c -g -o sputnik.o -Wall -Wextra
pause

gcc demo.c -c -g -o demo.o -Wall -Wextra
pause

gcc softpipe.o sputnik.o demo.o -lm -o demo.exe
demo.exe
