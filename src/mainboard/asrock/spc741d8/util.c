/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/config.h>

#if CONFIG(SOC_INTEL_HAS_CXL)
enum xeonsp_cxl_mode get_cxl_mode(void)
{
	return XEONSP_CXL_SYS_MEM;
}
#endif
