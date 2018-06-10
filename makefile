#! /bin/bash
server: server.o init.o crfo.o
	gcc -o server server.o init.o crfo.o -lpthread



server.o: server.c header.h decl.h request.h 
	gcc -c server.c -lpthread
init.o: init.c header.h decl.h request.h 
	gcc -c init.c 
crfo.o: crfo.c header.h decl.h request.h 
	gcc -c crfo.c 

clean:
	rm *.o
	rm server

