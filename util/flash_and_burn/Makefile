OBJS = jedec.o sst28sf040.o am29f040b.o mx29f002.c sst39sf020.o m29f400bt.o \
	w49f002u.o 82802ab.o msys_doc.o pm49fl004.o
CC = gcc -O2 -g -Wall -Werror

all: flash_rom flash_on

flash_rom: flash_rom.o ${OBJS}
	${CC} -o flash_rom flash_rom.c ${OBJS} -lpci

flash_on: flash_on.c
	${CC} -o flash_on flash_on.c

clean:
	rm -f flash_rom flash_on *.o *~

flash_rom.o: flash_rom.c flash.h jedec.h \
	82802ab.h am29f040b.h m29f400bt.h msys_doc.h mx29f002.h sst28sf040.h \
	sst39sf020.h w49f002u.h
flash_on.o: flash_on.c

82802ab.o:    82802ab.c    82802ab.h            flash.h
am29f040b.o:  am29f040b.c  am29f040b.h  jedec.h flash.h
m29f400bt.o:  m29f400bt.c  m29f400bt.h          flash.h
msys_doc.o:   msys_doc.c   msys_doc.h           flash.h
mx29f002.o:   mx29f002.c   mx29f002.h   jedec.h flash.h
sst28sf040.o: sst28sf040.c sst28sf040.h jedec.h flash.h
sst39sf020.o: sst39sf020.c sst39sf020.h jedec.h flash.h
w49f002u.o:   w49f002u.c   w49f002u.h   jedec.h flash.h
pm49fl004.o:   pm49fl004.c   pm49fl004.h   jedec.h flash.h

jedec.o:      jedec.c                   jedec.h flash.h
