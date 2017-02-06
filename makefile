all : 
	gcc -o sws lib/http.c lib/file.c lib/show.c lib/util.c sws.c
	
clean :
	rm sws *.o