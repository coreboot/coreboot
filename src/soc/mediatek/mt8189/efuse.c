/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/efuse.h>

#define MTK_EFUSE_ECC_ERROR_BIT BIT(7)

bool mtk_efuse_ecc_has_error(void)
{
	return !!(read32(&mtk_efuse->ecc_status_reg) & MTK_EFUSE_ECC_ERROR_BIT);
}
