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
	pei_data->ec_present = 1;

	/* P1 */
	pei_data_usb2_port(pei_data, 0, 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL);
	/* P2: left side port, USB debug */
	pei_data_usb2_port(pei_data, 1, 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL);
	/* P3: digitizer and right side ports (Microchip hub) */
	pei_data_usb2_port(pei_data, 2, 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL);
	/* P4: WLAN */
	pei_data_usb2_port(pei_data, 3, 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL);
	/* P5: fingerprint reader */
	pei_data_usb2_port(pei_data, 4, 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL);
	/* P6: WWAN */
	pei_data_usb2_port(pei_data, 5, 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL);
	/* P7: webcam */
	pei_data_usb2_port(pei_data, 6, 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL);
	/* P8 */
	pei_data_usb2_port(pei_data, 7, 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL);

	/* P1 */
	pei_data_usb3_port(pei_data, 0, 1, USB_OC_PIN_SKIP, 0);
	/* P2: left side, USB debug */
	pei_data_usb3_port(pei_data, 1, 1, USB_OC_PIN_SKIP, 0);
	/* P3: right side (Microchip hub) */
	pei_data_usb3_port(pei_data, 2, 1, USB_OC_PIN_SKIP, 0);
	/* P4 */
	pei_data_usb3_port(pei_data, 3, 1, USB_OC_PIN_SKIP, 0);
}
