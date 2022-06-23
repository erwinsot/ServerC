all: server cliente ejecutarS ejecutarC
server: server.c
	gcc server.c -o server
			
cliente: clienteVic.c
	gcc clienteVic.c -o cliente

ejecutarS:
	./server
ejecutarC:
	./cliente
