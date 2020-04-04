/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __SOC_MEDIATEK_MT8173_INFRACFG_H__
#define __SOC_MEDIATEK_MT8173_INFRACFG_H__

#include <soc/addressmap.h>
#include <types.h>

struct mt8173_infracfg_regs {
	u32 top_ckmuxsel;
	u8 reserved0[4];
	u32 top_ckdiv1;
	u8 reserved1[4];
	u32 top_dcmctl;
	u32 top_dcmdbc;
	u8 reserved2[24];
	u32 infra_rst0;
	u32 infra_rst1;
	u8 reserved3[8];
	u32 infra_pdn0;
	u32 infra_pdn1;
	u32 infra_pdn_sta;
	u8 reserved4[4];
	u32 infra_dcmctl;
	u32 infra_dcmdbc;
	u32 infra_dcmfsel;
	u8 reserved5[20];
	u32 devapc_pdn0;
	u32 devapc_pdn1;
	u32 devapc_pdn_sta;
	u8 reserved6[4];
	u32 trng_pdn0;
	u32 trng_pdn1;
	u32 trng_pdn_sta;
	u8 reserved7[4];
	u32 infra_pdn_sen;
	u8 reserved8[268];
	u32 infra_ao_mbist_delsel;
	u32 infra_ao_mbist_bsel;
	u32 infra_ao_mbist_cfg;
	u32 infra_ao_mbist_fuse_sramrom;
	u32 infra_ao_mbist_fuse_afe;
	u32 infra_ao_mbist_holdb;
	u32 infra_ao_mbist_mode;
	u32 infra_ao_mbist_mon_sel;
	u32 infra_ao_mbist_result;
	u8 reserved9[44];
	u32 infra_ao_mbist_fuse_mon;
	u8 reserved10[12];
	u32 topaxi_si0_ctl;
	u32 topaxi_si1_ctl;
	u8 reserved11[4];
	u32 infra_mci_si0_ctl;
	u32 infra_mci_si1_ctl;
	u32 infra_mci_si2_ctl;
	u32 infra_mci_async_ctrl;
	u32 infra_mci_cg_mfg_sec_sta;
	u32 topaxi_prot_en;
	u32 topaxi_prot_sta0;
	u32 topaxi_prot_sta1;
	u32 topaxi_axi_aslice_ctrl;
	u32 infra_apb_async_sta;
	u8 reserved12[12];
	u32 infra_mci_trans_con_read;
	u32 infra_mci_trans_con_write;
	u32 infra_mci_id_remap_con;
	u32 infra_mci_emi_trans_con;
	u8 reserved13[196];
	u32 cldma_map0;
	u8 reserved14[232];
	u32 peri_cci_sideband_con;
	u32 mfg_cci_sideband_con;
	u8 reserved15[248];
	u32 infra_ao_dbg_con0;
	u32 infra_ao_dbg_con1;
	u32 infra_ao_dbg_con2;
	u32 infra_ao_dbg_con3;
	u8 reserved16[752];
	u32 sramrom_boot_addr;
	u32 sramrom_sec_ctrl;
	u32 sramrom_sec_addr;
	u32 sramrom_fpc_boot_addr;
	u32 sramrom_fpc_boot_con;
	u8 reserved17[236];
	u32 infra_bonding;
	u8 reserved18[252];
	u32 infra_ao_scpsys_apb_async_sta;
	u32 infra_ao_md32_tx_apb_async_sta;
	u32 infra_ao_md32_rx_apb_async_sta;
	u32 infra_ao_cksys_apb_async_sta;
	u8 reserved19[1264];
	u32 infra_misc;
	u32 infra_acp;
};

check_member(mt8173_infracfg_regs, infra_pdn0, 0x40);
check_member(mt8173_infracfg_regs, topaxi_prot_sta1, 0x228);
check_member(mt8173_infracfg_regs, infra_misc, 0xf00);

static struct mt8173_infracfg_regs *const mt8173_infracfg =
	(void *)INFRACFG_AO_BASE;

enum {
	INFRA_PMIC_WRAP_RST = 1 << 7,
	L2C_SRAM_PDN = 1 << 7
};

enum {
	DDR_4GB_SUPPORT_EN = 1 << 13
};

#endif	/* __SOC_MEDIATEK_MT8173_INFRACFG_H__ */
