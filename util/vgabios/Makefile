CFLAGS=-Ix86emu/include -O2 -g
all: testme testbios

testbios: testbios.o helper_exec.o x86emu/src/x86emu/libx86emu.a 
	cc -o testbios testbios.o helper_exec.o  x86emu/src/x86emu/libx86emu.a 
testme: test.o helper_exec.o x86emu/src/x86emu/libx86emu.a 
	cc -o testme test.o helper_exec.o  x86emu/src/x86emu/libx86emu.a 
test.o: test.c test.h
helper_exec.o: helper_exec.c test.h

clean:
	rm -f *.o testme *~ testbios
