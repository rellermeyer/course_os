all: card.sd
	gcc -g -c main.c
	gcc -g -c fakelibs.c
	gcc -g -c -I. ../fat16/file.c -o file.o
	gcc -g -c -I. ../open_table.c -o open_table.o
	gcc -g -c -I. ../../data_structures/bitvector.c -o bv.o
	gcc -g main.o fakelibs.o file.o open_table.o bv.o

card.sd:
	dd if=/dev/zero of=card.sd conv=notrunc bs=4k count=32768

clean:
	rm *.o
