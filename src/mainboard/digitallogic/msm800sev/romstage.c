#include <stdint.h>
#include <stdlib.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/hlt.h>
#include <console/console.h>
#include "cpu/x86/bist.h"
#include "cpu/x86/msr.h"
#include <cpu/amd/lxdef.h>
#include <cpu/amd/geode_post_code.h>
#include "southbridge/amd/cs5536/cs5536.h"
#include <spd.h>

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

#include "southbridge/amd/cs5536/cs5536_early_smbus.c"
#include "southbridge/amd/cs5536/cs5536_early_setup.c"
#include "superio/winbond/w83627hf/w83627hf_early_serial.c"

static inline int spd_read_byte(unsigned device, unsigned address)
{
		return smbus_read_byte(device, address);
}

#define ManualConf 0		/* Do automatic strapped PLL config */
#define PLLMSRhi 0x00001490 /* manual settings for the PLL */
#define PLLMSRlo 0x02000030
#include "northbridge/amd/lx/raminit.h"
#include "northbridge/amd/lx/pll_reset.c"
#include "northbridge/amd/lx/raminit.c"
#include "lib/generic_sdram.c"
#include "cpu/amd/model_lx/cpureginit.c"
#include "cpu/amd/model_lx/syspreinit.c"
#include "cpu/amd/model_lx/msrinit.c"

static void mb_gpio_init(void)
{
	/* Early mainboard specific GPIO setup */
}

void main(unsigned long bist)
{
	post_code(0x01);

	static const struct mem_controller memctrl [] = {
		{.channel0 = {(0xa<<3)|0, (0xa<<3)|1}}
	};

	SystemPreInit();
	msr_init();

	cs5536_early_setup();

	/* NOTE: must do this AFTER the early_setup!
	 * it is counting on some early MSR setup
	 * for cs5536
	 */
	cs5536_disable_internal_uart();
	w83627hf_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	mb_gpio_init();
	uart_init();
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	pll_reset(ManualConf);

	cpuRegInit(0, DIMM0, DIMM1, DRAM_TERMINATED);

	sdram_initialize(1, memctrl);

	/* Check all of memory */
	ram_check(0x00000000, 640*1024);

	/* Switch from Cache as RAM to real RAM */
	/* There are two ways we could think about this.
	 1. If we are using the romstage.inc ROMCC way, the stack is going to be re-setup in the code following this code.
		Just wbinvd the stack to clear the cache tags. We don't care where the stack used to be.
	 2. This file is built as a normal .c -> .o and linked in etc. The stack might be used to return etc.
		That means we care about what is in the stack. If we are smart we set the CAR stack to the same location
		as the rest of coreboot. If that is the case we can just do a wbinvd. The stack will be written into real
		RAM that is now setup and we continue like nothing happened. If the stack is located somewhere other than
		where LB would like it, you need to write some code to do a copy from cache to RAM

	 We use method 1 on Norwich.
	*/
	post_code(0x02);
	print_err("POST 02\n");
	__asm__("wbinvd\n");
	print_err("Past wbinvd\n");
	/* we are finding the return does not work on this board. Explicitly call the label that is
	 * after the call to us. This is gross, but sometimes at this level it is the only way out
	 */
	void done_cache_as_ram_main(void);
	done_cache_as_ram_main();
}

