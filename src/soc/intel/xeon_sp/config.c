/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/config.h>

__weak enum xeonsp_cxl_mode get_cxl_mode(void)
{
	return XEONSP_CXL_DISABLED;
}
