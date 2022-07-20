/* SPDX-License-Identifier: BSD-3-Clause */

//=============================================================================
//  Include Files
//=============================================================================
//#include <common.h>
//#include <ett_common.h>
//#include <test_case_controller.h>
//#include <api.h>
//#include "gpio.h"
//#include "ett_cust.h"
//#include "emi_setting.h"
//#include "pll.h"
//#include "dramc_pi_api.h"

#include "dramc_common.h"
#include "dramc_int_global.h"
#include "dramc_top.h"
#include "dramc_custom.h"




#if !__FLASH_TOOL_DA__ && !__ETT__
//#include "custom_emi.h"   //[FOR_CHROMEOS]
#endif

#include <emi_hw.h>
#include <emi.h>
// #include "voltage.h"

#include <soc/dramc_param.h>
#include <soc/emi.h>
#include <soc/regulator.h>

#if DRAM_AUXADC_CONFIG
#include <mtk_auxadc_sw.h>
#endif


#if (FOR_DV_SIMULATION_USED==0)
#if 0 /* FIXME: need regulator control */
#include <pmic.h>
/* now we can use definition MTK_PMIC_MT6330
 * ==============================================================
 * PMIC   |Power |Dflt. Volt. |Step   |Support FPWM |Cmt.
 * --------------------------------------------------------------
 * MT6359 |Vcore |0.8v        |6.25mV |Yes          |
 *        |vio18_2  |1.8v        |0.1V   |No           |
 * --------------------------------------------------------------
 * MT6362 |Vdram |1.125v      |5mV    |Yes          |(DRAM Vdram)
 *        |vmddr |0.75v       |10mV   |No           |(AP Vdram)
 *        |Vddq  |0.6v        |10mV   |No           |
 * ==============================================================
 */
#define MTK_PMIC_MT6359
#endif
#endif

#if !__ETT__
#define mt_reg_sync_write(x,y) mt_reg_sync_writel(y,x)
#endif

#ifdef MTK_PMIC_MT6359
#include <regulator/mtk_regulator.h>
#include <mt6359.h>
#endif

#if !__ETT__
#define CQ_DMA_BASE (0x10212000)
#endif


#if !__ETT__ && CFG_ENABLE_DCACHE
#define DRAMC_ASSERT(expr)                                            \
    do {                                                        \
        if (!(expr)) {                                          \
      plat_clean_invalidate_dcache();														\
	    ASSERT(expr);					\
	}							\
    } while (0)
#else
#define DRAMC_ASSERT(expr)         ASSERT(expr)
#endif

//=============================================================================
//  Definition
//=============================================================================

#if DRAM_AUXADC_CONFIG
static unsigned int get_ch_num_by_auxadc(void);
#endif
//=============================================================================
//  Global Variables
//=============================================================================
static unsigned char auto_detect_done;
//static int enable_combo_dis = 0;
//static unsigned short mr5;
//static unsigned short mr6;
//static unsigned short mr7;
//static unsigned short mr8;
unsigned int channel_num_auxadc = 4;
#if DRAM_AUXADC_CONFIG
unsigned int dram_type_auxadc;
#endif

#ifdef MTK_PMIC_MT6359
//static struct mtk_regulator reg_vio18_2, reg_vmdd2, reg_vcore, reg_vmddq, reg_vmddr;
static struct mtk_regulator reg_vio18, reg_vdram, reg_vcore, reg_vddq, reg_vmddr;
#endif

#ifdef LAST_DRAMC
static LAST_DRAMC_INFO_T* last_dramc_info_ptr;
#endif

#ifdef VOLTAGE_SEL
static VOLTAGE_SEL_INFO_T voltage_sel_info_ptr;
#endif

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
#if (!__ETT__)
//#include <storage_api.h> //[FOR_CHROMEOS]
#endif
static int read_offline_dram_mdl_data(DRAM_INFO_BY_MRR_T *DramInfo);
static int write_offline_dram_mdl_data(DRAM_INFO_BY_MRR_T *DramInfo);
#endif

#if defined(SLT) && (!__ETT__)
#include <pl_version.h>
static u64 part_dram_data_addr_slt = 0;
int read_slt_data(DRAM_SLT_DATA_T *data);
int write_slt_data(DRAM_SLT_DATA_T *data);
int clean_slt_data(void);
#endif
//=============================================================================
//  External references
//=============================================================================
extern char* opt_dle_value;

void print_DBG_info(DRAMC_CTX_T *p);

#if ENABLE_PINMUX_FOR_RANK_SWAP
static void EMI_rank_swap_emi_setting(EMI_SETTINGS *emi_set)
{
	static unsigned int temp;

	if (emi_set->EMI_CONA_VAL & 0x20000) {
		temp = emi_set->EMI_CONA_VAL;
		emi_set->EMI_CONA_VAL &= ~(0xF3F0F0F0);
		emi_set->EMI_CONA_VAL |= (temp & 0xC0C0C0C0) >> 2;
		emi_set->EMI_CONA_VAL |= (temp & 0x30303030) << 2;
		emi_set->EMI_CONA_VAL |= (temp & 0x02000000) >> 1;
		emi_set->EMI_CONA_VAL |= (temp & 0x01000000) << 1;

		temp = emi_set->EMI_CONH_VAL;
		emi_set->EMI_CONH_VAL &= ~(0xFFFF0030);
		emi_set->EMI_CONH_VAL |= (temp & 0xF0F00000) >> 4;
		emi_set->EMI_CONH_VAL |= (temp & 0x0F0F0000) << 4;
		emi_set->EMI_CONH_VAL |= (temp & 0x00000020) >> 1;
		emi_set->EMI_CONH_VAL |= (temp & 0x00000010) << 1;

		temp = emi_set->CHN0_EMI_CONA_VAL;
		emi_set->CHN0_EMI_CONA_VAL &= ~(0x00FFF0FC);
		emi_set->CHN0_EMI_CONA_VAL |= (temp & 0x00F00000) >> 4;
		emi_set->CHN0_EMI_CONA_VAL |= (temp & 0x000F0000) << 4;
		emi_set->CHN0_EMI_CONA_VAL |= (temp & 0x0000C0C0) >> 2;
		emi_set->CHN0_EMI_CONA_VAL |= (temp & 0x00003030) << 2;
		emi_set->CHN0_EMI_CONA_VAL |= (temp & 0x00000008) >> 1;
		emi_set->CHN0_EMI_CONA_VAL |= (temp & 0x00000004) << 1;

		temp = emi_set->CHN1_EMI_CONA_VAL;
		emi_set->CHN1_EMI_CONA_VAL &= ~(0x00FFF0FC);
		emi_set->CHN1_EMI_CONA_VAL |= (temp & 0x00F00000) >> 4;
		emi_set->CHN1_EMI_CONA_VAL |= (temp & 0x000F0000) << 4;
		emi_set->CHN1_EMI_CONA_VAL |= (temp & 0x0000C0C0) >> 2;
		emi_set->CHN1_EMI_CONA_VAL |= (temp & 0x00003030) << 2;
		emi_set->CHN1_EMI_CONA_VAL |= (temp & 0x00000008) >> 1;
		emi_set->CHN1_EMI_CONA_VAL |= (temp & 0x00000004) << 1;

		emi_set->CHN0_EMI_CONA_VAL |= 0x80000000;
		emi_set->CHN1_EMI_CONA_VAL |= 0x80000000;

		temp = emi_set->DRAM_RANK_SIZE[0];
		emi_set->DRAM_RANK_SIZE[0] = emi_set->DRAM_RANK_SIZE[1];
		emi_set->DRAM_RANK_SIZE[1] = temp;

		if (emi_set->dram_cbt_mode_extern == CBT_R0_NORMAL_R1_BYTE)
			emi_set->dram_cbt_mode_extern = CBT_R0_BYTE_R1_NORMAL;
		else if (emi_set->dram_cbt_mode_extern == CBT_R0_BYTE_R1_NORMAL)
			emi_set->dram_cbt_mode_extern = CBT_R0_NORMAL_R1_BYTE;
	}
}

void EMI_rank_swap_handle(void)
{
	static unsigned int handled = 0;
	int i;

	if (!handled) {
		EMI_rank_swap_emi_setting(&g_default_emi_setting);
		handled = 1;
	}
}
#endif

void mdl_setting(DRAMC_CTX_T *p)
{
	EMI_SETTINGS *emi_set = &g_default_emi_setting;

	emi_init();

	enable_infra_emi_broadcast(1);

	set_cen_emi_cona(emi_set->EMI_CONA_VAL);
	set_cen_emi_conf(emi_set->EMI_CONF_VAL);
	set_cen_emi_conh(emi_set->EMI_CONH_VAL);


	set_chn_emi_cona(emi_set->CHN0_EMI_CONA_VAL);
	//set_chn_emi_conc(0x4);
	enable_infra_emi_broadcast(0);

	p->vendor_id = emi_set->iLPDDR3_MODE_REG_5;
}
#if 0
unsigned int check_gating_error(void)
{
	unsigned int ret = 0, i, phy_base, err_code = 0;
	unsigned int misc_stberr_all, misc_stberr_rk0_r, misc_stberr_rk0_f, misc_stberr_rk1_r, misc_stberr_rk1_f;

	phy_base = Channel_A_DDRPHY_AO_BASE_ADDRESS;

	for (i = 0; i < CHANNEL_NUM; ++i, phy_base += 0x10000, err_code = 0) {
		misc_stberr_all   = *(volatile unsigned int*)(phy_base + 0x1500);
		misc_stberr_rk0_r = *(volatile unsigned int*)(phy_base + 0x1504);
		misc_stberr_rk0_f = *(volatile unsigned int*)(phy_base + 0x1508);
		misc_stberr_rk1_r = *(volatile unsigned int*)(phy_base + 0x150c);
		misc_stberr_rk1_f = *(volatile unsigned int*)(phy_base + 0x1510);

		if (misc_stberr_all & (1 << 16)) {
			ret |= (1 << i);
#ifdef LAST_DRAMC
			if ((misc_stberr_rk0_r & 0xffff) != 0) {
				err_code |= ERR_DRAM_GATING_RK0_R;
			}
			if ((misc_stberr_rk0_f & 0xffff) != 0) {
				err_code |= ERR_DRAM_GATING_RK0_F;
			}
			if ((misc_stberr_rk1_r & 0xffff) != 0) {
				err_code |= ERR_DRAM_GATING_RK1_R;
			}
			if ((misc_stberr_rk1_f & 0xffff) != 0) {
				err_code |= ERR_DRAM_GATING_RK1_F;
			}
			dram_fatal_set_gating_err(i, err_code);
			dram_fatal_set_stberr(i, 0, (misc_stberr_rk0_r & 0xffff) | ((misc_stberr_rk0_f & 0xffff) << 16));
			dram_fatal_set_stberr(i, 1, (misc_stberr_rk1_r & 0xffff) | ((misc_stberr_rk1_f & 0xffff) << 16));
		} else {
			dram_fatal_set_gating_err(i, 0);
			dram_fatal_set_stberr(i, 0, 0);
			dram_fatal_set_stberr(i, 1, 0);
#endif
		}
	}

	return ret;
}
#endif
#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
void print_DBG_info(DRAMC_CTX_T *p)
{
#ifndef OLYMPUS_TO_BE_PORTING
    //unsigned int addr = 0x0;
    //U32 u4value;

#ifdef DDR_INIT_TIME_PROFILING
    return;
#endif

    mcSHOW_DBG_MSG(("EMI_CONA=%x\n", get_cen_emi_cona()));

    //RISCReadAll();
#endif
}
#endif

