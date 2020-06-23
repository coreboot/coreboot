/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/dramc_pi_api.h>
#include <soc/dramc_register.h>
#include <soc/infracfg.h>
#include <symbols.h>

static struct emi_regs *emi_reg = (void *)EMI_BASE;
static struct emi_mpu_regs *const emi_mpu = (void *)EMI_MPU_BASE;
static struct infra_ao_mem_regs *infra_ao_mem = (void *)INFRACFG_AO_MEM_BASE;

static void emi_center_config(void)
{
	/* set EMI default settings which EMI hardware needs */
	write32(&emi_reg->cona, 0xf053f154);
	write32(&emi_reg->conp, 0x182e2d33);
	write32(&emi_reg->conb, 0x0f251025);
	write32(&emi_reg->conq, 0x122a1027);
	write32(&emi_reg->conc, 0x1a31162d);
	write32(&emi_reg->conb_2nd, 0x182e2d33);
	write32(&emi_reg->conc_2nd, 0x0f251025);
	write32(&emi_reg->conp_2nd, 0x122a1027);
	write32(&emi_reg->conq_2nd, 0x1a31162d);
	write32(&emi_reg->conb_3rd, 0x1024202c);
	write32(&emi_reg->conc_3rd, 0x0b210c21);
	write32(&emi_reg->conp_3rd, 0x0f250d23);
	write32(&emi_reg->conq_3rd, 0x152b1228);
	write32(&emi_reg->conb_4th, 0x0c201a28);
	write32(&emi_reg->conc_4th, 0x0d230a20);
	write32(&emi_reg->conp_4th, 0x0e260d24);
	write32(&emi_reg->conq_4th, 0x132d1229);
	write32(&emi_reg->conb_5th, 0x0c201a28);
	write32(&emi_reg->conc_5th, 0x0d230a20);
	write32(&emi_reg->conp_5th, 0x0e260d24);
	write32(&emi_reg->conq_5th, 0x132d1229);
	write32(&emi_reg->conb_6th, 0x0c201a28);
	write32(&emi_reg->conc_6th, 0x0d230a20);
	write32(&emi_reg->conp_6th, 0x0e260d24);
	write32(&emi_reg->conq_6th, 0x132d1229);
	write32(&emi_reg->conb_7th, 0x0e290e28);
	write32(&emi_reg->conc_7th, 0x091e1322);
	write32(&emi_reg->mpud26_st, 0x0f29112a);
	write32(&emi_reg->conq_7th, 0x0c240a1f);
	write32(&emi_reg->conb_8th, 0x0e290e28);
	write32(&emi_reg->conc_8th, 0x091e1322);
	write32(&emi_reg->conp_8th, 0x0f29112a);
	write32(&emi_reg->conq_8th, 0x0c240a1f);
	write32(&emi_reg->cong, 0x37373a57);
	write32(&emi_reg->conr, 0x3f3f3c39);
	write32(&emi_reg->cong_2nd, 0x3836374e);
	write32(&emi_reg->conr_2nd, 0x41413d3a);
	write32(&emi_reg->cong_3rd, 0x33313241);
	write32(&emi_reg->conr_3rd, 0x3a3a3835);
	write32(&emi_reg->cong_4th, 0x34343542);
	write32(&emi_reg->conr_4th, 0x3b3b3835);
	write32(&emi_reg->cong_5th, 0x34343542);
	write32(&emi_reg->conr_5th, 0x3b3b3835);
	write32(&emi_reg->cong_6th, 0x34343542);
	write32(&emi_reg->conr_6th, 0x3b3b3835);
	write32(&emi_reg->cong_7th, 0x37333034);
	write32(&emi_reg->conr_7th, 0x39393a39);
	write32(&emi_reg->cong_8th, 0x37333034);
	write32(&emi_reg->conr_8th, 0x39393a39);
	write32(&emi_reg->cond, 0x3657587a);
	write32(&emi_reg->cone, 0x0000c042);
	write32(&emi_reg->conf, 0x08421000);
	write32(&emi_reg->conh, 0x00000083);
	write32(&emi_reg->conh_2nd, 0x00073210);
	write32(&emi_reg->coni, 0x00008802);
	write32(&emi_reg->conj, 0x00000000);
	write32(&emi_reg->conm, 0x007812ff);
	write32(&emi_reg->conn, 0x00000000);
	write32(&emi_reg->mdct, 0x11120c1f);
	write32(&emi_reg->shf0, 0x11120c1f);
	write32(&emi_reg->mdct_2nd, 0x00001123);
	write32(&emi_reg->shf1, 0x00001123);
	write32(&emi_reg->iocl, 0xa8a8a8a8);
	write32(&emi_reg->iocl_2nd, 0x25252525);
	write32(&emi_reg->iocm, 0xa8a8a8a8);
	write32(&emi_reg->iocm_2nd, 0x25252525);
	write32(&emi_reg->testb, 0x00060037);
	write32(&emi_reg->testc, 0x384a0014);
	write32(&emi_reg->testd, 0xa0000000);
	write32(&emi_reg->arba, 0x20107244);
	write32(&emi_reg->arbb, 0x10107044);
	write32(&emi_reg->arbc, 0x343450df);
	write32(&emi_reg->arbd, 0x0000f0d0);
	write32(&emi_reg->arbe, 0x10106048);
	write32(&emi_reg->arbf, 0x343450df);
	write32(&emi_reg->arbg, 0x83837044);
	write32(&emi_reg->arbh, 0x83837044);
	write32(&emi_reg->arbi, 0x00007108);
	write32(&emi_reg->arbi_2nd, 0x00007108);
	write32(&emi_reg->arbk, 0x090a4000);
	write32(&emi_reg->slct, 0xff0bff00);
	write32(&emi_reg->bmen, 0x00ff0001);
	write32(&emi_reg->clua, 0x10000008);
	write32(&emi_reg->slva, 0xffffffff);
	write32(&emi_reg->thro_os0, 0x24240101);
	write32(&emi_reg->thro_os1, 0x01012424);
	write32(&emi_reg->thro_os2, 0x50500101);
	write32(&emi_reg->thro_os3, 0x01015050);
	write32(&emi_reg->thro_ctrl0, 0x0fc39a30);
	write32(&emi_reg->thro_prd0, 0x05050003);
	write32(&emi_reg->thro_prd1, 0x254dffff);
	write32(&emi_reg->thro_lat0, 0x465a788c);
	write32(&emi_reg->thro_lat1, 0x000003e8);
	write32(&emi_reg->thro_lat2, 0x0000036b);
	write32(&emi_reg->thro_lat3, 0x00000290);
	write32(&emi_reg->thro_lat4, 0x00000200);
	write32(&emi_reg->thro_lat5, 0x00000000);
	write32(&emi_reg->thro_lat6, 0x00000000);
	write32(&emi_reg->thro_ctrl1, 0x02531cff);
	write32(&emi_reg->thro_prd2, 0x00002785);
	write32(&emi_reg->thro_lat7, 0x000001b5);
	write32(&emi_reg->thro_lat8, 0x003c0000);
	write32(&emi_reg->thro_prd3, 0x0255250d);
	write32(&emi_reg->bwlmta, 0xffff3c59);
	write32(&emi_reg->bwlmtb, 0xffff00ff);
	write32(&emi_reg->bwlmte, 0xffffffff);
	write32(&emi_reg->bwlmtf, 0x0000ffff);
	write32(&emi_reg->thro_lat9, 0x0000014b);
	write32(&emi_reg->thro_lat10, 0x002d0000);
	write32(&emi_reg->thro_lat11, 0x00000185);
	write32(&emi_reg->thro_lat12, 0x003c0000);
	write32(&emi_reg->thro_lat13, 0x00000185);
	write32(&emi_reg->thro_lat14, 0x003c0000);
	write32(&emi_reg->bwlmte_2nd, 0xffffffff);
	write32(&emi_reg->bwlmtf_2nd, 0xffffffff);
	write32(&emi_reg->bwlmtg_2nd, 0xffffffff);
	write32(&emi_reg->bwlmte_4th, 0xffffffff);
	write32(&emi_reg->bwlmtf_4th, 0x0000ffff);
	write32(&emi_reg->bwlmte_5th, 0xffffffff);
	write32(&emi_reg->bwlmtf_5th, 0xffffffff);
	write32(&emi_reg->bwlmtg_5th, 0xffffffff);
	write32(&emi_reg->thro_lat27, 0x41547082);
	write32(&emi_reg->thro_lat28, 0x38382a38);
	write32(&emi_reg->thro_lat29, 0x000001d4);
	write32(&emi_reg->thro_lat30, 0x00000190);
	write32(&emi_reg->thro_lat31, 0x0000012c);
	write32(&emi_reg->thro_lat32, 0x000000ed);
	write32(&emi_reg->thro_lat33, 0x000000c8);
	write32(&emi_reg->thro_lat34, 0x00000096);
	write32(&emi_reg->thro_lat35, 0x000000c8);
	write32(&emi_reg->thro_lat36, 0x000000c8);
	write32(&emi_reg->thro_lat41, 0x26304048);
	write32(&emi_reg->thro_lat42, 0x20201820);
	write32(&emi_reg->thro_lat55, 0x181e282f);
	write32(&emi_reg->thro_lat56, 0x14140f18);
	write32(&emi_reg->thro_lat69, 0x7496c8ea);
	write32(&emi_reg->thro_lat70, 0x64644b64);
	write32(&emi_reg->thro_lat83, 0x01010101);
	write32(&emi_reg->thro_lat84, 0x01010101);
	write32(&emi_reg->thro_lat97, 0x7496c8ea);
	write32(&emi_reg->thro_lat98, 0x64644b64);
	write32(&emi_reg->thro_lat111, 0x01010101);
	write32(&emi_reg->thro_lat112, 0x01010101);
	write32(&emi_reg->thro_prd5, 0x300ff025);
	write32(&emi_reg->thro_lat113, 0x000003e8);
	write32(&emi_reg->thro_lat114, 0x0000036b);
	write32(&emi_reg->thro_lat115, 0x00000290);
	write32(&emi_reg->thro_lat116, 0x00000200);
	write32(&emi_reg->thro_lat117, 0x000001b5);
	write32(&emi_reg->thro_lat118, 0x0000014b);
	write32(&emi_reg->thro_lat119, 0x00000185);
	write32(&emi_reg->thro_lat120, 0x00000185);
	write32(&emi_reg->thro_lat125, 0x52698ca0);
	write32(&emi_reg->thro_lat126, 0x46463546);
	write32(&emi_reg->thro_lat139, 0x01010101);
	write32(&emi_reg->thro_lat140, 0x01010101);
	write32(&emi_reg->qos_mdr_be0a, 0x00000009);
	write32(&emi_reg->qos_mdr_be1a, 0x00000000);
	write32(&emi_reg->qos_mdr_shf0, 0x00730000);
	write32(&emi_reg->qos_mdr_shf1, 0x00000808);
	write32(&emi_reg->qos_mdw_be0a, 0x00000028);
	write32(&emi_reg->qos_mdw_be1a, 0x00000000);
	write32(&emi_reg->qos_mdw_shf0, 0x00730000);
	write32(&emi_reg->qos_mdw_shf1, 0x00000808);
	write32(&emi_reg->qos_apr_be0a, 0x00000080);
	write32(&emi_reg->qos_apr_be1a, 0x00000000);
	write32(&emi_reg->qos_apr_shf0, 0x30201008);
	write32(&emi_reg->qos_apw_be0a, 0x00000800);
	write32(&emi_reg->qos_apw_be1a, 0x00000000);
	write32(&emi_reg->qos_mmr_be0a, 0x00008000);
	write32(&emi_reg->qos_mmr_be1a, 0x00020000);
	write32(&emi_reg->qos_mmr_be1b, 0x00001000);
	write32(&emi_reg->qos_mmr_be2a, 0x00010000);
	write32(&emi_reg->qos_mmr_be2b, 0x00000800);
	write32(&emi_reg->qos_mmr_shf0, 0x08080000);
	write32(&emi_reg->qos_mmr_shf1, 0x00073030);
	write32(&emi_reg->qos_mmw_be0a, 0x00040000);
	write32(&emi_reg->qos_mmw_be1a, 0x00100000);
	write32(&emi_reg->qos_mmw_be1b, 0x00004000);
	write32(&emi_reg->qos_mmw_be2a, 0x00080000);
	write32(&emi_reg->qos_mmw_be2b, 0x00002000);
	write32(&emi_reg->qos_mmw_shf0, 0x08080000);
	write32(&emi_reg->qos_mmw_shf1, 0x00074040);
	write32(&emi_reg->qos_mdhwr_be0a, 0x00400000);
	write32(&emi_reg->qos_mdhwr_be1a, 0x00200000);
	write32(&emi_reg->qos_mdhwr_shf0, 0x10100404);
	write32(&emi_reg->qos_mdhww_be0a, 0x01000000);
	write32(&emi_reg->qos_mdhww_be1a, 0x00800000);
	write32(&emi_reg->qos_gpur_be0a, 0x04000000);
	write32(&emi_reg->qos_gpur_be1a, 0x02000000);
	write32(&emi_reg->qos_gpur_shf0, 0x60602010);
	write32(&emi_reg->qos_gpuw_be0a, 0x10000000);
	write32(&emi_reg->qos_gpuw_be1a, 0x08000000);
	write32(&emi_reg->qos_arbr_be0a, 0x00000009);
	write32(&emi_reg->qos_arbr_be1a, 0x04400080);
	write32(&emi_reg->qos_arbr_shf0, 0x0f170f11);
	write32(&emi_reg->qos_ctrl1, 0x0303f7f7);
	write32(&emi_reg->ext_lt_con1_1st, 0x00000166);
	write32(&emi_reg->ext_lt_con2_1st, 0xffffffff);
	write32(&emi_reg->ext_lt_con3_1st, 0xffffffff);
	write32(&emi_reg->ext_lt_con1_2nd, 0x00400166);
	write32(&emi_reg->ext_lt_con2_2nd, 0xffffffff);
	write32(&emi_reg->ext_lt_con3_2nd, 0xffffffff);
	write32(&emi_reg->ext_lt_con1_3rd, 0x00000266);
	write32(&emi_reg->ext_lt_con2_3rd, 0xffffffff);
	write32(&emi_reg->ext_lt_con3_3rd, 0xffffffff);
	write32(&emi_reg->ext_lt_con1_4th, 0x00400266);
	write32(&emi_reg->ext_lt_con2_4th, 0xffffffff);
	write32(&emi_reg->ext_lt_con3_4th, 0xffffffff);
	write32(&emi_reg->prtcl_m0_cyc, 0xffffffff);
	write32(&emi_reg->prtcl_m0_ctl, 0x001ffc85);
	write32(&emi_reg->prtcl_m0_msk, 0xffffffff);
	write32(&emi_reg->prtcl_m1_cyc, 0xffffffff);
	write32(&emi_reg->prtcl_m1_ctl, 0x001ffc85);
	write32(&emi_reg->prtcl_m1_msk, 0xffffffff);
	write32(&emi_reg->prtcl_m2_cyc, 0xffffffff);
	write32(&emi_reg->prtcl_m2_ctl, 0x001ffc85);
	write32(&emi_reg->prtcl_m2_msk, 0xffffffff);
	write32(&emi_reg->prtcl_m3_cyc, 0xffffffff);
	write32(&emi_reg->prtcl_m3_ctl, 0x001ffc85);
	write32(&emi_reg->prtcl_m3_msk, 0xffffffff);
	write32(&emi_reg->prtcl_m4_cyc, 0xffffffff);
	write32(&emi_reg->prtcl_m4_ctl, 0x001ffc85);
	write32(&emi_reg->prtcl_m4_msk, 0xffffffff);
	write32(&emi_reg->prtcl_m5_cyc, 0xffffffff);
	write32(&emi_reg->prtcl_m5_ctl, 0x001ffc85);
	write32(&emi_reg->prtcl_m5_msk, 0xffffffff);
	write32(&emi_reg->prtcl_m6_cyc, 0xffffffff);
	write32(&emi_reg->prtcl_m6_ctl, 0x001ffc85);
	write32(&emi_reg->prtcl_m6_msk, 0xffffffff);
	write32(&emi_reg->prtcl_m7_cyc, 0xffffffff);
	write32(&emi_reg->prtcl_m7_ctl, 0x001ffc85);
	write32(&emi_reg->prtcl_m7_msk, 0xffffffff);
	write32(&emi_reg->dvfs_shf_con, 0x00000000);
	write32(&emi_reg->mxto0, 0x60606060);
	write32(&emi_reg->mxto1, 0x60606060);
	write32(&emi_reg->conk, 0x00000000);
	write32(&emi_reg->thro_slv_con0, 0x08ffbbff);
	write32(&emi_reg->thro_slv_con1, 0xffff5b3c);
	write32(&emi_reg->bwlmte_8th, 0xffff00ff);
	write32(&emi_reg->bwlmtf_8th, 0x00ffffff);
	write32(&emi_reg->bwlmtg_8th, 0xffff00ff);
	write32(&emi_reg->bwlmth_8th, 0x00ffffff);
	write32(&emi_reg->bwlmtg_7th, 0x00000000);
	write32(&emi_reg->chn_hash0, 0xC0000000);
}

