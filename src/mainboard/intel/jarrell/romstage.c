#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <stdlib.h>
#include <console/console.h>
#include "southbridge/intel/i82801ex/early_smbus.c"
#include "northbridge/intel/e7520/raminit.h"
#include "superio/nsc/pc87427/pc87427.h"
#include "cpu/x86/lapic/boot_cpu.c"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "watchdog.c"
#include "southbridge/intel/i82801ex/reset.c"
#include "power_reset_check.c"
#include "jarrell_fixups.c"
#include "superio/nsc/pc87427/early_init.c"
#include "northbridge/intel/e7520/memory_initialized.c"
#include "cpu/x86/bist.h"
#include <spd.h>

#define SIO_GPIO_BASE 0x680
#define SIO_XBUS_BASE 0x4880

#define CONSOLE_SERIAL_DEV PNP_DEV(0x2e, PC87427_SP2)
#define HIDDEN_SERIAL_DEV  PNP_DEV(0x2e, PC87427_SP1)

#define DEVPRES_CONFIG  (DEVPRES_D1F0 | DEVPRES_D2F0 | DEVPRES_D6F0)
#define DEVPRES1_CONFIG (DEVPRES1_D0F1 | DEVPRES1_D8F0)

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/intel/e7520/raminit.c"
#include "lib/generic_sdram.c"
#include "debug.c"
#include "arch/x86/lib/stages.c"

static void main(unsigned long bist)
{
	static const struct mem_controller mch[] = {
		{
			.node_id = 0,
			.channel0 = { DIMM2, DIMM1, DIMM0, 0 },
			.channel1 = { DIMM6, DIMM5, DIMM4, 0 },
		}
	};

	if (bist == 0) {
		/* Skip this if there was a built in self test failure */
		early_mtrr_init();
		if (memory_initialized())
			skip_romstage();
	}

	/* Setup the console */
	pc87427_disable_dev(CONSOLE_SERIAL_DEV);
	pc87427_disable_dev(HIDDEN_SERIAL_DEV);
	pc87427_enable_dev(CONSOLE_SERIAL_DEV, CONFIG_TTYS0_BASE);
        /* Enable Serial 2 lines instead of GPIO */
        outb(0x2c, 0x2e);
        outb((inb(0x2f) & (~1<<1)), 0x2f);
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	pc87427_enable_dev(PC87427_GPIO_DEV, SIO_GPIO_BASE);

	pc87427_enable_dev(PC87427_XBUS_DEV, SIO_XBUS_BASE);
	xbus_cfg(PC87427_XBUS_DEV);

	/* MOVE ME TO A BETTER LOCATION !!! */
	/* config LPC decode for flash memory access */
        device_t dev;
        dev = pci_locate_device(PCI_ID(0x8086, 0x24d0), 0);
        if (dev == PCI_DEV_INVALID)
                die("Missing ich5?");
        pci_write_config32(dev, 0xe8, 0x00000000);
        pci_write_config8(dev, 0xf0, 0x00);

#if 0
	print_pci_devices();
#endif
	enable_smbus();
#if 0
//	dump_spd_registers(&cpu[0]);
	int i;
	for(i = 0; i < 1; i++)
		dump_spd_registers();
#endif
	disable_watchdogs();
	power_down_reset_check();
//	dump_ipmi_registers();
	mainboard_set_e7520_leds();
	sdram_initialize(ARRAY_SIZE(mch), mch);
	ich5_watchdog_on();
#if 0
	dump_pci_devices();
	dump_pci_device(PCI_DEV(0, 0x00, 0));
	dump_bar14(PCI_DEV(0, 0x00, 0));
#endif
}
