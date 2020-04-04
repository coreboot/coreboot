/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <assert.h>
#include <arch/exception.h>
#include <arch/stages.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <program_loading.h>
#include <soc/clock.h>
#include <soc/nvidia/tegra/apbmisc.h>
#include <soc/pinmux.h>
#include <soc/power.h>
#include <timestamp.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* called from assembly in bootblock_asm.S */
void tegra124_main(void);

static void run_next_stage(void *entry)
{
	ASSERT(entry);
	clock_cpu0_config(entry);

	power_enable_and_ungate_cpu();

	/* Repair RAM on cluster0 and cluster1 after CPU is powered on. */
	ram_repair();

	clock_cpu0_remove_reset();

	clock_halt_avp();
}

void tegra124_main(void)
{
	// enable pinmux clamp inputs
	clamp_tristate_inputs();

	// enable JTAG at the earliest stage
	enable_jtag();

	clock_early_uart();

	// Serial out, tristate off.
	pinmux_set_config(PINMUX_KB_ROW9_INDEX, PINMUX_KB_ROW9_FUNC_UA3);
	// Serial in, tristate_on.
	pinmux_set_config(PINMUX_KB_ROW10_INDEX, PINMUX_KB_ROW10_FUNC_UA3 |
						 PINMUX_PULL_UP |
						 PINMUX_INPUT_ENABLE);
	// Mux some pins away from uart A.
	pinmux_set_config(PINMUX_UART2_CTS_N_INDEX,
			  PINMUX_UART2_CTS_N_FUNC_UB3 |
			  PINMUX_INPUT_ENABLE);
	pinmux_set_config(PINMUX_UART2_RTS_N_INDEX,
			  PINMUX_UART2_RTS_N_FUNC_UB3);

	if (CONFIG(BOOTBLOCK_CONSOLE)) {
		console_init();
		exception_init();
	}

	clock_init();

	bootblock_mainboard_init();

	pinmux_set_config(PINMUX_CORE_PWR_REQ_INDEX,
			  PINMUX_CORE_PWR_REQ_FUNC_PWRON);
	pinmux_set_config(PINMUX_CPU_PWR_REQ_INDEX,
			  PINMUX_CPU_PWR_REQ_FUNC_CPU);
	pinmux_set_config(PINMUX_PWR_INT_N_INDEX,
			  PINMUX_PWR_INT_N_FUNC_PMICINTR |
			  PINMUX_INPUT_ENABLE);

	timestamp_init(0);

	run_romstage();
}

void platform_prog_run(struct prog *prog)
{
	run_next_stage(prog_entry(prog));
}