static void emi_chn_config(void)
{
	struct emi_chn_regs *emi_chn = &ch->emi_chn;

	/* set EMI channel default settings which EMI hardware needs */
	write32(&emi_chn->cona, 0x0400f051);
	write32(&emi_chn->conb, 0x00ff6048);
	write32(&emi_chn->conc, 0x00000004);
	write32(&emi_chn->mdct, 0x99f08c03);
	write32(&emi_chn->shf0, 0x9a508c17);
	write32(&emi_chn->testb, 0x00038137);
	write32(&emi_chn->testc, 0x38460002);
	write32(&emi_chn->testd, 0x00000000);
	write32(&emi_chn->ap_early_cke, 0x000002ff);
	write32(&emi_chn->dqfr, 0x00003111);
	write32(&emi_chn->arbi, 0x22607188);
	write32(&emi_chn->arbi_2nd, 0x22607188);
	write32(&emi_chn->arbj, 0x3719595e);
	write32(&emi_chn->arbj_2nd, 0x2719595e);
	write32(&emi_chn->arbk, 0x64f3ff79);
	write32(&emi_chn->arbk_2nd, 0x64f3ff79);
	write32(&emi_chn->slct, 0x011b0868);
	write32(&emi_chn->arb_rff, 0xa7414222);
	write32(&emi_chn->drs_mon0, 0x0000f801);
	write32(&emi_chn->drs_mon1, 0x40000000);
	write32(&emi_chn->rkarb0, 0x000c802f);
	write32(&emi_chn->rkarb1, 0xbd3f3f7e);
	write32(&emi_chn->rkarb2, 0x7e003d7e);
	write32(&emi_chn->eco3, 0x00000000);
	write32(&emi_chn->md_pre_mask, 0xaa0148ff);
	write32(&emi_chn->md_pre_mask_shf, 0xaa6168ff);
	write32(&emi_chn->md_pre_mask_shf0, 0xaa516cff);
	write32(&emi_chn->md_pre_mask_shf1, 0xaa0140ff);
	write32(&emi_chn->qos_mdr_shf0, 0x9f658633);
}

