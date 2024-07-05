exec:
	gcc mips.c -c -g -lncurses
	gcc main.c mips.o -o exec -g -lm -lncurses

clear:
	rm mips.o
	rm exec
	clear

index: 
	rm mips.o
	rm exec
	gcc mips.c -c -g -lncurses
	gcc main.c mips.o -o exec -g -lm -lncurses