int mt_get_dram_type(void)
{
	unsigned int dtype = mt_get_dram_type_from_hw_trap();

	if (dtype == TYPE_LPDDR4X)
		return DTYPE_LPDDR4X;
	else
		DRAMC_ASSERT(0);

	return 0;
}

int mt_get_freq_setting(DRAMC_CTX_T *p)
{
    return p->frequency;
}

#ifdef DDR_RESERVE_MODE
extern u32 g_ddr_reserve_enable;
extern u32 g_ddr_reserve_success;
#define TIMEOUT 3
extern void before_Dramc_DDR_Reserved_Mode_setting(void);

#define	CHAN_DRAMC_NAO_MISC_STATUSA(base)	(base + 0x80)
#define SREF_STATE				(1 << 16)

static unsigned int is_dramc_exit_slf(void)
{
	unsigned int ret;

	ret = *(volatile unsigned *)CHAN_DRAMC_NAO_MISC_STATUSA(Channel_A_DRAMC_NAO_BASE_ADDRESS);
	if ((ret & SREF_STATE) != 0) {
		dramc_crit("DRAM CHAN-A is in self-refresh (MISC_STATUSA = 0x%x)\n", ret);
		return 0;
	}

	ret = *(volatile unsigned *)CHAN_DRAMC_NAO_MISC_STATUSA(Channel_B_DRAMC_NAO_BASE_ADDRESS);
	if ((ret & SREF_STATE) != 0) {
		dramc_crit("DRAM CHAN-B is in self-refresh (MISC_STATUSA = 0x%x)\n", ret);
		return 0;
	}

#if CHANNEL_NUM > 2
    if (channel_num_auxadc > 2) {
        ret = *(volatile unsigned *)CHAN_DRAMC_NAO_MISC_STATUSA(Channel_C_DRAMC_NAO_BASE_ADDRESS);
        if ((ret & SREF_STATE) != 0) {
            dramc_crit("DRAM CHAN-C is in self-refresh (MISC_STATUSA = 0x%x)\n", ret);
            return 0;
        }

        ret = *(volatile unsigned *)CHAN_DRAMC_NAO_MISC_STATUSA(Channel_D_DRAMC_NAO_BASE_ADDRESS);
        if ((ret & SREF_STATE) != 0) {
            dramc_crit("DRAM CHAN-D is in self-refresh (MISC_STATUSA = 0x%x)\n", ret);
            return 0;
        }
    }
#endif

	dramc_crit("ALL DRAM CHAN is not in self-refresh\n");
	return 1;
}

#endif

unsigned int dramc_set_vcore_voltage(unsigned int vcore)
{
#ifdef MTK_PMIC_MT6359
	return mtk_regulator_set_voltage(&reg_vcore, vcore, MAX_VCORE);
#elif defined(FOR_COREBOOT)
	dramc_debug("%s set vcore to %u\n", __func__, vcore);
	mainboard_set_regulator_voltage(MTK_REGULATOR_VCORE, vcore);
	return 0;
#else
	return 0;
#endif
}

unsigned int dramc_get_vcore_voltage(void)
{
#ifdef MTK_PMIC_MT6359
	return mtk_regulator_get_voltage(&reg_vcore);
#elif defined(FOR_COREBOOT)
	return mainboard_get_regulator_voltage(MTK_REGULATOR_VCORE);
#else
	return 0;
#endif
}

unsigned int dramc_set_vmdd_voltage(unsigned int ddr_type, unsigned int vdram)
{
#ifdef MTK_PMIC_MT6359
	mtk_regulator_set_voltage(&reg_vdram, vdram, MAX_VDRAM);
#elif defined(FOR_COREBOOT)
	mainboard_set_regulator_voltage(MTK_REGULATOR_VDD2, vdram);
#endif
	return 0;
}

unsigned int dramc_get_vmdd_voltage(unsigned int ddr_type)
{
#ifdef MTK_PMIC_MT6359
	return mtk_regulator_get_voltage(&reg_vdram);
#elif defined(FOR_COREBOOT)
	return mainboard_get_regulator_voltage(MTK_REGULATOR_VDD2);
#else
	return 0;
#endif
}

unsigned int dramc_set_vmddq_voltage(unsigned int ddr_type, unsigned int vddq)
{
#ifdef MTK_PMIC_MT6359
	mtk_regulator_set_voltage(&reg_vddq, vddq, MAX_VDDQ);
#elif defined(FOR_COREBOOT)
	mainboard_set_regulator_voltage(MTK_REGULATOR_VDDQ, vddq);
#endif
	return 0;
}

unsigned int dramc_get_vmddq_voltage(unsigned int ddr_type)
{
#ifdef MTK_PMIC_MT6359
	return mtk_regulator_get_voltage(&reg_vddq);
#elif defined(FOR_COREBOOT)
	return mainboard_get_regulator_voltage(MTK_REGULATOR_VDDQ);
#else
	return 0;
#endif
}

unsigned int dramc_set_vmddr_voltage(unsigned int vmddr)
{
#ifdef MTK_PMIC_MT6359
	return mtk_regulator_set_voltage(&reg_vmddr, vmddr, MAX_VMDDR);
#elif defined(FOR_COREBOOT)
	mainboard_set_regulator_voltage(MTK_REGULATOR_VMDDR, vmddr);
	return 0;
#else
	return 0;
#endif
}

unsigned int dramc_get_vmddr_voltage(void)
{
#ifdef MTK_PMIC_MT6359
	return mtk_regulator_get_voltage(&reg_vmddr);
#elif defined(FOR_COREBOOT)
	return mainboard_get_regulator_voltage(MTK_REGULATOR_VMDDR);
#else
	return 0;
#endif
}

unsigned int dramc_set_vio18_voltage(unsigned int vio18)
{
#ifdef MTK_PMIC_MT6359
	return mtk_regulator_set_voltage(&reg_vio18, vio18, MAX_VIO18);
#elif defined(FOR_COREBOOT)
	mainboard_set_regulator_voltage(MTK_REGULATOR_VDD1, vio18);
	return 0;
#else
	return 0;
#endif
}


unsigned int dramc_get_vio18_voltage(void)
{
#ifdef MTK_PMIC_MT6359
	return mtk_regulator_get_voltage(&reg_vio18);
#elif defined(FOR_COREBOOT)
	return mainboard_get_regulator_voltage(MTK_REGULATOR_VDD1);
#else
	return 0;
#endif
}

unsigned int is_discrete_lpddr4(void)
{
#if DRAM_AUXADC_CONFIG
	return dram_type_auxadc;
#else
	return TRUE;
#endif
}

unsigned int mt_get_dram_type_from_hw_trap(void)
{
	#if  1
		return TYPE_LPDDR4X;
	#else
	unsigned int trap = get_dram_type() & 0x7;
	switch (trap) {
		case 0:
			return TYPE_LPDDR4X;
		//case 1:
		//case 2:
		//case 3:
		//case 4:
		//case 5:
		//	return TYPE_LPDDR3;
		//case 6:
		//	return TYPE_LPDDR4;
		default:
			printf("[dramc] Wrond HW Trapping.\n");
			DRAMC_ASSERT(0);
			break;
	}
	#endif
	return 0;

}

void setup_dramc_voltage_by_pmic(void)
{
#ifdef VOLTAGE_SEL
	int vcore;
#endif
#ifdef MTK_PMIC_MT6359
	int ret;

	ret = mtk_regulator_get("vio18", &reg_vio18);
	if (ret)
		dramc_debug("mtk_regulator_get vio18 fail\n");

	ret = mtk_regulator_get("vgpu11", &reg_vcore);
	if (ret)
		dramc_debug("mtk_regulator_get vcore fail\n");

	ret = mtk_regulator_get("VDRAM1", &reg_vdram);
	if (ret)
		printf("mtk_regulator_get vdram fail\n");

	ret = mtk_regulator_get("VDRAM2", &reg_vddq);
	if (ret)
		printf("mtk_regulator_get vddq fail\n");

	ret = mtk_regulator_get("VMDDR", &reg_vmddr);
	if (ret)
		printf("mtk_regulator_get vmddr fail\n");

	mtk_regulator_set_mode(&reg_vcore, 0x1);
	mtk_regulator_set_mode(&reg_vdram, 0x1);

	#ifdef VOLTAGE_SEL
    //dramc_set_vio18_voltage(vio18_voltage_select());
	#else
    //dramc_set_vio18_voltage(SEL_VIO18);
	#endif
#if defined(VCORE_BIN)
	#ifdef VOLTAGE_SEL
	vcore = vcore_voltage_select(KSHU0);
	if (vcore)
		dramc_set_vcore_voltage(vcore);
	else
	#endif
	dramc_set_vcore_voltage(get_vcore_uv_table(0));
#else
	#ifdef VOLTAGE_SEL
	dramc_set_vcore_voltage(vcore_voltage_select(KSHU0));
	#else
	dramc_set_vcore_voltage(SEL_PREFIX_VCORE(LP4, KSHU0));
	#endif
#endif
	#ifdef VOLTAGE_SEL
	dramc_set_vmdd_voltage(TYPE_LPDDR4, vdram_voltage_select());
	#else
	dramc_set_vmdd_voltage(TYPE_LPDDR4, SEL_PREFIX_VDRAM(LP4));
	#endif

	#ifdef VOLTAGE_SEL
	dramc_set_vmddq_voltage(TYPE_LPDDR4, vddq_voltage_select());
	#else
	dramc_set_vmddq_voltage(TYPE_LPDDR4, SEL_PREFIX_VDDQ);
	#endif

	#ifdef VOLTAGE_SEL
	dramc_set_vmddr_voltage(vmddr_voltage_select());
	#else
	dramc_set_vmddr_voltage(SEL_PREFIX_VMDDR);
	#endif

	dramc_debug("Vio18 = %d\n", dramc_get_vio18_voltage());
	dramc_debug("Vcore = %d\n", dramc_get_vcore_voltage());
	dramc_debug("Vdram = %d\n", dramc_get_vmdd_voltage(TYPE_LPDDR4));
	dramc_debug("Vddq = %d\n", dramc_get_vmddq_voltage(TYPE_LPDDR4));
	dramc_debug("vmddr = %d\n", dramc_get_vmddr_voltage());
#endif
}

