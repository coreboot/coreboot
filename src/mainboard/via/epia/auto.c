#define ASSEMBLY 1


#include <stdint.h>
#include <device/pci_def.h>
#include <cpu/p6/apic.h>
#include <arch/io.h>
#include <device/pnp.h>
#include <arch/romcc_io.h>
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
#include "northbridge/via/vt8601/raminit.h"
void udelay(int usecs) {
  int i;
  for(i = 0; i < usecs; i++)
    outb(0x80, 0x80);
}

#include "lib/delay.c"
#include "cpu/p6/boot_cpu.c"
#include "debug.c"

#include "southbridge/via/vt8231/vt8231_early_smbus.c"
#define SIO_BASE 0x2e
#define MAXIMUM_CONSOLE_LOGLEVEL 9
#define DEFAULT_CONSOLE_LOGLEVEL 9

static void memreset_setup(void)
{
}

/*
static void memreset(int controllers, const struct mem_controller *ctrl)
{
}
*/
static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}



#include "northbridge/via/vt8601/raminit.c"
#include "sdram/generic_sdram.c"
/*
 */

static void main(void)
{
  struct mem_controller cpu[1];
  //	init_timer();

	uart_init();
	console_init();
	
	enable_smbus();
	memreset_setup();
//	sdram_initialize(sizeof(cpu)/sizeof(cpu[0]), cpu);


	/* Check all of memory */
#if 0
	msr_t msr;
	msr = rdmsr(TOP_MEM);
	print_debug("TOP_MEM: ");
	print_debug_hex32(msr.hi);
	print_debug_hex32(msr.lo);
	print_debug("\r\n");
#endif
#if 0
	ram_check(0x00000000, msr.lo);
#endif
#if 0
	static const struct {
		unsigned long lo, hi;
	} check_addrs[] = {
		/* Check 16MB of memory @ 0*/
		{ 0x00000000, 0x01000000 },
#if TOTAL_CPUS > 1
		/* Check 16MB of memory @ 2GB */
		{ 0x80000000, 0x81000000 },
#endif
	};
	int i;
	for(i = 0; i < sizeof(check_addrs)/sizeof(check_addrs[0]); i++) {
		ram_check(check_addrs[i].lo, check_addrs[i].hi);
	}
#endif
}
