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
DECLARE_OPTIONAL_REGION(dram_aop_cmd_db)
DECLARE_REGION(dram_aop_config)
DECLARE_REGION(dram_soc)
DECLARE_REGION(dcb)
DECLARE_OPTIONAL_REGION(delta_dcb)
DECLARE_REGION(dtb)
DECLARE_REGION(cpr_settings)
DECLARE_REGION(qc_blob_meta)
DECLARE_REGION(aop_blob_meta)
DECLARE_REGION(apdp_ramdump_meta)
DECLARE_OPTIONAL_REGION(cdt_data)
DECLARE_OPTIONAL_REGION(pmic)
DECLARE_REGION(limits_cfg)
DECLARE_REGION(aop)
DECLARE_REGION(modem_id)
DECLARE_REGION(aop_code_ram)
DECLARE_REGION(aop_data_ram)
DECLARE_OPTIONAL_REGION(spel_rvss_iram)
DECLARE_OPTIONAL_REGION(spel_rvss_dram)
DECLARE_REGION(dram_modem_wifi_only)
DECLARE_REGION(dram_modem_extra)
DECLARE_REGION(dram_wlan)
DECLARE_REGION(dram_wpss)
DECLARE_REGION(shrm)
DECLARE_REGION(cpucp)
DECLARE_REGION(pdp)
DECLARE_REGION(dram_cpucp_dtbs)
DECLARE_REGION(dram_cpucp)
DECLARE_REGION(dram_modem)
DECLARE_REGION(dram_tz)
DECLARE_OPTIONAL_REGION(dram_tz_ac)
DECLARE_OPTIONAL_REGION(dram_hyp_ac)
DECLARE_REGION(dram_ramdump)
DECLARE_REGION(dram_smem)
DECLARE_REGION(dram_ncc)
DECLARE_REGION(dram_xbl_log)
DECLARE_REGION(dram_tme_crashdump)
DECLARE_REGION(dram_tme_log)
DECLARE_REGION(dram_dc_log)
DECLARE_REGION(dram_gpu_prr)
DECLARE_REGION(dram_tpm_ctrl)
DECLARE_REGION(dram_usb_ucsi)
DECLARE_REGION(dram_qcskext)
DECLARE_REGION(dram_qup_fw)
DECLARE_REGION(dram_softsku)
DECLARE_REGION(dram_tz_static)
DECLARE_REGION(dram_acpi_ta)
DECLARE_REGION(dram_xbl_scratch_buf2)
DECLARE_REGION(dram_xbl_scratch_buf1)
DECLARE_REGION(dram_adsp_rpc_heap)
DECLARE_REGION(dram_spu_secure)
DECLARE_REGION(dram_smmu_pt)
DECLARE_REGION(dram_llcc_lpi)
DECLARE_REGION(dram_ta)
DECLARE_REGION(dram_bert)
DECLARE_REGION(dram_pdp)
DECLARE_REGION(dram_pdp_cdb)
DECLARE_REGION(dram_pdp_ns)
DECLARE_REGION(dram_pil)
DECLARE_REGION(shared_imem)
DECLARE_REGION(dram_pld_pep)
DECLARE_REGION(dram_pld_gmu)
DECLARE_REGION(dram_pld_pdp)
DECLARE_REGION(dram_oob_glink_always)
DECLARE_REGION(dram_oob_glink_assist)
DECLARE_REGION(dram_oob_mdm_assist)
DECLARE_REGION(dram_oob_wlan_assist)
DECLARE_OPTIONAL_REGION(pcie)

/*
 * DDR_SPACE (2 GB) aka `_dram`:  0x80000000 - 0x100000000
 * DDR_SPACE_1 (30 GB) aka `_dram_space_1`: 0x880000000 - 0x1000000000
 * DDR_SPACE_2 (480 GB) aka `dram_space_2`: 0x8800000000 - 0x10000000000
 */
DECLARE_OPTIONAL_REGION(dram_space_1)
DECLARE_OPTIONAL_REGION(dram_space_2)

#endif // _SOC_QUALCOMM_SYMBOLS_COMMON_H_
