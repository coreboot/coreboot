/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <stdint.h>
#include <device/pnp_def.h>
#include <device/pnp_ops.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
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
	struct pei_data pei_data_template = {
		.pei_version = PEI_VERSION,
		.mchbar = (uintptr_t)DEFAULT_MCHBAR,
		.dmibar = (uintptr_t)DEFAULT_DMIBAR,
		.epbar = DEFAULT_EPBAR,
		.pciexbar = CONFIG_MMCONF_BASE_ADDRESS,
		.smbusbar = CONFIG_FIXED_SMBUS_IO_BASE,
		.wdbbar = 0x4000000,
		.wdbsize = 0x1000,
		.hpet_address = CONFIG_HPET_ADDRESS,
		.rcba = (uintptr_t)DEFAULT_RCBA,
		.pmbase = DEFAULT_PMBASE,
		.gpiobase = DEFAULT_GPIOBASE,
		.thermalbase = 0xfed08000,
		.system_type = 0, /* 0 Mobile, 1 Desktop/Server */
		.tseg_size = CONFIG_SMM_TSEG_SIZE,
		.spd_addresses = { 0xA0, 0x00,0xA4,0x00 },
		.ts_addresses = { 0x00, 0x00, 0x00, 0x00 },
		.ec_present = 1,
		.gbe_enable = 1,
		.ddr3lv_support = 0,
		/*
		 * 0 = leave channel enabled
		 * 1 = disable dimm 0 on channel
		 * 2 = disable dimm 1 on channel
		 * 3 = disable dimm 0+1 on channel
		 */
		.dimm_channel0_disabled = 2,
		.dimm_channel1_disabled = 2,
		.max_ddr3_freq = 1600,
		.usb_port_config = {
			 /* enabled   USB oc pin    length */
			{ 1, 0, 0x0040 }, /* P0: lower left USB 3.0 (OC0) */
			{ 1, 0, 0x0040 }, /* P1: upper left USB 3.0 (OC0) */
			{ 1, 0, 0x0040 }, /* P2: lower right USB 3.0 (OC0) */
			{ 1, 0, 0x0040 }, /* P3: upper right USB 3.0 (OC0) */
			{ 1, 0, 0x0040 }, /* P4: lower USB 2.0 (OC0) */
			{ 1, 0, 0x0040 }, /* P5: upper USB 2.0 (OC0) */
			{ 1, 0, 0x0040 }, /* P6: front panel USB 2.0 (OC0) */
			{ 1, 0, 0x0040 }, /* P7: front panel USB 2.0 (OC0) */
			{ 1, 4, 0x0040 }, /* P8: internal USB 2.0 (OC4) */
			{ 1, 4, 0x0040 }, /* P9: internal USB 2.0 (OC4) */
			{ 1, 4, 0x0040 }, /* P10: internal USB 2.0 (OC4) */
			{ 1, 4, 0x0040 }, /* P11: internal USB 2.0 (OC4) */
			{ 1, 4, 0x0040 }, /* P12: internal USB 2.0 (OC4) */
			{ 1, 4, 0x0040 }, /* P13: internal USB 2.0 (OC4) */
		},
		.usb3 = {
			.mode =			3,	/* Smart Auto? */
			.hs_port_switch_mask =	0xf,	/* All four ports. */
			.preboot_support =	1,	/* preOS driver? */
			.xhci_streams =		1,	/* Enable. */
		},
		.pcie_init = 1,
	};
	*pei_data = pei_data_template;
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	/* enabled power  USB oc pin  */
	{ 1, 0, 0 }, /* P0: lower left USB 3.0 (OC0) */
	{ 1, 0, 0 }, /* P1: upper left USB 3.0 (OC0) */
	{ 1, 0, 0 }, /* P2: lower right USB 3.0 (OC0) */
	{ 1, 0, 0 }, /* P3: upper right USB 3.0 (OC0) */
	{ 1, 0, 0 }, /* P4: lower USB 2.0 (OC0) */
	{ 1, 0, 0 }, /* P5: upper USB 2.0 (OC0) */
	{ 1, 0, 0 }, /* P6: front panel USB 2.0 (OC0) */
	{ 1, 0, 0 }, /* P7: front panel USB 2.0 (OC0) */
	{ 1, 0, 4 }, /* P8: internal USB 2.0 (OC4) */
	{ 1, 0, 4 }, /* P9: internal USB 2.0 (OC4) */
	{ 1, 0, 4 }, /* P10: internal USB 2.0 (OC4) */
	{ 1, 0, 4 }, /* P11: internal USB 2.0 (OC4) */
	{ 1, 0, 4 }, /* P12: internal USB 2.0 (OC4) */
	{ 1, 0, 4 }, /* P13: internal USB 2.0 (OC4) */
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x52, id_only);
}

void mainboard_early_init(int s3resume)
{
	/* Enable PEG10 (1x16) */
	pci_write_config32(PCI_DEV(0, 0, 0), DEVEN,
			   pci_read_config32(PCI_DEV(0, 0, 0), DEVEN) |
			   DEVEN_PEG10);
}

int mainboard_should_reset_usb(int s3resume)
{
	return !s3resume;
}
