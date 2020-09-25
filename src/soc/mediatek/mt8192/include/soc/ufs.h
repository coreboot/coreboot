/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8192_UFS_H
#define SOC_MEDIATEK_MT8192_UFS_H

#include <device/mmio.h>
#include <soc/addressmap.h>

void ufs_disable_refclk(void);

enum ufshci_offset {
	REG_UFS_REFCLK_CTRL = 0x144,
};

#define UFS_REFCLK_CTRL	(UFSHCI_BASE + REG_UFS_REFCLK_CTRL)

#endif /* SOC_MEDIATEK_MT8192_UFS_H */
