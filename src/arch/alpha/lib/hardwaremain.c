#include <stdlib.h>
#include <boot/elf.h>
#include <printk.h>
#include <subr.h>

void hardwaremain(unsigned long signature, unsigned long memsize, 
	unsigned long cpu_speed)
{
	early_mainboard_init();
	displayinit();

	printk("\n\nsignature=0x%016lx memsize=0x%016lx cpu_speed=0x%016lx\n",
		signature, memsize, cpu_speed);
	elfboot(memsize >> 10 /* In kilobytes */);
	printk("\n after elfboot\n");
}

void fatal_error(unsigned long exception_handler, unsigned long exception_at, 
	unsigned long return_address)
{
	/* Just in case we are totally messed up */
	early_mainboard_init();
	displayinit();
	printk("\n\nFault: 0x%0lx\n", exception_handler);
	printk("PC: 0x%0lx\n", exception_at);
	printk("RA: 0x%0lx\n", return_address);
	/* Now spin forever */
	while(1) {;}
}
