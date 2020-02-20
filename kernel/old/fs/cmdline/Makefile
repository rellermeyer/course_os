INCLUDES+=-Iinclude

CC?=gcc

CFLAGS+=-O0
CFLAGS+=-g
CFLAGS+=-D__HOST

OBJS+=file.o
OBJS+=fakelibs.o
OBJS+=open_table.o
OBJS+=bitvector.o
OBJS+=main.o

PROG:=buildfs

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROG)

file.o: ../fat16/file.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	
bitvector.o: ../../../src/ds
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	
open_table.o: ../open_table.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@	

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

build/card.sd:
	dd if=/dev/zero of=$@ conv=notrunc bs=512 count=250000

clean:
	rm -f $(OBJS)
	rm -f $(PROG)
