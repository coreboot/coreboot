/*
 * STPC basic initialization
 * by
 * Steve M. Gehlbach
 *
 * Most stpc work has to be done earlier
 * so not much is in this file
 *
 */
#include <mem.h>
#include <cpu/stpc/consumer2/stpc.h>
#include <cpu/p5/io.h>
#include <printk.h>

int display_cpuid ( void ) {
	return 0;
}

int mtrr_check ( void ) {
	return 0;
}

struct mem_range *sizeram(void)
{
	static struct mem_range mem[3];
	int mem_size;

// get ram size from settings in stpc SDRAM controller registers
// the last bank register = top memory in MB less one
	mem_size = ( stpc_conf_readb(0x33) + 1 ) * 1024;
	printk_info("stpc memory size= %d MB\n",mem_size/1024);

	if (mem_size < 0 || mem_size > 128*1024) mem_size = 64*1024;

	mem[0].basek = 0;
	mem[0].sizek = 640;
	mem[1].basek = 1024;
	mem[1].sizek = mem_size - STPC_FRAME_BUF_SZ;
	mem[2].basek = 0;
	mem[2].sizek = 0;
	if (mem[1].sizek == 0) {
		mem[1].sizek = 64*1024;
	}
	mem[1].sizek -= mem[1].basek;
	return &mem[0];
}

int nvram_on ( void ) {
	return 0;
}

