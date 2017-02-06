all : 
	gcc -o sws lib/http.c sws.c
	
clean :
	rm sws *.o