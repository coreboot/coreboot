/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <baseboard/variants.h>
#include <device/pci_ops.h>
#include <device/pnp_ops.h>
#include <intelpch/espi.h>
#include <soc/pci_devs.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8613e/it8613e.h>

#define it8613e_index	0x2e

#define EC_DEV		PNP_DEV(it8613e_index, IT8613E_EC)
#define UART_DEV	PNP_DEV(it8613e_index, IT8613E_SP1)

/* Environment Controller IO bases (HWM block / PME). */
#define EC_HWM_BASE	0x0a40
#define EC_PME_BASE	0x0a30

/* Bring up the Environment Controller's logical-device config */
static void it8613e_ec_init(void)
{
	pnp_enter_conf_state(EC_DEV);
	pnp_set_logical_device(EC_DEV);
	pnp_set_iobase(EC_DEV, PNP_IDX_IO0, EC_HWM_BASE);
	pnp_set_iobase(EC_DEV, PNP_IDX_IO1, EC_PME_BASE);
	pnp_write_config(EC_DEV, 0xf1, 0xbf);
	pnp_write_config(EC_DEV, 0xf4, 0xe0);
	pnp_write_config(EC_DEV, 0xfb, 0x0c);
	pnp_set_enable(EC_DEV, 1);
	pnp_exit_conf_state(EC_DEV);
}

void bootblock_mainboard_early_init(void)
{
	const struct pad_config *early_pads;
	size_t early_num;

	/* Fixed IO decode for eSPI */
	pci_write_config32(PCH_DEV_ESPI, ESPI_IO_DEC, 0x3c030070);

	early_pads = variant_early_gpio_table(&early_num);
	gpio_configure_pads(early_pads, early_num);

	it8613e_ec_init();

	/* 5VSB_CTRL# disable */
	ite_reg_write(EC_DEV, 0xfa, 0);
	ite_disable_pme_out(EC_DEV);
	ite_ac_resume_southbridge(EC_DEV);

	ite_enable_serial(UART_DEV, CONFIG_TTYS0_BASE);
}
