/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>

void mb_get_spd_map(struct spd_info *spdi)
{
	spdi->addresses[0] = 0x50;
	spdi->addresses[2] = 0x52;
}

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	/* P1: Right Side Port (USB2) */
	pei_data_usb2_port(pei_data, 0, 0x0080, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL);
	/* P2: Right Side Port (USB2) */
	pei_data_usb2_port(pei_data, 1, 0x0080, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL);
	/* P3: Left Side Port (USB2 only) */
	pei_data_usb2_port(pei_data, 2, 0x0080, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL);
	/* P4: Left Side Port (USB2 only) */
	pei_data_usb2_port(pei_data, 3, 0x0080, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL);
	/* P5: Empty */
	pei_data_usb2_port(pei_data, 4, 0x0080, 0, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL);
	/* P6: Bluetooth */
	pei_data_usb2_port(pei_data, 5, 0x0080, 1, USB_OC_PIN_SKIP, USB_PORT_SKIP);
	/* P7: Camera */
	pei_data_usb2_port(pei_data, 6, 0x0080, 1, USB_OC_PIN_SKIP, USB_PORT_SKIP);
	/* P8: SD Card */
	pei_data_usb2_port(pei_data, 7, 0x0080, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL);

	/* P1: Right Side Port (USB3) */
	pei_data_usb3_port(pei_data, 0, 1, USB_OC_PIN_SKIP, 0);
	/* P2: Right Side Port (USB3) */
	pei_data_usb3_port(pei_data, 1, 1, USB_OC_PIN_SKIP, 0);
	/* P3: Empty */
	pei_data_usb3_port(pei_data, 2, 0, USB_OC_PIN_SKIP, 0);
	/* P4: Empty */
	pei_data_usb3_port(pei_data, 3, 0, USB_OC_PIN_SKIP, 0);
}
