/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <southbridge/intel/common/rcba.h>
#include <southbridge/intel/common/pmbase.h>

#include "pch.h"
#include "chip.h"

struct rcba_usb_init {
	u16 offset;
	u32 and;
	u32 or;
};

static const struct rcba_usb_init usb_mobile[] = {
	{0x3560, ~0, 0x20c8000},
	{0x3564, ~0x135c, 0x4a3},
	{0x3570, ~0x4000111, 0x2000771},
	{0x357c, ~0x603c0, 0x1630},
	{0x3580, ~0x3, 0},
	{0x3598, ~0x1, 0},
	{0, 0, 0}	/* End marker */
};

static const struct rcba_usb_init usb_desktop[] = {
	{0x3560, ~0, 0x20c8000},
	{0x3564, ~0x135c, 0x4a3},
	{0x3570, ~0x4000111, 0x2000661},
	{0x357c, ~0x603c0, 0x1530},
	{0x3580, ~0x3, 0},
	{0x3598, ~0x1, 0},
	{0, 0, 0}	/* End marker */
};

__weak uint16_t mb_usb20_port_override(void)
{
	return 0x3fff;
}

void early_usb_init(void)
{
	const struct rcba_usb_init *usb_init;
	u32 reg32;
	int i;

	usb_init = pch_is_mobile() ? usb_mobile : usb_desktop;
	for (i = 0; usb_init[i].offset; i++)
		RCBA32_AND_OR(usb_init[i].offset, usb_init[i].and, usb_init[i].or);

	/* Care should be taken to limit this array to not more than 80 (0x50) entries.
	 * See below. */
	const u32 currents[] = { USBIR_TXRX_GAIN_MOBILE_LOW, USBIR_TXRX_GAIN_DEFAULT,
				 USBIR_TXRX_GAIN_HIGH, 0x20000f51, 0x2000094a, 0x2000035f,
				 USBIR_TXRX_GAIN_DESKTOP6_LOW, USBIR_TXRX_GAIN_DESKTOP6_HIGH,
				 USBIR_TXRX_GAIN_DESKTOP7_LOW, USBIR_TXRX_GAIN_DESKTOP7_MED,
				 0x20000053, 0x2000055f, 0x20000f5f};
	const struct device *dev = pcidev_on_root(0x1d, 0);
	const struct southbridge_intel_bd82x6x_config *config = dev->chip_info;
	const struct southbridge_usb_port *portmap = config->usb_port_config;

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

	reg32 = 0;
	for (i = 0; i < 14; i++)
		if (!portmap[i].enabled)
			reg32 |= (1 << i);

	/* Allow mainboard to disable ports based on SKU or user config */
	reg32 |= ~mb_usb20_port_override();

	RCBA32(USBPDO) = reg32 & 0x3fff;
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

	pci_write_config32(PCH_XHCI_DEV, 0xe4, 0x00000000);

	/* Relock registers.  */
	write_pmbase16(UPRWC, 0);
}
