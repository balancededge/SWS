all: sws.c
    gcc -o sws sws.c

clean:
    $(RM) sws