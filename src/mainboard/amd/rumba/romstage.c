#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/hlt.h>
#include <console/console.h>
#include "superio/winbond/w83627hf/w83627hf_early_serial.c"
#include "cpu/x86/bist.h"
#include "cpu/x86/msr.h"
#include <cpu/amd/gx2def.h>
#include <cpu/amd/geode_post_code.h>
#include <spd.h>

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

#include "southbridge/amd/cs5536/cs5536_early_smbus.c"
#include "southbridge/amd/cs5536/cs5536_early_setup.c"

static inline int spd_read_byte(unsigned device, unsigned address)
{
 	if (device != DIMM0)
		return 0xFF;	/* No DIMM1, don't even try. */

	return smbus_read_byte(device, address);
}

#include "northbridge/amd/gx2/raminit.h"
#include "northbridge/amd/gx2/pll_reset.c"
#include "northbridge/amd/gx2/raminit.c"
#include "lib/generic_sdram.c"
#include "cpu/amd/model_gx2/cpureginit.c"
#include "cpu/amd/model_gx2/syspreinit.c"
#include "cpu/amd/model_lx/msrinit.c"

void main(unsigned long bist)
{
	static const struct mem_controller memctrl [] = {
		{.channel0 = {(0xa<<3)|0, (0xa<<3)|1}}
	};

	SystemPreInit();

	w83627hf_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	uart_init();
	console_init();

	cs5536_early_setup();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	pll_reset();

	cpuRegInit();
	print_err("done cpuRegInit\n");

	sdram_initialize(1, memctrl);

	msr_init();

	/* Check all of memory */
	//ram_check(0x00000000, 640*1024);
}

