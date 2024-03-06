all: client client_both server server2

clean:
	rm -f client client_both server server2

client: client.c
	gcc client.c -o client 

client_both: client_both.c
	gcc client_both.c -o client_both -lm

server: server.c
	gcc server.c -o server

server2: server2.c 
	gcc server2.c -o server2
