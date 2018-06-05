/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <stdarg.h>
#include <stdtypes.h>
#define die(x) { perror(x); exit(1); }
#define warn(x) { perror(x);  }

#include <x86emu/x86emu.h>
#include <console/console.h>
#include <arch/byteorder.h>
#include "device.h"

#include "testbios.h"
#include "pci-userspace.h"
int X86EMU_set_debug(int debug);

biosemu_device_t bios_device;

extern int teststart, testend;

#define BIOSMEM_SIZE (1024 * 1024)
unsigned char biosmem[BIOSMEM_SIZE];

int verbose = 0;

static unsigned char *mapitin(char *file, off_t where, size_t size)
{
	void *z;

	int fd = open(file, O_RDWR, 0);

	if (fd < 0)
		die(file);
	z = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, where);
	if (z == (void *) -1)
		die("mmap");
	close(fd);

	return z;
}

static unsigned short get_device(char *arg_val)
{
	unsigned short devfn=0;
	long bus=0,dev=0,fn=0,need_pack=0;
	char *tok;

	tok = strsep(&arg_val,":");
	if (arg_val != NULL) {
		bus = strtol(tok,0,16);
		need_pack = 1;
	}
	else {
		arg_val = tok;
	}

	tok = strsep(&arg_val,".");
	if (arg_val != NULL) {
		dev = strtol(tok,0,16);
		fn  = strtol(arg_val,0,16);
		need_pack = 1;
	}
	else {
		if (need_pack ==1 && (strlen(tok))) {
			dev = strtol(tok,0,16);
		}
	}

	if ( need_pack == 1) {
		devfn = bus<<8 | (dev<<3) | fn;
	}
	else {
		devfn = strtol(tok, 0, 0);
	}

	return devfn;
}

int printk(int msg_level, const char *fmt, ...)
{
	va_list args;
	int i;

	putchar('<');
	putchar('0' + msg_level);
	putchar('>');
	putchar(' ');
	va_start(args, fmt);
	i = vprintf(fmt, args);
	va_end(args);

	return i;
}

static void usage(char *name)
{
	printf
	    ("Usage: %s [-c codesegment] [-s size] [-b base] [-i ip] [-t] <filename> ... \n",
	     name);
}

/* main entry into YABEL biosemu, arguments are:
 * *biosmem = pointer to virtual memory
 * biosmem_size = size of the virtual memory
 * *dev = pointer to the device to be initialised
 * rom_addr = address of the OptionROM to be executed, if this is = 0, YABEL
 * will look for an ExpansionROM BAR and use the code from there.
 */
u32
biosemu(u8 *biosmem, u32 biosmem_size, struct device * dev, unsigned long
		rom_addr);


int main(int argc, char **argv)
{
	int ret;
	char *absegname = NULL;
	void *abseg = NULL;
	int c, trace = 0;
	unsigned char *cp;
	char *filename;
	ssize_t size = 0;
	int base = 0;
	int have_size = 0, have_base = 0, have_ip = 0, have_cs = 0;
	int have_devfn = 0;
	int parse_rom = 0;
	//char *fsegname = 0;
	//unsigned char *fsegptr;
	unsigned short initialip = 0, initialcs = 0, devfn = 0;
	//X86EMU_intrFuncs intFuncs[256];
	int debugflag = 0;
	struct device *dev;

	//const char *optstring = "vh?b:i:c:s:tpd:";
	const char *optstring = "vh?b:i:c:s:tpd:";
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"verbose", 0, 0, 'v'},
			{"help", 0, 0, 'h'},
			{"trace", 0, 0, 't'},
			{"base", 1, 0, 'b'},
			//{"fseg", 1, 0, 'f'},
			{"instructionpointer", 1, 0, 'i'},
			{"codesegment", 1, 0, 'c'},
			{"absegment", 1, 0, 'a'},
			{"size", 1, 0, 's'},
			{"parserom", 0, 0, 'p'},
			{"device", 1, 0, 'd'},
			{"debug", 1, 0, 'D'},
			{0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, optstring, long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 'v':
			verbose = 1;
			break;
		case 'h':
		case '?':
			usage(argv[0]);
			return 0;
		case 't':
			trace = 1;
			break;
		//case 'b':
		//	base = strtol(optarg, 0, 0);
		//	have_base = 1;
		//	break;
		case 'i':
			initialip = strtol(optarg, 0, 0);
			have_ip = 1;
			break;
		case 'c':
			initialcs = strtol(optarg, 0, 0);
			have_cs = 1;
			break;
		case 's':
			size = strtol(optarg, 0, 0);
			have_size = 1;
			break;
		case 'p':
			parse_rom = 1;
			break;
	//	case 'f':
	//		fsegname = optarg;
	//		break;
		case 'a':
			absegname = optarg;
			break;
		case 'd':
			devfn = get_device(optarg);
			have_devfn = 1;
			break;
		case 'D':
			debugflag = strtol(optarg, 0, 0);
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
		printf("running file %s\n", argv[optind]);
		filename = argv[optind];
		optind++;
		/* normally we would do continue, but for
		 * now only one filename is supported.
		 */
		/* continue; */
		break;
	}

	if (!have_size) {
		printf("No size specified. defaulting to 32k\n");
		size = 32 * 1024;
	}
	if (!have_base) {
		printf("No base specified. defaulting to 0xc0000\n");
		base = 0xc0000;
	}
	//if (!have_cs) {
	//	printf("No initial code segment specified. defaulting to 0xc000\n");
	//	initialcs = 0xc000;
	//}
	if (!have_ip) {
		printf
		    ("No initial instruction pointer specified. defaulting to 0x0003\n");
		initialip = 0x0003;
	}

	if (parse_rom)
		printf("Parsing rom images not implemented.\n");

	//printf("Point 1 int%x vector at %x\n", 0x42, getIntVect(0x42));
