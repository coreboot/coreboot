/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _COMMONLIB_SDHCI_NONPCI_INFO_H_
#define _COMMONLIB_SDHCI_NONPCI_INFO_H_

#include <stdint.h>

/*
 * Body of LB_TAG_SDHCI_NONPCI (fields following the standard LB tag/size header).
 *
 * Describes SDHCI controllers that are not discoverable via PCI. Payloads
 * such as edk2 register these as non-discoverable MMIO devices.
 */

#define SDHCI_NONPCI_INFO_VERSION	1

/* Matches Baytrail/Braswell SCC_NVS_* indices. */
#define SDHCI_NONPCI_SLOT_EMMC	0
#define SDHCI_NONPCI_SLOT_SDIO	1
#define SDHCI_NONPCI_SLOT_SD	2

#define SDHCI_NONPCI_FLAG_EMBEDDED	(1 << 0)

/* Enough for current SCC (3) with room to grow. */
#define SDHCI_NONPCI_CTRL_MAX	8

struct sdhci_nonpci_control {
	uint32_t mmio_base;
	uint32_t mmio_size;
	uint8_t slot;
	uint8_t flags;
	uint8_t reserved[2];
};

struct sdhci_nonpci_info {
	uint32_t version;
	uint32_t count;
	struct sdhci_nonpci_control ctrl[SDHCI_NONPCI_CTRL_MAX];
};

#endif /* _COMMONLIB_SDHCI_NONPCI_INFO_H_ */
