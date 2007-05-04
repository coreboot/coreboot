/*
*
* Copyright (C) 2007 Advanced Micro Devices
*
*/

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
#include "cpu/x86/bist.h"
#include "cpu/x86/msr.h"
#include <cpu/amd/lxdef.h>
#include <cpu/amd/geode_post_code.h>
#include "southbridge/amd/cs5536/cs5536.h"

#define POST_CODE(x) outb(x, 0x80)

#include "southbridge/amd/cs5536/cs5536_early_smbus.c"
#include "southbridge/amd/cs5536/cs5536_early_setup.c"

static inline int spd_read_byte(unsigned device, unsigned address)
{
		return smbus_read_byte(device, address);
}

#define ManualConf 0		/* Do automatic strapped PLL config */
#define PLLMSRhi 0x00001490 /* manual settings for the PLL */
#define PLLMSRlo 0x02000030
#define DIMM0 0xA0
#define DIMM1 0xA2
#include "northbridge/amd/lx/raminit.h"
#include "northbridge/amd/lx/pll_reset.c"
#include "northbridge/amd/lx/raminit.c"
#include "sdram/generic_sdram.c"
#include "cpu/amd/model_lx/cpureginit.c"
#include "cpu/amd/model_lx/syspreinit.c"

static void msr_init(void)
{
	/* Setup access to the MC for low memory. Note MC not setup yet. */
	__builtin_wrmsr(CPU_RCONF_DEFAULT,	 0x10f3bf00, 0x24fffc02);

	__builtin_wrmsr(MSR_GLIU0 + 0x20, 0xfff80, 0x20000000);
	__builtin_wrmsr(MSR_GLIU0 + 0x21, 0x80fffe0, 0x20000000);

	__builtin_wrmsr(MSR_GLIU1 + 0x20, 0xfff80, 0x20000000);
	__builtin_wrmsr(MSR_GLIU1 + 0x21, 0x80fffe0, 0x20000000);
}

static void mb_gpio_init(void)
{
	/* Early mainboard specific GPIO setup */
}

static void main(unsigned long bist)
{
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
	/* cs5536_disable_internal_uart	 disable them for now, set them up later...*/
	cs5536_setup_onchipuart(); /* if debug. real setup done in chipset init via config.lb */
	mb_gpio_init();
	uart_init();
	console_init();

	pll_reset(ManualConf);

	cpuRegInit();

	sdram_initialize(1, memctrl);

	/* Check all of memory */
	//ram_check(0x00000000, 640*1024);
}
