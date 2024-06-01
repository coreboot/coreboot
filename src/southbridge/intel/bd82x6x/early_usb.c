/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <southbridge/intel/common/rcba.h>
#include <southbridge/intel/common/pmbase.h>

#include "pch.h"

void early_usb_init(const struct southbridge_usb_port *portmap)
{
	u32 reg32;
	const u32 rcba_dump[8] = {
		/* 3560 */ 0x024c8001, 0x000024a3, 0x00040002, 0x01000050,
		/* 3570 */ 0x02000772, 0x16000f9f, 0x1800ff4f, 0x0001d630,
	};
	/* Care should be taken to limit this array to not more than 80 (0x50) entries.
	 * See below. */
	const u32 currents[] = { USBIR_TXRX_GAIN_MOBILE_LOW, USBIR_TXRX_GAIN_DEFAULT,
				 USBIR_TXRX_GAIN_HIGH, 0x20000f51, 0x2000094a, 0x2000035f,
				 USBIR_TXRX_GAIN_DESKTOP6_LOW, USBIR_TXRX_GAIN_DESKTOP6_HIGH,
				 USBIR_TXRX_GAIN_DESKTOP7_LOW, USBIR_TXRX_GAIN_DESKTOP7_MED,
				 0x20000053, 0x2000055f, 0x20000f5f};
	int i;

	/* Unlock registers.  */
	write_pmbase16(UPRWC, read_pmbase16(UPRWC) | UPRWC_WR_EN);

	for (i = 0; i < 14; i++) {
		/*
		 * If the value from devicetree is beyond the highest possible current map
		 * index, it is meant to go directly into (bottom 12 bits of) USBIRx.
		 */
		if (portmap[i].current >= ARRAY_SIZE(currents)) {
			RCBA32(USBIR0 + 4 * i) = 0x20000000 | (portmap[i].current & 0xfff);
			continue;
		}
		if (portmap[i].enabled && !pch_is_mobile() &&
		    currents[portmap[i].current] == USBIR_TXRX_GAIN_MOBILE_LOW) {
			/*
			 * Note for developers: You can fix this by re-running autoport on
			 * vendor firmware and then updating portmap currents accordingly.
			 * If that is not possible, another option is to choose a non-zero
			 * current setting. In either case, please test all the USB ports.
			 */
			printk(BIOS_ERR, "%s: USB%02d: USBIR_TXRX_GAIN_MOBILE_LOW is an invalid setting for desktop!\n",
			       __func__, i);

			RCBA32(USBIR0 + 4 * i) = USBIR_TXRX_GAIN_DEFAULT;
		} else {
			RCBA32(USBIR0 + 4 * i) = currents[portmap[i].current];
		}
	}
	for (i = 0; i < 10; i++)
		RCBA32(0x3538 + 4 * i) = 0;

	for (i = 0; i < 8; i++)
		RCBA32(0x3560 + 4 * i) = rcba_dump[i];
	for (i = 0; i < 8; i++)
		RCBA32(0x3580 + 4 * i) = 0;
	reg32 = 0;
	for (i = 0; i < 14; i++)
		if (!portmap[i].enabled)
			reg32 |= (1 << i);
	RCBA32(USBPDO) = reg32;
	reg32 = 0;
	for (i = 0; i < 8; i++)
		if (portmap[i].enabled && portmap[i].oc_pin >= 0)
			reg32 |= (1 << (i + 8 * portmap[i].oc_pin));
	RCBA32(USBOCM1) = reg32;
	reg32 = 0;
	for (i = 8; i < 14; i++)
		if (portmap[i].enabled && portmap[i].oc_pin >= 4)
			reg32 |= (1 << (i - 8 + 8 * (portmap[i].oc_pin - 4)));
	RCBA32(USBOCM2) = reg32;
	for (i = 0; i < 22; i++)
		RCBA32(0x35a8 + 4 * i) = 0;

	pci_write_config32(PCH_XHCI_DEV, 0xe4, 0x00000000);

	/* Relock registers.  */
	write_pmbase16(UPRWC, 0);
}
