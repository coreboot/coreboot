/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOUTHBRIDGE_INTEL_COMMON_EARLY_SPI_H
#define SOUTHBRIDGE_INTEL_COMMON_EARLY_SPI_H

#include <device/pci_ops.h>

static inline void enable_spi_prefetching_and_caching(void)
{
	pci_update_config8(PCI_DEV(0, 0x1f, 0), 0xdc, ~(3 << 2), 2 << 2);
}

#endif