static void restore_vcore_setting(void)
{
#ifdef VOLTAGE_SEL
	int vcore;
#endif
#ifdef MTK_PMIC_MT6359
	int ret;

	ret = mtk_regulator_get("vgpu11", &reg_vcore);
	if (ret)
		printf("mtk_regulator_get vcore fail\n");

#if defined(VCORE_BIN)
	#ifdef VOLTAGE_SEL
	vcore = vcore_voltage_select(KSHU0);
	if ((doe_get_config("dram_fix_3094_0825")) || (doe_get_config("dram_all_3094_0825")) || (doe_get_config("dram_opp0_3733_others_3094_0825")))
			dramc_set_vcore_voltage(825000);
	else if (doe_get_config("dram_fix_3094_0725") || (doe_get_config("dram_fix_2400_0725")) || (doe_get_config("dram_fix_1534_0725")) || (doe_get_config("dram_fix_1200_0725")) || (doe_get_config("dram_all_3094_0725")) || (doe_get_config("dram_all_1534_0725")) || (doe_get_config("dram_opp0_3094_others_1534_0725")) || (doe_get_config("dram_opp0_2400_others_1534_0725")))
			dramc_set_vcore_voltage(725000);
	else if ((doe_get_config("dram_fix_1200_065")) || (doe_get_config("dram_fix_800_065")))
			dramc_set_vcore_voltage(650000);
	else if (vcore)
			dramc_set_vcore_voltage(vcore);
	else
	#endif
	dramc_set_vcore_voltage(get_vcore_uv_table(0));
#else
	#ifdef VOLTAGE_SEL
	dramc_set_vcore_voltage(vcore_voltage_select(KSHU0));
	#else
	dramc_set_vcore_voltage(SEL_PREFIX_VCORE(LP4, KSHU0));
	#endif
#endif

	dramc_debug("Vcore = %d\n", dramc_get_vcore_voltage());
#endif
}
#if 0
static void restore_pmic_setting(void)
{
#ifdef MTK_PMIC_MT6359
	int ret;

	restore_vcore_setting();

	ret = mtk_regulator_get("VDRAM1", &reg_vdram);
	if (ret) {
		printf("mtk_regulator_get vdram fail\n");
		return;
	}

	ret = mtk_regulator_get("VDRAM2", &reg_vddq);
	if (ret) {
		printf("mtk_regulator_get vddq fail\n");
		return;
	}

	ret = mtk_regulator_get("VMDDR", &reg_vmddr);
	if (ret) {
		printf("mtk_regulator_get vmddr fail\n");
		return;
	}

	ret = mtk_regulator_get("vio18", &reg_vio18);
	if (ret) {
		printf("mtk_regulator_get vio18 fail\n");
		return;
	}

#if 0
	dramc_set_vmdd_voltage(TYPE_LPDDR4, 1125000);
	dramc_set_vmddq_voltage(TYPE_LPDDR4, 600000);
	dramc_set_vmddr_voltage(750000);
#else
	#ifdef VOLTAGE_SEL
	dramc_set_vmdd_voltage(TYPE_LPDDR4, vdram_voltage_select());
	#else
	dramc_set_vmdd_voltage(TYPE_LPDDR4, SEL_PREFIX_VDRAM(LP4));
	#endif

	#ifdef VOLTAGE_SEL
	dramc_set_vmddq_voltage(TYPE_LPDDR4, vddq_voltage_select());
	#else
	dramc_set_vmddq_voltage(TYPE_LPDDR4, SEL_PREFIX_VDDQ);
	#endif

	#ifdef VOLTAGE_SEL
	dramc_set_vmddr_voltage(vmddr_voltage_select());
	#else
	dramc_set_vmddr_voltage(SEL_PREFIX_VMDDR);
	#endif

	#ifdef VOLTAGE_SEL
	dramc_set_vio18_voltage(vio18_voltage_select());
	#else
	dramc_set_vio18_voltage(SEL_VIO18);
	#endif
#endif
	dramc_debug("Vdram = %d\n", dramc_get_vmdd_voltage(TYPE_LPDDR4));
	dramc_debug("Vddq = %d\n", dramc_get_vmddq_voltage(TYPE_LPDDR4));
	dramc_debug("vmddr = %d\n", dramc_get_vmddr_voltage());
	dramc_debug("Vio18 = %d\n", dramc_get_vio18_voltage());
#endif
}
#endif
void switch_dramc_voltage_to_auto_mode(void)
{
#ifdef MTK_PMIC_MT6359
	mtk_regulator_set_mode(&reg_vcore, 0x0);
	mtk_regulator_set_mode(&reg_vdram, 0x0);
#endif
}

void release_dram(void)
{
#ifdef DDR_RESERVE_MODE
    int i;
    int counter = TIMEOUT;


    restore_pmic_setting();

    drm_release_rg_dramc_conf_iso();

#if DDR_RESERVE_NEW_MODE
    ASVA5_8_New_Mode_1();
    Dramc_DDR_Reserved_Mode_setting();
    drm_release_rg_dramc_iso();
    ASVA5_8_New_Mode_2();
#else
    Dramc_DDR_Reserved_Mode_setting();
    ASVA5_8_CSCA_Pull_Down_EN();
    drm_release_rg_dramc_iso();
    ASVA5_8_CSCA_Pull_Down_DIS();
#endif

    drm_release_rg_dramc_sref();

    while(counter)
    {
        if(is_dramc_exit_slf() == 1)
            break;
        counter--;
    }

    if(counter == 0)
    {
        if(g_ddr_reserve_enable==1 && g_ddr_reserve_success==1)
        {
            dramc_crit("[DDR Reserve] release dram from self-refresh FAIL!\n");
            g_ddr_reserve_success = 0;
        }
    }
    else
    {
         dramc_crit("[DDR Reserve] release dram from self-refresh PASS!\n");
    }
    Dramc_DDR_Reserved_Mode_AfterSR();

#if DDR_RESERVE_NEW_MODE
    ASVA5_8_New_Mode_3();
#endif


    dramc_crit("[DDR reserve] EMI CEN CONA: %x\n", get_cen_emi_cona());
    dramc_crit("[DDR reserve] EMI CHN CONA: %x\n", get_chn_emi_cona());
    for (i=0;i<10;i++);

    return;
#endif
}

unsigned int DRAM_MRR(int MRR_num)
{
    u16 MRR_value = 0x0;
    DRAMC_CTX_T *p = psCurrDramCtx;

    DramcModeRegRead(p, MRR_num, &MRR_value);
    return MRR_value;
}

static int mt_get_dram_type_for_dis(void)
{
    return TYPE_LPDDR4X;
}

#ifdef DRAM_QVL_CHECK
static int check_qvl(DRAM_INFO_BY_MRR_T *dram_info, unsigned int dram_type)
{
	unsigned int mr5;
	unsigned long long rank_size[2];
	char id[22];
	int emmc_nand_id_len=16;
	int fw_id_len;
	int result;
	int i;

	mr5 = dram_info->u2MR5VendorID & 0xFF;

	rank_size[0] = dram_info->u8MR8Density[0];
	rank_size[1] = dram_info->u8MR8Density[1];

        result = platform_get_mcp_id(id, emmc_nand_id_len,&fw_id_len);
	for (i = 0; i < num_of_emi_records; i++) {
		dramc_crit("[DRAMC] %s(%d),%s(%x),%s(%x),%s(0x%llx),%s(0x%llx)\n",
			"qvl", i,
			"type", qvl_list[i].type,
			"mr5", qvl_list[i].iLPDDR3_MODE_REG_5,
			"rank0_size", qvl_list[i].DRAM_RANK_SIZE[0],
			"rank1_size", qvl_list[i].DRAM_RANK_SIZE[1]);

		if ((qvl_list[i].type & 0xF) != (dram_type & 0xF))
			continue;


		if (qvl_list[i].iLPDDR3_MODE_REG_5 != mr5)
			continue;


		if (qvl_list[i].DRAM_RANK_SIZE[0] != rank_size[0])
			continue;
		if (qvl_list[i].DRAM_RANK_SIZE[1] != rank_size[1])
			continue;


		if (qvl_list[i].type & 0xF00) {
			if (!result) {
				if (memcmp(id, qvl_list[i].ID, qvl_list[i].id_length)) {
					dramc_crit("[DRAMC] storage id mismatch\n", i);
					continue;
				} else
					return 0;
			}
		} else
			return 0;
	}

	return -1;
}
#endif

int get_dram_channel_support_nr(void)
{
    return DRAMC_MAX_CH;
}

int get_dram_channel_nr(void)
{
	return get_channel_nr_by_emi();
}

int get_dram_rank_nr(void)
{
	int cen_emi_cona;

#ifdef DDR_RESERVE_MODE
	if (g_ddr_reserve_enable==1 && g_ddr_reserve_success==1)
		return get_rank_nr_by_emi();
#endif

#ifdef DRAM_ADAPTIVE
	if (!auto_detect_done)
		DRAMC_ASSERT(0);
#endif

	cen_emi_cona = g_default_emi_setting.EMI_CONA_VAL;

	if ((cen_emi_cona & (1 << 17)) != 0 ||
	    (cen_emi_cona & (1 << 16)) != 0 )
		return 2;
	else
		return 1;
}

int get_dram_mr_cnt(void)
{
    return DRAMC_MR_CNT;
}

int get_dram_freq_cnt(void)
{
    return DRAMC_FREQ_CNT;
}

#if (FOR_DV_SIMULATION_USED==0)
#if !__FLASH_TOOL_DA__ && !__ETT__


