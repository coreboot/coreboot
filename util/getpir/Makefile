# change to the path of your linuxbios tree
#LINUXBIOSROOT=/home/rminnich/src//freebios/
LINUXBIOSROOT=../..

INCLUDEPATH=$(LINUXBIOSROOT)/src/arch/i386/include
INCLUDE2=$(LINUXBIOSROOT)/src/include

getpir: getpir.c
	gcc -o getpir -I$(INCLUDEPATH) -I$(INCLUDE2) getpir.c

all: getpir checkpir

checkpir: checkpir.c irq_tables.o
	gcc -o checkpir -I$(INCLUDEPATH) -I$(INCLUDE2) irq_tables.o checkpir.c


irq_tables.o: irq_tables.c  
	gcc -c -I$(INCLUDEPATH) -I$(INCLUDE2) irq_tables.c

clean:
	rm -f irq_tables.o getpir checkpir *~

cleantable:
	rm -f irq_table.o

test:   checkpir
	./checkpir ;\
	exit 0;
