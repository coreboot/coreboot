#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <lib.h>
#include <spd.h>
#include "southbridge/intel/i82801ex/early_smbus.c"
#include "northbridge/intel/e7501/raminit.h"
#include "northbridge/intel/e7501/debug.c"
#include "superio/winbond/w83627hf/early_serial.c"
#include "cpu/x86/bist.h"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

// FIXME: There's another hard_reset() in reset.c. Why?
static void hard_reset(void)
{
        /* full reset */
	outb(0x0a, 0x0cf9);
        outb(0x0e, 0x0cf9);
}

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/intel/e7501/raminit.c"
#include "northbridge/intel/e7501/reset_test.c"
#include "lib/generic_sdram.c"

void main(unsigned long bist)
{
	static const struct mem_controller memctrl[] = {
                {
                        .d0 = PCI_DEV(0, 0, 0),
                        .d0f1 = PCI_DEV(0, 0, 1),
                        .channel0 = { DIMM0, DIMM1, DIMM2, 0 },
                        .channel1 = { DIMM4, DIMM5, DIMM6, 0 },
                },
	};

	if (bist == 0)
		enable_lapic();

 	w83627hf_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
        console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	if (bios_reset_detected())
		hard_reset();

	enable_smbus();
#if 0
	dump_spd_registers(&memctrl[0]);
	dump_smbus_registers();
#endif

	sdram_initialize(1, memctrl);

#if 0
	dump_pci_devices();
#endif

#if 1
        dump_pci_device(PCI_DEV(0, 0, 0));
#endif
}