void get_dram_rank_size(u64 dram_rank_size[DRAMC_MAX_RK])
{
	int rank_nr, i;

#ifdef DDR_RESERVE_MODE
	if(g_ddr_reserve_enable==1 && g_ddr_reserve_success==1){
		get_rank_size_by_emi(dram_rank_size);
		return;
	}
#endif

#ifdef DRAM_ADAPTIVE
	if (!auto_detect_done)
		DRAMC_ASSERT(0);
#endif

	rank_nr = get_dram_rank_nr();

	for(i = 0; i < rank_nr; i++) {
		dram_rank_size[i] = g_default_emi_setting.DRAM_RANK_SIZE[i];
		if (channel_num_auxadc > 2)
			dram_rank_size[i] = dram_rank_size[i] << 1;
		dramc_debug("%d:dram_rank_size:%llx\n",i,dram_rank_size[i]);
	}
}

void get_dram_freq_step(u32 dram_freq_step[])
{
	unsigned int i;
	unsigned int defined_step[DRAMC_FREQ_CNT] = {
		4266, 3200, 2400, 1866, 1600, 1200, 800};

	for (i = 0; i < DRAMC_FREQ_CNT; i++) {
		dram_freq_step[i] = defined_step[i];
	}
}

void set_dram_mr(unsigned int index, unsigned short value)
{
#if 0
	unsigned short value_2rk;

	value_2rk = value & 0xFF;
	value_2rk |= (value_2rk << 8);

	switch (index) {
	case 5:
		mr5 = value_2rk;
		break;
	case 6:
		mr6 = value_2rk;
		break;
	case 7:
		mr7 = value_2rk;
		break;
	case 8:
		mr8 = value;
	default:
		break;
	}
#endif
}

unsigned short get_dram_mr(unsigned int index)
{
	unsigned int value = 0;
#if 0
	switch (index) {
	case 5:
		value = last_dramc_info_ptr->mr5;
		break;
	case 6:
		value = last_dramc_info_ptr->mr6;
		break;
	case 7:
		value = last_dramc_info_ptr->mr7;
		break;
	case 8:
		value = last_dramc_info_ptr->mr8;
	default:
		break;
	}
	return (unsigned short)(value & 0xFFFF);
#else
	return (unsigned short)(value & 0xFFFF);
#endif
}

void get_dram_mr_info(struct mr_info_t mr_info[])
{
#if 0
	unsigned int i;
	unsigned int mr_list[DRAMC_MR_CNT] = {5, 6, 7, 8};

	for (i = 0; i < DRAMC_MR_CNT; i++) {
		mr_info[i].mr_index = mr_list[i];
		mr_info[i].mr_value = get_dram_mr(mr_list[i]);
	}
#endif
}

#endif //#if !__FLASH_TOOL_DA__ && !__ETT__
#endif
#if 0
void freq_table_are_all_3094(void)
{
   gFreqTbl[0].freq_sel = LP4_DDR3200;
   gFreqTbl[0].divmode = DIV8_MODE;
   gFreqTbl[0].SRAMIdx= SRAM_SHU1;
   gFreqTbl[0].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[0].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[0].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[1].freq_sel = LP4_DDR3200;
   gFreqTbl[1].divmode = DIV8_MODE;
   gFreqTbl[1].SRAMIdx = SRAM_SHU3;
   gFreqTbl[1].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[1].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[1].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[2].freq_sel = LP4_DDR3200;
   gFreqTbl[2].divmode = DIV8_MODE;
   gFreqTbl[2].SRAMIdx = SRAM_SHU2;
   gFreqTbl[2].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[2].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[2].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[3].freq_sel = LP4_DDR3200;
   gFreqTbl[3].divmode = DIV8_MODE;
   gFreqTbl[3].SRAMIdx = SRAM_SHU0;
   gFreqTbl[3].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[3].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[3].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[4].freq_sel = LP4_DDR3200;
   gFreqTbl[4].divmode = DIV8_MODE;
   gFreqTbl[4].SRAMIdx = SRAM_SHU5;
   gFreqTbl[4].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[4].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[4].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[5].freq_sel = LP4_DDR3200;
   gFreqTbl[5].divmode = DIV8_MODE;
   gFreqTbl[5].SRAMIdx = SRAM_SHU4;
   gFreqTbl[5].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[5].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[5].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[6].freq_sel = LP4_DDR3200;
   gFreqTbl[6].divmode = DIV8_MODE;
   gFreqTbl[6].SRAMIdx = SRAM_SHU6;
   gFreqTbl[6].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[6].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[6].ddr_loop_mode = CLOSE_LOOP_MODE;

}

void freq_table_are_all_1534(void)
{
   gFreqTbl[0].freq_sel = LP4_DDR1600;
   gFreqTbl[0].divmode = DIV8_MODE;
   gFreqTbl[0].SRAMIdx = SRAM_SHU1;
   gFreqTbl[0].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[0].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[0].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[1].freq_sel = LP4_DDR1600;
   gFreqTbl[1].divmode = DIV8_MODE;
   gFreqTbl[1].SRAMIdx = SRAM_SHU3;
   gFreqTbl[1].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[1].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[1].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[2].freq_sel = LP4_DDR1600;
   gFreqTbl[2].divmode = DIV8_MODE;
   gFreqTbl[2].SRAMIdx = SRAM_SHU2;
   gFreqTbl[2].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[2].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[2].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[3].freq_sel = LP4_DDR1600;
   gFreqTbl[3].divmode = DIV8_MODE;
   gFreqTbl[3].SRAMIdx = SRAM_SHU0;
   gFreqTbl[3].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[3].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[3].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[4].freq_sel = LP4_DDR1600;
   gFreqTbl[4].divmode = DIV8_MODE;
   gFreqTbl[4].SRAMIdx = SRAM_SHU5;
   gFreqTbl[4].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[4].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[4].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[5].freq_sel = LP4_DDR1600;
   gFreqTbl[5].divmode = DIV8_MODE;
   gFreqTbl[5].SRAMIdx = SRAM_SHU4;
   gFreqTbl[5].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[5].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[5].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[6].freq_sel = LP4_DDR1600;
   gFreqTbl[6].divmode = DIV8_MODE;
   gFreqTbl[6].SRAMIdx = SRAM_SHU6;
   gFreqTbl[6].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[6].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[6].ddr_loop_mode = CLOSE_LOOP_MODE;

}

void freq_table_opp0_3733_others_3094(void)
{
   gFreqTbl[0].freq_sel = LP4_DDR3200;
   gFreqTbl[0].divmode = DIV8_MODE;
   gFreqTbl[0].SRAMIdx = SRAM_SHU1;
   gFreqTbl[0].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[0].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[0].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[1].freq_sel = LP4_DDR3200;
   gFreqTbl[1].divmode = DIV8_MODE;
   gFreqTbl[1].SRAMIdx = SRAM_SHU3;
   gFreqTbl[1].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[1].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[1].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[2].freq_sel = LP4_DDR3200;
   gFreqTbl[2].divmode = DIV8_MODE;
   gFreqTbl[2].SRAMIdx = SRAM_SHU2;
   gFreqTbl[2].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[2].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[2].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[3].freq_sel = LP4_DDR3733;
   gFreqTbl[3].divmode = DIV8_MODE;
   gFreqTbl[3].SRAMIdx = SRAM_SHU0;
   gFreqTbl[3].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[3].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[3].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[4].freq_sel = LP4_DDR3200;
   gFreqTbl[4].divmode = DIV8_MODE;
   gFreqTbl[4].SRAMIdx = SRAM_SHU5;
   gFreqTbl[4].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[4].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[4].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[5].freq_sel = LP4_DDR3200;
   gFreqTbl[5].divmode = DIV8_MODE;
   gFreqTbl[5].SRAMIdx = SRAM_SHU4;
   gFreqTbl[5].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[5].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[5].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[6].freq_sel = LP4_DDR3200;
   gFreqTbl[6].divmode = DIV8_MODE;
   gFreqTbl[6].SRAMIdx = SRAM_SHU6;
   gFreqTbl[6].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[6].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[6].ddr_loop_mode = CLOSE_LOOP_MODE;
}

void freq_table_opp0_3094_others_1534(void)
{
   gFreqTbl[0].freq_sel = LP4_DDR1600;
   gFreqTbl[0].divmode = DIV8_MODE;
   gFreqTbl[0].SRAMIdx = SRAM_SHU1;
   gFreqTbl[0].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[0].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[0].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[1].freq_sel = LP4_DDR1600;
   gFreqTbl[1].divmode = DIV8_MODE;
   gFreqTbl[1].SRAMIdx = SRAM_SHU3;
   gFreqTbl[1].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[1].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[1].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[2].freq_sel = LP4_DDR1600;
   gFreqTbl[2].divmode = DIV8_MODE;
   gFreqTbl[2].SRAMIdx = SRAM_SHU2;
   gFreqTbl[2].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[2].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[2].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[3].freq_sel = LP4_DDR3200;
   gFreqTbl[3].divmode = DIV8_MODE;
   gFreqTbl[3].SRAMIdx = SRAM_SHU0;
   gFreqTbl[3].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[3].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[3].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[4].freq_sel = LP4_DDR1600;
   gFreqTbl[4].divmode = DIV8_MODE;
   gFreqTbl[4].SRAMIdx = SRAM_SHU5;
   gFreqTbl[4].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[4].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[4].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[5].freq_sel = LP4_DDR1600;
   gFreqTbl[5].divmode = DIV8_MODE;
   gFreqTbl[5].SRAMIdx = SRAM_SHU4;
   gFreqTbl[5].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[5].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[5].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[6].freq_sel = LP4_DDR1600;
   gFreqTbl[6].divmode = DIV8_MODE;
   gFreqTbl[6].SRAMIdx = SRAM_SHU6;
   gFreqTbl[6].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[6].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[6].ddr_loop_mode = CLOSE_LOOP_MODE;
}

