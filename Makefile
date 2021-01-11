all: tac

tac: tac.o
#	 gcc -O -fsanitize=address -g -o tac tac.o
	 gcc -g -o tac tac.o

tac.o: tac.c
#	 gcc -O -g -c -fsanitize=address tac.c
	 gcc -g -c tac.c
     
clean:
	 rm tac.o tac
   