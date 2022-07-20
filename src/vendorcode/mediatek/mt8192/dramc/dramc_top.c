/* SPDX-License-Identifier: BSD-3-Clause */

//=============================================================================
//	Include Files
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
#include <assert.h>
#include <print.h>
#include <string.h>
#include "dramc_common.h"
#include "dramc_int_global.h"

#include <emi_hw.h>
#include <emi.h>

#if !__FLASH_TOOL_DA__ && !__ETT__
#include "custom_emi.h"   // fix build error: emi_settings
#endif

#if (FOR_DV_SIMULATION_USED==0)
#include <soc/mt6359p.h>
#include <soc/regulator.h>

/* now we can use definition MTK_PMIC_MT6359
 * ==============================================================
 * PMIC   |Power |Dflt. Volt. |Step   |Support FPWM |Cmt.
 * --------------------------------------------------------------
 * MT6359 |Vcore |0.8v		  |6.25mV |Yes			|
 *		  |Vm18  |1.8v		  |0.1V   |No			|
 * --------------------------------------------------------------
 * MT6360 |Vdram |1.125v	  |5mV	  |Yes			|(DRAM Vdram)
 *		  |Vmddr |0.75v 	  |10mV   |No			|(AP Vdram)
 *		  |Vddq  |0.6v		  |10mV   |No			|
 * ==============================================================
 */
//#define MTK_PMIC_MT6359
#endif

#if !__ETT__
#define mt_reg_sync_write(x,y) mt_reg_sync_writel(y,x)
#endif
#define seclib_get_devinfo_with_index(x)	0

#ifdef MTK_PMIC_MT6359
#include <regulator/mtk_regulator.h>
#include <mt6359.h>
#endif

#include <soc/dramc_param.h>
#include <soc/emi.h>

//=============================================================================
//	Definition
//=============================================================================

//=============================================================================
//	Global Variables
//=============================================================================
int emi_setting_index = -1;

#ifdef MTK_PMIC_MT6359
static struct mtk_regulator reg_vio18, reg_vdram, reg_vcore, reg_vddq, reg_vmddr;
#endif

#ifdef LAST_DRAMC
static LAST_DRAMC_INFO_T* last_dramc_info_ptr;
#endif

#ifdef VOLTAGE_SEL
static VOLTAGE_SEL_INFO_T voltage_sel_info_ptr;
#endif

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
extern u64 get_part_addr(const char *name);
#endif

#if defined(SLT) && (!__ETT__)
#include <pl_version.h>
static u64 part_dram_data_addr_slt = 0;
int read_slt_data(DRAM_SLT_DATA_T *data);
int write_slt_data(DRAM_SLT_DATA_T *data);
int clean_slt_data(void);
#endif
//=============================================================================
//	External references
//=============================================================================
extern char* opt_dle_value;

void print_DBG_info(DRAMC_CTX_T *p);

void mdl_setting(DRAMC_CTX_T *p)
{
	EMI_SETTINGS *emi_set;

	if(emi_setting_index == -1)
		emi_set = &default_emi_setting;
#if (FOR_DV_SIMULATION_USED==0)
	else
		emi_set = &emi_settings[emi_setting_index];
#endif

	emi_init();

	//The following is MDL settings
	set_cen_emi_cona(emi_set->EMI_CONA_VAL);
	set_cen_emi_conf(emi_set->EMI_CONF_VAL);
	set_cen_emi_conh(emi_set->EMI_CONH_VAL);

	// CHNA and CHNB uses the same CH0 setting
	set_chn_emi_cona(emi_set->CHN0_EMI_CONA_VAL);
	//set_chn_emi_conc(0x4);

	p->vendor_id = emi_set->iLPDDR3_MODE_REG_5;
}

void print_DBG_info(DRAMC_CTX_T *p)
{
}

int mt_get_dram_type(void)
{
	unsigned int dtype = mt_get_dram_type_from_hw_trap();

	if (dtype == TYPE_LPDDR4X)
		return DTYPE_LPDDR4X;
	else
		ASSERT(0);

	return 0;
}

