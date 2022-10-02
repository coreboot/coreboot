/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOUTHBRIDGE_INTEL_COMMON_EARLY_SMBUS_H
#define SOUTHBRIDGE_INTEL_COMMON_EARLY_SMBUS_H

#include <device/pci_def.h>
#include <device/pci_type.h>

#define PCI_DEV_SMBUS		PCI_DEV(0, 0x1f, 3)

#define SMB_BASE		PCI_BASE_ADDRESS_4
#define HOSTC			0x40

/* HOSTC bits */
#define I2C_EN			(1 << 2)
#define SMB_SMI_EN		(1 << 1)
#define HST_EN			(1 << 0)

#endif /* SOUTHBRIDGE_INTEL_COMMON_EARLY_SMBUS_H */
