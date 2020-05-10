/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QUALCOMM_SDM845_EFUSE_ADDRESS_MAP_H__
#define __SOC_QUALCOMM_SDM845_EFUSE_ADDRESS_MAP_H__

/**
 *  USB EFUSE registers
 */
struct qfprom_corr {
	u8 rsvd[0x41E8 - 0x0];
	u32 qusb_hstx_trim_lsb;
	u32 qusb_hstx_trim_msb;
};

check_member(qfprom_corr, qusb_hstx_trim_lsb, 0x41E8);
check_member(qfprom_corr, qusb_hstx_trim_msb, 0x41EC);
#endif /* __SOC_QUALCOMM_SDM845_EFUSE_ADDRESS_MAP_H__ */
