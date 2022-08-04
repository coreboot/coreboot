/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_DFD_COMMON_H
#define SOC_MEDIATEK_DFD_COMMON_H

#define CPC_FLOW_CTRL_CFG	0x0C53A814
#define RESET_ON_KEEP_EN	BIT(17)

/* DFD dump address and size need to be the same as defined in Kernel DTS. */
#define DFD_DUMP_ADDRESS        0x6A000000
#define DFD_DUMP_SIZE           (1 * MiB)

static u32 *const dfd_cfg = (void *)CPC_FLOW_CTRL_CFG;

void dfd_init(void);

#endif
