/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <stdint.h>
#include <device/pnp_ops.h>
#include <device/pnp.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <southbridge/intel/bd82x6x/pch.h>
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

void mainboard_fill_pei_data(struct pei_data *const pei_data)
{
	const struct pei_data pei_data_template = {
		.pei_version = PEI_VERSION,
		.mchbar = CONFIG_FIXED_MCHBAR_MMIO_BASE,
		.dmibar = CONFIG_FIXED_DMIBAR_MMIO_BASE,
		.epbar = CONFIG_FIXED_EPBAR_MMIO_BASE,
		.pciexbar = CONFIG_MMCONF_BASE_ADDRESS,
		.smbusbar = CONFIG_FIXED_SMBUS_IO_BASE,
		.wdbbar = 0x4000000,
		.wdbsize = 0x1000,
		.hpet_address = CONFIG_HPET_ADDRESS,
		.rcba = (uintptr_t)DEFAULT_RCBA,
		.pmbase = DEFAULT_PMBASE,
		.gpiobase = DEFAULT_GPIOBASE,
		.thermalbase = 0xfed08000,
		.system_type = 0, // 0 Mobile, 1 Desktop/Server
		.tseg_size = CONFIG_SMM_TSEG_SIZE,
		.spd_addresses = { 0xA0, 0xA2, 0xA4, 0xA6 },
		.ts_addresses = { 0x00, 0x00, 0x00, 0x00 },
		.ec_present = 1,
		.gbe_enable = 1,
		.ddr3lv_support = 0,
		.max_ddr3_freq = 1600,
		.usb_port_config = {
			/* Enabled / OC PIN / Length */
			{ 1, 0, 0x0080 }, /* P00: 1st (left)     USB3 (OC #0) */
			{ 1, 0, 0x0080 }, /* P01: 2nd (left)     USB3 (OC #0) */
			{ 1, 1, 0x0080 }, /* P02: 1st Multibay   USB3 (OC #1) */
			{ 1, 1, 0x0080 }, /* P03: 2nd Multibay   USB3 (OC #1) */
			{ 1, 8, 0x0040 }, /* P04: MiniPCIe 1     USB2 (no OC) */
			{ 1, 8, 0x0040 }, /* P05: MiniPCIe 2     USB2 (no OC) */
			{ 1, 8, 0x0040 }, /* P06: USB Hub x4     USB2 (no OC) */
			{ 1, 8, 0x0040 }, /* P07: MiniPCIe 4     USB2 (no OC) */
			{ 1, 8, 0x0080 }, /* P08: SD card reader USB2 (no OC) */
			{ 1, 4, 0x0080 }, /* P09: 3rd (right)    USB2 (OC #4) */
			{ 1, 5, 0x0040 }, /* P10: 4th (right)    USB2 (OC #5) */
			{ 1, 8, 0x0040 }, /* P11: 3rd Multibay   USB2 (no OC) */
			{ 1, 8, 0x0080 }, /* P12: misc internal  USB2 (no OC) */
			{ 1, 6, 0x0080 }, /* P13: misc internal  USB2 (OC #6) */
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
	/* Enabled / Power / OC PIN */
	{ 1, 1, 0 }, /* P00: 1st (left)     USB3 (OC #0) */
	{ 1, 1, 0 }, /* P01: 2nd (left)     USB3 (OC #0) */
	{ 1, 1, 1 }, /* P02: 1st Multibay   USB3 (OC #1) */
	{ 1, 1, 1 }, /* P03: 2nd Multibay   USB3 (OC #1) */
	{ 1, 0, 8 }, /* P04: MiniPCIe 1     USB2 (no OC) */
	{ 1, 0, 8 }, /* P05: MiniPCIe 2     USB2 (no OC) */
	{ 1, 0, 8 }, /* P06: USB Hub x4     USB2 (no OC) */
	{ 1, 0, 8 }, /* P07: MiniPCIe 4     USB2 (no OC) */
	{ 1, 1, 8 }, /* P08: SD card reader USB2 (no OC) */
	{ 1, 1, 4 }, /* P09: 3rd (right)    USB2 (OC #4) */
	{ 1, 0, 5 }, /* P10: 4th (right)    USB2 (OC #5) */
	{ 1, 0, 8 }, /* P11: 3rd Multibay   USB2 (no OC) */
	{ 1, 1, 8 }, /* P12: misc internal  USB2 (no OC) */
	{ 1, 1, 6 }, /* P13: misc internal  USB2 (OC #6) */
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[1], 0x51, id_only);
	read_spd(&spd[2], 0x52, id_only);
	read_spd(&spd[3], 0x53, id_only);
}
