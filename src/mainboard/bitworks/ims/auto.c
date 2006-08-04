#define ASSEMBLY 1

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/hlt.h>
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
#include "southbridge/intel/i8371eb/i8371eb_early_smbus.c"
#include "superio/NSC/pc87351/pc87351_early_serial.c"
#include "northbridge/intel/i440bx/raminit.h"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"

#define SERIAL_DEV PNP_DEV(0x2e, PC87351_SP1)

/*
 */
void udelay(int usecs) 
{
	int i;
	for(i = 0; i < usecs; i++)
		outb(i&0xff, 0x80);
}

#include "debug.c"
#include "lib/delay.c"


static void memreset_setup(void)
{
}

/*
  static void memreset(int controllers, const struct mem_controller *ctrl)
  {
  }
*/


static void enable_mainboard_devices(void) 
{
	device_t dev;
	/* dev 0 for southbridge */
  
	dev = pci_locate_device(PCI_ID(0x1106,0x8231), 0);
  
	if (dev == PCI_DEV_INVALID) {
		die("Southbridge not found!!!\n");
	}
	pci_write_config8(dev, 0x50, 7);
	pci_write_config8(dev, 0x51, 0xff);
#if 0
	// This early setup switches IDE into compatibility mode before PCI gets 
	// // a chance to assign I/Os
	//         movl    $CONFIG_ADDR(0, 0x89, 0x42), %eax
	//         //      movb    $0x09, %dl
	//                 movb    $0x00, %dl
	//                         PCI_WRITE_CONFIG_BYTE
	//
#endif
	/* we do this here as in V2, we can not yet do raw operations 
	 * to pci!
	 */
        dev += 0x100; /* ICKY */

	pci_write_config8(dev, 0x42, 0);
}

static void enable_shadow_ram(void) 
{
	device_t dev = 0; /* no need to look up 0:0.0 */
	unsigned char shadowreg;
	/* dev 0 for southbridge */
	shadowreg = pci_read_config8(dev, 0x63);
	/* 0xf0000-0xfffff */
	shadowreg |= 0x30;
	pci_write_config8(dev, 0x63, shadowreg);
}

static inline int spd_read_byte(unsigned device, unsigned address)
{
	int c;
	c = smbus_read_byte(device, address);
	return c;
}


#include "northbridge/intel/i440bx/raminit.c"
#include "northbridge/intel/i440bx/debug.c"
#include "sdram/generic_sdram.c"

static void main(unsigned long bist)
{
	static const struct mem_controller cpu[] = {
		{
		 .channel0 = {
			 (0xa << 3) | 0,
			 (0xa << 3) | 1,
			 (0xa << 3) | 2, 
			 (0xa << 3) | 3,
		 	},
		 }
	};
	unsigned long x;
	int result;
	
	if (bist == 0) {
		early_mtrr_init();
	}
	pc87351_enable_serial(SERIAL_DEV, TTYS0_BASE);
	uart_init();
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);
	
	enable_smbus();
/*
	result = spd_read_byte(cpu[0].channel0[0],0x03);
	print_debug("Result: ");
	print_debug_hex16(result);
	print_debug("\r\n");
*/
	dump_spd_registers(&cpu[0]);

#if 0
	enable_shadow_ram();
	/*
	  memreset_setup();
	  this is way more generic than we need.
	  sdram_initialize(sizeof(cpu)/sizeof(cpu[0]), cpu);
	*/
	sdram_set_registers((const struct mem_controller *) 0);
	sdram_set_spd_registers((const struct mem_controller *) 0);
	sdram_enable(0, (const struct mem_controller *) 0);
#endif
	
	/* Check all of memory */
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
