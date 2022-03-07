/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#include "chip.h"
#include "hudson.h"

static void sd_init(struct device *dev)
{
	struct southbridge_amd_pi_hudson_config *sd_chip = dev->chip_info;
	u32 stepping = pci_read_config32(pcidev_on_root(0x18, 3), 0xFC);
	u8 sd_mode = 0;

	if (sd_chip)
		sd_mode = sd_chip->sd_mode;

	if (sd_mode == 3) {	/* SD 3.0 mode */
		pci_write_config32(dev, 0xA4, 0x31FEC8B2);
		pci_write_config32(dev, 0xA8, 0x00002503);
		pci_write_config32(dev, 0xB0, 0x02180C19);
		pci_write_config32(dev, 0xD0, 0x0000078B);
	}
	else {				/* SD 2.0 mode */
		if ((stepping & 0x0000000F) == 0) {	/* Stepping A0 */
			pci_write_config32(dev, 0xA4, 0x31DE32B2);
			pci_write_config32(dev, 0xB0, 0x01180C19);
			pci_write_config32(dev, 0xD0, 0x0000058B);
		}
		else {					/* Stepping >= A1 */
			pci_write_config32(dev, 0xA4, 0x31FE32B2);
			pci_write_config32(dev, 0xA8, 0x00000070);
			pci_write_config32(dev, 0xB0, 0x01180C19);
			pci_write_config32(dev, 0xD0, 0x0000078B);
		}
	}
}

static struct device_operations sd_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = sd_init,
};

static const struct pci_driver sd_driver __pci_driver = {
	.ops = &sd_ops,
	.vendor = PCI_VID_AMD,
	.device = PCI_DID_AMD_YANGTZE_SD,
};