static void emi_init(void)
{
	dramc_set_broadcast(DRAMC_BROADCAST_ON);
	emi_center_config();
	emi_chn_config();
	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
}

void emi_mdl_init(const struct emi_mdl *emi_con)
{
	emi_init();

	write32(&emi_reg->cona, emi_con->cona_val);
	write32(&emi_reg->conf, emi_con->conf_val);
	write32(&emi_reg->conh, emi_con->conh_val);
	for (u8 chn = CHANNEL_A; chn < CHANNEL_MAX; chn++)
		write32(&ch[chn].emi_chn.cona, emi_con->chn_cona_val);
}

u32 get_column_num(void)
{
	u32 ma_type = read32(&emi_reg->cona);
	u32 ma_type_r0 = ((ma_type >> 20) & 0x3) + 1;
	u32 ma_type_r1 = ((ma_type >> 22) & 0x3) + 1;

	ma_type = MIN(ma_type_r0, ma_type_r1);

	return ma_type;
}

static void emi_sw_setting(void)
{
	setbits32(&emi_mpu->mpu_ctrl_d[1], BIT(4));
	setbits32(&emi_mpu->mpu_ctrl_d[7], BIT(4));

	/* set EMI default bandwidth threshold */
	write32(&emi_reg->bwct0, 0x05008305);
	write32(&emi_reg->bwct0_6th, 0x08ff8705);
	write32(&emi_reg->bwct0_3rd, 0x0dff8a05);
	setbits32(&emi_reg->thro_ctrl1, 0x3 << 8);
}