#if 0
	if (initialip == 0x0003) {
		if ((devfn == 0) || (have_devfn == 0)) {
			printf("WARNING! It appears you are trying to run an option ROM.\n");
			printf("  (initial ip = 0x0003)\n");
			if (have_devfn) {
				printf("  However, the device you have specified is 0x00\n");
				printf("  It is very unlikely that your device is at this address\n");
				printf("  Please check your -d option\n");
			}
			else {
				printf("  Please specify a device with -d\n");
				printf("  The default is not likely to work\n");
			}
		}
	}
#endif

	if (absegname) {
		abseg = mapitin(absegname, (off_t) 0xa0000, 0x20000);
		if (!abseg)
			die(absegname);
	}

	ioperm(0, 0x400, 1);

	if (iopl(3) < 0) {
		warn("iopl failed, continuing anyway");
	}

	/* Emergency sync ;-) */
	sync();
	sync();

	/* Setting up interrupt environment.
	 * basically this means initializing PCI and
	 * intXX handlers.
	 */
	pci_initialize();

#if 0
	for (i = 0; i < 256; i++)
		intFuncs[i] = do_int;
	X86EMU_setupIntrFuncs(intFuncs);
#endif
	cp = mapitin(filename, (off_t) 0, size);

	if (devfn) {
		printf("Loading ax with BusDevFn = %x\n",devfn);
	}

#if 0
	current->ax = devfn   ? devfn : 0xff;
	current->dx = 0x80;
	//      current->ip = 0;
	for (i = 0; i < size; i++)
		wrb(base + i, cp[i]);

	if (fsegname) {
		fsegptr = mapitin(fsegname, (off_t) 0, 0x10000);
		for (i = 0; i < 0x10000; i++)
			wrb(0xf0000 + i, fsegptr[i]);
	} else {
		const char *date = "01/01/99";
		for (i = i; date[i]; i++)
			wrb(0xffff5 + i, date[i]);
		wrb(0xffff7, '/');
		wrb(0xffffa, '/');
	}
	/* cpu setup */
	X86_AX = devfn ? devfn : 0xff;
	X86_DX = 0x80;
	X86_EIP = initialip;
	X86_CS = initialcs;

	/* Initialize stack and data segment */
	X86_SS = 0x0030;
	X86_DS = 0x0040;
	X86_SP = 0xfffe;
	/* We need a sane way to return from bios
	 * execution. A hlt instruction and a pointer
	 * to it, both kept on the stack, will do.
	 */
	pushw(0xf4f4);		/* hlt; hlt */
	pushw(X86_SS);
	pushw(X86_SP + 2);

	X86_ES = 0x0000;
#endif

	if (trace) {
		printf("Switching to single step mode.\n");
		//X86EMU_trace_on();
	}
	if (debugflag) {
		printf("Enable Debug = %x.\n",debugflag);
		//X86EMU_set_debug(debugflag);
	}
#if 0
	X86EMU_exec();
#endif

	ret = biosemu(biosmem, BIOSMEM_SIZE, dev, base);

#if 0
	current = &p;
	X86EMU_setMemBase(biosmem, sizeof(biosmem));
	M.abseg = (unsigned long)abseg;
	X86EMU_setupPioFuncs(&myfuncs);
#endif

	/* Cleaning up */
	pci_exit();

	return 0;
}
