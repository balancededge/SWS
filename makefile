all : 
	gcc -o sws lib/http.c lib/file.c sws.c
	
clean :
	rm sws *.o