void freq_table_opp0_2400_others_1534(void)
{
   gFreqTbl[0].freq_sel = LP4_DDR1600;
   gFreqTbl[0].divmode = DIV8_MODE;
   gFreqTbl[0].SRAMIdx = SRAM_SHU1;
   gFreqTbl[0].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[0].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[0].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[1].freq_sel = LP4_DDR1600;
   gFreqTbl[1].divmode = DIV8_MODE;
   gFreqTbl[1].SRAMIdx = SRAM_SHU3;
   gFreqTbl[1].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[1].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[1].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[2].freq_sel = LP4_DDR1600;
   gFreqTbl[2].divmode = DIV8_MODE;
   gFreqTbl[2].SRAMIdx = SRAM_SHU2;
   gFreqTbl[2].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[2].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[2].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[3].freq_sel = LP4_DDR2400;
   gFreqTbl[3].divmode = DIV8_MODE;
   gFreqTbl[3].SRAMIdx = SRAM_SHU0;
   gFreqTbl[3].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[3].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[3].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[4].freq_sel = LP4_DDR1600;
   gFreqTbl[4].divmode = DIV8_MODE;
   gFreqTbl[4].SRAMIdx = SRAM_SHU5;
   gFreqTbl[4].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[4].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[4].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[5].freq_sel = LP4_DDR1600;
   gFreqTbl[5].divmode = DIV8_MODE;
   gFreqTbl[5].SRAMIdx = SRAM_SHU4;
   gFreqTbl[5].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[5].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[5].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[6].freq_sel = LP4_DDR1600;
   gFreqTbl[6].divmode = DIV8_MODE;
   gFreqTbl[6].SRAMIdx = SRAM_SHU6;
   gFreqTbl[6].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[6].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[6].ddr_loop_mode = CLOSE_LOOP_MODE;
}
#endif

#if (CFG_DRAM_LOG_TO_STORAGE)
#include <blkdev.h>
#include <partition.h>
#include <storage_api.h>

extern u64 get_part_addr(const char *name);
u64 part_dram_data_addr_uart = 0;
u32 log_start = 0;
static char logbuf[1024];
static int logcount;
#endif

#ifdef VOLTAGE_SEL
void update_voltage_select_info(void)
{
	voltage_sel_info_ptr.vcore = doe_get_config("vcore");
	voltage_sel_info_ptr.vdram = doe_get_config("vdram");
	voltage_sel_info_ptr.vddq = doe_get_config("vddq");
	voltage_sel_info_ptr.vmddr = doe_get_config("vmddr");
	voltage_sel_info_ptr.vio18 = doe_get_config("vio18");

	print("DOE setting: vcore %d, vdram %d, vddq %d, vmddr %d, vio18 %d \n",
		voltage_sel_info_ptr.vcore, voltage_sel_info_ptr.vdram,
		voltage_sel_info_ptr.vddq, voltage_sel_info_ptr.vmddr,
		voltage_sel_info_ptr.vio18);
}

int vio18_voltage_select()
{
	if (voltage_sel_info_ptr.vio18 == LEVEL_LV) {
		return HQA_VIO18_LV;
	} else if (voltage_sel_info_ptr.vio18 == LEVEL_HV) {
		return HQA_VIO18_HV;
	} else {
		return HQA_VIO18_NV;
	}
}

int vmddr_voltage_select()
{
	if (voltage_sel_info_ptr.vmddr == LEVEL_LV) {
		return HQA_VMDDR_LV_LP4;
	} else if (voltage_sel_info_ptr.vmddr == LEVEL_HV) {
		return HQA_VMDDR_HV_LP4;
	} else {
		return HQA_VMDDR_NV_LP4;
	}
}

int vddq_voltage_select()
{
	if (voltage_sel_info_ptr.vddq == LEVEL_LV) {
		return HQA_VDDQ_LV_LP4;
	} else if (voltage_sel_info_ptr.vddq == LEVEL_HV) {
		return HQA_VDDQ_HV_LP4;
	} else {
		return HQA_VDDQ_NV_LP4;
	}
}

int vdram_voltage_select(void)
{
	if (voltage_sel_info_ptr.vdram == LEVEL_LV) {
		return HQA_VDRAM_LV_LP4;
	} else if (voltage_sel_info_ptr.vdram == LEVEL_HV) {
		return HQA_VDRAM_HV_LP4;
	} else {
		return HQA_VDRAM_NV_LP4;
	}
}

int vcore_voltage_select(DRAM_KSHU kshu)
{
	int ret = 0;
	if (voltage_sel_info_ptr.vcore == LEVEL_LV) {
		switch(kshu) {
			case KSHU0:
				ret = HQA_VCORE_LV_LP4_KSHU0_PL;
				break;
			case KSHU1:
				ret = HQA_VCORE_LV_LP4_KSHU1_PL;
				break;
			case KSHU2:
				ret = HQA_VCORE_LV_LP4_KSHU2_PL;
				break;
			case KSHU3:
				ret = HQA_VCORE_LV_LP4_KSHU3_PL;
				break;
			case KSHU4:
				ret = HQA_VCORE_LV_LP4_KSHU4_PL;
				break;
			case KSHU5:
				ret = HQA_VCORE_LV_LP4_KSHU5_PL;
				break;
			case KSHU6:
				ret = HQA_VCORE_LV_LP4_KSHU6_PL;
				break;
		};
	} else if (voltage_sel_info_ptr.vcore == LEVEL_HV) {
		switch(kshu) {
			case KSHU0:
				ret = HQA_VCORE_HV_LP4_KSHU0_PL;
				break;
			case KSHU1:
				ret = HQA_VCORE_HV_LP4_KSHU1_PL;
				break;
			case KSHU2:
				ret = HQA_VCORE_HV_LP4_KSHU2_PL;
				break;
			case KSHU3:
				ret = HQA_VCORE_HV_LP4_KSHU3_PL;
				break;
			case KSHU4:
				ret = HQA_VCORE_HV_LP4_KSHU4_PL;
				break;
			case KSHU5:
				ret = HQA_VCORE_HV_LP4_KSHU5_PL;
				break;
			case KSHU6:
				ret = HQA_VCORE_HV_LP4_KSHU6_PL;
				break;
		};
	} else {
#if defined(VCORE_BIN)
		ret = 0;
#else
		switch(kshu) {
			case KSHU0:
				ret = HQA_VCORE_NV_LP4_KSHU0_PL;
				break;
			case KSHU1:
				ret = HQA_VCORE_NV_LP4_KSHU1_PL;
				break;
			case KSHU2:
				ret = HQA_VCORE_NV_LP4_KSHU2_PL;
				break;
			case KSHU3:
				ret = HQA_VCORE_NV_LP4_KSHU3_PL;
				break;
			case KSHU4:
				ret = HQA_VCORE_NV_LP4_KSHU4_PL;
				break;
			case KSHU5:
				ret = HQA_VCORE_NV_LP4_KSHU5_PL;
				break;
			case KSHU6:
				ret = HQA_VCORE_NV_LP4_KSHU6_PL;
				break;
		};
#endif
	}

	return ret;
}

#endif

#ifdef DRAM_ADAPTIVE
static int update_dram_setting(EMI_SETTINGS *default_emi_setting, unsigned int dram_type, DRAM_INFO_BY_MRR_T *dram_info)
{
	default_emi_setting->type = dram_type;

	if (dram_info != NULL) {
		default_emi_setting->DRAM_RANK_SIZE[0] = (u64)dram_info->u8MR8Density[0];
		default_emi_setting->DRAM_RANK_SIZE[1] = (u64)dram_info->u8MR8Density[1];

		default_emi_setting->iLPDDR3_MODE_REG_5 = dram_info->u2MR5VendorID;

		if (dram_info->u4RankNum == 1) {
			if (dram_info->u1DieNum[RANK_0] == 1)
				default_emi_setting->dram_cbt_mode_extern = CBT_R0_R1_NORMAL;
			else if (dram_info->u1DieNum[RANK_0] == 2)
				default_emi_setting->dram_cbt_mode_extern = CBT_R0_R1_BYTE;
			else
				return -1;
		} else if (dram_info->u4RankNum == 2) {
			if ((dram_info->u1DieNum[RANK_0] == 1) && (dram_info->u1DieNum[RANK_1] == 1))
				default_emi_setting->dram_cbt_mode_extern = CBT_R0_R1_NORMAL;
			else if ((dram_info->u1DieNum[RANK_0] == 1) && (dram_info->u1DieNum[RANK_1] == 2))
				default_emi_setting->dram_cbt_mode_extern = CBT_R0_NORMAL_R1_BYTE;
			else if ((dram_info->u1DieNum[RANK_0] == 2) && (dram_info->u1DieNum[RANK_1] == 1))
				default_emi_setting->dram_cbt_mode_extern = CBT_R0_BYTE_R1_NORMAL;
			else if ((dram_info->u1DieNum[RANK_0] == 2) && (dram_info->u1DieNum[RANK_1] == 2))
				default_emi_setting->dram_cbt_mode_extern = CBT_R0_R1_BYTE;
			else
				return -2;
		} else
			return -3;
	} else
		return -4;

	return 0;
}

static int decode_emi_info(EMI_INFO_T *emi_info, unsigned int dram_type, DRAM_INFO_BY_MRR_T *dram_info)
{
	unsigned int i;
	unsigned long long die_size;

	emi_info->dram_type = dram_type;
	emi_info->ch_num = 2;
	emi_info->bank_width[0] = 3;
	emi_info->bank_width[1] = 3;
	emi_info->col_width[0] = 10;
	emi_info->col_width[1] = 10;

	if (dram_info != NULL) {
        emi_info->rank_size[0] = (u64)dram_info->u8MR8Density[0];
        emi_info->rank_size[1] = (u64)dram_info->u8MR8Density[1];
        /**
         * die size = chn * rank_num * rank_size
         **/
        //emi_info->rank_size[0] /= emi_info->ch_num;
        //emi_info->rank_size[1] /= emi_info->ch_num;
		emi_info->rk_num = dram_info->u4RankNum;

		for (i = 0; i < emi_info->rk_num; i++) {
			die_size = emi_info->rank_size[i] / dram_info->u1DieNum[i];

			switch (die_size | (dram_info->u1DieNum[i] << 4) | u1IsLP4Family(dram_type)) {
			case 0x20000011ULL:
			case 0x20000021ULL:
			case 0x40000021ULL:
			case 0x30000011ULL:
			case 0x40000011ULL:
				emi_info->row_width[i] = 15;
				break;
			case 0x30000021ULL:
			case 0x60000011ULL:
			case 0x80000011ULL:
				emi_info->row_width[i] = 16;
				break;
			case 0x060000021ULL:
			case 0x080000021ULL:
			case 0x0C0000011ULL:
			case 0x100000011ULL:
				emi_info->row_width[i] = 17;
				break;
			case 0x0C0000021ULL:
			case 0x100000021ULL:
				emi_info->row_width[i] = 18;
				break;
			default:
				return -1;
			}
		}
	} else
		return -1;
	return 0;
}
#endif