#ifdef DDR_RESERVE_MODE
extern u32 g_ddr_reserve_enable;
extern u32 g_ddr_reserve_success;
#define TIMEOUT 3
extern void before_Dramc_DDR_Reserved_Mode_setting(void);

#define	CHAN_DRAMC_NAO_MISC_STATUSA(base)	(base + 0x80)
#define SREF_STATE				(1 << 16)

unsigned int is_dramc_exit_slf(void)
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

	dramc_crit("ALL DRAM CHAN is not in self-refresh\n");
	return 1;
}

#endif

unsigned int dramc_set_vcore_voltage(unsigned int vcore)
{
#ifdef MTK_PMIC_MT6359
	return mtk_regulator_set_voltage(&reg_vcore, vcore, MAX_VCORE);
#endif
	dramc_debug("%s set vcore to %d\n", __func__, vcore);
	//mt6359p_buck_set_voltage(MT6359P_GPU11, vcore);

	mainboard_set_regulator_voltage(MTK_REGULATOR_VCORE, vcore);

	return 0;
}

unsigned int dramc_get_vcore_voltage(void)
{
#ifdef MTK_PMIC_MT6359
	return mtk_regulator_get_voltage(&reg_vcore);
#else
	return mainboard_get_regulator_voltage(MTK_REGULATOR_VCORE);
#endif
}

unsigned int dramc_set_vdram_voltage(unsigned int ddr_type, unsigned int vdram)
{
#ifdef MTK_PMIC_MT6359
	mtk_regulator_set_voltage(&reg_vdram, vdram, MAX_VDRAM);
#endif
	mainboard_set_regulator_voltage(MTK_REGULATOR_VDD2, vdram);
	return 0;
}

unsigned int dramc_get_vdram_voltage(unsigned int ddr_type)
{
#ifdef MTK_PMIC_MT6359
	return mtk_regulator_get_voltage(&reg_vdram);
#else
	return mainboard_get_regulator_voltage(MTK_REGULATOR_VDD2);
#endif
}

unsigned int dramc_set_vddq_voltage(unsigned int ddr_type, unsigned int vddq)
{
#ifdef MTK_PMIC_MT6359
	mtk_regulator_set_voltage(&reg_vddq, vddq, MAX_VDDQ);
#endif
	mainboard_set_regulator_voltage(MTK_REGULATOR_VDDQ, vddq);
	return 0;
}

unsigned int dramc_get_vddq_voltage(unsigned int ddr_type)
{
#ifdef MTK_PMIC_MT6359
	return mtk_regulator_get_voltage(&reg_vddq);
#else
	return mainboard_get_regulator_voltage(MTK_REGULATOR_VDDQ);
#endif
}

unsigned int dramc_set_vmddr_voltage(unsigned int vmddr)
{
#ifdef MTK_PMIC_MT6359
	return mtk_regulator_set_voltage(&reg_vmddr, vmddr, MAX_VMDDR);
#endif
	mainboard_set_regulator_voltage(MTK_REGULATOR_VMDDR, vmddr);
	return 0;
}

unsigned int dramc_get_vmddr_voltage(void)
{
#ifdef MTK_PMIC_MT6359
	return mtk_regulator_get_voltage(&reg_vmddr);
#else
	return mainboard_get_regulator_voltage(MTK_REGULATOR_VMDDR);
#endif
}

unsigned int dramc_set_vio18_voltage(unsigned int vio18)
{
#ifdef MTK_PMIC_MT6359
	unsigned int twist = vio18 % UNIT_VIO18_STEP / UNIT_VIO18;
	vio18 -= vio18 % UNIT_VIO18_STEP;
	pmic_config_interface(PMIC_RG_VM18_VOCAL_ADDR, twist, PMIC_RG_VM18_VOCAL_MASK, PMIC_RG_VM18_VOCAL_SHIFT);
	return mtk_regulator_set_voltage(&reg_vio18, vio18, MAX_VIO18);
#else
	mainboard_set_regulator_voltage(MTK_REGULATOR_VDD1, vio18);
	return 0;
#endif
}


