/*
 * TS5300 specific initialization code.
 *   written by Stefan Reinauer <stepan@coresystems.de>
 *   (c) 2006 coresystems GmbH
 */

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/hlt.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include "cpu/x86/bist.h"

#define TS5300_LED_OFF outb((inb(0x77)&0xfe), 0x77)
#define TS5300_LED_ON  outb((inb(0x77)|1), 0x77)

#define TS9500_LED_OFF outb((inb(0x19a)&0xfe), 0x19a)
#define TS9500_LED_ON  outb((inb(0x19a)|1), 0x19a)

/* PAR register setup */
void setup_pars(void)
{
        volatile unsigned long *par;
        par = (unsigned long *) 0xfffef088;

        /* NOTE: Ron says, move this to mainboard.c */
	*par++ = 0x00000000;
	*par++ = 0x340f0070;
	*par++ = 0x380701f0;
	*par++ = 0x3c0103f6;
	*par++ = 0x2c0f0300;
	*par++ = 0x447c00a0;
	*par++ = 0xe600000c;
	*par++ = 0x300046e8;
	*par++ = 0x500400d0;
	*par++ = 0x281f0140;
	*par++ = 0x00000000;
	*par++ = 0x00000000;
	*par++ = 0x00000000;
	*par++ = 0x8a07c940; /* Flash setup */
	*par++ = 0x00000000;
	*par++ = 0xee00400e;
}

#include "cpu/amd/sc520/raminit.c"

static void identify_ts9500(void)
{
	unsigned i, val;

	TS9500_LED_ON;

	print_err("TS-9500 add-on found:\n");
	val=inb(0x19b);
	for (i=0; i<8; i++) {
		print_err("  DIP");
		print_err_char(i+0x31);
		print_err(": ");
		if((val&(1<<i))!=0)
			print_err("on\n");
		else
			print_err("off\n");
	}
	print_err("\n");

	val=inb(0x19a);

	for (i=6; i<8; i++) {
		print_err("  JP");
		print_err_char(i+0x30-5);
		print_err(": ");
		if((val&(1<<i))!=0)
			print_err("on\n");
		else
			print_err("off\n");
	}
	print_err("\n");

	TS9500_LED_OFF;
}

static void identify_system(void)
{
	unsigned i,val;

	print_err("Mainboard: ");
	val=inb(0x74);
	switch(val) {
	case 0x50: print_err("TS-5300\n"); break;
	case 0x40: print_err("TS-5400\n"); break;
	case 0x60: print_err("TS-5500\n"); break;
	case 0x20: print_err("TS-5600\n"); break;
	case 0x70: print_err("TS-5700\n"); break;
	default:   print_err("unknown\n"); break;
	}

	val=inb(0x75);
	print_err("  SRAM option:   ");
	if((val&1)==0) print_err("not ");
	print_err("installed\n");

	print_err("  RS-485 option: ");
	if((val&2)==0) print_err("not ");
	print_err("installed\n");

	val=inb(0x76);
	print_err("  Temp. range:   ");
	if((val&2)==0) print_err("commercial\n");
	else print_err("industrial\n");

	print_err("\n");

	val=inb(0x77);
	for (i=1; i<8; i++) {
		print_err("  JP");
		print_err_char(i+0x30);
		print_err(": ");
		if((val&(1<<i))!=0)
			print_err("on\n");
		else
			print_err("off\n");
	}
	print_err("\n");

	/* Detect TS-9500 */
	val=inb(0x19d);
	if(val==0x5f)
		identify_ts9500();
}

static void hard_reset(void)
{
	print_err("Hard reset called.\n");
	while (1) ;
}

static void main(unsigned long bist)
{
	volatile int i;
	unsigned val;

	TS5300_LED_ON;

	// Let the hardware settle a bit.
	for(i = 0; i < 100; i++)
		;

        setupsc520();
        console_init();


	print_err("Technologic Systems TS5300 - http://www.embeddedx86.com/\n");
	staticmem();
	print_err("Memory initialized: 32MB\n");

#if 1
	identify_system();
#endif

	TS5300_LED_OFF;
}
