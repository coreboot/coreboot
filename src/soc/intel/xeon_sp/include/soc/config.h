/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _XEON_SP_SOC_CONFIG_H_
#define _XEON_SP_SOC_CONFIG_H_

enum xeonsp_cxl_mode {
	XEONSP_CXL_DISABLED = 0,
	XEONSP_CXL_SYS_MEM,
	XEONSP_CXL_SP_MEM,
};

enum xeonsp_cxl_mode get_cxl_mode(void);

#endif