unsigned int dramc_get_vio18_voltage(void)
{
#ifdef MTK_PMIC_MT6359
	unsigned int twist = 0;
	pmic_read_interface(PMIC_RG_VM18_VOCAL_ADDR, &twist, PMIC_RG_VM18_VOCAL_MASK, PMIC_RG_VM18_VOCAL_SHIFT);
	return mtk_regulator_get_voltage(&reg_vio18) + twist * UNIT_VIO18;
#else
	return mainboard_get_regulator_voltage(MTK_REGULATOR_VDD1);
#endif
}

#define GPIO_TRAPPING_REG (0x100056f0)
unsigned int is_discrete_lpddr4(void)
{
	unsigned int type, ret;

	type = get_ddr_type();

	ret = (type == DDR_TYPE_DISCRETE) ? 1 : 0;
	dramc_debug("%s: %d\n", __func__, ret);

	return ret;
}

unsigned int mt_get_dram_type_from_hw_trap(void)
{
	return TYPE_LPDDR4X;
}

void setup_dramc_voltage_by_pmic(void)
{
#ifdef VOLTAGE_SEL
	int vcore;
#endif
#ifdef MTK_PMIC_MT6359
	int ret;

	ret = mtk_regulator_get("vm18", &reg_vio18);
	if (ret)
		dramc_debug("mtk_regulator_get vio18 fail\n");

	ret = mtk_regulator_get("vcore", &reg_vcore);
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
	dramc_set_vio18_voltage(vio18_voltage_select());
	#else
	dramc_set_vio18_voltage(SEL_VIO18);
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
	dramc_set_vdram_voltage(TYPE_LPDDR4, vdram_voltage_select());
	#else
	dramc_set_vdram_voltage(TYPE_LPDDR4, SEL_PREFIX_VDRAM(LP4));
	#endif

	#ifdef VOLTAGE_SEL
	dramc_set_vddq_voltage(TYPE_LPDDR4, vddq_voltage_select());
	#else
	dramc_set_vddq_voltage(TYPE_LPDDR4, SEL_PREFIX_VDDQ);
	#endif

	#ifdef VOLTAGE_SEL
	dramc_set_vmddr_voltage(vmddr_voltage_select());
	#else
	dramc_set_vmddr_voltage(SEL_PREFIX_VMDDR);
	#endif

	dramc_debug("Vio18 = %d\n", dramc_get_vio18_voltage());
	dramc_debug("Vcore = %d\n", dramc_get_vcore_voltage());
	dramc_debug("Vdram = %d\n", dramc_get_vdram_voltage(TYPE_LPDDR4));
	dramc_debug("Vddq = %d\n", dramc_get_vddq_voltage(TYPE_LPDDR4));
	dramc_debug("Vmddr = %d\n", dramc_get_vmddr_voltage());
#endif
}

