# change to the path of your linuxbios tree
#LINUXBIOSROOT=/home/rminnich/src//freebios/
LINUXBIOSROOT=../..

INCLUDEPATH=$(LINUXBIOSROOT)/src/arch/i386/include
INCLUDE2=$(LINUXBIOSROOT)/src/include

getpir: getpir.c checksum.o code_gen.o
	gcc -o getpir -I$(INCLUDEPATH) -I$(INCLUDE2) getpir.c checksum.o code_gen.o

code_gen.o: code_gen.c
	gcc -c -I$(INCLUDEPATH) -I$(INCLUDE2) code_gen.c

all: getpir checkpir

checkpir: checkpir.c checksum.o irq_tables.o
	gcc -o checkpir -I$(INCLUDEPATH) -I$(INCLUDE2) irq_tables.o checksum.o checkpir.c

checksum.o: checksum.c
	gcc -c -I$(INCLUDEPATH) -I$(INCLUDE2) checksum.c 

irq_tables.o: irq_tables.c  
	gcc -c -I$(INCLUDEPATH) -I$(INCLUDE2) irq_tables.c

clean:
	rm -f getpir checkpir *.o *~

cleantable:
	rm -f irq_table.o

test:   checkpir
	./checkpir ;\
	exit 0;