#if (FOR_DV_SIMULATION_USED==0)
void dram_auto_detection(void)
{
	DRAM_INFO_BY_MRR_T dram_info;
	EMI_INFO_T emi_info;
	DRAM_CBT_MODE_EXTERN_T dram_mode;
	unsigned int dram_type;
	int ret;

	dram_type = (unsigned int)mt_get_dram_type_for_dis();
	g_default_emi_setting.type &= ~0xFF;
	g_default_emi_setting.type |= (dram_type & 0xFF);
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	if (!u1IsLP4Family(dram_type) ||
		read_offline_dram_mdl_data(&dram_info) < 0) {
#endif
		dram_mode = (u1IsLP4Family(dram_type))?
			CBT_BYTE_MODE1 : CBT_NORMAL_MODE;
#if defined(SLT)
		SLT_Test_Main_Flow(dram_type, dram_mode, &dram_info, SLT_USED);
#endif
		Init_DRAM(dram_type, dram_mode, &dram_info, GET_MDL_USED);
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
		if (u1IsLP4Family(dram_type))
			write_offline_dram_mdl_data(&dram_info);
	}
#endif

#ifdef DRAM_QVL_CHECK
	ret = check_qvl(&dram_info, dram_type);
	if (ret) {
		dramc_crit("[DRAMC] check_qvl err %d\n", ret);
		DRAMC_ASSERT(0);
	}
#endif

	ret = update_dram_setting(&g_default_emi_setting, dram_type, &dram_info);
    if (ret) {
		dramc_crit("[DRAMC] update_dram_setting err %d\n", ret);
		DRAMC_ASSERT(0);
	}

	ret = decode_emi_info(&emi_info, dram_type, &dram_info);
	if (ret) {
		dramc_crit("[DRAMC] decode_emi_info err %d\n", ret);
		DRAMC_ASSERT(0);
	}


	ret = update_emi_setting(&g_default_emi_setting, &emi_info);
	if (ret) {
		dramc_crit("[DRAMC] update_emi_setting err %d\n", ret);
		DRAMC_ASSERT(0);
	}

	auto_detect_done = 1;
}

void mt_set_emi(struct dramc_param *dparam)
{
	//int index;
	/*unsigned int SW_CTRL_VC, HW_CTRL_VC;*/
	EMI_SETTINGS *emi_set;
	dramc_crit("[DRAMC] Dram fast K start\n");
#ifdef VOLTAGE_SEL
	update_voltage_select_info();
#endif

#if ENABLE_PINMUX_FOR_RANK_SWAP
	EMI_rank_swap_handle();
#endif


	setup_dramc_voltage_by_pmic();

#if DRAM_AUXADC_CONFIG
	get_ch_num_by_auxadc();
#endif

#ifdef DRAM_ADAPTIVE
	dram_auto_detection();
#endif
	emi_set = &g_default_emi_setting;

#ifdef DDR_RESERVE_MODE
	if(g_ddr_reserve_enable==1 && g_ddr_reserve_success==0)
		Before_Init_DRAM_While_Reserve_Mode_fail(emi_set->type & 0xF);
#endif

#if (CFG_DRAM_LOG_TO_STORAGE)
		log_start = 1;
		print("log_start=0x%x part_dram_data_addr_uart=0x%llx \n",log_start,part_dram_data_addr_uart);
#endif
#if defined(SLT)
    SLT_Init_DRAM((emi_set->type & 0xF), emi_set->dram_cbt_mode_extern, NULL, NORMAL_USED);
#else
	Init_DRAM((emi_set->type & 0xF), emi_set->dram_cbt_mode_extern, NULL, NORMAL_USED);
#endif
	switch_dramc_voltage_to_auto_mode();
	restore_vcore_setting();

#if (CFG_DRAM_LOG_TO_STORAGE)
    log_start = 0;
    print("log_start=0x%x part_dram_data_addr_uart=0x%llx \n",log_start,part_dram_data_addr_uart);
#endif
#if 0
	{
		DRAMC_CTX_T * p = psCurrDramCtx;
		DramcRegDump(p);
	}
#endif
}
#endif

#define DRAMC_ADDR_SHIFT_CHN(addr, channel) (addr + (channel * 0x10000))

static void put_dummy_read_pattern(unsigned long long dst_pa, unsigned long long src_pa, unsigned int len)
{
	*((volatile unsigned int *)(CQ_DMA_BASE + 0x018)) = 7 << 16;

	*((volatile unsigned int *)(CQ_DMA_BASE + 0x01c)) = src_pa;
	*((volatile unsigned int *)(CQ_DMA_BASE + 0x060)) = 0;

	*((volatile unsigned int *)(CQ_DMA_BASE + 0x020)) = dst_pa & 0xffffffff;
	*((volatile unsigned int *)(CQ_DMA_BASE + 0x064)) = dst_pa >> 32;

	*((volatile unsigned int *)(CQ_DMA_BASE + 0x024)) = len;
	dsb();
	*((volatile unsigned int *)(CQ_DMA_BASE + 0x008)) = 0x1;

	while(*((volatile unsigned int *)(CQ_DMA_BASE + 0x008)));
}

unsigned int get_dramc_addr(dram_addr_t *dram_addr, unsigned int offset)
{
	static char init_pattern = 0;
	unsigned int channel_num, rank_num;
	unsigned long long dummy_read_addr;
	unsigned long long rank_size[DRAMC_MAX_RK];
	unsigned int index;
	unsigned int *src_addr;

	channel_num = (unsigned int) get_dram_channel_nr();
	rank_num = (unsigned int) get_dram_rank_nr();
	get_rank_size_by_emi(rank_size);
	dummy_read_addr = 0x40000000;
	src_addr = (unsigned int *) 0x40000000;

	if (dram_addr->ch >= channel_num) {
		mcSHOW_DBG_MSG(("[DRAMC] invalid channel: %d\n", dram_addr->ch));
		return 0;
	}

	if (dram_addr->rk >= rank_num) {
		mcSHOW_DBG_MSG(("[DRAMC] invalid rank: %d\n", dram_addr->rk));
		return 0;
	}

	for (index = 0; index <= dram_addr->rk; index++)
		dummy_read_addr += rank_size[index];
	dummy_read_addr -= offset;
	dummy_read_addr &= ~(0x300);

	if (offset == 0x20) {
		for (index = 0; index < 4; index++)
			*(src_addr + index) = 0xAAAA5555;

		if (!init_pattern) {
			for (index = 0; index < channel_num; index++) {
				put_dummy_read_pattern(dummy_read_addr | (index << 8),
					(unsigned long long) src_addr, 16);
			}

			init_pattern = 1;
		}
	}

	dram_addr->full_sys_addr = dummy_read_addr;
	phy_addr_to_dram_addr(dram_addr, dummy_read_addr);

	return dram_addr->addr;
}

unsigned int get_dummy_read_addr(dram_addr_t *dram_addr)
{
	return get_dramc_addr(dram_addr, 0x20);
}

static unsigned int get_ta2_addr(dram_addr_t *dram_addr)
{
	return (get_dramc_addr(dram_addr, 0x1000) >> 2) & 0xFFFFFFF8;
}

void init_ta2_single_channel(unsigned int channel)
{
	unsigned int temp;
	dram_addr_t dram_addr;
	DRAMC_CTX_T *p = psCurrDramCtx;
	int test_cnt;


	temp = u4IO32Read4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_TEST2_A3, channel)) & 0x1FFFFFFF;
	vIO32Write4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_TEST2_A3, channel), temp);


	temp = u4IO32Read4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_TEST2_A4, channel)) & 0x8FFFFFFF;
	temp |= (0x4 << 28);
	vIO32Write4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_TEST2_A4, channel), temp);


	temp = u4IO32Read4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_TEST2_A3, channel)) & 0xFFFFFFF0;
	vIO32Write4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_TEST2_A3, channel), temp | 0x1);


	dram_addr.ch = channel;
	dram_addr.rk = 0;
	temp = (u4IO32Read4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_RK_TEST2_A1, channel)) & 0x00000007);
	vIO32Write4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_RK_TEST2_A1, channel), temp | get_ta2_addr(&dram_addr));
	dram_addr.rk = 1;
	temp = (u4IO32Read4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_RK_TEST2_A1+0x200, channel)) & 0x00000007);
	vIO32Write4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_RK_TEST2_A1+0x200, channel), temp | get_ta2_addr(&dram_addr));


	temp = (u4IO32Read4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_TEST2_A2, channel)) & 0x0000000F) | (0x20 << 4);
	vIO32Write4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_TEST2_A2, channel), temp);


	test_cnt = (get_dram_rank_nr() > 1) ? 1 : 0;
	vIO32WriteFldAlign(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_TEST2_A3, channel), 0, TEST2_A3_TESTAUDPAT);
	vIO32WriteFldAlign(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_TEST2_A3, channel), test_cnt, TEST2_A3_TESTCNT);
	vIO32WriteFldAlign(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_TEST2_A4, channel), 1, TEST2_A4_TESTXTALKPAT);

	return;
}

#ifdef LAST_DRAMC

static unsigned int is_last_dramc_initialized(void)
{
	if(last_dramc_info_ptr->ta2_result_magic != LAST_DRAMC_MAGIC_PATTERN) {
		return 0;
	} else {
		return 1;
	}
}

void update_last_dramc_info(void)
{
	unsigned int chn;
	unsigned long long latch_result = 0;
	unsigned int temp;
	unsigned int *curr;
	DRAMC_CTX_T *p = psCurrDramCtx;


	if(last_dramc_info_ptr->ta2_result_magic != LAST_DRAMC_MAGIC_PATTERN) {
		last_dramc_info_ptr->ta2_result_magic = LAST_DRAMC_MAGIC_PATTERN;
		last_dramc_info_ptr->ta2_result_last = 0;
		last_dramc_info_ptr->ta2_result_past = 0;
		last_dramc_info_ptr->ta2_result_checksum = LAST_DRAMC_MAGIC_PATTERN;
		last_dramc_info_ptr->reboot_count = 0;
		last_dramc_info_ptr->mr5 = mr5;
		last_dramc_info_ptr->mr6 = mr6;
		last_dramc_info_ptr->mr7 = mr7;
		last_dramc_info_ptr->mr8 = mr8;
	} else {
		last_dramc_info_ptr->ta2_result_checksum ^= last_dramc_info_ptr->reboot_count;
		last_dramc_info_ptr->reboot_count++;
		last_dramc_info_ptr->ta2_result_checksum ^= last_dramc_info_ptr->reboot_count;
	}




	for (chn = 0; chn < CHANNEL_NUM; ++chn) {
		//dramc_crit("[LastDRAMC] latch result before RST: %x\n", u4IO32Read4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_WDT_DBG_SIGNAL, chn)));
		latch_result = (latch_result << 16) | u4IO32Read4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_WDT_DBG_SIGNAL, chn)) & 0xFFFF;
		temp = u4IO32Read4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_WDT_RST, chn));
		vIO32Write4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_WDT_RST, chn), temp | 0x00000001);
		vIO32Write4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_WDT_RST, chn), temp & 0xFFFFFFFE);
		//dramc_crit("[LastDRAMC] latch result after RST: %x\n", u4IO32Read4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_WDT_DBG_SIGNAL, chn)));

	}

	last_dramc_info_ptr->ta2_result_checksum ^= last_dramc_info_ptr->ta2_result_past ^ latch_result;
	last_dramc_info_ptr->ta2_result_past = last_dramc_info_ptr->ta2_result_last;
	last_dramc_info_ptr->ta2_result_last = latch_result;
	for (temp = 0; temp < sizeof(LAST_DRAMC_INFO_T) / sizeof(temp); temp++) {
		curr = (unsigned int *)last_dramc_info_ptr + temp;
		dramc_crit("[LastDRAMC] 0x%x: 0x%x\n", curr, *curr);
	}

	return;
}

