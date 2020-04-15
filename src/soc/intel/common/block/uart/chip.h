/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
/* Indirect include for static.c: */
#include <device/pci_ids.h>

#ifndef _SOC_INTEL_COMMON_BLOCK_UART_CHIP_H_
#define _SOC_INTEL_COMMON_BLOCK_UART_CHIP_H_

struct soc_intel_common_block_uart_config {
	/* The Device ID read from config space at offset[2:4] when not hidden */
	u16 devid;
};

#endif /* _SOC_INTEL_COMMON_BLOCK_UART_CHIP_H_ */
