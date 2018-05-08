/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	pei_data->ec_present = 1;

	/* One DIMM slot */
	pei_data->dimm_channel1_disabled = 3;
	pei_data->spd_addresses[0] = 0xa0;

	/* P1: Left Side Port (USB2 only) */
	pei_data_usb2_port(pei_data, 0, 0x0080, 1, USB_OC_PIN_SKIP,
			   USB_PORT_BACK_PANEL);
	/* P2: Right Side Port (USB2) */
	pei_data_usb2_port(pei_data, 1, 0x0080, 1, USB_OC_PIN_SKIP,
			   USB_PORT_BACK_PANEL);
	/* P3: Empty */
	pei_data_usb2_port(pei_data, 2, 0x0000, 0, USB_OC_PIN_SKIP,
			   USB_PORT_SKIP);
	/* P4: Camera */
	pei_data_usb2_port(pei_data, 3, 0x0080, 1, USB_OC_PIN_SKIP,
			   USB_PORT_BACK_PANEL);
	/* P5: Bluetooth */
	pei_data_usb2_port(pei_data, 4, 0x0080, 1, USB_OC_PIN_SKIP,
			   USB_PORT_BACK_PANEL);
	/* P6: Empty */
	pei_data_usb2_port(pei_data, 5, 0x0080, 0, USB_OC_PIN_SKIP,
			   USB_PORT_SKIP);
	/* P7: Empty */
	pei_data_usb2_port(pei_data, 6, 0x0080, 0, USB_OC_PIN_SKIP,
			   USB_PORT_SKIP);
	/* P8: SD Card */
	pei_data_usb2_port(pei_data, 7, 0x0080, 1, USB_OC_PIN_SKIP,
			   USB_PORT_BACK_PANEL);

	/* P1: Empty */
	pei_data_usb3_port(pei_data, 0, 0, USB_OC_PIN_SKIP, 0);
	/* P2: Right Side Port (USB3) */
	pei_data_usb3_port(pei_data, 1, 1, USB_OC_PIN_SKIP, 0);
	/* P3: Empty */
	pei_data_usb3_port(pei_data, 2, 0, USB_OC_PIN_SKIP, 0);
	/* P4: Empty */
	pei_data_usb3_port(pei_data, 3, 0, USB_OC_PIN_SKIP, 0);
}
