/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2012 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <cbfs.h>
#include <console/console.h>
#include <cpu/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/raminit.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <southbridge/intel/lynxpoint/lp_gpio.h>
#include "gpio.h"

const struct rcba_config_instruction rcba_config[] = {

	/*
	 *             GFX    INTA -> PIRQA (MSI)
	 * D28IP_P1IP  PCIE   INTA -> PIRQA
	 * D29IP_E1P   EHCI   INTA -> PIRQD
	 * D20IP_XHCI  XHCI   INTA -> PIRQC (MSI)
	 * D31IP_SIP   SATA   INTA -> PIRQF (MSI)
	 * D31IP_SMIP  SMBUS  INTB -> PIRQG
	 * D31IP_TTIP  THRT   INTC -> PIRQA
	 * D27IP_ZIP   HDA    INTA -> PIRQG (MSI)
	 */

	/* Device interrupt pin register (board specific) */
	RCBA_SET_REG_32(D31IP, (INTC << D31IP_TTIP) | (NOINT << D31IP_SIP2) |
			(INTB << D31IP_SMIP) | (INTA << D31IP_SIP)),
	RCBA_SET_REG_32(D29IP, (INTA << D29IP_E1P)),
	RCBA_SET_REG_32(D28IP, (INTA << D28IP_P1IP) | (INTC << D28IP_P3IP) |
			(INTB << D28IP_P4IP)),
	RCBA_SET_REG_32(D27IP, (INTA << D27IP_ZIP)),
	RCBA_SET_REG_32(D26IP, (INTA << D26IP_E2P)),
	RCBA_SET_REG_32(D22IP, (NOINT << D22IP_MEI1IP)),
	RCBA_SET_REG_32(D20IP, (INTA << D20IP_XHCI)),

	/* Device interrupt route registers */
	RCBA_SET_REG_32(D31IR, DIR_ROUTE(PIRQG, PIRQC, PIRQB, PIRQA)),/* LPC */
	RCBA_SET_REG_32(D29IR, DIR_ROUTE(PIRQD, PIRQD, PIRQD, PIRQD)),/* EHCI */
	RCBA_SET_REG_32(D28IR, DIR_ROUTE(PIRQA, PIRQB, PIRQC, PIRQD)),/* PCIE */
	RCBA_SET_REG_32(D27IR, DIR_ROUTE(PIRQG, PIRQG, PIRQG, PIRQG)),/* HDA */
	RCBA_SET_REG_32(D22IR, DIR_ROUTE(PIRQA, PIRQA, PIRQA, PIRQA)),/* ME */
	RCBA_SET_REG_32(D21IR, DIR_ROUTE(PIRQE, PIRQF, PIRQF, PIRQF)),/* SIO */
	RCBA_SET_REG_32(D20IR, DIR_ROUTE(PIRQC, PIRQC, PIRQC, PIRQC)),/* XHCI */
	RCBA_SET_REG_32(D23IR, DIR_ROUTE(PIRQH, PIRQH, PIRQH, PIRQH)),/* SDIO */

	/* Disable unused devices (board specific) */
	RCBA_RMW_REG_32(FD, ~0, PCH_DISABLE_ALWAYS),

	RCBA_END_CONFIG,
};

/* Copy SPD data for on-board memory */
static void copy_spd(struct pei_data *peid)
{
	const int gpio_vector[] = {13, 9, 47, -1};
	int spd_index = get_gpios(gpio_vector);
	struct cbfs_file *spd_file;

	printk(BIOS_DEBUG, "SPD index %d\n", spd_index);
	spd_file = cbfs_get_file(CBFS_DEFAULT_MEDIA, "spd.bin");
	if (!spd_file)
		die("SPD data not found.");

	if (ntohl(spd_file->len) <
	    ((spd_index + 1) * sizeof(peid->spd_data[0]))) {
		printk(BIOS_ERR, "SPD index override to 0 - old hardware?\n");
		spd_index = 0;
	}

	if (spd_file->len < sizeof(peid->spd_data[0]))
		die("Missing SPD data.");

	/* Index 0-2 are 4GB config with both CH0 and CH1
	 * Index 3-5 are 2GB config with CH0 only
	 */
	if (spd_index > 2)
		peid->dimm_channel1_disabled = 3;

	memcpy(peid->spd_data[0],
	       ((char*)CBFS_SUBHEADER(spd_file)) +
	       spd_index * sizeof(peid->spd_data[0]),
	       sizeof(peid->spd_data[0]));
}

void mainboard_romstage_entry(unsigned long bist)
{
	struct pei_data pei_data = {
		pei_version: PEI_VERSION,
		mchbar: DEFAULT_MCHBAR,
		dmibar: DEFAULT_DMIBAR,
		epbar: DEFAULT_EPBAR,
		pciexbar: DEFAULT_PCIEXBAR,
		smbusbar: SMBUS_IO_BASE,
		wdbbar: 0x4000000,
		wdbsize: 0x1000,
		hpet_address: HPET_ADDR,
		rcba: DEFAULT_RCBA,
		pmbase: DEFAULT_PMBASE,
		gpiobase: DEFAULT_GPIOBASE,
		temp_mmio_base: 0xfed08000,
		system_type: 5, /* ULT */
		tseg_size: CONFIG_SMM_TSEG_SIZE,
		spd_addresses: { 0xff, 0x00, 0xff, 0x00 },
		ec_present: 1,
		// 0 = leave channel enabled
		// 1 = disable dimm 0 on channel
		// 2 = disable dimm 1 on channel
		// 3 = disable dimm 0+1 on channel
		dimm_channel0_disabled: 2,
		dimm_channel1_disabled: 2,
		// Enable 2x refresh mode
		ddr_refresh_2x: 1,
		max_ddr3_freq: 1600,
		usb_xhci_on_resume: 1,
		usb2_ports: {
			/* Length, Enable, OCn#, Location */
			{ 0x0064, 1, 0,               /* P0: Port A, CN8 */
			  USB_PORT_BACK_PANEL },
			{ 0x0052, 1, 0,               /* P1: Port B, CN9  */
			  USB_PORT_BACK_PANEL },
			{ 0x0040, 1, USB_OC_PIN_SKIP, /* P2: CCD */
			  USB_PORT_INTERNAL },
			{ 0x0040, 1, USB_OC_PIN_SKIP, /* P3: BT */
			  USB_PORT_INTERNAL },
			{ 0x0040, 1, USB_OC_PIN_SKIP, /* P4: LTE */
			  USB_PORT_INTERNAL },
			{ 0x0040, 1, USB_OC_PIN_SKIP, /* P5: TOUCH */
			  USB_PORT_INTERNAL },
			{ 0x0040, 1, USB_OC_PIN_SKIP, /* P6: SD Card */
			  USB_PORT_INTERNAL },
			{ 0x0123, 1, 3,               /* P7: USB2 Port */
			  USB_PORT_INTERNAL },
		},
		usb3_ports: {
			/* Enable, OCn# */
			{ 1, 0               }, /* P1; Port A, CN8 */
			{ 1, 0               }, /* P2; Port B, CN9  */
			{ 0, USB_OC_PIN_SKIP }, /* P3; */
			{ 0, USB_OC_PIN_SKIP }, /* P4; */
		},
	};

	struct romstage_params romstage_params = {
		.pei_data = &pei_data,
		.gpio_map = &mainboard_gpio_map,
		.rcba_config = &rcba_config[0],
		.bist = bist,
		.copy_spd = copy_spd,
	};

	/* Call into the real romstage main with this board's attributes. */
	romstage_common(&romstage_params);
}
