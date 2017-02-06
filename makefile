all : 
	gcc -o sws lib/http.c lib/file.c lib/show.c sws.c
	
clean :
	rm sws *.o