/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	/* P0: HOST PORT */
	pei_data_usb2_port(pei_data, 0, 0x0080, 1, 0, USB_PORT_BACK_PANEL);
	/* P1: HOST PORT */
	pei_data_usb2_port(pei_data, 1, 0x0080, 1, 1, USB_PORT_BACK_PANEL);
	/* P2: RAIDEN */
	pei_data_usb2_port(pei_data, 2, 0x0080, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL);
	/* P3: SD CARD */
	pei_data_usb2_port(pei_data, 3, 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_INTERNAL);
	/* P4: RAIDEN */
	pei_data_usb2_port(pei_data, 4, 0x0080, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL);
	/* P5: WWAN (Disabled) */
	pei_data_usb2_port(pei_data, 5, 0x0000, 0, USB_OC_PIN_SKIP, USB_PORT_SKIP);
	/* P6: CAMERA */
	pei_data_usb2_port(pei_data, 6, 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_INTERNAL);
	/* P7: BT */
	pei_data_usb2_port(pei_data, 7, 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_INTERNAL);

	/* P1: HOST PORT */
	pei_data_usb3_port(pei_data, 0, 1, 0, 0);
	/* P2: HOST PORT */
	pei_data_usb3_port(pei_data, 1, 1, 1, 0);
	/* P3: RAIDEN */
	pei_data_usb3_port(pei_data, 2, 1, USB_OC_PIN_SKIP, 0);
	/* P4: RAIDEN */
	pei_data_usb3_port(pei_data, 3, 1, USB_OC_PIN_SKIP, 0);
}
