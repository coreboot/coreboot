#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/hlt.h>
#include <stdlib.h>
#include <console/console.h>
#include "northbridge/via/vt8623/raminit.h"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"
#include "drivers/pc80/udelay_io.c"
#include "lib/delay.c"
#include "cpu/x86/lapic/boot_cpu.c"
#include "lib/debug.c"
#include "southbridge/via/vt8235/early_smbus.c"
#include "southbridge/via/vt8235/early_serial.c"

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/via/vt8623/raminit.c"

static void enable_mainboard_devices(void)
{
	device_t dev;

	dev = pci_locate_device_on_bus(PCI_ID(PCI_VENDOR_ID_VIA,
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
	device_t dev;

	/* Enable VGA; 32MB buffer. */
	pci_write_config8(0, 0xe1, 0xdd);

	/*
	 * Disable the firewire stuff, which apparently steps on IO 0+ on
	 * reset. Doh!
	 */
	dev = pci_locate_device_on_bus(PCI_ID(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_6305), 0);
	if (dev != PCI_DEV_INVALID)
		pci_write_config8(dev, 0x15, 0x1c);

	enable_vt8235_serial();
	console_init();

	enable_smbus();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	// init_timer();

	post_code(0x05);

	print_debug(" Enabling mainboard devices\n");
	enable_mainboard_devices();

	print_debug(" Enabling shadow ram\n");
	enable_shadow_ram();

	ddr_ram_setup((const struct mem_controller *)0);

	if (bist == 0)
		early_mtrr_init();

	//dump_pci_devices();
}
