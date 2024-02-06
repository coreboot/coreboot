/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pnp_def.h>
#include <device/pnp_ops.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <superio/winbond/common/winbond.h>

void mainboard_pch_lpc_setup(void)
{
	/* Set COM3/COM1 decode ranges: 0x3e8/0x3f8 */
	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x0070);

	/* Enable KBC on 0x06/0x64 (KBC),
	 *        EC on 0x62/0x66 (MC),
	 *        EC on 0x20c-0x20f (GAMEH),
	 *        Super I/O on 0x2e/0x2f (CNF1),
	 *	  COM1/COM3 decode ranges. */
	pci_write_config16(PCH_LPC_DEV, LPC_EN,
			   KBC_LPC_EN | MC_LPC_EN |
			   CNF1_LPC_EN | GAMEH_LPC_EN |
			   COMA_LPC_EN | COMB_LPC_EN);
}

void bootblock_mainboard_early_init(void)
{
	int lvds_3v = 0; /* 0 (5V) or 1 (3V3) */
	int dis_bl_inv = 1; /* backlight inversion: 1 = disabled, 0 = enabled */
	const pnp_devfn_t dev = PNP_DEV(0x2e, 0x9);
	pnp_enter_conf_state(dev);
	pnp_write_config(dev, 0x29, 0x02); /* Pins 119, 120 are GPIO21, 20 */
	pnp_write_config(dev, PNP_IDX_EN, 0x03); /* Enable GPIO2+3 */
	pnp_write_config(dev, 0x2a, 0x01); /* Pins 62, 63, 65, 66 are
					      GPIO27, 26, 25, 24 */
	pnp_write_config(dev, 0x2c, 0xc3); /* Pin 90 is GPIO32,
					      Pins 78~85 are UART B */
	pnp_write_config(dev, 0x2d, 0x00); /* Pins 67, 68, 70~73, 75, 77 are
					      GPIO57~50 */
	pnp_set_logical_device(dev);
	/* Values can only be changed, when devices are enabled. */
	pnp_write_config(dev, 0xe3, 0xdd); /* GPIO2 bits 1, 5 are output */
	pnp_write_config(dev, 0xe4, (dis_bl_inv << 5) | (lvds_3v << 1)); /* GPIO2 bits 1, 5 */
	/* Disable suspend LED during normal operation */
	pnp_write_config(dev, PNP_IDX_MSC3, 0x40);
	pnp_exit_conf_state(dev);
}

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
}

void mainboard_early_init(int s3resume)
{
	/* Enable PEG10 (1x16) */
	pci_write_config32(PCI_DEV(0, 0, 0), DEVEN,
			   pci_read_config32(PCI_DEV(0, 0, 0), DEVEN) |
			   DEVEN_PEG10);
}
