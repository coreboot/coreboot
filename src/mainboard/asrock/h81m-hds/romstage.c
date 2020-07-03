/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <cpu/intel/haswell/haswell.h>
#include <device/pnp_ops.h>
#include <northbridge/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/pei_data.h>
#include <northbridge/intel/haswell/raminit.h>
#include <southbridge/intel/common/gpio.h>
#include <southbridge/intel/lynxpoint/pch.h>

void mainboard_config_rcba(void)
{
	RCBA16(D31IR) = DIR_ROUTE(PIRQA, PIRQD, PIRQC, PIRQA);
	RCBA16(D29IR) = DIR_ROUTE(PIRQH, PIRQD, PIRQA, PIRQC);
	RCBA16(D28IR) = DIR_ROUTE(PIRQA, PIRQB, PIRQC, PIRQD);
	RCBA16(D27IR) = DIR_ROUTE(PIRQG, PIRQB, PIRQC, PIRQD);
	RCBA16(D26IR) = DIR_ROUTE(PIRQA, PIRQF, PIRQC, PIRQD);
	RCBA16(D25IR) = DIR_ROUTE(PIRQE, PIRQF, PIRQG, PIRQH);
	RCBA16(D22IR) = DIR_ROUTE(PIRQA, PIRQD, PIRQC, PIRQB);
	RCBA16(D20IR) = DIR_ROUTE(PIRQA, PIRQB, PIRQC, PIRQD);
}

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	pei_data->system_type = 1; /* Desktop/Server */
	pei_data->spd_addresses[0] = 0xa0;
	pei_data->spd_addresses[2] = 0xa4;
	pei_data->ec_present = 0;
	/*
	 * 0 = leave channel enabled
	 * 1 = disable dimm 0 on channel
	 * 2 = disable dimm 1 on channel
	 * 3 = disable dimm 0+1 on channel
	 */
	pei_data->dimm_channel0_disabled = 2;
	pei_data->dimm_channel1_disabled = 2;
	pei_data->max_ddr3_freq = 1600;

	struct usb2_port_setting usb2_ports[MAX_USB2_PORTS] = {
		/* Length, Enable, OCn#, Location */
		{ 0x0040, 1, 0, USB_PORT_BACK_PANEL },
		{ 0x0040, 1, 0, USB_PORT_BACK_PANEL },
		{ 0x0040, 1, 1, USB_PORT_BACK_PANEL },
		{ 0x0040, 1, 1, USB_PORT_BACK_PANEL },
		{ 0x0040, 1, 2, USB_PORT_BACK_PANEL },
		{ 0x0040, 1, 2, USB_PORT_BACK_PANEL },
		{ 0x0040, 0, USB_OC_PIN_SKIP, USB_PORT_SKIP },
		{ 0x0040, 0, USB_OC_PIN_SKIP, USB_PORT_SKIP },
		{ 0x0040, 1, 4, USB_PORT_BACK_PANEL },
		{ 0x0040, 1, 4, USB_PORT_BACK_PANEL },
		{ 0x0040, 1, 5, USB_PORT_BACK_PANEL },
		{ 0x0040, 1, 5, USB_PORT_BACK_PANEL },
		{ 0x0040, 0, USB_OC_PIN_SKIP, USB_PORT_SKIP },
		{ 0x0040, 0, USB_OC_PIN_SKIP, USB_PORT_SKIP },
	};

	struct usb3_port_setting usb3_ports[MAX_USB3_PORTS] = {
		/* Enable, OCn# */
		{ 1, 0 },
		{ 1, 0 },
		{ 0, USB_OC_PIN_SKIP },
		{ 0, USB_OC_PIN_SKIP },
		{ 0, USB_OC_PIN_SKIP },
		{ 0, USB_OC_PIN_SKIP },
	};

	memcpy(pei_data->usb2_ports, usb2_ports, sizeof(usb2_ports));
	memcpy(pei_data->usb3_ports, usb3_ports, sizeof(usb3_ports));
}
