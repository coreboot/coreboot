#Genmake v1.1 01/09/92  Copyright(c) 1990,'91,'92 by Kyle Saunders
CC= cc
CFLAGS= -g -O -I. -I../include
LFLAGS= -lc

TOUCH = /usr/bin/touch

#
install:	commlib.a

commlib.a:	disassm.o romhead.o
		-rm -f commlib.a
		ar cr commlib.a disassm.o romhead.o
		ar ts commlib.a >/dev/null

disassm.o: disassm.c
		$(CC) $(CFLAGS) -c disassm.c

romhead.o: romhead.c
		$(CC) $(CFLAGS) -c romhead.c

depend: disassm.c romhead.c
		makedepend -f $(MAKEFILE) -- $(CFLAGS) -- disassm.c romhead.c

clean:
		rm -f *.o *~ \#*\# *.bak core

# DO NOT DELETE THIS LINE -- make depend depends on it.

disassm.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
disassm.o: /usr/include/ctype.h /usr/include/features.h
disassm.o: /usr/include/sys/cdefs.h /usr/include/string.h
disassm.o: /usr//include/linux/stddef.h
disassm.o: /usr/include/stdlib.h /usr/include/errno.h
disassm.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
disassm.o: /usr/include/alloca.h
romhead.o: /usr/include/stdlib.h /usr/include/features.h
romhead.o: /usr/include/sys/cdefs.h
romhead.o: /usr/include/errno.h /usr/include/linux/errno.h
romhead.o: /usr/include/asm/errno.h
romhead.o: /usr/include/alloca.h
