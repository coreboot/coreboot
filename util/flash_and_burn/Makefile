OBJS = jedec.o sst28sf040.o am29f040b.o mx29f002.c
CC = gcc -O2

all: ${OBJS}
	${CC} -o flash_rom flash_rom.c ${OBJS}
#	${CC} -o flash_on flash_on.c

clean:
	rm -f flash_rom flash_on *.o *~
