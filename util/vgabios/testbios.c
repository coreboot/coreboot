#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#define die(x) { perror(x); exit(1); }

#include <x86emu.h>
#include "test.h"

_ptr p;
ptr current = 0;
extern int teststart, testend;
void
test()
{
  __asm__ __volatile__(".code16\nteststart:movb $4, %al\n outb %al, $0x80\n.code32\nhlt\ntestend:");
}

static void
do_int(int num) 
{
	setup_int(current);
	//	if(int_handler(current))
	  ; //X86EMU_Exec();
	finish_int(current);
}

unsigned char biosmem[1024*1024];


unsigned char *mapitin(char *file, size_t size)
{
  void * z;
  off_t offset;

  int fd = open(file, O_RDONLY, 0);

  if (fd < 0)
    die(file);
  offset = 0;
  z = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, offset);
  if (z == (void *) -1)
    die("mmap");
  close(fd);

  return z;

}

u8 x_inb(u16 port);
u16 x_inw(u16 port);
void x_outb(u16 port, u8 val);
void x_outw(u16 port, u16 val);
u32 x_inl(u16 port);
void x_outl(u16 port, u32 val);


X86EMU_pioFuncs myfuncs = {
        x_inb, x_inw, x_inl,
        x_outb, x_outw, x_outl
};


int
main(int argc, char **argv)
{
	int i;
	unsigned char *cp;
	char *filename;
	size_t size, howmuch;
	int base;
	unsigned short initialip, initialcs;
	X86EMU_intrFuncs intFuncs[256];
	void X86EMU_setMemBase(void *base, size_t size);
	void x86emu_dump_xregs (void);

	if (argc < 4) 
		die("Usage: testbios <file> <size> <base> <initial IP> <initial CS>");

	filename = argv[1];
	size = strtol(argv[2], 0, 0);
	base = strtol(argv[3], 0, 0);
	initialip = strtol(argv[4], 0, 0);
	initialcs = strtol(argv[5], 0, 0);

	current = &p;
	X86EMU_setMemBase(biosmem, sizeof(biosmem));
	X86EMU_setupPioFuncs(&myfuncs);

	if (iopl(3) < 0) 
		die("iopl");

	sync();
	sync();
	//	for (i=0;i<256;i++)
	//	intFuncs[i] = do_int;
	//	X86EMU_setupIntrFuncs(intFuncs);
	cp = mapitin(filename, size);

	current->ax = 0xff;
	current->dx = 0x80;
	//	current->ip = 0;
	for(i = 0; i < size; i++)
	  wrb(base + i, cp[i]);
	X86_EIP = initialip;
	X86_CS = initialcs;
	//	M.x86.saved_cs = 0;
	//	M.x86.saved_ip = 0;
	X86EMU_trace_on();
//	x86emu_single_step ();
	X86EMU_exec();
//	x86emu_single_step ();
	//x86emu_single_step ();
	//x86emu_single_step ();
	//x86emu_single_step ();
	


	return 0;
}


