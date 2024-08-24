/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/hpet.h>
#include <bootblock_common.h>
#include <stdint.h>
#include <device/pnp_ops.h>
#include <device/pnp.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <superio/ite/it8783ef/it8783ef.h>
#include <superio/ite/common/ite.h>

void bootblock_mainboard_early_init(void)
{
	const pnp_devfn_t dev = PNP_DEV(0x2e, IT8783EF_GPIO);

	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);

	pnp_write_config(dev, 0x23, ITE_UART_CLK_PREDIVIDE_24);

	/* Switch multi function for UART4 */
	pnp_write_config(dev, 0x2a, 0x04);
	/* Switch multi function for UART3 */
	pnp_write_config(dev, 0x2c, 0x13);

	/* No GPIOs used: Clear any output / pull-up that's set by default */
	pnp_write_config(dev, 0xb8, 0x00);
	pnp_write_config(dev, 0xc0, 0x00);
	pnp_write_config(dev, 0xc3, 0x00);
	pnp_write_config(dev, 0xc8, 0x00);
	pnp_write_config(dev, 0xcb, 0x00);
	pnp_write_config(dev, 0xef, 0x00);

	pnp_exit_conf_state(dev);
}

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	/* TODO: Confirm if need to enable peg10 in devicetree */
	pei_data->pcie_init = 1;
}
