all : sws

sws : sws.o
        gcc -o sws sws.o 

sws.o : sws.c
        gcc -c sws/http.c sws.c

clean :
        rm test *.o