static void dramc_dcm_setting(void)
{
	/* set EMI dcm default settgings */
	write32(&infra_ao_mem->emi_idle_bit_en_0, 0xFFFFFFFF);
	write32(&infra_ao_mem->emi_idle_bit_en_1, 0xFFFFFFFF);
	write32(&infra_ao_mem->emi_idle_bit_en_2, 0xFFFFFFFF);
	write32(&infra_ao_mem->emi_idle_bit_en_3, 0xFFFFFFFF);
	write32(&infra_ao_mem->emi_m0m1_idle_bit_en_0, 0x01F00000);
	write32(&infra_ao_mem->emi_m0m1_idle_bit_en_1, 0xC0040180);
	write32(&infra_ao_mem->emi_m0m1_idle_bit_en_2, 0x00000000);
	write32(&infra_ao_mem->emi_m0m1_idle_bit_en_3, 0x00000003);
	write32(&infra_ao_mem->emi_m2m5_idle_bit_en_0, 0x0C000000);
	write32(&infra_ao_mem->emi_m2m5_idle_bit_en_1, 0x00C00000);
	write32(&infra_ao_mem->emi_m2m5_idle_bit_en_2, 0x01F08000);
	write32(&infra_ao_mem->emi_m2m5_idle_bit_en_3, 0x00000000);
	write32(&infra_ao_mem->emi_m3_idle_bit_en_0, 0x20003040);
	write32(&infra_ao_mem->emi_m3_idle_bit_en_1, 0x00000000);
	write32(&infra_ao_mem->emi_m3_idle_bit_en_2, 0x00001000);
	write32(&infra_ao_mem->emi_m3_idle_bit_en_3, 0x00000000);
	write32(&infra_ao_mem->emi_m4_idle_bit_en_0, 0x10020F20);
	write32(&infra_ao_mem->emi_m4_idle_bit_en_1, 0x00019000);
	write32(&infra_ao_mem->emi_m4_idle_bit_en_2, 0x040A0818);
	write32(&infra_ao_mem->emi_m4_idle_bit_en_3, 0x00000370);
	write32(&infra_ao_mem->emi_m6m7_idle_bit_en_0, 0xC001C080);
	write32(&infra_ao_mem->emi_m6m7_idle_bit_en_1, 0x33000E01);
	write32(&infra_ao_mem->emi_m6m7_idle_bit_en_2, 0x180067E1);
	write32(&infra_ao_mem->emi_m6m7_idle_bit_en_3, 0x000C008C);
	write32(&infra_ao_mem->emi_sram_idle_bit_en_0, 0x020C0008);
	write32(&infra_ao_mem->emi_sram_idle_bit_en_1, 0x0C00007E);
	write32(&infra_ao_mem->emi_sram_idle_bit_en_2, 0x80050006);
	write32(&infra_ao_mem->emi_sram_idle_bit_en_3, 0x00030000);

	write32(&infra_ao_mem->emi_dcm_cfg0, 0x0000000F);
	write32(&infra_ao_mem->emi_dcm_cfg1, 0x00000000);
	write32(&infra_ao_mem->emi_dcm_cfg2, 0x001F0044);
	write32(&infra_ao_mem->top_ck_anchor_cfg, 0x200000FF);

	setbits32(&mt8192_infracfg->mem_dcm_ctrl, BIT(27));
}

