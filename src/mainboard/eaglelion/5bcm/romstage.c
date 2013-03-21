#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/hlt.h>
#include <stdlib.h>
#include <console/console.h>
#include "superio/nsc/pc97317/early_serial.c"
#include "cpu/x86/bist.h"
#include "southbridge/amd/cs5530/enable_rom.c"
#include "northbridge/amd/gx1/raminit.c"

#define SERIAL_DEV PNP_DEV(0x2e, PC97317_SP1)

static void main(unsigned long bist)
{
	pc97317_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();
	report_bist_failure(bist);
	cs5530_enable_rom();
	sdram_init();
}
