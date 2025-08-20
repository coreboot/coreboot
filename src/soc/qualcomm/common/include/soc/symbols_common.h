/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_SYMBOLS_COMMON_H_
#define _SOC_QUALCOMM_SYMBOLS_COMMON_H_

#include <symbols.h>

DECLARE_REGION(ddr_training);
DECLARE_REGION(qclib_serial_log);
DECLARE_REGION(ddr_information);
DECLARE_REGION(ssram)
DECLARE_REGION(bsram)
DECLARE_REGION(dram_aop)
DECLARE_REGION(dram_soc)
DECLARE_REGION(dcb)
DECLARE_REGION(dtb)
DECLARE_REGION(cpr_settings)
DECLARE_REGION(qc_blob_meta)
DECLARE_REGION(aop_blob_meta)
DECLARE_OPTIONAL_REGION(pmic)
DECLARE_REGION(limits_cfg)
DECLARE_REGION(aop)
DECLARE_REGION(modem_id)
DECLARE_REGION(aop_code_ram)
DECLARE_REGION(aop_data_ram)
DECLARE_REGION(dram_modem_wifi_only)
DECLARE_REGION(dram_modem_extra)
DECLARE_REGION(dram_wlan)
DECLARE_REGION(dram_wpss)
DECLARE_REGION(shrm)
DECLARE_REGION(dram_cpucp_dtbs)
DECLARE_REGION(dram_cpucp)
DECLARE_REGION(dram_modem)
DECLARE_REGION(dram_tz)
DECLARE_REGION(dram_tz_rem)
DECLARE_REGION(dram_ramdump)
DECLARE_REGION(dram_smem)

/*
 * DDR_SPACE (2 GB) aka `_dram`:  0x80000000 - 0x100000000
 * DDR_SPACE_1 (30 GB) aka `_dram_space_1`: 0x880000000 - 0x1000000000
 * DDR_SPACE_2 (480 GB) aka `dram_space_2`: 0x8800000000 - 0x10000000000
 */
DECLARE_OPTIONAL_REGION(dram_space_1)
DECLARE_OPTIONAL_REGION(dram_space_2)

#endif // _SOC_QUALCOMM_SYMBOLS_COMMON_H_
