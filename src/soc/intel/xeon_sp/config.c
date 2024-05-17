/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/config.h>

__weak enum xeonsp_cxl_mode get_cxl_mode(void)
{
	return XEONSP_CXL_DISABLED;
}

__weak enum xeonsp_fast_boot_mode get_fast_boot_mode(void)
{
	return XEONSP_FAST_BOOT_COLD | XEONSP_FAST_BOOT_WARM;
}