static void restore_vcore_setting(void)
{
#ifdef VOLTAGE_SEL
	int vcore;
#endif
#ifdef MTK_PMIC_MT6359
	int ret;

	ret = mtk_regulator_get("vcore", &reg_vcore);
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

void switch_dramc_voltage_to_auto_mode(void)
{
#ifdef MTK_PMIC_MT6359
	mtk_regulator_set_mode(&reg_vcore, 0x0);
	mtk_regulator_set_mode(&reg_vdram, 0x0);
#endif
}

#ifdef COMBO_MCP
static int mt_get_mdl_number(void)
{
	static int found = 0;
	static int mdl_number = -1;

	found = 1;
	mdl_number = get_ddr_geometry();

	return mdl_number;
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
	int index;
	int cen_emi_cona;

#ifdef COMBO_MCP
#ifdef DDR_RESERVE_MODE
	if(g_ddr_reserve_enable==1 && g_ddr_reserve_success==1) {
		return get_rank_nr_by_emi();
	} else
#endif
	{
		index = mt_get_mdl_number();
		if (index < 0 || index >=  num_of_emi_records)
			return 0;

		cen_emi_cona = emi_settings[index].EMI_CONA_VAL;
	}
#else
	cen_emi_cona = default_emi_setting.EMI_CONA_VAL;
#endif

	if ((cen_emi_cona & (1 << 17)) != 0 || //for channel 0
		(cen_emi_cona & (1 << 16)) != 0 )  //for channel 1
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

void get_dram_rank_size(u64 dram_rank_size[])
{
#ifdef COMBO_MCP
	int index, rank_nr, i;

#ifdef DDR_RESERVE_MODE
	if(g_ddr_reserve_enable==1 && g_ddr_reserve_success==1)
	{
		get_rank_size_by_emi(dram_rank_size);
	}
	else
#endif
	{
		index = mt_get_mdl_number();

		if (index < 0 || index >= num_of_emi_records)
		{
			return;
		}

		rank_nr = get_dram_rank_nr();

		for(i = 0; i < rank_nr; i++){
			dram_rank_size[i] = emi_settings[index].DRAM_RANK_SIZE[i];
			dramc_debug("%d:dram_rank_size:%llx\n",i,dram_rank_size[i]);
		}
	}
	return;
#else
	get_rank_size_by_emi(dram_rank_size);
	return;
#endif
}

void get_dram_freq_step(u32 dram_freq_step[])
{
	unsigned int i;
	unsigned int defined_step[DRAMC_FREQ_CNT] = {
		4266, 3200, 2400, 1866, 1600, 1200, 800};

	if (is_discrete_lpddr4()) {
		defined_step[0] = 3200;
	}
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

static void freq_table_are_all_3094(void)
{
   gFreqTbl[0].freq_sel = LP4_DDR3200;
   gFreqTbl[0].divmode = DIV8_MODE;
   gFreqTbl[0].shuffleIdx = SRAM_SHU1;
   gFreqTbl[0].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[0].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[0].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[1].freq_sel = LP4_DDR3200;
   gFreqTbl[1].divmode = DIV8_MODE;
   gFreqTbl[1].shuffleIdx = SRAM_SHU3;
   gFreqTbl[1].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[1].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[1].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[2].freq_sel = LP4_DDR3200;
   gFreqTbl[2].divmode = DIV8_MODE;
   gFreqTbl[2].shuffleIdx = SRAM_SHU2;
   gFreqTbl[2].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[2].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[2].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[3].freq_sel = LP4_DDR3200;
   gFreqTbl[3].divmode = DIV8_MODE;
   gFreqTbl[3].shuffleIdx = SRAM_SHU0;
   gFreqTbl[3].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[3].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[3].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[4].freq_sel = LP4_DDR3200;
   gFreqTbl[4].divmode = DIV8_MODE;
   gFreqTbl[4].shuffleIdx = SRAM_SHU5;
   gFreqTbl[4].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[4].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[4].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[5].freq_sel = LP4_DDR3200;
   gFreqTbl[5].divmode = DIV8_MODE;
   gFreqTbl[5].shuffleIdx = SRAM_SHU4;
   gFreqTbl[5].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[5].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[5].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[6].freq_sel = LP4_DDR3200;
   gFreqTbl[6].divmode = DIV8_MODE;
   gFreqTbl[6].shuffleIdx = SRAM_SHU6;
   gFreqTbl[6].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[6].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[6].ddr_loop_mode = CLOSE_LOOP_MODE;

}

static void freq_table_are_all_1534(void)
{
   gFreqTbl[0].freq_sel = LP4_DDR1600;
   gFreqTbl[0].divmode = DIV8_MODE;
   gFreqTbl[0].shuffleIdx = SRAM_SHU1;
   gFreqTbl[0].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[0].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[0].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[1].freq_sel = LP4_DDR1600;
   gFreqTbl[1].divmode = DIV8_MODE;
   gFreqTbl[1].shuffleIdx = SRAM_SHU3;
   gFreqTbl[1].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[1].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[1].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[2].freq_sel = LP4_DDR1600;
   gFreqTbl[2].divmode = DIV8_MODE;
   gFreqTbl[2].shuffleIdx = SRAM_SHU2;
   gFreqTbl[2].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[2].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[2].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[3].freq_sel = LP4_DDR1600;
   gFreqTbl[3].divmode = DIV8_MODE;
   gFreqTbl[3].shuffleIdx = SRAM_SHU0;
   gFreqTbl[3].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[3].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[3].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[4].freq_sel = LP4_DDR1600;
   gFreqTbl[4].divmode = DIV8_MODE;
   gFreqTbl[4].shuffleIdx = SRAM_SHU5;
   gFreqTbl[4].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[4].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[4].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[5].freq_sel = LP4_DDR1600;
   gFreqTbl[5].divmode = DIV8_MODE;
   gFreqTbl[5].shuffleIdx = SRAM_SHU4;
   gFreqTbl[5].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[5].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[5].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[6].freq_sel = LP4_DDR1600;
   gFreqTbl[6].divmode = DIV8_MODE;
   gFreqTbl[6].shuffleIdx = SRAM_SHU6;
   gFreqTbl[6].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[6].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[6].ddr_loop_mode = CLOSE_LOOP_MODE;

}

static void freq_table_opp0_3733_others_3094(void)
{
   gFreqTbl[0].freq_sel = LP4_DDR3200;
   gFreqTbl[0].divmode = DIV8_MODE;
   gFreqTbl[0].shuffleIdx = SRAM_SHU1;
   gFreqTbl[0].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[0].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[0].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[1].freq_sel = LP4_DDR3200;
   gFreqTbl[1].divmode = DIV8_MODE;
   gFreqTbl[1].shuffleIdx = SRAM_SHU3;
   gFreqTbl[1].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[1].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[1].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[2].freq_sel = LP4_DDR3200;
   gFreqTbl[2].divmode = DIV8_MODE;
   gFreqTbl[2].shuffleIdx = SRAM_SHU2;
   gFreqTbl[2].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[2].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[2].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[3].freq_sel = LP4_DDR3733;
   gFreqTbl[3].divmode = DIV8_MODE;
   gFreqTbl[3].shuffleIdx = SRAM_SHU0;
   gFreqTbl[3].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[3].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[3].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[4].freq_sel = LP4_DDR3200;
   gFreqTbl[4].divmode = DIV8_MODE;
   gFreqTbl[4].shuffleIdx = SRAM_SHU5;
   gFreqTbl[4].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[4].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[4].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[5].freq_sel = LP4_DDR3200;
   gFreqTbl[5].divmode = DIV8_MODE;
   gFreqTbl[5].shuffleIdx = SRAM_SHU4;
   gFreqTbl[5].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[5].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[5].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[6].freq_sel = LP4_DDR3200;
   gFreqTbl[6].divmode = DIV8_MODE;
   gFreqTbl[6].shuffleIdx = SRAM_SHU6;
   gFreqTbl[6].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[6].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[6].ddr_loop_mode = CLOSE_LOOP_MODE;
}

static void freq_table_opp0_3094_others_1534(void)
{
   gFreqTbl[0].freq_sel = LP4_DDR1600;
   gFreqTbl[0].divmode = DIV8_MODE;
   gFreqTbl[0].shuffleIdx = SRAM_SHU1;
   gFreqTbl[0].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[0].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[0].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[1].freq_sel = LP4_DDR1600;
   gFreqTbl[1].divmode = DIV8_MODE;
   gFreqTbl[1].shuffleIdx = SRAM_SHU3;
   gFreqTbl[1].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[1].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[1].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[2].freq_sel = LP4_DDR1600;
   gFreqTbl[2].divmode = DIV8_MODE;
   gFreqTbl[2].shuffleIdx = SRAM_SHU2;
   gFreqTbl[2].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[2].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[2].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[3].freq_sel = LP4_DDR3200;
   gFreqTbl[3].divmode = DIV8_MODE;
   gFreqTbl[3].shuffleIdx = SRAM_SHU0;
   gFreqTbl[3].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[3].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[3].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[4].freq_sel = LP4_DDR1600;
   gFreqTbl[4].divmode = DIV8_MODE;
   gFreqTbl[4].shuffleIdx = SRAM_SHU5;
   gFreqTbl[4].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[4].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[4].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[5].freq_sel = LP4_DDR1600;
   gFreqTbl[5].divmode = DIV8_MODE;
   gFreqTbl[5].shuffleIdx = SRAM_SHU4;
   gFreqTbl[5].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[5].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[5].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[6].freq_sel = LP4_DDR1600;
   gFreqTbl[6].divmode = DIV8_MODE;
   gFreqTbl[6].shuffleIdx = SRAM_SHU6;
   gFreqTbl[6].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[6].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[6].ddr_loop_mode = CLOSE_LOOP_MODE;
}

static void freq_table_opp0_2400_others_1534(void)
{
   gFreqTbl[0].freq_sel = LP4_DDR1600;
   gFreqTbl[0].divmode = DIV8_MODE;
   gFreqTbl[0].shuffleIdx = SRAM_SHU1;
   gFreqTbl[0].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[0].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[0].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[1].freq_sel = LP4_DDR1600;
   gFreqTbl[1].divmode = DIV8_MODE;
   gFreqTbl[1].shuffleIdx = SRAM_SHU3;
   gFreqTbl[1].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[1].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[1].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[2].freq_sel = LP4_DDR1600;
   gFreqTbl[2].divmode = DIV8_MODE;
   gFreqTbl[2].shuffleIdx = SRAM_SHU2;
   gFreqTbl[2].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[2].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[2].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[3].freq_sel = LP4_DDR2400;
   gFreqTbl[3].divmode = DIV8_MODE;
   gFreqTbl[3].shuffleIdx = SRAM_SHU0;
   gFreqTbl[3].duty_calibration_mode = DUTY_NEED_K;
   gFreqTbl[3].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[3].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[4].freq_sel = LP4_DDR1600;
   gFreqTbl[4].divmode = DIV8_MODE;
   gFreqTbl[4].shuffleIdx = SRAM_SHU5;
   gFreqTbl[4].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[4].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[4].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[5].freq_sel = LP4_DDR1600;
   gFreqTbl[5].divmode = DIV8_MODE;
   gFreqTbl[5].shuffleIdx = SRAM_SHU4;
   gFreqTbl[5].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[5].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[5].ddr_loop_mode = CLOSE_LOOP_MODE;

   gFreqTbl[6].freq_sel = LP4_DDR1600;
   gFreqTbl[6].divmode = DIV8_MODE;
   gFreqTbl[6].shuffleIdx = SRAM_SHU6;
   gFreqTbl[6].duty_calibration_mode = DUTY_DEFAULT;
   gFreqTbl[6].vref_calibartion_enable = VREF_CALI_ON;
   gFreqTbl[6].ddr_loop_mode = CLOSE_LOOP_MODE;
}
#if (CFG_DRAM_LOG_TO_STORAGE)

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

#if (FOR_DV_SIMULATION_USED==0)
#if !__ETT__
void mt_set_emi(struct dramc_param *dparam)
{
	int index;
	/*unsigned int SW_CTRL_VC, HW_CTRL_VC;*/
	EMI_SETTINGS *emi_set = &emi_settings[0];

	int segment;

#ifdef VOLTAGE_SEL
	update_voltage_select_info();
#endif
#if ENABLE_PINMUX_FOR_RANK_SWAP
	EMI_rank_swap_handle();
#endif

	// set voltage and hw trapping before mdl
	setup_dramc_voltage_by_pmic();

	if ((doe_get_config("dram_all_3094_0825")) || (doe_get_config("dram_all_3094_0725")))
		freq_table_are_all_3094();
	else if	(doe_get_config("dram_all_1534_0725"))
		freq_table_are_all_1534();
	else if	(doe_get_config("dram_opp0_3733_others_3094_0825"))
		freq_table_opp0_3733_others_3094();
	else if	(doe_get_config("dram_opp0_3094_others_1534_0725"))
		freq_table_opp0_3094_others_1534();
	else if	(doe_get_config("dram_opp0_2400_others_1534_0725"))
		freq_table_opp0_2400_others_1534();

#ifdef COMBO_MCP

	index = mt_get_mdl_number();
	dramc_crit("[EMI] MDL number = %d\r\n", index);
	if (index < 0 || index >=  num_of_emi_records)
	{
		die("[EMI] setting failed 0x%x\r\n", index);
	}
	else
	{
		emi_setting_index = index;
		emi_set = &emi_settings[emi_setting_index];
	}
	dramc_crit("[EMI] Get MDL freq = %d\r\n", emi_set->DRAMC_ACTIME_UNION[0]);
#else
	dramc_crit("[EMI] ComboMCP not ready, using default setting\n");
	emi_setting_index = -1;
	emi_set = &default_emi_setting;
#endif
	segment = (seclib_get_devinfo_with_index(7) & 0xFF);
	if ((segment == 0x80) || (segment == 0x01) || (segment == 0x40) || (segment == 0x02))
	{
		emi_set->DRAMC_ACTIME_UNION[0] = 3733;
	}
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
#endif

#define DRAMC_ADDR_SHIFT_CHN(addr, channel) (addr + (channel * 0x10000))

#if (FOR_DV_SIMULATION_USED==0) // for DV sim build pass
int doe_get_config(const char* feature)
{
#if defined(ENABLE_DOE)
	char *doe_feature = dconfig_getenv(feature);
	int doe_result = atoi(doe_feature);
	dramc_crit("DOE force setting %s=%d\n", feature, doe_result);
	return doe_result;
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
		part_dram_data_addr_uart = get_part_addr("boot_para") + 0x100000; // addr = 0x1f300000, the first 1MB for debug
		memset(&logbuf, 0, sizeof(logbuf));
		for (clr_count = 0; clr_count < 3072 ; clr_count++) //3M
			ret = blkdev_write(bootdev, (part_dram_data_addr_uart + (1024 * clr_count)), 1024, (u8*)&logbuf, storage_get_part_id(STORAGE_PHYS_PART_USER));
	}

	if (log_start) {
		if (((((char) c >> 4) & 0x7) > 1) & ((((char) c >> 4) & 0x7) < 7))
		logbuf[logcount] = ((char) c & 0xF0) | (((char) c >> 2) & 0x03) | (((char) c << 2) & 0x0C);
			else
				logbuf[logcount] = (char) c;
		logcount = logcount + 1;
		//write to storage
		if (logcount==1024) {
			logcount = 0;
			ret = blkdev_write(bootdev, part_dram_data_addr_uart, 1024, (u8*)&logbuf, storage_get_part_id(STORAGE_PHYS_PART_USER));
			part_dram_data_addr_uart = part_dram_data_addr_uart + 1024;
		}
	}
}
#endif
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION

u32 g_dram_storage_api_err_code;

#if !__ETT__
int read_offline_dram_calibration_data(DRAM_DFS_SHUFFLE_TYPE_T shuffle, SAVE_TIME_FOR_CALIBRATION_T *offLine_SaveData)
{
	struct sdram_params *params;

	if (!dramc_params)
		return -1;
	params = &dramc_params->dramc_datas.freq_params[shuffle];

	dramc_info("read calibration data from shuffle %d(For verify: WL B0:%u, B1: %u)\n",
		   shuffle, params->wr_level[CHANNEL_A][RANK_0][0], params->wr_level[CHANNEL_B][RANK_0][0]);
	/* copy the data stored in storage to the data structure for calibration */
	memcpy(offLine_SaveData, params, sizeof(*offLine_SaveData));

	return 0;
}

int write_offline_dram_calibration_data(DRAM_DFS_SHUFFLE_TYPE_T shuffle, SAVE_TIME_FOR_CALIBRATION_T *offLine_SaveData)
{
	return 0;
}

int clean_dram_calibration_data(void)
{
	return 0;
}
#endif

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

