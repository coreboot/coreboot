/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <string.h>
#include <ec/google/chromeec/ec.h>
#include <northbridge/intel/haswell/raminit.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <southbridge/intel/lynxpoint/lp_gpio.h>
#include "../../onboard.h"
#include "../../variant.h"

unsigned int variant_get_spd_index(void)
{
	const int gpio_vector[] = {13, 9, 47, -1};
	return get_gpios(gpio_vector);
}

bool variant_is_dual_channel(const unsigned int spd_index)
{
	uint32_t board_version = PEPPY_BOARD_VERSION_PROTO;
	google_chromeec_get_board_version(&board_version);
	switch (board_version) {
	case PEPPY_BOARD_VERSION_PROTO:
		/* Index 0 is 2GB config with CH0 only. */
		return spd_index != 0;

	case PEPPY_BOARD_VERSION_EVT:
	default:
		/* Index 0-3 are 4GB config with both CH0 and CH1.
		   Index 4-7 are 2GB config with CH0 only. */
		return spd_index <= 3;
	}
}

const struct usb2_port_setting mainboard_usb2_ports[MAX_USB2_PORTS] = {
	/* Length, Enable, OCn#, Location */
	{ 0x0150, 1, USB_OC_PIN_SKIP, /* P0: LTE */
	  USB_PORT_MINI_PCIE },
	{ 0x0040, 1, 0,               /* P1: Port A, CN10 */
	  USB_PORT_BACK_PANEL },
	{ 0x0080, 1, USB_OC_PIN_SKIP, /* P2: CCD */
	  USB_PORT_INTERNAL },
	{ 0x0040, 1, USB_OC_PIN_SKIP, /* P3: BT */
	  USB_PORT_MINI_PCIE },
	{ 0x0040, 1, 2,               /* P4: Port B, CN6  */
	  USB_PORT_BACK_PANEL },
	{ 0x0000, 0, USB_OC_PIN_SKIP, /* P5: EMPTY */
	  USB_PORT_SKIP },
	{ 0x0150, 1, USB_OC_PIN_SKIP, /* P6: SD Card */
	  USB_PORT_FLEX },
	{ 0x0000, 0, USB_OC_PIN_SKIP, /* P7: EMPTY */
	  USB_PORT_SKIP },
};

const struct usb3_port_setting mainboard_usb3_ports[MAX_USB3_PORTS] = {
	/* Enable, OCn# */
	{ 1, 0               }, /* P1; Port A, CN6 */
	{ 0, USB_OC_PIN_SKIP }, /* P2; */
	{ 0, USB_OC_PIN_SKIP }, /* P3; */
	{ 0, USB_OC_PIN_SKIP }, /* P4; */
};
