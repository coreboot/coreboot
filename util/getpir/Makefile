# change to the path of your linuxbios tree
LINUXBIOSROOT=/users/rminnich//src/bios/freebios/

INCLUDEPATH=$(LINUXBIOSROOT)/src/arch/i386/include/arch
INCLUDE2=$(LINUXBIOSROOT)/src/include
INCLUDE3=$(LINUXBIOSROOT)/src/arch/i386/include


getpir: getpir.c
	gcc -o getpir -I$(INCLUDEPATH) -I$(INCLUDE2) getpir.c

checkpir: checkpir.c irq_tables.o
	gcc -o checkpir -I$(INCLUDEPATH) -I$(INCLUDE2) irq_tables.o checkpir.c

irq_tables.o: irq_tables.c  
	gcc -c -I$(INCLUDEPATH) -I$(INCLUDE2) -I$(INCLUDE3) irq_tables.c

clean:
	rm -f irq_table.o getpir checkpir

cleantable:
	rm -f irq_table.o

test:   checkpir
	./checkpir ;\
	exit 0;
