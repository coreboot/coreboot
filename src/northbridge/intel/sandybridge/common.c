/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include "sandybridge.h"

enum platform_type get_platform_type(void)
{
	switch (pci_s_read_config16(HOST_BRIDGE, PCI_DEVICE_ID) & 0xc) {
	case 0x0: /* Desktop */
		return PLATFORM_DESKTOP_SERVER;
	case 0x4: /* Mobile */
		return PLATFORM_MOBILE;
	case 0x8: /* Server */
	default:
		return PLATFORM_DESKTOP_SERVER;
	}
}
