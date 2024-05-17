/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _XEON_SP_SOC_CONFIG_H_
#define _XEON_SP_SOC_CONFIG_H_

enum xeonsp_cxl_mode {
	XEONSP_CXL_DISABLED = 0,
	XEONSP_CXL_SYS_MEM,
	XEONSP_CXL_SP_MEM,
};

enum xeonsp_cxl_mode get_cxl_mode(void);

enum xeonsp_fast_boot_mode {
	XEONSP_FAST_BOOT_DISABLED = 0x0,
	XEONSP_FAST_BOOT_COLD     = 0x1,
	XEONSP_FAST_BOOT_WARM     = 0x2,
};

enum xeonsp_fast_boot_mode get_fast_boot_mode(void);

#endif
