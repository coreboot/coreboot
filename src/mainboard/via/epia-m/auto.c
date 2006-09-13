#define ASSEMBLY 1

#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#if 0
#include <cpu/x86/lapic.h>
#endif
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/hlt.h>
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
#include "northbridge/via/vt8623/raminit.h"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"

/*
 */
void udelay(int usecs) 
{
	int i;
	for(i = 0; i < usecs; i++)
		outb(i&0xff, 0x80);
}

#include "lib/delay.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "debug.c"

#include "southbridge/via/vt8235/vt8235_early_smbus.c"

#include "southbridge/via/vt8235/vt8235_early_serial.c"
static void memreset_setup(void)
{
}

static inline int spd_read_byte(unsigned device, unsigned address)
{
	unsigned char c;
	c = smbus_read_byte(device, address);
	return c;
}

#include "northbridge/via/vt8623/raminit.c"

static void enable_mainboard_devices(void) 
{
	device_t dev;
  
	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_8235), 0);
  
	if (dev == PCI_DEV_INVALID) {
		die("Southbridge not found!!!\n");
	}
	pci_write_config8(dev, 0x50, 0x80);
	pci_write_config8(dev, 0x51, 0x1f);
#if 0
	// This early setup switches IDE into compatibility mode before PCI gets 
	// a chance to assign I/Os
	// movl    $CONFIG_ADDR(0, 0x89, 0x42), %eax
	// //      movb    $0x09, %dl
	// movb    $0x00, %dl
	// PCI_WRITE_CONFIG_BYTE
#endif
	/* we do this here as in V2, we can not yet do raw operations 
	 * to pci!
	 */
        dev += 0x100; /* ICKY */

	pci_write_config8(dev, 0x04, 7);
	pci_write_config8(dev, 0x40, 3);
	pci_write_config8(dev, 0x42, 0);
	pci_write_config8(dev, 0x3c, 0xe);
	pci_write_config8(dev, 0x3d, 0);
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

static void main(unsigned long bist)
{
	unsigned long x;
	device_t dev;

	/*
	 * Enable VGA; 32MB buffer.
	 */
	pci_write_config8(0, 0xe1, 0xdd);

	/*
	 * Disable the firewire stuff, which apparently steps on IO 0+ on
	 * reset. Doh!
	 */
	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_6305), 0);
	if (dev != PCI_DEV_INVALID) {
		pci_write_config8(dev, 0x15, 0x1c);
	}

	enable_smbus();
	
	enable_vt8235_serial();
	uart_init();
	console_init();

	print_spew("In auto.c:main()\r\n");

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	// init_timer();

	outb(5, 0x80);	

	print_debug(" Enabling mainboard devices\r\n");
	enable_mainboard_devices();

	print_debug(" Enabling shadow ram\r\n");
	enable_shadow_ram();

	ddr_ram_setup((const struct mem_controller *)0);
	
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

	if (bist == 0) {
		print_debug(" Doing MTRR init.\r\n");
		early_mtrr_init();
	}

	//dump_pci_devices();
	
	print_spew("Leaving auto.c:main()\r\n");
}