void init_ta2_all_channel(void)
{
	unsigned int chn;

	update_last_dramc_info();


	#if CFG_ENABLE_DCACHE
	plat_clean_invalidate_dcache();
	#endif

	for (chn = 0; chn < CHANNEL_NUM; ++chn)
		init_ta2_single_channel(chn);
}


unsigned int check_gating_err_in_dramc_latch(void)
{
	unsigned int chn, ret = 0;
	DRAMC_CTX_T *p = psCurrDramCtx;

	if ((g_boot_reason == BR_POWER_KEY) || (g_boot_reason == BR_USB)
			|| mtk_wdt_is_pmic_full_reset() || (is_last_dramc_initialized() == 0)){
		dramc_crit("for cold boot, always return 0\n");
		return 0;
	}

	for (chn = 0; chn <= 3; ++chn) {
		dramc_crit("[dramc] latch check in channel %d (0x%x)\n",
				chn, u4IO32Read4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_WDT_DBG_SIGNAL, chn)));
		if (u4IO32Read4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_WDT_DBG_SIGNAL, chn)) & 0x4000) {
			dramc_crit("[dramc] found gating error in channel %d (0x%x)\n",
					chn, u4IO32Read4B(DRAMC_ADDR_SHIFT_CHN(DRAMC_REG_WDT_DBG_SIGNAL, chn)));
			ret |= (1 << chn);
		}
	}

	return ret;
}

void dram_fatal_exception_detection_start(void)
{
	last_dramc_info_ptr = (LAST_DRAMC_INFO_T *) get_dbg_info_base(KEY_LAST_DRAMC);

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	part_dram_data_addr = get_part_addr("boot_para") + 0x100000;
	if (part_dram_data_addr != 0x0)
		dramc_crit("[dramc] init partition address is 0x%llx\n", part_dram_data_addr);
	else {
		dramc_crit("[dramc] init partition address is incorrect !!!\n");
	}
#endif

#if defined(SLT)
	part_dram_data_addr_slt = get_part_addr("boot_para") + 0x100000;
#endif

	if ((g_boot_reason == BR_POWER_KEY) || (g_boot_reason == BR_USB)
			|| mtk_wdt_is_pmic_full_reset() || (is_last_dramc_initialized() == 0)){

		dramc_crit("[dramc] init SRAM region for DRAM exception detection\n");
		last_dramc_info_ptr->last_fatal_err_flag = 0x0;
		last_dramc_info_ptr->storage_api_err_flag = 0x0;
		dram_fatal_init_stberr();
	} else {
		last_dramc_info_ptr->last_fatal_err_flag = last_dramc_info_ptr->fatal_err_flag;
		last_dramc_info_ptr->storage_api_err_flag = 0x0;
		dram_fatal_backup_stberr();
		dram_fatal_init_stberr();
	}

	last_dramc_info_ptr->fatal_err_flag = 1 << OFFSET_DRAM_FATAL_ERR;
	dsb();
}

void dram_fatal_exception_detection_end(void)
{
	last_dramc_info_ptr->fatal_err_flag = 0x0;
	dsb();
}

unsigned int check_dram_fatal_exception(void)
{
	dramc_crit("[dramc] DRAM_FATAL_ERR_FLAG = 0x%x\n", last_dramc_info_ptr->fatal_err_flag);

	return ((last_dramc_info_ptr->fatal_err_flag & ~((1 << OFFSET_DRAM_FATAL_ERR)|DDR_RSV_MODE_ERR_MASK)) != 0x0) ? 1 : 0;
}

unsigned int check_last_dram_fatal_exception(void)
{
	dramc_crit("[dramc] LAST_DRAM_FATAL_ERR_FLAG = 0x%x\n", last_dramc_info_ptr->last_fatal_err_flag);

	return ((last_dramc_info_ptr->last_fatal_err_flag & ~(DDR_RSV_MODE_ERR_MASK)) != 0x0) ? 1 : 0;
}

void dram_fatal_set_ta2_err(unsigned int chn, unsigned int err_code)
{
	unsigned int shift = OFFSET_DRAM_TA2_ERR + 2 * chn, ret;

	if (chn > 3)
		return;

	ret = last_dramc_info_ptr->fatal_err_flag & ~(0x7 << shift);
	last_dramc_info_ptr->fatal_err_flag = ret | ((err_code & 0x7) << shift);
	dsb();
}

void dram_fatal_set_gating_err(unsigned int chn, unsigned int err_code)
{
	unsigned int shift = OFFSET_DRAM_GATING_ERR + 4 * chn, ret;

	if (chn > 3)
		return;

	ret = last_dramc_info_ptr->fatal_err_flag & ~(0xf << shift);
	last_dramc_info_ptr->fatal_err_flag = ret | ((err_code & 0xf) << shift);
	dsb();
}

void dram_fatal_init_stberr(void)
{
	last_dramc_info_ptr->gating_err[0][0] = 0x0;
	last_dramc_info_ptr->gating_err[0][1] = 0x0;
	last_dramc_info_ptr->gating_err[1][0] = 0x0;
	last_dramc_info_ptr->gating_err[1][1] = 0x0;
	last_dramc_info_ptr->gating_err[2][0] = 0x0;
	last_dramc_info_ptr->gating_err[2][1] = 0x0;
	last_dramc_info_ptr->gating_err[3][0] = 0x0;
	last_dramc_info_ptr->gating_err[3][1] = 0x0;

	dsb();
}

void dram_fatal_backup_stberr(void)
{
	last_dramc_info_ptr->last_gating_err[0][0] = last_dramc_info_ptr->gating_err[0][0];
	last_dramc_info_ptr->last_gating_err[0][1] = last_dramc_info_ptr->gating_err[0][1];
	last_dramc_info_ptr->last_gating_err[1][0] = last_dramc_info_ptr->gating_err[1][0];
	last_dramc_info_ptr->last_gating_err[1][1] = last_dramc_info_ptr->gating_err[1][1];
	last_dramc_info_ptr->last_gating_err[2][0] = last_dramc_info_ptr->gating_err[2][0];
	last_dramc_info_ptr->last_gating_err[2][1] = last_dramc_info_ptr->gating_err[2][1];
	last_dramc_info_ptr->last_gating_err[3][0] = last_dramc_info_ptr->gating_err[3][0];
	last_dramc_info_ptr->last_gating_err[3][1] = last_dramc_info_ptr->gating_err[3][1];

	dsb();
}

void dram_fatal_set_stberr(unsigned int chn, unsigned int rk, unsigned int err_code)
{
	if ((chn > 3) || (rk > 1))
		return;

	last_dramc_info_ptr->gating_err[chn][rk] = err_code;

	dsb();
}

void dram_fatal_set_err(unsigned int err_code, unsigned int mask, unsigned int offset)
{
	unsigned int ret;

	ret = last_dramc_info_ptr->fatal_err_flag & ~(mask << offset);
	last_dramc_info_ptr->fatal_err_flag = ret | ((err_code & mask) << offset);
	dsb();
}

#endif

#if (FOR_DV_SIMULATION_USED==0)
int doe_get_config(const char* feature)
{
#if defined(ENABLE_DOE)
	char *doe_feature = dconfig_getenv(feature);
	int doe_result = atoi(doe_feature);
	dramc_crit("DOE force setting %s=%d\n", feature, doe_result);
	return doe_result;
#elif defined(NVCORE_NVDRAM)
	return LEVEL_NV;
#elif defined(LVCORE_LVDRAM)
	return LEVEL_LV;
#elif defined(HVCORE_HVDRAM)
	return LEVEL_HV;
#else
	return 0;
#endif
}
#endif

#if (CFG_DRAM_LOG_TO_STORAGE)
void log_to_storage(const char c)
{
	int ret, clr_count;
	blkdev_t *bootdev = NULL;
	static u8 logen = 0;

	bootdev = blkdev_get(CFG_BOOT_DEV);

	if (log_start && (!logen)) {
		logen = 1;
		logcount = 0;
		part_dram_data_addr_uart = get_part_addr("boot_para") + 0x100000;
		memset(&logbuf, 0, sizeof(logbuf));
		for (clr_count = 0; clr_count < 3072 ; clr_count++)
			ret = blkdev_write(bootdev, (part_dram_data_addr_uart + (1024 * clr_count)), 1024, (u8*)&logbuf, storage_get_part_id(STORAGE_PHYS_PART_USER));
	}

	if (log_start) {
//    	if (((((char) c >> 4) & 0x7) > 1) & ((((char) c >> 4) & 0x7) < 7))
//        logbuf[logcount] = ((char) c & 0xF0) | (((char) c >> 2) & 0x03) | (((char) c << 2) & 0x0C);
//			else
				logbuf[logcount] = (char) c;
    	logcount = logcount + 1;

    	if (logcount==1024) {
    		logcount = 0;
    		ret = blkdev_write(bootdev, part_dram_data_addr_uart, 1024, (u8*)&logbuf, storage_get_part_id(STORAGE_PHYS_PART_USER));
    		part_dram_data_addr_uart = part_dram_data_addr_uart + 1024;
    	}
    }
}
#endif
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION

#if !__ETT__
//[FOR_CHROMEOS]
//#include <blkdev.h>
//#include <partition.h>
//#include <pl_version.h>
#else
#include "ett_common.h"
#include "emi.h"
#endif
u32 g_dram_storage_api_err_code;
#if 0 //[FOR_CHROMEOS]
static u16 crc16(const u8* data, u32 length){
	u8 x;
	u16 crc = 0xFFFF;

	while (length--) {
		x = crc >> 8 ^ *data++;
		x ^= x >> 4;
		crc = (crc << 8) ^ ((u8)(x << 12)) ^ ((u8)(x <<5)) ^ ((u8)x);
	}
	return crc;
}

