OBJS = jedec.o sst28sf040.o am29f040b.o mx29f002.c sst39sf020.o m29f400bt.o \
	w49f002u.o
OBJS += 82802ab.o
CC = gcc -O2 -g

all: ${OBJS}
	${CC} -o flash_rom flash_rom.c ${OBJS} -lpci
#	${CC} -o flash_on flash_on.c

clean:
	rm -f flash_rom flash_on *.o *~
