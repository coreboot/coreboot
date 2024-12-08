/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on Intel Panther Lake Processor PCH Datasheet
 * Document number: TBD
 */

#ifndef _SOC_PANTHERLAKE_UFS_H_
#define _SOC_PANTHERLAKE_UFS_H_

#include <soc/pci_devs.h>

/* Calculate _ADR for Intel UFS Controller */
#define UFS_ACPI_DEVICE (PCI_DEV_SLOT_UFS << 16)

#define R_SCS_CFG_PCS		0x84
#define R_SCS_CFG_PG_CONFIG	0xA2

#endif
