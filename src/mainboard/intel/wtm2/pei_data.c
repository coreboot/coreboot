/*
 * This file is part of the coreboot project.
 *
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

#include <stdint.h>
#include <soc/gpio.h>
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	/* One installed DIMM per channel */
	pei_data->dimm_channel0_disabled = 2;
	pei_data->dimm_channel1_disabled = 2;

	pei_data->spd_addresses[0] = 0xa2;
	pei_data->spd_addresses[2] = 0xa2;

	pei_data_usb2_port(pei_data, 0, 0x40, 1, USB_OC_PIN_SKIP,
			   USB_PORT_FRONT_PANEL);
	pei_data_usb2_port(pei_data, 1, 0x40, 1, USB_OC_PIN_SKIP,
			   USB_PORT_FRONT_PANEL);
	pei_data_usb2_port(pei_data, 2, 0x40, 1, USB_OC_PIN_SKIP,
			   USB_PORT_FRONT_PANEL);
	pei_data_usb2_port(pei_data, 3, 0x40, 1, USB_OC_PIN_SKIP,
			   USB_PORT_FRONT_PANEL);
	pei_data_usb2_port(pei_data, 4, 0x40, 1, USB_OC_PIN_SKIP,
			   USB_PORT_FRONT_PANEL);
	pei_data_usb2_port(pei_data, 5, 0x40, 1, USB_OC_PIN_SKIP,
			   USB_PORT_FRONT_PANEL);
	pei_data_usb2_port(pei_data, 6, 0x40, 1, USB_OC_PIN_SKIP,
			   USB_PORT_FRONT_PANEL);
	pei_data_usb2_port(pei_data, 7, 0x40, 1, USB_OC_PIN_SKIP,
			   USB_PORT_FRONT_PANEL);

	pei_data_usb3_port(pei_data, 0, 1, USB_OC_PIN_SKIP, 0);
	pei_data_usb3_port(pei_data, 1, 1, USB_OC_PIN_SKIP, 0);
	pei_data_usb3_port(pei_data, 2, 1, USB_OC_PIN_SKIP, 0);
	pei_data_usb3_port(pei_data, 3, 1, USB_OC_PIN_SKIP, 0);
}
