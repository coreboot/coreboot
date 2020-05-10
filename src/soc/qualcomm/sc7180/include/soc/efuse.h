/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QUALCOMM_SC7180_EFUSE_ADDRESS_MAP_H__
#define __SOC_QUALCOMM_SC7180_EFUSE_ADDRESS_MAP_H__

/**
 *  USB EFUSE registers
 */
struct qfprom_corr {
	u8 rsvd[0x4258 - 0x0];
	u32 qusb_hstx_trim_lsb;
};
check_member(qfprom_corr, qusb_hstx_trim_lsb, 0x4258);

#endif /* __SOC_QUALCOMM_SC7180_EFUSE_ADDRESS_MAP_H__ */
