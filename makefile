exec:
	gcc mips.c -c -g
	gcc main.c mips.o -o exec -g -lm
	./exec
	rm mips.o

clear:
	rm mips.o
	rm exec
	clear
