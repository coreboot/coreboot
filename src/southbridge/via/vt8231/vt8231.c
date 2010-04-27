#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>

#include <pc80/mc146818rtc.h>
#include <pc80/keyboard.h>

#include "chip.h"

/* Base 8231 controller */
static device_t lpc_dev;

static void keyboard_on(void)
{
	unsigned char regval;

	if (lpc_dev) {
		regval = pci_read_config8(lpc_dev, 0x51);
		regval |= 0x0f;
		pci_write_config8(lpc_dev, 0x51, regval);
	}
	pc_keyboard_init(0);
}

static void com_port_on(void)
{
#if 0
	// enable com1 and com2.
	enables = pci_read_config8(dev, 0x6e);

	/* 0x80 is enable com port b, 0x10 is to make it com2, 0x8
	 * is enable com port a as com1 kevinh/Ispiri - Old code
	 * thought 0x01 would make it com1, that was wrong enables =
	 * 0x80 | 0x10 | 0x8 ; pci_write_config8(dev, 0x6e,
	 * enables); // note: this is also a redo of some port of
	 * assembly, but we want everything up.
	 */

	/* set com1 to 115 kbaud not clear how to do this yet.
	 * forget it; done in assembly.
	 */
#endif
}

/* FixME: to be removed ? */
static void vt8231_enable(struct device *dev)
{
	struct southbridge_via_vt8231_config *conf = dev->chip_info;

	if (!lpc_dev) {
		/* the first time called, enable devices not on PCI bus
		 * FIXME: is that device struct there yet? */
		lpc_dev = dev_find_device(PCI_VENDOR_ID_VIA,
					  PCI_DEVICE_ID_VIA_8231, 0);
		if (conf->enable_keyboard)
			keyboard_on();
		if (conf->enable_com_ports)
			com_port_on();
	}
}

struct chip_operations southbridge_via_vt8231_ops = {
	CHIP_NAME("VIA VT8231 Southbridge")
	.enable_dev	= vt8231_enable,
};
