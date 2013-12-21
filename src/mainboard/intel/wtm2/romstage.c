/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
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
#include <stddef.h>
#include <console/console.h>
#include "cpu/intel/haswell/haswell.h"
#include "northbridge/intel/haswell/haswell.h"
#include "northbridge/intel/haswell/raminit.h"
#include "southbridge/intel/lynxpoint/pch.h"
#include "southbridge/intel/lynxpoint/lp_gpio.h"
#include "gpio.h"

const struct rcba_config_instruction rcba_config[] = {

	/*
	 *             GFX    INTA -> PIRQA (MSI)
	 * D28IP_P1IP  WLAN   INTA -> PIRQB
	 * D28IP_P4IP  ETH0   INTB -> PIRQC
	 * D29IP_E1P   EHCI1  INTA -> PIRQD
	 * D20IP_XHCI  XHCI   INTA -> PIRQA
	 * D31IP_SIP   SATA   INTA -> PIRQF (MSI)
	 * D31IP_SMIP  SMBUS  INTB -> PIRQG
	 * D31IP_TTIP  THRT   INTC -> PIRQH
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
	RCBA_SET_REG_32(D25IP, (NOINT << D25IP_LIP)),
	RCBA_SET_REG_32(D22IP, (NOINT << D22IP_MEI1IP)),
	RCBA_SET_REG_32(D20IR, (INTA << D20IP_XHCI)),

	/* Device interrupt route registers */
	RCBA_SET_REG_32(D31IR, DIR_ROUTE(PIRQF, PIRQG, PIRQH, PIRQA)),
	RCBA_SET_REG_32(D29IR, DIR_ROUTE(PIRQD, PIRQE, PIRQF, PIRQG)),
	RCBA_SET_REG_32(D28IR, DIR_ROUTE(PIRQB, PIRQC, PIRQD, PIRQE)),
	RCBA_SET_REG_32(D27IR, DIR_ROUTE(PIRQG, PIRQH, PIRQA, PIRQB)),
	RCBA_SET_REG_32(D26IR, DIR_ROUTE(PIRQE, PIRQF, PIRQG, PIRQH)),
	RCBA_SET_REG_32(D25IR, DIR_ROUTE(PIRQA, PIRQB, PIRQC, PIRQD)),
	RCBA_SET_REG_32(D22IR, DIR_ROUTE(PIRQA, PIRQB, PIRQC, PIRQD)),
	RCBA_SET_REG_32(D21IR, DIR_ROUTE(PIRQA, PIRQB, PIRQC, PIRQD)),
	RCBA_SET_REG_32(D20IR, DIR_ROUTE(PIRQA, PIRQB, PIRQC, PIRQD)),
	RCBA_SET_REG_32(D23IR, DIR_ROUTE(PIRQA, 0, 0, 0)),

	/* Disable unused devices (board specific) */
	RCBA_RMW_REG_32(FD, ~0, PCH_DISABLE_ALWAYS),

	RCBA_END_CONFIG,
};

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
		spd_addresses: { 0xa2, 0x00, 0xa2, 0x00 },
		ec_present: 1,
		// 0 = leave channel enabled
		// 1 = disable dimm 0 on channel
		// 2 = disable dimm 1 on channel
		// 3 = disable dimm 0+1 on channel
		dimm_channel0_disabled: 2,
		dimm_channel1_disabled: 2,
		max_ddr3_freq: 1600,
		usb2_ports: {
			/* Length, Enable, OCn# */
			{ 0x40, 1, USB_OC_PIN_SKIP, /* P0: */
			  USB_PORT_FRONT_PANEL },
			{ 0x40, 1, USB_OC_PIN_SKIP, /* P1: */
			  USB_PORT_FRONT_PANEL },
			{ 0x40, 1, USB_OC_PIN_SKIP, /* P2: */
			  USB_PORT_FRONT_PANEL },
			{ 0x40, 1, USB_OC_PIN_SKIP, /* P3: */
			  USB_PORT_FRONT_PANEL },
			{ 0x40, 1, USB_OC_PIN_SKIP, /* P4: */
			  USB_PORT_FRONT_PANEL },
			{ 0x40, 1, USB_OC_PIN_SKIP, /* P5: */
			  USB_PORT_FRONT_PANEL },
			{ 0x40, 1, USB_OC_PIN_SKIP, /* P6: */
			  USB_PORT_FRONT_PANEL },
			{ 0x40, 0, USB_OC_PIN_SKIP, /* P7: */
			  USB_PORT_FRONT_PANEL },
		},
		usb3_ports: {
			/* Enable, OCn# */
			{ 1, USB_OC_PIN_SKIP }, /* P1; */
			{ 1, USB_OC_PIN_SKIP }, /* P2; */
			{ 1, USB_OC_PIN_SKIP }, /* P3; */
			{ 1, USB_OC_PIN_SKIP }, /* P4; */
		},
	};

	struct romstage_params romstage_params = {
		.pei_data = &pei_data,
		.gpio_map = &mainboard_gpio_map,
		.rcba_config = &rcba_config[0],
		.bist = bist,
		.copy_spd = NULL,
	};

	/* Call into the real romstage main with this board's attributes. */
	romstage_common(&romstage_params);
}