static void assign_checksum_for_dram_data(DRAM_CALIBRATION_SHU_DATA_T *shu_data)
{

	shu_data->checksum = 0;
	shu_data->checksum = crc16((u8*)shu_data, sizeof(*shu_data));
}

static int check_checksum_for_dram_data(DRAM_CALIBRATION_SHU_DATA_T *shu_data)
{
	u16 checksum_in_storage = shu_data->checksum;

	assign_checksum_for_dram_data(shu_data);

	return (shu_data->checksum == checksum_in_storage) ? 1 : 0;
}

#if !__ETT__
static void assign_checksum_for_mdl_data(DRAM_CALIBRATION_MRR_DATA_T *mrr_info)
{

	mrr_info->checksum = 0;
	mrr_info->checksum = crc16((u8*)mrr_info, sizeof(*mrr_info));
}


static int check_checksum_for_mdl_data(DRAM_CALIBRATION_MRR_DATA_T *mrr_info)
{
	u16 checksum_in_storage = mrr_info->checksum;

	assign_checksum_for_mdl_data(mrr_info);

	return (mrr_info->checksum == checksum_in_storage) ? 1 : 0;
}
#endif
#endif
static void fastk_data_dump(struct sdram_params *params, u8 shu){
#if 0
	print("[Full_K]Fastk data dump \n");
	print("shuffle %d(For verify: cbt_final_vref CHA:%u, CHB: %u)\n", shu, params->cbt_final_vref[CHANNEL_A][RANK_0], params->cbt_final_vref[CHANNEL_B][RANK_0]);
	print("shuffle %d(For verify: WL B0:%u, B1: %u)\n", shu, params->wr_level[CHANNEL_A][RANK_0][0], params->wr_level[CHANNEL_B][RANK_0][0]);
	print("shuffle %d(For verify: tx_window_vref CHA:%u, CHB: %u)\n", shu, params->tx_window_vref[CHANNEL_A][RANK_0], params->tx_window_vref[CHANNEL_B][RANK_0]);
	print("shuffle %d(For verify: rx_datlat CHA:%u, CHB: %u)\n", shu, params->rx_datlat[CHANNEL_A][RANK_0], params->rx_datlat[CHANNEL_B][RANK_0]);
	print("shuffle %d(For verify: rx_datlat CHA:%u, CHB: %u)\n", shu, params->rx_datlat[CHANNEL_A][RANK_0], params->rx_datlat[CHANNEL_B][RANK_0]);

	print("shuffle %d(For verify: cbt_final_vref CHC:%u, CHD: %u)\n", shu, params->cbt_final_vref[CHANNEL_C][RANK_0], params->cbt_final_vref[CHANNEL_D][RANK_0]);
	print("shuffle %d(For verify: WL CHC:%u, CHD: %u)\n", shu, params->wr_level[CHANNEL_C][RANK_0][0], params->wr_level[CHANNEL_D][RANK_0][0]);
	print("shuffle %d(For verify: tx_window_vref CHC:%u, CHD: %u)\n", shu, params->tx_window_vref[CHANNEL_C][RANK_0], params->tx_window_vref[CHANNEL_D][RANK_0]);
	print("shuffle %d(For verify: rx_datlat CHC:%u, CHD: %u)\n", shu, params->rx_datlat[CHANNEL_C][RANK_0], params->rx_datlat[CHANNEL_D][RANK_0]);
	print("shuffle %d(For verify: rx_datlat CHC:%u, CHD: %u)\n", shu, params->rx_datlat[CHANNEL_C][RANK_0], params->rx_datlat[CHANNEL_D][RANK_0]);
	print("\n");
#endif
}
static int read_offline_dram_mdl_data(DRAM_INFO_BY_MRR_T *DramInfo)
{
	return -1;
}

static int write_offline_dram_mdl_data(DRAM_INFO_BY_MRR_T *DramInfo)
{
    return -1;
}
int read_offline_dram_calibration_data(DRAM_DFS_SRAM_SHU_T shuffle, SAVE_TIME_FOR_CALIBRATION_T *offLine_SaveData)
{

	struct sdram_params *params;

	if (!dramc_params)
		return -1;
	params = &dramc_params->dramc_datas.freq_params[shuffle];

	dramc_info("read calibration data from shuffle %d(For verify: WL B0:%u, B1: %u)\n",
		   shuffle, params->wr_level[CHANNEL_A][RANK_0][0], params->wr_level[CHANNEL_B][RANK_0][0]);

	memcpy(offLine_SaveData, params, sizeof(*offLine_SaveData));
	fastk_data_dump(params, shuffle);

	return 0;
}

int write_offline_dram_calibration_data(DRAM_DFS_SRAM_SHU_T shuffle, SAVE_TIME_FOR_CALIBRATION_T *offLine_SaveData)
{
    return 0;
}

int clean_dram_calibration_data(void)
{
    return 0;
}

#else
#if 0
DRAM_CALIBRATION_DATA_T dram_data;

static int read_offline_dram_mdl_data(DRAM_INFO_BY_MRR_T *DramInfo)
{
    return -1;
}
static int write_offline_dram_mdl_data(DRAM_INFO_BY_MRR_T *DramInfo)
{
   return -1;
}

int read_offline_dram_calibration_data(DRAM_DFS_SRAM_SHU_T shuffle, SAVE_TIME_FOR_CALIBRATION_T *offLine_SaveData)
{
	return 0;
}

int write_offline_dram_calibration_data(DRAM_DFS_SRAM_SHU_T shuffle, SAVE_TIME_FOR_CALIBRATION_T *offLine_SaveData)
{
	return 0;
}

int clean_dram_calibration_data(void)
{
	return;
}
#endif
#endif

#ifdef LAST_DRAMC
void set_err_code_for_storage_api(void)
{

	last_dramc_info_ptr->storage_api_err_flag = g_dram_storage_api_err_code;
	dsb();

}
#endif

#if defined(SLT) && (!__ETT__)
#include <storage_api.h>
#include <emi.h>
int clean_slt_data(void)
{
	DRAM_SLT_DATA_T data;
	data.header.stage_status = -1;
	data.header.pl_version = PL_VERSION;
	return write_slt_data(&data);
}

int read_slt_data(DRAM_SLT_DATA_T *data)
{
	int i, ret;
	blkdev_t *bootdev = NULL;

	if (data == NULL) {
		dramc_crit("[dramc_slt] SLT data == NULL, skip\n");
	}

	bootdev = blkdev_get(CFG_BOOT_DEV);
	if (bootdev == NULL) {
		dramc_crit("[dramc_slt] can't find boot device(%d)\n", CFG_BOOT_DEV);
		return SLT_ERR_NO_DEV;
	}

	if (!part_dram_data_addr_slt) {
		return SLT_ERR_NO_ADDR;
	}

	ret = blkdev_read(bootdev, part_dram_data_addr_slt, sizeof(DRAM_SLT_DATA_T), (u8*)data, storage_get_part_id(STORAGE_PHYS_PART_USER));

	if (ret != 0) {
		return SLT_ERR_READ_FAIL;
	}

	if (data->header.pl_version != PL_VERSION) {
		dramc_crit("[dramc_slt] PL_VERSION mismatch\n");
		clean_slt_data();
		blkdev_read(bootdev, part_dram_data_addr_slt, sizeof(DRAM_SLT_DATA_T), (u8*)data, storage_get_part_id(STORAGE_PHYS_PART_USER));
	}

	return 0;
}

int write_slt_data(DRAM_SLT_DATA_T *data)
{
	int ret;
	blkdev_t *bootdev = NULL;

	if (data == NULL) {
		dramc_crit("[dramc_slt] data == NULL, skip\n");
		return SLT_ERR_NO_DATA;
	}

	bootdev = blkdev_get(CFG_BOOT_DEV);
	if (bootdev == NULL) {
		dramc_crit("[dramc_slt] can't find boot device(%d)\n", CFG_BOOT_DEV);
		return SLT_ERR_NO_DEV;
	}

	if (!part_dram_data_addr_slt) {
		return SLT_ERR_NO_ADDR;
	}

	data->header.pl_version = PL_VERSION;

	ret = blkdev_write(bootdev, part_dram_data_addr_slt, sizeof(DRAM_SLT_DATA_T), (u8*)data, storage_get_part_id(STORAGE_PHYS_PART_USER));
	if (ret != 0) {
		dramc_crit("[dramc_slt] blkdev_write failed\n");
		return SLT_ERR_WRITE_FAIL;
	}

	return 0;
}
#endif

#if __FLASH_TOOL_DA__
unsigned int get_mr8_by_mrr(U8 channel, U8 rank)
{
	DRAMC_CTX_T *p = psCurrDramCtx;
	unsigned int mr8_value;

	p->channel = channel;

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), rank, SWCMD_CTRL0_MRRRK);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), 8, SWCMD_CTRL0_MRSMA);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 1, SWCMD_EN_MRREN);
	while (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), SPCMDRESP_MRR_RESPONSE) ==0)
		mcDELAY_US(1);
	mr8_value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MRR_STATUS), MRR_STATUS_MRR_REG);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0, SWCMD_EN_MRREN);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), 0, SWCMD_CTRL0_MRRRK);

	return (mr8_value & 0xff);
}
#endif


#if DRAM_AUXADC_CONFIG
static unsigned int get_ch_num_by_auxadc(void)
{
    unsigned int ret = 0, voltage = 0;
    ret = iio_read_channel_processed(5, &voltage);
    if (ret == 0) {
        if (voltage < 700)
        	{
				channel_num_auxadc = CHANNEL_FOURTH;
				dram_type_auxadc = PINMUX_DSC;
        	}
        else if (voltage >= 700 && voltage < 1200)
        	{
	    		channel_num_auxadc = CHANNEL_DUAL;
				dram_type_auxadc = PINMUX_EMCP;
        	}
		else
			{
	    		channel_num_auxadc = CHANNEL_DUAL;
				dram_type_auxadc = PINMUX_DSC;
			}
        dramc_crit("Channel num from auxadc: %d, \n", channel_num_auxadc);
        dramc_crit("dram_type_auxadc from auxadc: %d, \n", dram_type_auxadc);
        dramc_crit("voltage from auxadc: %d, \n", voltage);
   	}
    else
        dramc_crit("Error! Read AUXADC value fail\n");

}
#endif

