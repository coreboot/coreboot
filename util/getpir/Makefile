# change to the path of your linuxbios tree
#LINUXBIOSROOT=/home/rminnich/src//freebios/
LINUXBIOSROOT=../..

INCLUDEPATH=$(LINUXBIOSROOT)/src/arch/i386/include/arch
INCLUDE2=$(LINUXBIOSROOT)/src/include
INCLUDE3=$(LINUXBIOSROOT)/src/arch/i386/include


getpir: getpir.c
	gcc -o getpir -I$(INCLUDEPATH) -I$(INCLUDE2) getpir.c

checkpir: checkpir.c irq_tables.o
	gcc -o checkpir -I$(INCLUDEPATH) -I$(INCLUDE2) irq_tables.o checkpir.c

assignirq: assignirq.c irq_tables.o
	gcc -g -o assignirq -I$(INCLUDEPATH) -I$(INCLUDE2) assignirq.c -lpci

irq_tables.o: irq_tables.c  
	gcc -c -I$(INCLUDEPATH) -I$(INCLUDE2) -I$(INCLUDE3) irq_tables.c

clean:
	rm -f irq_table.o getpir checkpir assignirq

cleantable:
	rm -f irq_table.o

test:   checkpir
	./checkpir ;\
	exit 0;
