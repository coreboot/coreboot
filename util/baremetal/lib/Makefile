
ARCH = i386
CC = gcc

CFLAGS = -I ../include -I ../include/$(ARCH) -O2 

LB_SOURCE=../../../src

DEPS = printk.o serial_subr.o subr.o vsprintf.o memcpy.o malloc.o memset.o compute_ip_checksum.o elfboot.o boot.o ide.o linuxbios.o delay_tsc.o

baremetal.a : $(DEPS)
	ar -cr baremetal.a $(DEPS)	

compute_ip_checksum.o: $(LB_SOURCE)/lib/compute_ip_checksum.c
	$(CC) $(CFLAGS) -c $<

memcpy.o: $(LB_SOURCE)/lib/memcpy.c
	$(CC) $(CFLAGS) -c $<

vsprintf.o: $(LB_SOURCE)/lib/vsprintf.c
	$(CC) $(CFLAGS) -c $<

ide.o: $(LB_SOURCE)/pc80/ide/ide.c
	$(CC) $(CFLAGS) -c $<

memset.o: $(LB_SOURCE)/lib/memset.c
	$(CC) $(CFLAGS) -c $<

malloc.o: $(LB_SOURCE)/lib/malloc.c
	$(CC) $(CFLAGS) -c $<

printk.o: $(LB_SOURCE)/lib/printk.c
	$(CC) $(CFLAGS) -c $<

delay_tsc.o: $(LB_SOURCE)/cpu/p5/delay_tsc.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(DEPS) baremetal.a
