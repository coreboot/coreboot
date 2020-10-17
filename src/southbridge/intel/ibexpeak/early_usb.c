/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <southbridge/intel/common/rcba.h>
#include <southbridge/intel/common/pmbase.h>

#include "pch.h"

#define TOTAL_USB_PORTS 14

void early_usb_init(const struct southbridge_usb_port *portmap)
{
	u32 reg32;
	const u16 currents[] = { 0xf57, 0xf5f, 0x753, 0x75f, 0x14b, 0x74b,
				 0x557, 0x757, 0x55f, 0x54b
	};
	int i;

	/* Unlock registers.  */
	write_pmbase16(UPRWC, read_pmbase16(UPRWC) | UPRWC_WR_EN);

	for (i = 0; i < TOTAL_USB_PORTS; i++)
		RCBA32_AND_OR(USBIR0 + 4 * i, ~0xfff, currents[portmap[i].current]);

	/* USB Initialization Registers. We follow what EDS recommends here.
	   TODO maybe vendor firmware values are better? */
	RCBA32(USBIRC) &= ~(1 << 8);
	RCBA32_OR(USBIRA, (7 << 12) | (7 << 8) | (7 << 4) | (2 << 0));
	RCBA32_AND_OR(USBIRB, ~0x617f0, (3 << 17) | (1 << 12) | (1 << 10)
		      | (1 << 8) | (4 << 4));
	/* Set to Rate Matching Hub Mode to make PCI devices appear. */
	RCBA32(0x3598) = 0;

	reg32 = 0;
	for (i = 0; i < TOTAL_USB_PORTS; i++) {
		if (!portmap[i].enabled)
			reg32 |= (1 << i);
	}
	RCBA32(USBPDO) = reg32;
	reg32 = 0;
	/* The OC pins of the first 8 USB ports are mapped in USBOCM1 */
	for (i = 0; i < 8; i++) {
		if (portmap[i].enabled && portmap[i].oc_pin >= 0)
			reg32 |= (1 << (i + 8 * portmap[i].oc_pin));
	}
	RCBA32(USBOCM1) = reg32;
	reg32 = 0;
	/* The OC pins of the remainder 6 USB ports are mapped in USBOCM2 */
	for (i = 8; i < TOTAL_USB_PORTS; i++) {
		if (portmap[i].enabled && portmap[i].oc_pin >= 4)
			reg32 |= (1 << (i - 8 + 8 * (portmap[i].oc_pin - 4)));
	}
	RCBA32(USBOCM2) = reg32;

	/* Relock registers. */
	write_pmbase16(UPRWC, 0);
}
