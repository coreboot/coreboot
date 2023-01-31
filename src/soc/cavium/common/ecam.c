/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Derived from Cavium's BSD-3 Clause OCTEONTX-SDK-6.2.0.
 */

#define __SIMPLE_DEVICE__

#include <device/pci_ops.h>
#include <device/pci.h>
#include <soc/addressmap.h>
#include <soc/ecam.h>

/**
 * Get PCI BAR address from cavium specific extended capability.
 * Use regular BAR if not found in extended capability space.
 *
 * @return The physical address of the BAR, zero on error
 */
uint64_t ecam0_get_bar_val(pci_devfn_t dev, u8 bar)
{
	size_t cap_offset = pci_s_find_capability(dev, 0x14);
	uint64_t h, l, ret = 0;
	if (cap_offset) {
		/* Found EA */
		u8 es, bei;
		u8 ne = pci_read_config8(dev, cap_offset + 2) & 0x3f;

		cap_offset += 4;
		while (ne) {
			uint32_t dw0 = pci_read_config32(dev, cap_offset);

			es = dw0 & 7;
			bei = (dw0 >> 4) & 0xf;
			if (bei == bar) {
				h = 0;
				l = pci_read_config32(dev, cap_offset + 4);
				if (l & 2)
					h = pci_read_config32(dev,
							      cap_offset + 12);
				ret = (h << 32) | (l & ~0xfull);
				break;
			}
			cap_offset += (es + 1) * 4;
			ne--;
		}
	} else {
		h = 0;
		l = pci_read_config32(dev, bar * 4 + PCI_BASE_ADDRESS_0);
		if (l & 4)
			h = pci_read_config32(dev, bar * 4 + PCI_BASE_ADDRESS_0
					      + 4);
		ret = (h << 32) | (l & ~0xfull);
	}
	return ret;
}
