#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <getopt.h>

#define die(x) { perror(x); exit(1); }

#include <x86emu.h>
#include "test.h"
#include "pci-userspace.h"

void x86emu_dump_xregs (void);
int int15_handler(void);
int int16_handler(void);
int int1A_handler(void);
#ifndef _PC
int int42_handler(void);
#endif
int intE6_handler(void);

void pushw(u16 val);

extern int teststart, testend;

_ptr p;
ptr current = 0;
unsigned char biosmem[1024*1024];

int verbose=0;


/* Interrupt multiplexer */

void do_int(int num)
{
    int ret = 0;

    /* This is a pInt leftover */
    current->num=num;

    switch (num) {
#ifndef _PC
    case 0x10:
    case 0x42:
    case 0x6D:
	ret = int42_handler();
	break;
#endif
    case 0x15:
	ret = int15_handler();
	break;
    case 0x16:
	ret = int16_handler();
	break;
    case 0x1A:
	ret = int1A_handler();
	break;
    case 0xe6:
	ret = intE6_handler();
	break;
    default:
	break;
    }

    if (!ret) {
	 printf("\nint%x: not implemented\n",num);
	 x86emu_dump_xregs();
    }
}

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


void usage(char *name)
{
	printf("Usage: %s [-c codesegment] [-s size] [-b base] [-i ip] [-t] <filename> ... \n",name); 
}

int main(int argc, char **argv)
{
	int i,c,trace=0;
	unsigned char *cp;
	char *filename;
	size_t size=0;
	int base=0;
	int have_size=0, have_base=0, have_ip=0, have_cs=0;
	int parse_rom=0;
	unsigned short initialip=0, initialcs=0, devfn=0;
	X86EMU_intrFuncs intFuncs[256];
	void X86EMU_setMemBase(void *base, size_t size);
	void x86emu_dump_xregs (void);

	const char *optstring="vh?b:i:c:s:tpd:";
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{ "verbose", 0, 0, 'v' },
			{ "help", 0, 0, 'h' },
			{ "trace", 0, 0, 't' },
			{ "base", 1, 0, 'b' },
			{ "instructionpointer", 1, 0, 'i' },
			{ "codesegment", 1, 0, 'c' },
			{ "size", 1, 0, 's' },
			{ "parserom", 0, 0, 'p' },
			{ "device", 1, 0, 'd' },
			{ 0, 0, 0, 0 }
		};
		c = getopt_long (argc, argv, optstring,
					long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 'v':
			verbose=1;
			break;
		case 'h':
		case '?':
			usage(argv[0]);
			return 0;
		case 't':
			trace=1;
			break;
		case 'b':
			base = strtol(optarg, 0, 0);
			have_base=1;
			break;
		case 'i':
			initialip=strtol(optarg, 0, 0);
			have_ip=1;
			break;
		case 'c':
			initialcs=strtol(optarg, 0, 0);
			have_cs=1;
			break;
		case 's':
			size=strtol(optarg, 0, 0);
			have_size=1;
			break;
		case 'p':
			printf("Parsing rom images not implemented.\n");
			parse_rom=1;
			break;
		case 'd':
			devfn=strtol(optarg, 0, 0);
			break;
		default:
			printf("Unknown option \n");
			usage(argv[0]);
			return 1;
		}
	}

	if (optind >= argc) {
		printf("Filename missing.\n");
		usage(argv[0]);
		return 1;
	}

	while (optind < argc) {
		printf("running file %s\n",argv[optind]);
		filename=argv[optind];
		optind++;
		/* normally we would do continue, but for
		 * now only one filename is supported.
		 */
		/* continue; */
		break;
	}

	if (!have_size) {
		printf("No size specified. defaulting to 32k\n");
		size=32*1024;
	}
	if (!have_base) {
		printf("No base specified. defaulting to 0xc0000\n");
		base=0xc0000;
	}
	if (!have_cs) {
		printf("No initial code segment specified. defaulting to 0xc000\n");
		initialcs=0xc000;
	}
	if (!have_ip) {
		printf("No initial instruction pointer specified. defaulting to 0x0003\n");
		initialip=0x0003;
	}

	current = &p;
	X86EMU_setMemBase(biosmem, sizeof(biosmem));
	X86EMU_setupPioFuncs(&myfuncs);

	if (iopl(3) < 0) 
		die("iopl");

	/* Emergency sync ;-) */
	sync();
	sync();

	/* Setting up interrupt environment.
	 * basically this means initializing PCI and
	 * intXX handlers.
	 */
	
	pciInit();

	for (i=0;i<256;i++)
		intFuncs[i] = do_int;
		X86EMU_setupIntrFuncs(intFuncs);
	cp = mapitin(filename, size);

	current->ax = devfn?devfn:0xff;
	current->dx = 0x80;
	//	current->ip = 0;
	for(i = 0; i < size; i++)
	  wrb(base + i, cp[i]);

	/* cpu setup */
	X86_AX	= devfn?devfn:0xff;
	X86_DX	= 0x80;
	X86_EIP = initialip;
	X86_CS	= initialcs;

	/* Initialize stack and data segment */
	X86_SS	= 0x0030;
	X86_DS	= 0x0040;
	X86_SP	= 0xfffe;

	/* We need a sane way to return from bios
	 * execution. A hlt instruction and a pointer
	 * to it, both kept on the stack, will do.
	 */
	pushw(0xf4f4);	/* hlt; hlt */
	pushw(X86_SS);
	pushw(X86_SP+2);

	X86_ES	= 0x0000;

	if (trace) {
		printf("Switching to single step mode.\n");
		X86EMU_trace_on();
	}

	X86EMU_exec();
	
	/* Cleaning up */
	pciExit();

	return 0;
}