static void dramc_dcm_setting2(void)
{
	u32 emi_temp_data;

	write32(&infra_ao_mem->emi_disph_cfg, 0x00000007);
	write32(&infra_ao_mem->emi_disph_cfg, 0x80000007);

	emi_temp_data = read32(&infra_ao_mem->emi_disph_cfg);
	emi_temp_data = emi_temp_data & 0xf;
	setbits32(&emi_reg->chn_hash0, emi_temp_data);
}

void emi_init2(void)
{
	u32 tmp;
	u32 bc_bak;
	struct emi_chn_regs *emi_chn = &ch->emi_chn;

	bc_bak = dramc_get_broadcast();
	dramc_set_broadcast(DRAMC_BROADCAST_ON);
	setbits32(&emi_chn->conc, 0x1);
	setbits32(&emi_reg->conm, BIT(10));
	setbits32(&emi_mpu->mpu_ctrl, BIT(4));

	clrbits32(&emi_chn->rkarb0, 0x1);
	tmp = read32(&emi_chn->cona) & 0x1;
	setbits32(&emi_chn->rkarb0, tmp);

	dramc_dcm_setting();

	/* do basic memory read/write for adjusting EMI HW settings */
	write32((void *)_dram, read32((void *)_dram));
	write32((void *)(_dram + 0x100), read32((void *)(_dram + 0x100)));
	write32((void *)(_dram + 0x200), read32((void *)(_dram + 0x200)));
	write32((void *)(_dram + 0x300), read32((void *)(_dram + 0x300)));

	setbits32(&emi_reg->conn, BIT(22));
	setbits32(&emi_chn->testc, BIT(2));

	dramc_dcm_setting2();

	setbits32(&emi_reg->conn, BIT(21));
	setbits32(&emi_chn->testc, BIT(4));
	write32(&emi_chn->ebg_con, 0x40);

	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
	emi_sw_setting();
	dramc_set_broadcast(bc_bak);
}

size_t sdram_size(void)
{
	size_t dram_size = 0x100000000;

	return dram_size;
}

void mt_set_emi(const struct dramc_data *dparam)
{
	init_dram(dparam);
}
