#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/hlt.h>
#include <stdlib.h>
#include <lib.h>
#include "drivers/pc80/udelay_io.c"
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include "southbridge/intel/i82801dx/i82801dx.h"
#include "northbridge/intel/i855/raminit.h"
#include "northbridge/intel/i855/debug.c"
#include "superio/winbond/w83627hf/early_serial.c"
#include "cpu/x86/bist.h"
#include <spd.h>

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/intel/i855/raminit.c"
#include "northbridge/intel/i855/reset_test.c"

void main(unsigned long bist)
{
	if (bist == 0) {
#if 0
		enable_lapic();
		init_timer();
#endif
	}

        w83627hf_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
        console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

#if 0
	print_pci_devices();
#endif

	if (!bios_reset_detected()) {
        	enable_smbus();
#if 0
		dump_spd_registers();
		dump_smbus_registers();
#endif
		sdram_set_registers();
		sdram_set_spd_registers();
		sdram_enable();
	}

#if 0
	dump_pci_devices();
	dump_pci_device(PCI_DEV(0, 0, 0));
#endif
}
