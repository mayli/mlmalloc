CC=gcc
CFLAGS+=-Wno-deprecated-declarations -Wno-deprecated

all: victim

victim: victim.c mlmalloc
	$(CC) -o victim victim.c mlmalloc.o

mlmalloc: mlmalloc.o
	$(CC) -c mlmalloc.c

run: victim
	./victim

clean:
	rm victim *.o