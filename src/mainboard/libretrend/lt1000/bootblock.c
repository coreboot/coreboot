/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootblock_common.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8786e/it8786e.h>

#define GPIO_DEV	PNP_DEV(0x2e, IT8786E_GPIO)
#define SERIAL1_DEV	PNP_DEV(0x2e, IT8786E_SP1)
#define SERIAL3_DEV	PNP_DEV(0x2e, IT8786E_SP3)
#define SERIAL4_DEV	PNP_DEV(0x2e, IT8786E_SP4)
#define SERIAL5_DEV	PNP_DEV(0x2e, IT8786E_SP5)
#define SERIAL6_DEV	PNP_DEV(0x2e, IT8786E_SP6)

void bootblock_mainboard_early_init(void)
{
	ite_conf_clkin(GPIO_DEV, ITE_UART_CLK_PREDIVIDE_24);
	ite_enable_3vsbsw(GPIO_DEV);
	ite_kill_watchdog(GPIO_DEV);
	ite_enable_serial(SERIAL1_DEV, CONFIG_TTYS0_BASE);

	/*
	 * FIXME:
	 * IT8786E has 6 COM ports, COM1/3/5 have default IO base 0x3f8 and
	 * COM2/4/6 have 0x2f8. When enabling devices before setting resources
	 * from devicetree, the output on debugging COM1 becomes very slow due
	 * to the same IO bases for multiple COM ports. For now set different
	 * hardcoded IO bases for COM3/4/5/6 ports, they will be set later to
	 * desired values from devicetree. They can be also turned off.
	 */
	ite_enable_serial(SERIAL3_DEV, 0x3e8);
	ite_enable_serial(SERIAL4_DEV, 0x2e8);
	ite_enable_serial(SERIAL5_DEV, 0x2f0);
	ite_enable_serial(SERIAL6_DEV, 0x2e0);
}
