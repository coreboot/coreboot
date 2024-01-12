/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6791d/nct6791d.h>
#include <superio/aspeed/common/aspeed.h>
#include <superio/aspeed/ast2400/ast2400.h>
#include <console/uart.h>
#include <console/console.h>
#include <device/pnp_ops.h>
#include <delay.h>
#include "gpio.h"
#include <gpio.h>
#include <timer.h>

static int64_t bmc_ready_elapsed = 0;

void bootblock_mainboard_early_init(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));

	/*
	 * We must wait for the BMC firmware to be ready before proceeding past
	 * early init:
	 * - The BMC UART (the physical serial port) won't work until the BMC is
	 *   configured
	 * - The BMC will reset the TPM configuration during startup,
	 *   coreboot must not configure or communicate with the TPM before then
	 *
	 * This usually takes about 30 seconds for a boot right after power on.
	 */
	struct stopwatch sw;
	stopwatch_init_msecs_expire(&sw, 120 * 1000);
	while (!stopwatch_expired(&sw)) {
		/* Wait for J2 to go low - BMC ready signal from BMC firmware */
		if (!gpio_get(GPP_J2))
			break;
		mdelay(1000);
	}
	/* Trace the time spent later once the console is up */
	if (stopwatch_expired(&sw))
		bmc_ready_elapsed = -1;
	else
		bmc_ready_elapsed = stopwatch_duration_msecs(&sw);

	/*
	 * Disable the Nuvoton NCT6791D SuperIO UART1.  It is enabled by
	 * default, but the AST2500's is connected to the serial port.
	 */
	const pnp_devfn_t nvt_serial_dev = PNP_DEV(0x2E, NCT6791D_SP1);
	nuvoton_pnp_enter_conf_state(nvt_serial_dev);
	pnp_set_logical_device(nvt_serial_dev);
	pnp_set_enable(nvt_serial_dev, 0);
	nuvoton_pnp_exit_conf_state(nvt_serial_dev);

	/* Enable AST2500 SuperIO UART1 */
	const pnp_devfn_t ast_serial_dev = PNP_DEV(0x4E, AST2400_SUART1);
	aspeed_enable_serial(ast_serial_dev, CONFIG_TTYS0_BASE);
}

void bootblock_mainboard_init(void)
{
	if (bmc_ready_elapsed >= 0) {
		printk(BIOS_INFO, "BMC became ready in early init after %lld ms\n",
			bmc_ready_elapsed);
	} else {
		/* Better to boot than to render the system unusable, but some
		   peripherals may not work */
		printk(BIOS_ERR, "BMC was not ready within timeout, booting anyway\n");
	}
}
