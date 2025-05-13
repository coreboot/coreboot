/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __SOC_MEDIATEK_MT8189_INCLUDE_SOC_SPM_MTCMOS_H__
#define __SOC_MEDIATEK_MT8189_INCLUDE_SOC_SPM_MTCMOS_H__

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <types.h>

struct mtk_reg_cfg {
	u32 status;
	u32 set;
	u32 clr;
	u32 ready;
};

struct mtk_vlpcfg_regs {
	u32 reserved1;
	u32 vlp_test_ck_ctrl;
	u32 reserved2[130];
	u32 bus_vlp_topaxi_protecten;
	u32 bus_vlp_topaxi_protecten_set;
	u32 bus_vlp_topaxi_protecten_clr;
	u32 bus_vlp_topaxi_protecten_sta1;
};
check_member(mtk_vlpcfg_regs, vlp_test_ck_ctrl, 0x0004);
check_member(mtk_vlpcfg_regs, bus_vlp_topaxi_protecten, 0x0210);

struct mtk_infracfg_ao_regs {
	u32 reserved1[28];
	u32 infra_bus_dcm_ctrl;
	u32 reserved2[3];
	u32 infracfg_ao_module_cg_0_set;
	u32 infracfg_ao_module_cg_0_clr;
	u32 infracfg_ao_module_cg_1_set;
	u32 infracfg_ao_module_cg_1_clr;
	u32 infracfg_ao_module_cg_0;
	u32 infracfg_ao_module_cg_1;
	u32 reserved3[3];
	u32 infracfg_ao_module_cg_2_set;
	u32 infracfg_ao_module_cg_2_clr;
	u32 infracfg_ao_module_cg_2;
	u32 reserved4[4];
	u32 infracfg_ao_module_cg_3_set;
	u32 infracfg_ao_module_cg_3_clr;
	u32 infracfg_ao_module_cg_3;
	u32 reserved5[721];
	struct mtk_reg_cfg mmsys_protect[2];
	u32 reserved6[4];
	struct mtk_reg_cfg infrasys_protect[2];
	struct mtk_reg_cfg emisys_protect;
	u32 reserved7[4];
	struct mtk_reg_cfg perisys_protect;
	struct mtk_reg_cfg mcu_connsys_protect;
	struct mtk_reg_cfg md_mfgsys_protect;
};
check_member(mtk_infracfg_ao_regs, infra_bus_dcm_ctrl, 0x0070);
check_member(mtk_infracfg_ao_regs, infracfg_ao_module_cg_0_set, 0x0080);
check_member(mtk_infracfg_ao_regs, infracfg_ao_module_cg_1, 0x0094);
check_member(mtk_infracfg_ao_regs, infracfg_ao_module_cg_2_set, 0x00A4);
check_member(mtk_infracfg_ao_regs, infracfg_ao_module_cg_3_set, 0x00C0);
check_member(mtk_infracfg_ao_regs, mmsys_protect[0].status, 0x0C10);
check_member(mtk_infracfg_ao_regs, mmsys_protect[0].set, 0x0C14);
check_member(mtk_infracfg_ao_regs, mmsys_protect[0].clr, 0x0C18);
check_member(mtk_infracfg_ao_regs, mmsys_protect[0].ready, 0x0C1C);
check_member(mtk_infracfg_ao_regs, infrasys_protect[0].status, 0x0C40);
check_member(mtk_infracfg_ao_regs, infrasys_protect[0].set, 0x0C44);
check_member(mtk_infracfg_ao_regs, infrasys_protect[0].clr, 0x0C48);
check_member(mtk_infracfg_ao_regs, infrasys_protect[0].ready, 0x0C4C);
check_member(mtk_infracfg_ao_regs, perisys_protect.status, 0x0C80);
check_member(mtk_infracfg_ao_regs, perisys_protect.set, 0x0C84);
check_member(mtk_infracfg_ao_regs, perisys_protect.clr, 0x0C88);
check_member(mtk_infracfg_ao_regs, perisys_protect.ready, 0x0C8C);

struct mtk_emicfg_ao_mem_regs {
	u32 reserved[32];
	struct mtk_reg_cfg gals_slp_prot;
};
check_member(mtk_emicfg_ao_mem_regs, gals_slp_prot.status, 0x0080);
check_member(mtk_emicfg_ao_mem_regs, gals_slp_prot.set, 0x0084);
check_member(mtk_emicfg_ao_mem_regs, gals_slp_prot.clr, 0x0088);
check_member(mtk_emicfg_ao_mem_regs, gals_slp_prot.ready, 0x008C);

static struct mtk_vlpcfg_regs *const mtk_vlpcfg = (void *)VLPCFG_REG_BASE;
static struct mtk_infracfg_ao_regs *const mtk_infracfg_ao = (void *)INFRACFG_AO_BASE;
static struct mtk_emicfg_ao_mem_regs *const mtk_emicfg_ao_mem = (void *)EMICFG_AO_MEM_BASE;

void spm_power_on(void);
void mtcmos_init(void);

#endif /* __SOC_MEDIATEK_MT8189_INCLUDE_SOC_SPM_MTCMOS_H__ */
