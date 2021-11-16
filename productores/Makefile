all: productor2 productor1 delivery_manager productor3
CFLAGS = -Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11 -g

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

delivery_manager: delivery_manager.o
			gcc $(CFLAGS) -o delivery delivery_manager.o -lrt

delivery_manager.o: delivery_manager.c
			gcc $(CFLAGS) -c delivery_manager.c


.PHONY: clean
clean:
		rm -fv *.o productor1 productor2 delivery productor3

