LINUXBIOS=../../
CFLAGS += -O2 -g
LDFLAGS += -g
INCLUDE=

spd_82801: spd_82801.o  pcibios.o

spd_82801.o: spd_82801.c
	cc -g -I. -O2  -c -o $@ $<

clean:
	rm -f *.o spd_82801
