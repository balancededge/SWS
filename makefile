all: sws.c
	gcc -g -Wall -o sws sws.c

clean: 
	$(RM) sws