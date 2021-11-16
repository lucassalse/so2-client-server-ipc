all: productor2 productor1 server productor3 cliente1 cliente3 cli
CFLAGS = -Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11 -g

server: server.o funciones.o
	gcc $(CFLAGS) -o server server.o funciones.o -lrt -lrt -lcrypto -lzip

server.o: server.c funciones.h
	gcc $(CFLAGS) -c server.c

#funciones.o: funciones.c funciones.h funciones.h
#	gcc $(CFLAGS) -c funciones.c

funciones.o: funciones.c funciones.h
	gcc $(CFLAGS) -c funciones.c -lssl -lcrypto -lzip

productor1: productor1.o
			gcc $(CFLAGS) -o productor1 productor1.o -lrt

productor1.o: productor1.c
			gcc $(CFLAGS) -c productor1.c

productor2: productor2.o
			gcc $(CFLAGS) -o productor2 productor2.o -lrt

productor2.o: productor2.c
			gcc $(CFLAGS) -c productor2.c

productor3: productor3.o
			gcc $(CFLAGS) -o productor3 productor3.o -lrt

productor3.o: productor3.c
			gcc $(CFLAGS) -c productor3.c

cliente1: cliente.o
			gcc $(CFLAGS) -o cliente1 cliente.o -lssl -lcrypto

cliente.o: cliente.c
			gcc $(CFLAGS) -c cliente.c

cliente3: cliente3.o
			gcc $(CFLAGS) -o cliente3 cliente3.o -lssl -lcrypto

cliente3.o: cliente3.c
			gcc $(CFLAGS) -c cliente3.c

cli: cli.o
			gcc $(CFLAGS) -o cli cli.o -lrt

cli.o: cli.c
			gcc $(CFLAGS) -c cli.c


clean:
	rm -f myshell *.o *.txt productor1 productor2 server productor3 cliente1 cliente3 cli