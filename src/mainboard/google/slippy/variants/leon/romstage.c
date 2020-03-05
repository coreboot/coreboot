/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <cbfs.h>
#include <console/console.h>
#include <cpu/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/raminit.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <southbridge/intel/lynxpoint/lp_gpio.h>
#include "../../variant.h"

/* Copy SPD data for on-board memory */
void copy_spd(struct pei_data *peid)
{
	const int gpio_vector[] = {13, 9, 47, -1};
	int spd_index = get_gpios(gpio_vector);
	char *spd_file;
	size_t spd_file_len;
	size_t spd_len = sizeof(peid->spd_data[0]);

	printk(BIOS_DEBUG, "SPD index %d\n", spd_index);
	spd_file = cbfs_map("spd.bin", &spd_file_len);
	if (!spd_file)
		die("SPD data not found.");

	if (spd_file_len < ((spd_index + 1) * spd_len)) {
		printk(BIOS_ERR, "SPD index override to 0 - old hardware?\n");
		spd_index = 0;
	}

	if (spd_file_len < spd_len)
		die("Missing SPD data.");

	memcpy(peid->spd_data[0], spd_file + (spd_index * spd_len), spd_len);

	/* Limiting to a single dimm for 2GB configuration
	 * Identified by bit 3
	 */
	if (spd_index & 0x4)
		peid->dimm_channel1_disabled = 3;
	else
		memcpy(peid->spd_data[1],
			spd_file + (spd_index * spd_len), spd_len);
}

void variant_romstage_entry(struct pei_data *pei_data)
{
	struct usb2_port_setting usb2_ports[MAX_USB2_PORTS] = {
		/* Length, Enable, OCn#, Location */
		{ 0x0040, 1, 0,               /* P0: Port A, CN10 */
		  USB_PORT_BACK_PANEL },
		{ 0x0040, 1, 2,               /* P1: Port B, CN11 */
		  USB_PORT_BACK_PANEL },
		{ 0x0080, 1, USB_OC_PIN_SKIP, /* P2: CCD */
		  USB_PORT_INTERNAL },
		{ 0x0040, 1, USB_OC_PIN_SKIP, /* P3: BT */
		  USB_PORT_MINI_PCIE },
		{ 0x0080, 1, USB_OC_PIN_SKIP, /* P4: SD Card */
		  USB_PORT_INTERNAL },
		{ 0x0040, 1, USB_OC_PIN_SKIP, /* P5: LTE */
		  USB_PORT_INTERNAL },
		{ 0x0040, 1, USB_OC_PIN_SKIP, /* P6: SIM CARD */
		  USB_PORT_FLEX },
		{ 0x0000, 0, USB_OC_PIN_SKIP, /* P7: EMPTY */
		  USB_PORT_SKIP },
	};

	struct usb3_port_setting usb3_ports[MAX_USB3_PORTS] = {
		/* Enable, OCn# */
		{ 1, 0               }, /* P1; Port A, CN10 */
		{ 1, 2               }, /* P2; Port B, CN11 */
		{ 0, USB_OC_PIN_SKIP }, /* P3; */
		{ 0, USB_OC_PIN_SKIP }, /* P4; */
	};

	memcpy(pei_data->usb2_ports, usb2_ports, sizeof(usb2_ports));
	memcpy(pei_data->usb3_ports, usb3_ports, sizeof(usb3_ports));
}
