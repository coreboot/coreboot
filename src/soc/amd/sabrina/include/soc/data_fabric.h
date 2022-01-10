/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Check if this is still correct */

#ifndef AMD_SABRINA_DATA_FABRIC_H
#define AMD_SABRINA_DATA_FABRIC_H

#include <types.h>

/* SoC-specific bits in D18F0_MMIO_CTRL0 */
#define   DF_MMIO_NP			BIT(16)

#define IOMS0_FABRIC_ID			10

#define NUM_NB_MMIO_REGS		8

void data_fabric_set_mmio_np(void);

#endif /* AMD_SABRINA_DATA_FABRIC_H */
