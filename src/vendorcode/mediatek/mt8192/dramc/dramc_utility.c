/* SPDX-License-Identifier: BSD-3-Clause */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "dramc_top.h"
#include "dramc_common.h"
#include "dramc_int_global.h" //for gu1BroadcastIsLP4
#include "dramc_dv_init.h"
#include "dramc_actiming.h"
#include "x_hal_io.h"
#if __ETT__
#include <barriers.h>
#endif
#include "emi.h"

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

#if (fcFOR_CHIP_ID == fcA60868)
U8 u1EnterRuntime;
#endif

U8 u1IsLP4Family(DRAM_DRAM_TYPE_T dram_type)
{
	if (dram_type == TYPE_LPDDR5)
		return FALSE;
	else
		return TRUE;
}

u8 is_lp5_family(DRAMC_CTX_T *p)
{
	return p->dram_type == TYPE_LPDDR5? TRUE: FALSE;
}

u8 is_heff_mode(DRAMC_CTX_T *p)
{
	u8 res = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_COMMON0),
			SHU_COMMON0_LP5HEFF_MODE);
	mcSHOW_DBG_MSG5(("HEFF Mode: %d\n", res));
	return res? TRUE: FALSE;
}

static u8 lp5heff;

u8 lp5heff_save_disable(DRAMC_CTX_T *p)
{
	/* save it */
	lp5heff = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_COMMON0),
			SHU_COMMON0_LP5HEFF_MODE);

	/* disable it */
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_COMMON0),
			P_Fld(0, SHU_COMMON0_LP5HEFF_MODE));
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RKCFG),
		0, RKCFG_CKE2RANK);


	return lp5heff;
}

void lp5heff_restore(DRAMC_CTX_T *p)
{
	/* restore it */
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHU_COMMON0),
			P_Fld(lp5heff, SHU_COMMON0_LP5HEFF_MODE));
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RKCFG),
		lp5heff, RKCFG_CKE2RANK);
}

#if FOR_DV_SIMULATION_USED
U8 u1BroadcastOnOff = 0;
#endif
U32 GetDramcBroadcast(void)
{
#if (fcFOR_CHIP_ID == fcA60868)
	return 0;
#endif

#if (FOR_DV_SIMULATION_USED == 0)
	return *((volatile unsigned int *)(DRAMC_WBR));
#else
	return u1BroadcastOnOff;
#endif
}

void DramcBroadcastOnOff(U32 bOnOff)
{
#if (FOR_DV_SIMULATION_USED==0 && SW_CHANGE_FOR_SIMULATION==0)

	#if (fcFOR_CHIP_ID == fcA60868)
		return; //disable broadcast in A60868
	#endif

	#if __Petrus_TO_BE_PORTING__
		U8 u1BroadcastStatus = 0;
		// INFRA_RSVD3[9:8] = protect_set_clr_mask
		u1BroadcastStatus = (*((volatile unsigned int *)(INFRA_RSVD3)) >> 8) & 0x3;
		if (u1BroadcastStatus & 0x1) // Enable new infra access by Preloader
		{
			if (bOnOff == DRAMC_BROADCAST_ON)
				*((volatile unsigned int *)(DRAMC_WBR_SET)) = DRAMC_BROADCAST_SET;
			else
				*((volatile unsigned int *)(DRAMC_WBR_CLR)) = DRAMC_BROADCAST_CLR;
		}
		else
	#endif
			*((volatile unsigned int *)(DRAMC_WBR)) = bOnOff;
		dsb();
#endif

#if (FOR_DV_SIMULATION_USED == 1)
		if (gu1BroadcastIsLP4 == TRUE)
		{
			#if (fcFOR_CHIP_ID == fcA60868)
			bOnOff = 0;
			#endif
			if (bOnOff)
			{
				broadcast_on();
				mcSHOW_DBG_MSG(("Broadcast ON\n"));
				u1BroadcastOnOff = bOnOff;
			}
			else
			{
				broadcast_off();
				mcSHOW_DBG_MSG(("Broadcast OFF\n"));
				u1BroadcastOnOff = bOnOff;
			}
		}
#endif

#ifdef DUMP_INIT_RG_LOG_TO_DE
		if(gDUMP_INIT_RG_LOG_TO_DE_RG_log_flag)
		{
			U8 u1BroadcastStatus = 0;
			U32 addr, val;

			addr = DRAMC_WBR;
			val = bOnOff;
			// *((volatile unsigned int *)(DRAMC_WBR)) = bOnOff;

			mcSHOW_DUMP_INIT_RG_MSG(("*((UINT32P)(0x%x)) = 0x%x;\n",addr, val));
	//		  mcDELAY_MS(1);
#if (FOR_DV_SIMULATION_USED==0)
			GPT_Delay_ms(1);
#endif
		}
#endif
	}



#if __ETT__
const U32 u4Cannot_Use_Dramc_WBR_Reg[]=
{
	DDRPHY_REG_CA_DLL_ARPI5,
	DDRPHY_REG_B0_DLL_ARPI5,
	DDRPHY_REG_B1_DLL_ARPI5,

	DDRPHY_REG_SHU_CA_DLL0,
	DDRPHY_REG_SHU_CA_DLL1,

	DDRPHY_REG_CA_LP_CTRL0,

	DDRPHY_REG_MISC_DVFSCTL2,
	DDRPHY_REG_MISC_SHU_OPT,

	DDRPHY_REG_MISC_DVFSCTL,
	DDRPHY_REG_MISC_DVFSCTL3,

	DDRPHY_REG_MISC_CKMUX_SEL,
	DRAMC_REG_DVFS_CTRL0
};
#define CANNOT_USE_WBR_SIZE ((sizeof(u4Cannot_Use_Dramc_WBR_Reg)) / (sizeof(U32)))
void CheckDramcWBR(U32 u4address)
{

	U32 i, channel_and_value;
	if (GetDramcBroadcast()==DRAMC_BROADCAST_ON)
	{
		#if ((CHANNEL_NUM == 1) || (CHANNEL_NUM == 2))
			channel_and_value = 0x1;
		#else //for channel number = 3 or 4
			channel_and_value = 0x3;
		#endif
		if ((((u4address - Channel_A_DRAMC_NAO_BASE_VIRTUAL) >> POS_BANK_NUM) & channel_and_value) != CHANNEL_A)
		{
			mcSHOW_ERR_MSG(("Error! virtual address 0x%x is not CHA and cannot use Dramc WBR\n", u4address));
			while (1);
		}
			for (i = 0; i < CANNOT_USE_WBR_SIZE; i++)
			{
				if (u4Cannot_Use_Dramc_WBR_Reg[i] == u4address)
				{
					mcSHOW_ERR_MSG(("Error! virtual address 0x%x cannot use Dramc WBR\n", u4address));
					while (1);
				}
			 }
	 }
}
#endif

void vSetPHY2ChannelMapping(DRAMC_CTX_T *p, U8 u1Channel)
{
	p->channel = (DRAM_CHANNEL_T)u1Channel;
}

U8 vGetPHY2ChannelMapping(DRAMC_CTX_T *p)
{
	return p->channel;
}

void vSetChannelNumber(DRAMC_CTX_T *p)
{
	#if 1//(!FOR_DV_SIMULATION_USED)
	p->support_channel_num = CHANNEL_NUM;
	#else
	p->support_channel_num = CHANNEL_SINGLE;
	#endif
}

void vSetRank(DRAMC_CTX_T *p, U8 ucRank)
{
	p->rank = (DRAM_RANK_T)ucRank;
}

U8 u1GetRank(DRAMC_CTX_T *p)
{
	return p->rank;
}

void vSetRankNumber(DRAMC_CTX_T *p)
{
#if(FOR_DV_SIMULATION_USED==0 && SW_CHANGE_FOR_SIMULATION==0)
	if (u4IO32ReadFldAlign(DRAMC_REG_SA_RESERVE, SA_RESERVE_SINGLE_RANK) == 1)
	{
		p->support_rank_num =RANK_SINGLE;
	}
	else
#endif
	{
		p->support_rank_num = RANK_DUAL;
	}
}

void vSetFSPNumber(DRAMC_CTX_T *p)
{
#if (__LP5_COMBO__ == TRUE)
	if (TRUE == is_lp5_family(p))
		p->support_fsp_num = 3;
	else
#endif
		p->support_fsp_num = 2;
}

static void setFreqGroup(DRAMC_CTX_T *p)
{

	/* Below listed conditions represent freqs that exist in ACTimingTable
	 * -> Should cover freqGroup settings for all real freq values
	 */
#if (__LP5_COMBO__ == TRUE)
	if (TRUE == is_lp5_family(p))
	{
		if (p->frequency <= 400) // DDR800
		{
			p->freqGroup = 400;
		}
		else if (p->frequency <= 600) // DDR1200
		{
			p->freqGroup = 600;
		}
		else if (p->frequency <= 800) // DDR1600
		{
			p->freqGroup = 800;
		}
		else if (p->frequency <= 933) //DDR1866
		{
			p->freqGroup = 933;
		}
		else if (p->frequency <= 1200) //DDR2400, DDR2280
		{
			p->freqGroup = 1200;
		}
		else if (p->frequency <= 1600) // DDR3200
		{
			p->freqGroup = 1600;
		}
		else if (p->frequency <= 1866) // DDR3733
		{
			p->freqGroup = 1866;
		}
		else if (p->frequency <= 2133) // DDR4266
		{
			p->freqGroup = 2133;
		}
		else if (p->frequency <= 2400) // DDR4800
		{
			p->freqGroup = 2400;
		}
		else if (p->frequency <= 2750) // DDR5500
		{
			p->freqGroup = 2750;
		}
		else if (p->frequency <= 3000) // DDR6000
		{
			p->freqGroup = 3000;
		}
		else // DDR6600
		{
			p->freqGroup = 3300;
		}
	}
	else
#endif
	{
		if (p->frequency <= 200) // DDR400
		{
			p->freqGroup = 200;
		}
		else if (p->frequency <= 400) // DDR800
		{
			p->freqGroup = 400;
		}
		else if (p->frequency <= 600) // DDR1200
		{
			p->freqGroup = 600;
		}
		else if (p->frequency <= 800) // DDR1600
		{
			p->freqGroup = 800;
		}
		else if (p->frequency <= 933) //DDR1866
		{
			p->freqGroup = 933;
		}
		else if (p->frequency <= 1200) //DDR2400, DDR2280
		{
			p->freqGroup = 1200;
		}
		else if (p->frequency <= 1333) // DDR2667
		{
			p->freqGroup = 1333;
		}
		else if (p->frequency <= 1600) // DDR3200
		{
			p->freqGroup = 1600;
		}
		else if (p->frequency <= 1866) // DDR3733
		{
			p->freqGroup = 1866;
		}
		else // DDR4266
		{
			p->freqGroup = 2133;
		}
	}

	mcSHOW_DBG_MSG3(("[setFreqGroup] p-> frequency %u, freqGroup: %u\n", p->frequency, p->freqGroup));
	return;
}


#define CKGEN_FMETER 0x0
#define ABIST_FMETER 0x1

U16 gddrphyfmeter_value = 0;
U16 DDRPhyFMeter(void)
{
	return gddrphyfmeter_value;
}

#if __ETT__  || defined(SLT)
void GetPhyPllFrequency(DRAMC_CTX_T *p)
{
	//U8 u1ShuLevel = u4IO32ReadFldAlign(DRAMC_REG_SHUSTATUS, SHUSTATUS_SHUFFLE_LEVEL);
	U8 u1ShuLevel = u4IO32ReadFldAlign(DDRPHY_REG_DVFS_STATUS, DVFS_STATUS_OTHER_SHU_GP);
	U32 u4PLL5_ADDR = DDRPHY_REG_SHU_PHYPLL1 + SHU_GRP_DDRPHY_OFFSET * u1ShuLevel;
	U32 u4PLL8_ADDR = DDRPHY_REG_SHU_PHYPLL2 + SHU_GRP_DDRPHY_OFFSET * u1ShuLevel;
	U32 u4B0_DQ = DDRPHY_REG_SHU_B0_DQ1 + SHU_GRP_DDRPHY_OFFSET * u1ShuLevel;
	U32 u4PLL3_ADDR = DDRPHY_REG_SHU_PHYPLL3 + SHU_GRP_DDRPHY_OFFSET * u1ShuLevel;
	//Darren-U32 u4PLL4 = DDRPHY_SHU_PLL4 + SHU_GRP_DDRPHY_OFFSET * u1ShuLevel; // for DDR4266
	U32 u4B0_DQ6 = DDRPHY_REG_SHU_B0_DQ6 + SHU_GRP_DDRPHY_OFFSET * u1ShuLevel;

	/* VCOFreq = FREQ_XTAL x ((RG_RCLRPLL_SDM_PCW) / 2^(RG_*_RCLRPLL_PREDIV)) / 2^(RG_*_RCLRPLL_POSDIV) */
	U32 u4SDM_PCW = u4IO32ReadFldAlign(u4PLL5_ADDR, SHU_PHYPLL1_RG_RPHYPLL_SDM_PCW);
	U32 u4PREDIV = u4IO32ReadFldAlign(u4PLL8_ADDR, SHU_PHYPLL2_RG_RPHYPLL_PREDIV);
	U32 u4POSDIV = u4IO32ReadFldAlign(u4PLL8_ADDR, SHU_PHYPLL2_RG_RPHYPLL_POSDIV);
	U32 u4CKDIV4 = u4IO32ReadFldAlign(u4B0_DQ, SHU_B0_DQ1_RG_ARPI_MIDPI_CKDIV4_EN_B0);
	U8 u1FBKSEL = u4IO32ReadFldAlign(u4PLL3_ADDR, SHU_PHYPLL3_RG_RPHYPLL_FBKSEL);
	//Darren-U16 u2CKMUL2 = u4IO32ReadFldAlign(u4PLL4, SHU_PLL4_RG_RPHYPLL_RESERVED);
	U8 u1SopenDQ = u4IO32ReadFldAlign(u4B0_DQ6, SHU_B0_DQ6_RG_ARPI_SOPEN_EN_B0);

	U32 u4VCOFreq = (((52>>u4PREDIV)*(u4SDM_PCW>>8))>>u4POSDIV) << u1FBKSEL;
	U32 u4DataRate = u4VCOFreq>>u4CKDIV4;
	if (u1SopenDQ == ENABLE) // for 1:4 mode DDR800 (3.2G/DIV4)
		u4DataRate >>= 2;

	//mcSHOW_DBG_MSG(("PCW=0x%X, u4PREDIV=%d, u4POSDIV=%d, CKDIV4=%d, DataRate=%d\n", u4SDM_PCW, u4PREDIV, u4POSDIV, u4CKDIV4, u4DataRate));
	mcSHOW_DBG_MSG(("[F] DataRate=%d at SHU%d\n", u4DataRate, u1ShuLevel));
}
#endif

DRAM_PLL_FREQ_SEL_T vGet_PLL_FreqSel(DRAMC_CTX_T *p)
{
	return p->pDFSTable->freq_sel;
}

void vSet_PLL_FreqSel(DRAMC_CTX_T *p, DRAM_PLL_FREQ_SEL_T sel)
{
	p->pDFSTable->freq_sel = sel;
}

DDR800_MODE_T vGet_DDR_Loop_Mode(DRAMC_CTX_T *p)
{
	return p->pDFSTable->ddr_loop_mode;
}


void vSet_Div_Mode(DRAMC_CTX_T *p, DIV_MODE_T eMode)
{
	p->pDFSTable->divmode = eMode;
}

DIV_MODE_T vGet_Div_Mode(DRAMC_CTX_T *p)
{
	return p->pDFSTable->divmode;
}

void vSet_Current_ShuLevel(DRAMC_CTX_T *p, DRAM_DFS_SRAM_SHU_T u1ShuIndex)
{
	p->pDFSTable->shuffleIdx = u1ShuIndex;
}

DRAM_DFS_SRAM_SHU_T vGet_Current_ShuLevel(DRAMC_CTX_T *p)
{
	return p->pDFSTable->shuffleIdx;
}


#if 0
void vSet_Duty_Calibration_Mode(DRAMC_CTX_T *p, U8 kMode)
{
	p->pDFSTable->duty_calibration_mode = kMode;
}
#endif

DUTY_CALIBRATION_T Get_Duty_Calibration_Mode(DRAMC_CTX_T *p)
{
	return p->pDFSTable->duty_calibration_mode;
}

VREF_CALIBRATION_ENABLE_T Get_Vref_Calibration_OnOff(DRAMC_CTX_T *p)
{
	return p->pDFSTable->vref_calibartion_enable;
}

/* vGet_Dram_CBT_Mode
 * Due to current HW design (both ranks share the same set of ACTiming regs), mixed
 * mode LP4 now uses byte mode ACTiming settings. This means most calibration steps
 * should use byte mode code flow.
 * Note: The below items must have per-rank settings (Don't use this function)
 * 1. CBT training 2. TX tracking
 */
DRAM_CBT_MODE_T vGet_Dram_CBT_Mode(DRAMC_CTX_T *p)
{
	if (p->support_rank_num == RANK_DUAL)
	{
		if(p->dram_cbt_mode[RANK_0] == CBT_NORMAL_MODE && p->dram_cbt_mode[RANK_1] == CBT_NORMAL_MODE)
			return CBT_NORMAL_MODE;
	}
	else // Single rank
	{
		if(p->dram_cbt_mode[RANK_0] == CBT_NORMAL_MODE)
			return CBT_NORMAL_MODE;
	}

	return CBT_BYTE_MODE1;
}

void vPrintCalibrationBasicInfo(DRAMC_CTX_T *p)
{
#if __ETT__
	mcSHOW_DBG_MSG(("===============================================================================\n"
					"Dram Type= %d, Freq= %u, FreqGroup= %u, CH_%d, rank %d\n"
					"fsp= %d, odt_onoff= %d, Byte mode= %d, DivMode= %d\n"
					"===============================================================================\n",
						p->dram_type, DDRPhyFMeter()?DDRPhyFMeter():p->frequency, p->freqGroup, p->channel, p->rank,
						p->dram_fsp, p->odt_onoff, p->dram_cbt_mode[p->rank], vGet_Div_Mode(p)));
#else
	mcSHOW_DBG_MSG(("==\n"
					"Dram Type= %d, Freq= %u, CH_%d, rank %d\n"
					"fsp= %d, odt_onoff= %d, Byte mode= %d, DivMode= %d\n"
					"==\n",
						p->dram_type,
						DDRPhyFMeter(),
						p->channel,
						p->rank,
						p->dram_fsp,
						p->odt_onoff,
						p->dram_cbt_mode[p->rank],
						vGet_Div_Mode(p)));
#endif
}

#if VENDER_JV_LOG
void vPrintCalibrationBasicInfo_ForJV(DRAMC_CTX_T *p)
{
	mcSHOW_DBG_MSG5(("\n\nDram type:"));

	switch (p->dram_type)
	{
		case TYPE_LPDDR4:
			mcSHOW_DBG_MSG5(("LPDDR4\t"));
			break;

		case TYPE_LPDDR4X:
			mcSHOW_DBG_MSG5(("LPDDR4X\t"));
			break;

		case TYPE_LPDDR4P:
			mcSHOW_DBG_MSG5(("LPDDR4P\t"));
			break;
	}

	mcSHOW_DBG_MSG5(("Freq: %d, FreqGroup %u, channel %d, rank %d\n"
					 "dram_fsp= %d, odt_onoff= %d, Byte mode= %d, DivMode= %d\n\n",
										p->frequency, p->freqGroup, p->channel, p->rank,
										p->dram_fsp, p->odt_onoff, p->dram_cbt_mode[p->rank], vGet_Div_Mode(p)));

	return;
}
#endif

U16 GetFreqBySel(DRAMC_CTX_T *p, DRAM_PLL_FREQ_SEL_T sel)
{
	U16 u2freq=0;

	switch(sel)
	{
		case LP4_DDR4266:
			u2freq=2133;
			break;
		case LP4_DDR3733:
			u2freq=1866;
			break;
		case LP4_DDR3200:
			u2freq=1600;
			break;
		case LP4_DDR2667:
			u2freq=1333;
			break;
		case LP4_DDR2400:
			u2freq=1200;
			break;
		case LP4_DDR1866:
			u2freq=933;
			break;
		case LP4_DDR1600:
			u2freq=800;
			break;
		case LP4_DDR1200:
			u2freq=600;
			break;
		case LP4_DDR800:
			u2freq=400;
			break;
		case LP4_DDR400:
			u2freq=200;
			break;

		case LP5_DDR6400:
			u2freq=3200;
			break;
		case LP5_DDR6000:
			u2freq=3000;
			break;
		case LP5_DDR5500:
			u2freq=2750;
			break;
		case LP5_DDR4800:
			u2freq=2400;
			break;
		case LP5_DDR4266:
			u2freq=2133;
			break;
		case LP5_DDR3733:
			u2freq=1866;
			break;
		case LP5_DDR3200:
			u2freq=1600;
			break;
		case LP5_DDR2400:
			u2freq=1200;
			break;
		case LP5_DDR1600:
			u2freq=800;
			break;
		case LP5_DDR1200:
			u2freq=600;
			break;
		case LP5_DDR800:
			u2freq=400;
			break;

		default:
			mcSHOW_ERR_MSG(("[GetFreqBySel] freq sel is incorrect !!!\n"));
			break;
	}

	return u2freq;
}

DRAM_PLL_FREQ_SEL_T GetSelByFreq(DRAMC_CTX_T *p, U16 u2freq)
{
	DRAM_PLL_FREQ_SEL_T sel=0;

	switch(u2freq)
	{
		case 2133:
			sel=LP4_DDR4266;
			break;
		case 1866:
			sel=LP4_DDR3733;
			break;
		case 1600:
			sel=LP4_DDR3200;
			break;
		case 1333:
			sel=LP4_DDR2667;
			break;
		case 1200:
			sel=LP4_DDR2400;
			break;
		case 933:
			sel=LP4_DDR1866;
			break;
		case 800:
			sel=LP4_DDR1600;
			break;
		case 600:
			sel=LP4_DDR1200;
			break;
		case 400:
			sel=LP4_DDR800;
			break;
		case 200:
			sel=LP4_DDR400;
			break;
		default:
			mcSHOW_ERR_MSG(("[GetSelByFreq] sel is incorrect !!!\n"));
			break;
	}

	return sel;
}

void DDRPhyFreqSel(DRAMC_CTX_T *p, DRAM_PLL_FREQ_SEL_T sel)
{
	p->freq_sel = sel;
	p->frequency = GetFreqBySel(p, sel);

	if(is_lp5_family(p))
	{
		///TODO: Dennis
		//p->dram_fsp = (p->frequency < LP5_MRFSP_TERM_FREQ)? FSP_0: FSP_1;
		p->dram_fsp = FSP_0;
		#if LP5_DDR4266_RDBI_WORKAROUND
		if(p->frequency >= 2133)
			p->DBI_R_onoff[FSP_0] = DBI_ON;
		#endif
		p->odt_onoff = (p->frequency < LP5_MRFSP_TERM_FREQ)? ODT_OFF: ODT_ON;

		if(p->frequency >= 2750)
			vSet_Div_Mode(p, DIV16_MODE);
	}
	else
	{
		p->dram_fsp = (p->frequency < LP4_MRFSP_TERM_FREQ)? FSP_0: FSP_1;
		p->odt_onoff = (p->frequency < LP4_MRFSP_TERM_FREQ)? ODT_OFF: ODT_ON;
	}

	if (p->dram_type == TYPE_LPDDR4P)
		p->odt_onoff = ODT_OFF;

	p->shu_type = get_shuffleIndex_by_Freq(p);
	setFreqGroup(p); /* Set p->freqGroup to support freqs not in ACTimingTable */

	///TODO: add DBI_onoff by condition
	//p->DBI_onoff = p->odt_onoff;
}


U16 u2DFSGetHighestFreq(DRAMC_CTX_T * p)
{
	U8 u1ShuffleIdx = 0;
	U16 u2Freq=0;
	static U16 u2FreqMax=0;

	if ((u2FreqMax == 0) || (gUpdateHighestFreq == TRUE))
	{
		gUpdateHighestFreq = FALSE;
		u2FreqMax = 0;
		for (u1ShuffleIdx = DRAM_DFS_SHUFFLE_1; u1ShuffleIdx < DRAM_DFS_SHUFFLE_MAX; u1ShuffleIdx++)
		{
			u2Freq = GetFreqBySel(p, gFreqTbl[u1ShuffleIdx].freq_sel);
			if(u2FreqMax < u2Freq)
				u2FreqMax = u2Freq;
		}
	}

	return u2FreqMax;
}

U8 GetEyeScanEnable(DRAMC_CTX_T * p, U8 get_type)
{
#if ENABLE_EYESCAN_GRAPH
#if (fcFOR_CHIP_ID == fcA60868)  //need check unterm highest freq is saved at SRAM_SHU4??
	//CBT
	if (get_type == 0)
		if (ENABLE_EYESCAN_CBT==1) return ENABLE;  //TO DO :Temp Force open EYESCAN

	//RX
	if (get_type == 1)
		if (ENABLE_EYESCAN_RX==1) return ENABLE;  //TO DO :Temp Force open EYESCAN

	//TX
	if (get_type == 2)
		if (ENABLE_EYESCAN_TX==1) return ENABLE;  //TO DO :Temp Force open EYESCAN

#else
	//CBT
	if (get_type == 0)
	{
		if (gCBT_EYE_Scan_flag==0) return DISABLE;
		if (gCBT_EYE_Scan_only_higheset_freq_flag == 0) return ENABLE;	//K All freq
		if (p->frequency == u2DFSGetHighestFreq(p)) return ENABLE;		// K highest freq
		if (gEye_Scan_unterm_highest_flag==1 && vGet_Current_ShuLevel(p)==SRAM_SHU2) return ENABLE; 		// K unterm highest freq
	}

	//RX
	if (get_type == 1)
	{
		if (gRX_EYE_Scan_flag==0) return DISABLE;
		if (gRX_EYE_Scan_only_higheset_freq_flag == 0) return ENABLE;  //K All freq
		if (p->frequency == u2DFSGetHighestFreq(p)) return ENABLE;		// K highest freq
		if (gEye_Scan_unterm_highest_flag==1 && vGet_Current_ShuLevel(p)==SRAM_SHU2) return ENABLE; 		// K unterm highest freq
	}

	//TX
	if (get_type == 2)
	{
		if (gTX_EYE_Scan_flag==0) return DISABLE;
		if (gTX_EYE_Scan_only_higheset_freq_flag == 0) return ENABLE;  //K All freq
		if (p->frequency == u2DFSGetHighestFreq(p)) return ENABLE;		// K highest freq
		if (gEye_Scan_unterm_highest_flag==1 && vGet_Current_ShuLevel(p)==SRAM_SHU2) return ENABLE; 		// K unterm highest freq
	}

#endif
#endif

	return DISABLE;
}

void DramcWriteDBIOnOff(DRAMC_CTX_T *p, U8 onoff)
{
	// DRAMC Write-DBI On/Off
	vIO32WriteFldAlign_All(DRAMC_REG_SHU_TX_SET0, onoff, SHU_TX_SET0_DBIWR);
	mcSHOW_DBG_MSG(("DramC Write-DBI %s\n", ((onoff == DBI_ON) ? "on" : "off")));
}

void DramcReadDBIOnOff(DRAMC_CTX_T *p, U8 onoff)
{
	// DRAMC Read-DBI On/Off
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B0_DQ7, onoff, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_B1_DQ7, onoff, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1);
	mcSHOW_DBG_MSG(("DramC Read-DBI %s\n", ((onoff == DBI_ON) ? "on" : "off")));
}
#if ENABLE_READ_DBI
void SetDramModeRegForReadDBIOnOff(DRAMC_CTX_T *p, U8 u1fsp, U8 onoff)
{
#if MRW_CHECK_ONLY
	mcSHOW_MRW_MSG(("\n==[MR Dump] %s==\n", __func__));
#endif
	//mcSHOW_DBG_MSG(("--Fsp%d --\n", p->dram_fsp));

	//DRAM MR3[6] read-DBI On/Off
	u1MR03Value[u1fsp] = ((u1MR03Value[u1fsp] & 0xbf) | (onoff << 6));
	DramcModeRegWriteByRank(p, p->rank, 3, u1MR03Value[u1fsp]);
}
#endif

#if ENABLE_WRITE_DBI
void SetDramModeRegForWriteDBIOnOff(DRAMC_CTX_T *p, U8 u1fsp, U8 onoff)
{
#if MRW_CHECK_ONLY
	mcSHOW_MRW_MSG(("\n==[MR Dump] %s==\n", __func__));
#endif
	//DRAM MR3[7] write-DBI On/Off
	u1MR03Value[u1fsp] = ((u1MR03Value[u1fsp] & 0x7F) | (onoff << 7));
	DramcModeRegWriteByRank(p, p->rank, 3, u1MR03Value[u1fsp]);
}
#endif

void CKEFixOnOff(DRAMC_CTX_T *p, U8 u1RankIdx, CKE_FIX_OPTION option, CKE_FIX_CHANNEL WriteChannelNUM)
{
	U8 u1CKEOn, u1CKEOff;

	if (option == CKE_DYNAMIC) //if CKE is dynamic, set both CKE fix On and Off as 0
	{						   //After CKE FIX on/off, CKE should be returned to dynamic (control by HW)
		u1CKEOn = u1CKEOff = 0;
	}
	else //if CKE fix on is set as 1, CKE fix off should also be set as 0; vice versa
	{
		u1CKEOn = option;
		u1CKEOff = (1 - option);
	}

	if (WriteChannelNUM == CKE_WRITE_TO_ALL_CHANNEL) //write register to all channel
	{
		if((u1RankIdx == RANK_0)||(u1RankIdx == CKE_WRITE_TO_ALL_RANK))
		{
			vIO32WriteFldMulti_All(DRAMC_REG_CKECTRL, P_Fld(u1CKEOff, CKECTRL_CKEFIXOFF)
													| P_Fld(u1CKEOn, CKECTRL_CKEFIXON));
		}

		if(u1RankIdx == RANK_1||((u1RankIdx == CKE_WRITE_TO_ALL_RANK) && (p->support_rank_num == RANK_DUAL)))
		{
			vIO32WriteFldMulti_All(DRAMC_REG_CKECTRL, P_Fld(u1CKEOff, CKECTRL_CKE1FIXOFF)
													| P_Fld(u1CKEOn, CKECTRL_CKE1FIXON));
		}
	}
	else
	{
		if((u1RankIdx == RANK_0) || (u1RankIdx == CKE_WRITE_TO_ALL_RANK))
		{
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), P_Fld(u1CKEOff, CKECTRL_CKEFIXOFF)
																| P_Fld(u1CKEOn, CKECTRL_CKEFIXON));
		}

		if((u1RankIdx == RANK_1) ||((u1RankIdx == CKE_WRITE_TO_ALL_RANK) && (p->support_rank_num == RANK_DUAL)))
		{
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), P_Fld(u1CKEOff, CKECTRL_CKE1FIXOFF)
																| P_Fld(u1CKEOn, CKECTRL_CKE1FIXON));
		}
	}
}


void vAutoRefreshSwitch(DRAMC_CTX_T *p, U8 option)
{
	if (option == ENABLE)
	{
		//enable autorefresh
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0), 0, REFCTRL0_REFDIS); 	 //REFDIS=0, enable auto refresh
	}
	else	// DISABLE
	{
		//disable autorefresh
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0), 1, REFCTRL0_REFDIS); 	 //REFDIS=1, disable auto refresh

		//because HW will actually disable autorefresh after refresh_queue empty, so we need to wait quene empty.
		mcDELAY_US(u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MISC_STATUSA), MISC_STATUSA_REFRESH_QUEUE_CNT) * 4);   //wait refresh_queue_cnt * 3.9us
	}
}


//-------------------------------------------------------------------------
/** vCKERankCtrl
 *	Control CKE toggle mode (toggle both ranks 1. at the same time (CKE_RANK_DEPENDENT) 2. individually (CKE_RANK_INDEPENDENT))
 *	Note: Sets CKE toggle mode for all channels
 *	@param p				Pointer of context created by DramcCtxCreate.
 *	@param CKECtrlMode		Indicates
 */
//-------------------------------------------------------------------------
void vCKERankCtrl(DRAMC_CTX_T *p, CKE_CTRL_MODE_T CKECtrlMode)
{
	/* Struct indicating all register fields mentioned in "multi rank CKE control" */
	typedef struct
	{
		U8 u1CKE2Rank: Fld_wid(RKCFG_CKE2RANK);
		U8 u1CKE2Rank_Opt :Fld_wid(CKECTRL_CKE2RANK_OPT);
		U8 u1CKE2Rank_Opt2 :Fld_wid(CKECTRL_CKE2RANK_OPT2);
		U8 u1CKE2Rank_Opt3: Fld_wid(CKECTRL_CKE2RANK_OPT3);
		U8 u1CKE2Rank_Opt5: Fld_wid(CKECTRL_CKE2RANK_OPT5);
		U8 u1CKE2Rank_Opt6: Fld_wid(CKECTRL_CKE2RANK_OPT6);
		U8 u1CKE2Rank_Opt7: Fld_wid(CKECTRL_CKE2RANK_OPT7);
		U8 u1CKE2Rank_Opt8: Fld_wid(CKECTRL_CKE2RANK_OPT8);
		U8 u1CKETimer_Sel: Fld_wid(CKECTRL_CKETIMER_SEL);
		U8 u1FASTWake: Fld_wid(SHU_DCM_CTRL0_FASTWAKE);
		U8 u1FASTWake2: Fld_wid(SHU_DCM_CTRL0_FASTWAKE2);
		U8 u1FastWake_Sel: Fld_wid(CKECTRL_FASTWAKE_SEL);
		U8 u1CKEWake_Sel: Fld_wid(CKECTRL_CKEWAKE_SEL);
		U8 u1ClkWiTrfc: Fld_wid(ACTIMING_CTRL_CLKWITRFC);
	} CKE_CTRL_T;

	/* CKE_Rank dependent/independent mode register setting values */
	CKE_CTRL_T CKE_Mode, CKE_Rank_Independent = { .u1CKE2Rank = 0, .u1CKE2Rank_Opt3 = 0, .u1CKE2Rank_Opt2 = 1,
												  .u1CKE2Rank_Opt5 = 0, .u1CKE2Rank_Opt6 = 0, .u1CKE2Rank_Opt7 = 1, .u1CKE2Rank_Opt8 = 0,
												  .u1CKETimer_Sel = 0, .u1FASTWake = 1, .u1FASTWake2 = 1, .u1FastWake_Sel = 1, .u1CKEWake_Sel = 0, .u1ClkWiTrfc = 0
												},
						 CKE_Rank_Dependent = { .u1CKE2Rank = 1, .u1CKE2Rank_Opt3 = 0,
												.u1CKE2Rank_Opt5 = 0, .u1CKE2Rank_Opt6 = 0, .u1CKE2Rank_Opt7 = 0, .u1CKE2Rank_Opt8 = 0, .u1CKETimer_Sel = 1,
												.u1FASTWake = 1, .u1FASTWake2 = 0, .u1FastWake_Sel = 0, .u1CKEWake_Sel = 0, .u1ClkWiTrfc = 0
											  };
	//Select CKE control mode
	CKE_Mode = (CKECtrlMode == CKE_RANK_INDEPENDENT)? CKE_Rank_Independent: CKE_Rank_Dependent;

	//Apply CKE control mode register settings
	vIO32WriteFldAlign_All(DRAMC_REG_RKCFG, CKE_Mode.u1CKE2Rank, RKCFG_CKE2RANK);
	vIO32WriteFldMulti_All(DRAMC_REG_CKECTRL, P_Fld(CKE_Mode.u1CKE2Rank_Opt3, CKECTRL_CKE2RANK_OPT3)
					| P_Fld(CKE_Mode.u1CKE2Rank_Opt, CKECTRL_CKE2RANK_OPT)
					| P_Fld(CKE_Mode.u1CKE2Rank_Opt2, CKECTRL_CKE2RANK_OPT2)
					| P_Fld(CKE_Mode.u1CKE2Rank_Opt5, CKECTRL_CKE2RANK_OPT5)
					| P_Fld(CKE_Mode.u1CKE2Rank_Opt6, CKECTRL_CKE2RANK_OPT6)
					| P_Fld(CKE_Mode.u1CKE2Rank_Opt7, CKECTRL_CKE2RANK_OPT7)
					| P_Fld(CKE_Mode.u1CKE2Rank_Opt8, CKECTRL_CKE2RANK_OPT8)
					| P_Fld(CKE_Mode.u1CKETimer_Sel, CKECTRL_CKETIMER_SEL)
					| P_Fld(CKE_Mode.u1FastWake_Sel, CKECTRL_FASTWAKE_SEL)
					| P_Fld(CKE_Mode.u1CKEWake_Sel, CKECTRL_CKEWAKE_SEL));

	vIO32WriteFldMulti_All(DRAMC_REG_SHU_DCM_CTRL0, P_Fld(CKE_Mode.u1FASTWake, SHU_DCM_CTRL0_FASTWAKE) | P_Fld(CKE_Mode.u1FASTWake2, SHU_DCM_CTRL0_FASTWAKE2));

	vIO32WriteFldAlign_All(DRAMC_REG_ACTIMING_CTRL, CKE_Mode.u1ClkWiTrfc, ACTIMING_CTRL_CLKWITRFC);
}


#define MAX_CMP_CPT_WAIT_LOOP 100000   // max loop
static void DramcSetRWOFOEN(DRAMC_CTX_T *p, U8 u1onoff)
{
	U32 u4loop_count = 0;

	{
		while(u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MISC_STATUSA), MISC_STATUSA_REQQ_EMPTY) != 1)
		{
			mcDELAY_US(1);
			u4loop_count ++;

			if(u4loop_count > MAX_CMP_CPT_WAIT_LOOP)
			{
				mcSHOW_ERR_MSG(("RWOFOEN timout! queue is not empty\n"));
			#if __ETT__
				while(1);
			#else
				break;
			#endif
			}
		}
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SCHEDULER_COM), u1onoff, SCHEDULER_COM_RWOFOEN);
	}
}


//static void DramcEngine2CleanWorstSiPattern(DRAMC_CTX_T *p)
//{
//	  vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3),
//			  P_Fld(0, TEST2_A3_AUTO_GEN_PAT) |
//			  P_Fld(0, TEST2_A3_HFIDPAT) |
//			  P_Fld(0, TEST2_A3_TEST_AID_EN));
//}


static void DramcEngine2SetUiShift(DRAMC_CTX_T *p, U8 option)//UI shift function
{
	if(option == ENABLE)
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A0),
				P_Fld(1, TEST2_A0_TA2_LOOP_EN) |
				P_Fld(3, TEST2_A0_LOOP_CNT_INDEX));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3),
				P_Fld(1, TEST2_A3_TEST2_PAT_SHIFT) |
				P_Fld(0, TEST2_A3_PAT_SHIFT_SW_EN));
	}
	else
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A0),
				P_Fld(0, TEST2_A0_TA2_LOOP_EN) |
				P_Fld(0, TEST2_A0_LOOP_CNT_INDEX));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3),
				P_Fld(0, TEST2_A3_TEST2_PAT_SHIFT));
	}
}


void DramcSetRankEngine2(DRAMC_CTX_T *p, U8 u1RankSel)
{
	//LPDDR2_3_ADRDECEN_TARKMODE =0, always rank0
	/* ADRDECEN_TARKMODE: rank input selection
	 *	 1'b1 select CTO_AGENT1_RANK, 1'b0 rank by address decode
	 */
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3), 1, TEST2_A3_ADRDECEN_TARKMODE);

	// DUMMY_TESTAGENTRKSEL =0, select rank according to CATRAIN_TESTAGENTRK
	/* TESTAGENTRKSEL: Test agent access rank mode selection
	 * 2'b00: rank selection by TESTAGENTRK,		   2'b01: rank selection by CTO_AGENT_1_BK_ADR[0]
	 * 2'b10: rank selection by CTO_AGENT1_COL_ADR[3], 2'b11: rank selection by CTO_AGENT1_COL_ADR[4]
	 */
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A4), 0, TEST2_A4_TESTAGENTRKSEL);

	//CATRAIN_TESTAGENTRK = u1RankSel
	/* TESTAGENTRK: Specify test agent rank
	 * 2'b00 rank 0, 2'b01 rank 1, 2'b10 rank 2
	 */
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A4), u1RankSel, TEST2_A4_TESTAGENTRK);
}


void DramcEngine2SetPat(DRAMC_CTX_T *p, U8 u1TestPat, U8 u1LoopCnt, U8 u1Len1Flag, U8 u1EnableUiShift) //u1LoopCnt is related to rank
{

	if ((u1TestPat == TEST_XTALK_PATTERN) || (u1TestPat == TEST_SSOXTALK_PATTERN)) //xtalk or SSO+XTALK
	{
		//TEST_REQ_LEN1=1 is new feature, hope to make dq bus continously.
		//but DV simulation will got problem of compare err
		//so go back to use old way
		//TEST_REQ_LEN1=0, R_DMRWOFOEN=1
		if (u1Len1Flag != 0)
		{
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A4),
					P_Fld(1, TEST2_A4_TEST_REQ_LEN1)); //test agent 2 with cmd length = 0, LEN1 of 256bits data
			DramcSetRWOFOEN(p, 0); //@IPM will fix for LEN1=1 issue

		}
		else
		{
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A4),
					P_Fld(0, TEST2_A4_TEST_REQ_LEN1)); //test agent 2 with cmd length = 0
			DramcSetRWOFOEN(p, 1);
		}

		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3),
				P_Fld(0, TEST2_A3_AUTO_GEN_PAT) |
				P_Fld(0, TEST2_A3_HFIDPAT) |
				P_Fld(0, TEST2_A3_TEST_AID_EN) |
				P_Fld(0, TEST2_A3_TESTAUDPAT) |
				P_Fld(u1LoopCnt, TEST2_A3_TESTCNT)); //dont use audio pattern

		if (u1TestPat == TEST_SSOXTALK_PATTERN)
		{
			//set addr 0x48[16] to 1, TESTXTALKPAT = 1
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A4),
					P_Fld(1, TEST2_A4_TESTXTALKPAT) |
					P_Fld(0, TEST2_A4_TESTAUDMODE) |
					P_Fld(0, TEST2_A4_TESTAUDBITINV)); //use XTALK pattern, dont use audio pattern

			//R_DMTESTSSOPAT=0, R_DMTESTSSOXTALKPAT=0
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A4),
					P_Fld(1, TEST2_A4_TESTSSOPAT) |
					P_Fld(0, TEST2_A4_TESTSSOXTALKPAT)); //dont use sso, sso+xtalk pattern
		}
		else //select XTALK pattern
		{
			//set addr 0x48[16] to 1, TESTXTALKPAT = 1
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A4),
					P_Fld(1, TEST2_A4_TESTXTALKPAT) |
					P_Fld(0, TEST2_A4_TESTAUDMODE) |
					P_Fld(0, TEST2_A4_TESTAUDBITINV)); //use XTALK pattern, dont use audio pattern

			//R_DMTESTSSOPAT=0, R_DMTESTSSOXTALKPAT=0
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A4),
					P_Fld(0, TEST2_A4_TESTSSOPAT) |
					P_Fld(0, TEST2_A4_TESTSSOXTALKPAT)); //dont use sso, sso+xtalk pattern
		}
	}
	else if (u1TestPat == TEST_AUDIO_PATTERN) //AUDIO
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A4),
				P_Fld(0, TEST2_A4_TEST_REQ_LEN1));	 //test agent 2 with cmd length = 0
		// set AUDINIT=0x11 AUDINC=0x0d AUDBITINV=1 AUDMODE=1(1:read only(address fix), 0: write/read address change)
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A4),
				P_Fld(0x00000011, TEST2_A4_TESTAUDINIT) |
				P_Fld(0x0000000d, TEST2_A4_TESTAUDINC) |
				P_Fld(0, TEST2_A4_TESTXTALKPAT) |
				P_Fld(0, TEST2_A4_TESTAUDMODE) |
				P_Fld(1, TEST2_A4_TESTAUDBITINV));

		// set addr 0x044 [7] to 1 ,select audio pattern
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3),
				P_Fld(0, TEST2_A3_AUTO_GEN_PAT) |
				P_Fld(0, TEST2_A3_HFIDPAT) |
				P_Fld(0, TEST2_A3_TEST_AID_EN) |
				P_Fld(1, TEST2_A3_TESTAUDPAT) |
				P_Fld(u1LoopCnt, TEST2_A3_TESTCNT));
	}
	else if (u1TestPat == TEST_WORST_SI_PATTERN) //TEST2_OFF > 'h56
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A4),
				P_Fld(u1Len1Flag, TEST2_A4_TEST_REQ_LEN1)|
				P_Fld(0, TEST2_A4_TESTAUDINIT) |
				P_Fld(0, TEST2_A4_TESTAUDINC) |
				P_Fld(0, TEST2_A4_TESTXTALKPAT) |
				P_Fld(0, TEST2_A4_TESTSSOPAT)
				);
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3),
				P_Fld(0, TEST2_A3_TESTAUDPAT) |
				P_Fld(1, TEST2_A3_AUTO_GEN_PAT) |
				P_Fld(1, TEST2_A3_HFIDPAT) |
				P_Fld(1, TEST2_A3_TEST_AID_EN) |
				P_Fld(u1LoopCnt, TEST2_A3_TESTCNT)
				);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A2), 0x56, TEST2_A2_TEST2_OFF);//Set to min value to save time;
	}
	else //ISI
	{
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A4),
				P_Fld(0, TEST2_A4_TEST_REQ_LEN1)); //test agent 2 with cmd length = 0

		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3),
				P_Fld(0, TEST2_A3_AUTO_GEN_PAT) |
				P_Fld(0, TEST2_A3_HFIDPAT) |
				P_Fld(0, TEST2_A3_TEST_AID_EN) |
				P_Fld(0, TEST2_A3_TESTAUDPAT) |
				P_Fld(u1LoopCnt, TEST2_A3_TESTCNT));
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A4), 0, TEST2_A4_TESTXTALKPAT);
	}

	DramcEngine2SetUiShift(p, u1EnableUiShift); //Enalbe/Disable UI shift
}

#define CMP_CPT_POLLING_PERIOD 1	   // timeout for TE2: (CMP_CPT_POLLING_PERIOD X MAX_CMP_CPT_WAIT_LOOP)
#define MAX_CMP_CPT_WAIT_LOOP 100000   // max loop
static void DramcEngine2CheckComplete(DRAMC_CTX_T *p, U8 u1status)
{
	U32 u4loop_count = 0;
	U32 u4Ta2_loop_count = 0;
	U32 u4ShiftUiFlag = 0;//Use TEST_WORST_SI_PATTERN_UI_SHIFT

	while ((u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_TESTRPT)) & u1status) != u1status)
	{
		mcDELAY_US(CMP_CPT_POLLING_PERIOD);
		u4loop_count++;
		if ((u4loop_count > 3) && (u4loop_count <= MAX_CMP_CPT_WAIT_LOOP))
		{
			//mcSHOW_ERR_MSG(("TESTRPT_DM_CMP_CPT: %d\n", u4loop_count));
		}
		else if (u4loop_count > MAX_CMP_CPT_WAIT_LOOP)
		{
			/*TINFO="fcWAVEFORM_MEASURE_A %d: time out\n", u4loop_count*/
			mcSHOW_DBG_MSG(("fcWAVEFORM_MEASURE_A %d :time out, [22:20]=0x%x\n", u4loop_count, u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TESTRPT), TESTRPT_TESTSTAT)));

			//mcFPRINTF((fp_A60501, "fcWAVEFORM_MEASURE_A %d: time out\n", u4loop_count));

			break;
		}
	}

	u4loop_count = 0;
	u4ShiftUiFlag = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3), TEST2_A3_TEST2_PAT_SHIFT);
	if(u4ShiftUiFlag)//Use TEST_WORST_SI_PATTERN_UI_SHIFT
	{
		while ((u4Ta2_loop_count = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_TEST_LOOP_CNT))) != 8)
		{
			u4loop_count++;
			if(u4loop_count > MAX_CMP_CPT_WAIT_LOOP)
			{
				mcSHOW_DBG_MSG(("over MAX_CMP_CPT_WAIT_LOOP[%d] TEST_LOOP_CNT[%d]\n", u4loop_count, u4Ta2_loop_count));
				break;
			}
		}
	}
}

static U32 DramcEngine2Compare(DRAMC_CTX_T *p, DRAM_TE_OP_T wr)
{
	U32 u4result = 0xffffffff;
	U32 u4loopcount;
	U8	u1status = 1; //RK0
	U32 u4ShiftUiFlag = 0;

	u4loopcount = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3), TEST2_A3_TESTCNT);
	if (u4loopcount == 1)
		u1status = 3; //RK0/1

	u4ShiftUiFlag = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3), TEST2_A3_TEST2_PAT_SHIFT);

	if (wr == TE_OP_WRITE_READ_CHECK)
	{
		if(!u4ShiftUiFlag)//Could not use while UI shift is open
		{
			// read data compare ready check
			DramcEngine2CheckComplete(p, u1status);

			// disable write
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3),
					P_Fld(0, TEST2_A3_TEST2W) |
					P_Fld(0, TEST2_A3_TEST2R) |
					P_Fld(0, TEST2_A3_TEST1));

			mcDELAY_US(1);

			// enable read
			vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3),
					P_Fld(0, TEST2_A3_TEST2W) |
					P_Fld(1, TEST2_A3_TEST2R) |
					P_Fld(0, TEST2_A3_TEST1));
		}
	}

	// 5
	// read data compare ready check
	DramcEngine2CheckComplete(p, u1status);

	// delay 10ns after ready check from DE suggestion (1ms here)
	//mcDELAY_US(1);

	u4result = (u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_TESTRPT)) >> 4) & u1status; //CMP_ERR_RK0/1

	return u4result;
}

//-------------------------------------------------------------------------
/** DramcEngine2
 *	start the self test engine 2 inside dramc to test dram w/r.
 *	@param p				Pointer of context created by DramcCtxCreate.
 *	@param	wr				(DRAM_TE_OP_T): TE operation
 *	@param	test2_1 		(U32): 28bits,base address[27:0].
 *	@param	test2_2 		(U32): 28bits,offset address[27:0]. (unit is 16-byte, i.e: 0x100 is 0x1000).
 *	@param	loopforever 	(S16): 0	read\write one time ,then exit
 *								  >0 enable eingie2, after "loopforever" second ,write log and exit
 *								  -1 loop forever to read\write, every "period" seconds ,check result ,only when we find error,write log and exit
 *								  -2 loop forever to read\write, every "period" seconds ,write log ,only when we find error,write log and exit
 *								  -3 just enable loop forever ,then exit
 *	@param period			(U8):  it is valid only when loopforever <0; period should greater than 0
 *	@param u1LoopCnt	(U8): test loop number of test agent2 loop number =2^(u1LoopCnt) ,0 one time
 *	@retval status			(U32): return the value of DM_CMP_ERR  ,0  is ok ,others mean  error
 */
//-------------------------------------------------------------------------
static U32 uiReg0D0h;
DRAM_STATUS_T DramcEngine2Init(DRAMC_CTX_T *p, U32 test2_1, U32 test2_2, U8 u1TestPat, U8 u1LoopCnt, U8 u1EnableUiShift)
{
	U8 u1Len1Flag;

	// error handling
	if (!p)
	{
		mcSHOW_ERR_MSG(("context is NULL\n"));
		return DRAM_FAIL;
	}

	// check loop number validness
//	  if ((u1LoopCnt > 15) || (u1LoopCnt < 0))		// U8 >=0 always.
	if (u1LoopCnt > 15)
	{
		mcSHOW_ERR_MSG(("wrong param: u1LoopCnt > 15\n"));
		return DRAM_FAIL;
	}

	u1Len1Flag = (u1TestPat & 0x80) >> 7;
	u1TestPat = u1TestPat & 0x7f;

	DramcSetRankEngine2(p, p->rank);

	uiReg0D0h = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_DUMMY_RD));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DUMMY_RD),
			P_Fld(0, DUMMY_RD_DQSG_DMYRD_EN) |
			P_Fld(0, DUMMY_RD_DQSG_DMYWR_EN) |
			P_Fld(0, DUMMY_RD_DUMMY_RD_EN) |
			P_Fld(0, DUMMY_RD_SREF_DMYRD_EN) |
			P_Fld(0, DUMMY_RD_DMY_RD_DBG) |
			P_Fld(0, DUMMY_RD_DMY_WR_DBG));  //must close dummy read when do test agent

	//fixme-zj vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TESTCHIP_DMA1), 0, TESTCHIP_DMA1_DMA_LP4MATAB_OPT);

	// disable self test engine1 and self test engine2
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3),
			P_Fld(0, TEST2_A3_TEST2W) |
			P_Fld(0, TEST2_A3_TEST2R) |
			P_Fld(0, TEST2_A3_TEST1));

	// 1.set pattern ,base address ,offset address
	// 2.select  ISI pattern or audio pattern or xtalk pattern
	// 3.set loop number
	// 4.enable read or write
	// 5.loop to check DM_CMP_CPT
	// 6.return CMP_ERR
	// currently only implement ucengine_status = 1, others are left for future extension

	// 1
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A0),
			P_Fld(test2_1 >> 24, TEST2_A0_TEST2_PAT0) |
			P_Fld(test2_2 >> 24, TEST2_A0_TEST2_PAT1));

#if (__LP5_COMBO__ == TRUE)
	if (TRUE == is_lp5_family(p))
	{
		// LP5 TA2 base: 0x0
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RK_TEST2_A1),
				test2_1 & 0x00ffffff, RK_TEST2_A1_TEST2_BASE);
	}
	else
#endif
	{
		// LP4 TA2 base: 0x10000. It's only TBA constrain, but not HW.
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RK_TEST2_A1),
				(test2_1 + 0x10000) & 0x00ffffff, RK_TEST2_A1_TEST2_BASE);
	}

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A2),
			test2_2 & 0x00ffffff, TEST2_A2_TEST2_OFF);

	// 2 & 3
	// (TESTXTALKPAT, TESTAUDPAT) = 00 (ISI), 01 (AUD), 10 (XTALK), 11 (UNKNOWN)
	DramcEngine2SetPat(p, u1TestPat, u1LoopCnt, u1Len1Flag, u1EnableUiShift);

	return DRAM_OK;
}


U32 DramcEngine2Run(DRAMC_CTX_T *p, DRAM_TE_OP_T wr, U8 u1TestPat)
{
	U32 u4result = 0xffffffff;

	// 4
	if (wr == TE_OP_READ_CHECK)
	{
		if ((u1TestPat == 1) || (u1TestPat == 2))
		{
			//if audio pattern, enable read only (disable write after read), AUDMODE=0x48[15]=0
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A4), 0, TEST2_A4_TESTAUDMODE);
		}

		// enable read, 0x008[31:29]
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3),
				P_Fld(0, TEST2_A3_TEST2W) |
				P_Fld(1, TEST2_A3_TEST2R) |
				P_Fld(0, TEST2_A3_TEST1));
	}
	else if (wr == TE_OP_WRITE_READ_CHECK)
	{
		// enable write
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3),
				P_Fld(1, TEST2_A3_TEST2W) |
				P_Fld(0, TEST2_A3_TEST2R) |
				P_Fld(0, TEST2_A3_TEST1));
	}
	DramcEngine2Compare(p, wr);

	// delay 10ns after ready check from DE suggestion (1ms here)
	mcDELAY_US(1);

	// 6
	// return CMP_ERR, 0 is ok ,others are fail,diable test2w or test2r
	// get result
	// or all result
	u4result = (u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_CMP_ERR)));

	// disable read
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3),
			P_Fld(0, TEST2_A3_TEST2W) |
			P_Fld(0, TEST2_A3_TEST2R) |
			P_Fld(0, TEST2_A3_TEST1));

	return u4result;
}

void DramcEngine2End(DRAMC_CTX_T *p)
{
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A4),
			P_Fld(0, TEST2_A4_TEST_REQ_LEN1));	 //test agent 2 with cmd length = 0
	DramcSetRWOFOEN(p, 1);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_DUMMY_RD), uiReg0D0h);
}

U8 u1MaType = 0x2; // for DV sim
void TA2_Test_Run_Time_HW_Set_Column_Num(DRAMC_CTX_T * p)
{
	U8 u1ChannelIdx = 0;
	U8 u1EmiChIdx = 0;
	U32 u4matypeR0 = 0, u4matypeR1 = 0;
	U32 u4matype = 0;
	U32 u4EmiOffset = 0;
	DRAM_CHANNEL_T eOriChannel = p->channel;

	for (u1ChannelIdx = 0; u1ChannelIdx < p->support_channel_num; u1ChannelIdx++)
	{
		vSetPHY2ChannelMapping(p, u1ChannelIdx);

		u4EmiOffset = 0;
		u1EmiChIdx = u1ChannelIdx;
#if (CHANNEL_NUM > 2)
		if (u1ChannelIdx >= CHANNEL_C)
		{
			u4EmiOffset = 0x4000; // 0x1021D000 for CH2/3
			u1EmiChIdx = u1ChannelIdx-2;
		}
#endif

		u4matype = u4IO32Read4B(EMI_APB_BASE + u4EmiOffset);
		u4matypeR0 = ((u4matype >> (4 + u1EmiChIdx * 16)) & 0x3) + 1; //refer to init_ta2_single_channel()
		u4matypeR1 = ((u4matype >> (6 + u1EmiChIdx * 16)) & 0x3) + 1; //refer to init_ta2_single_channel()

		if(p->support_rank_num==RANK_SINGLE)
		{
			u4matype = u4matypeR0;
		}
		else
		{
			u4matype = (u4matypeR0 > u4matypeR1) ? u4matypeR1 : u4matypeR0; //get min value
		}
		//vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MATYPE), u4matype, MATYPE_MATYPE);
	}
	vSetPHY2ChannelMapping(p, eOriChannel);
	u1MaType = u4matype;

	return;
}

/* ----------------------------------------------------------------------
 * LP4 RG Address
 * bit:   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 9 8 7 6 5 4 3 2 1 0
 *		  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *		  ---------------------------------------------------------------
 * RG:	  - - R R R R R R R R R R R R R R R R R R|B B B|C C C C C C - - -
 * 2_BASE	  1 1 1 1 1 1 1 1 9 8 7 6 5 4 3 2 1 0|A A A|9 8 7 6 5 4
 *			  7 6 5 4 3 2 1 0					 |2 1 0|
 * AXI	  ---------------------------------------------------------------
 * Addr:	R R R R R R R R R R R R R R R R|B B B|C C C|C|C C C C C C C -
 *			1 1 1 1 1 1 9 8 7 6 5 4 3 2 1 0|A A A|9 8 7|H|6 5 4 3 2 1 0
 *			5 4 3 2 1 0 				   |2 1 0|	   | |
 * ----------------------------------------------------------------------
 */
#define TA2_RANK0_ADDRESS	(0x40200000)
#define AXI_CHAN_BIT_WIDTH 1//2: 4_channel 1: 2_channel
#define OFFSET_OF_RG_BASE_AND_AXI 2
#define LEN1_INTRINSIC_OFFSET 2
#define TRANSFER_DRAM_ADDR_BY_EMI_API 1 //1: by emi API 0: by above table
void TA2_Test_Run_Time_HW_Presetting(DRAMC_CTX_T * p, U32 len, TA2_RKSEL_TYPE_T rksel_mode)/* Should call after TA2_Test_Run_Time_Pat_Setting() */
{
	DRAM_CHANNEL_T eOriChannel = p->channel;
	DRAM_RANK_T eOriRank = p->rank;
	U32 u4BaseR0, u4BaseR1, u4Offset, u4Addr;
	U32 u4matypeR0, u4matypeR1, u4LEN1;
	U8 u1ChannelIdx, uiRWOFOEN, u1RkIdx;

	u4Addr = TA2_RANK0_ADDRESS & 0x1fffffff;
	if (u1IsLP4Family(p->dram_type))
	{
#if TRANSFER_DRAM_ADDR_BY_EMI_API
		{
			dram_addr_t dram_addr = {0};
			unsigned long long ull_axi_addr = TA2_RANK0_ADDRESS;
			phy_addr_to_dram_addr(&dram_addr, ull_axi_addr); //Make sure row. bank. column are correct
			u4BaseR0 = ((dram_addr.row << 12) | (dram_addr.bk << 9) | (dram_addr.col >> 1)) >> 3;// >>1: RG C4 @3th bit >>3: RG start with bit 3
		}
#else
		// >>AXI_CHAN_BIT_WIDTH: drop bit8; >>OFFSET_OF_RG_BASE_AND_AXI: align with RG row;  >>3: RG start with bit 3
		u4BaseR0 = (((u4Addr & ~0x1ff) >> AXI_CHAN_BIT_WIDTH) | (u4Addr & 0xff)) >> (OFFSET_OF_RG_BASE_AND_AXI + 3);
#endif
		u4Offset = len >> (AXI_CHAN_BIT_WIDTH + 5);//5:0x20 bytes(256 bits) address coverage per pattern(128 bits data + 128 bits bubble); offset should bigger than 0xFF
	}
	else
	{
		u4BaseR0 = u4Addr >> 4;
		if (rksel_mode == TA2_RKSEL_XRT)
		{
			u4Offset = len >> 4;//16B per pattern
		}
		else
		{
			u4Offset = (len >> 4) >> 1;//16B per pattern //len should be >>2 or test engine will time out
		}
	}
	u4BaseR1 = u4BaseR0;

	u4matypeR0 = ((u4IO32Read4B(EMI_APB_BASE) >> 4) & 0x3) + 1;
	u4matypeR1 = ((u4IO32Read4B(EMI_APB_BASE) >> 6) & 0x3) + 1;
	if (u4matypeR0 != u4matypeR1)//R0 R1 mix mode
	{
		(u4matypeR0 > u4matypeR1)? (u4BaseR0 >>= 1): (u4BaseR1 >>= 1);//set the correct start address, refer to mapping table
		u4Offset >>= 1;//set min value
	}

	u4Offset = (u4Offset == 0) ? 1 : u4Offset; //halt if u4Offset = 0

	u4LEN1 = u4IO32ReadFldAlign(DRAMC_REG_TEST2_A4, TEST2_A4_TEST_REQ_LEN1);
	if(u4LEN1)
	{
		u4Offset = u4Offset - LEN1_INTRINSIC_OFFSET;
	}

#if ENABLE_EMI_LPBK_TEST && EMI_USE_TA2
	if (gEmiLpbkTest)
	{
		u4matypeR0 = 2;
		u4matypeR1 = 2;
		u4Offset = 3;
		//u4Offset = 6;//3; //6: for emilpbk_dq_dvs_leadlag_toggle_ringcnt
	}
#endif

	if (TA2_RKSEL_XRT == rksel_mode)
	{
		// In order to enhance XRT R2R/W2W probability, use TEST2_4_TESTAGENTRKSEL=3, PERFCTL0_RWOFOEN=0 mode
		uiRWOFOEN = 0;
		mcSHOW_DBG_MSG(("=== TA2 XRT R2R/W2W\n"));
	}
	else
	{
		uiRWOFOEN = 1;
#if !ENABLE_EMI_LPBK_TEST
		mcSHOW_DBG_MSG(("=== TA2 HW\n"));
#endif
	}
#if !ENABLE_EMI_LPBK_TEST
	mcSHOW_DBG_MSG(("=== OFFSET:0x%x\n", u4Offset));
#endif
	for (u1ChannelIdx = 0; u1ChannelIdx < p->support_channel_num; u1ChannelIdx++)
	{
		p->channel = (DRAM_CHANNEL_T)u1ChannelIdx;

		for(u1RkIdx = 0; u1RkIdx < p->support_rank_num; u1RkIdx++)
		{
			p->rank = (DRAM_RANK_T)u1RkIdx;
			vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RK_TEST2_A1), u4BaseR0, RK_TEST2_A1_TEST2_BASE);//fill based on RG table for Rank 0
		}
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A2), u4Offset, TEST2_A2_TEST2_OFF);//128 bits data length per offest
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A4), rksel_mode, TEST2_A4_TESTAGENTRKSEL);
		DramcSetRWOFOEN(p, uiRWOFOEN);
	}
	p->channel = eOriChannel;
	p->rank = eOriRank;
	//TA2_Test_Run_Time_HW_Set_Column_Num(p);

	return;
}
#if ETT_MINI_STRESS_USE_TA2_LOOP_MODE
#define TA2_PAT TEST_WORST_SI_PATTERN
#else
#define TA2_PAT TEST_XTALK_PATTERN
#endif
void TA2_Test_Run_Time_Pat_Setting(DRAMC_CTX_T *p, U8 PatSwitch)
{
	static U8 u1Pat = TA2_PAT, u1loop = 1;
	U8 u1ChannelIdx = 0;
	DRAM_CHANNEL_T eOriChannel = p->channel;

	if (u1loop || PatSwitch == TA2_PAT_SWITCH_ON)
	{
#if !ENABLE_EMI_LPBK_TEST
		mcSHOW_DBG_MSG(("TA2 PAT: %d\n", u1Pat));
#endif
		for (u1ChannelIdx = CHANNEL_A; u1ChannelIdx < p->support_channel_num; u1ChannelIdx++)
		{
			p->channel = (DRAM_CHANNEL_T)u1ChannelIdx;
			DramcEngine2SetPat(p, u1Pat, p->support_rank_num - 1, 0, TE_NO_UI_SHIFT);
		}
		p->channel = eOriChannel;

		#if !ETT_MINI_STRESS_USE_TA2_LOOP_MODE
		{
			U32 u4Value = 0;
		u4Value = (u1Pat == TEST_WORST_SI_PATTERN) ? 1 : 0; //Worst SI pattern + loop mode + LEN1
		vIO32WriteFldAlign_All(DRAMC_REG_TEST2_A0, u4Value, TEST2_A0_TA2_LOOP_EN);
		vIO32WriteFldAlign_All(DRAMC_REG_TEST2_A0, u4Value, TEST2_A0_LOOP_NV_END);
		vIO32WriteFldAlign_All(DRAMC_REG_TEST2_A0, u4Value, TEST2_A0_ERR_BREAK_EN);
		vIO32WriteFldAlign_All(DRAMC_REG_TEST2_A4, u4Value, TEST2_A4_TEST_REQ_LEN1);
		}
		#endif
		if (PatSwitch)
			u1Pat = (u1Pat + 1) % 4;
		else
			u1loop = 0;
	}
	return;
}

void TA2_Test_Run_Time_HW_Write(DRAMC_CTX_T * p, U8 u1Enable)
{
	DRAM_CHANNEL_T eOriChannel = p->channel;
	U8 u1ChannelIdx;

#if !ENABLE_EMI_LPBK_TEST
	mcSHOW_DBG_MSG(("\nTA2 Trigger Write\n"));
#endif
	for (u1ChannelIdx = 0; u1ChannelIdx < p->support_channel_num; u1ChannelIdx++)
	{
		p->channel = (DRAM_CHANNEL_T)u1ChannelIdx;
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3), u1Enable, TEST2_A3_TEST2W);
	}
	p->channel = eOriChannel;
	return;
}

static void TA2_Show_Cnt(DRAMC_CTX_T * p, U32 u4ErrorValue)
{
	static U32 err_count = 0;
	static U32 pass_count = 0;
	U8 u1RankIdx = 0;
	for (u1RankIdx = 0; u1RankIdx < p->support_rank_num; u1RankIdx++)
	{
		if (u4ErrorValue & (1 << u1RankIdx))
		{
			err_count++;
			mcSHOW_DBG_MSG(("HW channel(%d) Rank(%d), TA2 failed, pass_cnt:%d, err_cnt:%d\n", p->channel, u1RankIdx, pass_count, err_count));
		}
		else
		{
			pass_count++;
			mcSHOW_DBG_MSG(("HW channel(%d) Rank(%d), TA2 pass, pass_cnt:%d, err_cnt:%d\n", p->channel, u1RankIdx, pass_count, err_count));
		}
	}
}
#if ETT_MINI_STRESS_USE_TA2_LOOP_MODE
U32 TA2_Test_Run_Time_HW_Status(DRAMC_CTX_T * p)
{
	U8 u1ChannelIdx = 0;
	U32 u4ErrorValue = 0;
	U32 bit_error = 0;
	DRAM_CHANNEL_T eOriChannel = p->channel;
	for (u1ChannelIdx = 0; u1ChannelIdx < p->support_channel_num; u1ChannelIdx++)
	{
		vSetPHY2ChannelMapping(p, u1ChannelIdx);
		u4ErrorValue = DramcEngine2Compare(p, TE_OP_WRITE_READ_CHECK);
		if (u4ErrorValue & 0x3) //RK0 or RK1 test fail
		{
			mcSHOW_DBG_MSG(("=== HW channel(%d) u4ErrorValue: 0x%x, bit error: 0x%x\n", u1ChannelIdx, u4ErrorValue, u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_CMP_ERR))));
#if defined(SLT)
			mcSHOW_ERR_MSG(("[dramc] DRAM_FATAL_ERR_FLAG = 0x80000000, line: %d\n",__LINE__));
			while (1);
#endif
		}
		TA2_Show_Cnt(p, u4ErrorValue);
		bit_error |= u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_CMP_ERR));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3), P_Fld(0, TEST2_A3_TEST2W) | P_Fld(0, TEST2_A3_TEST2R) | P_Fld(0, TEST2_A3_TEST1));
	}
	vSetPHY2ChannelMapping(p, eOriChannel);
	return bit_error;
}
#else
U32 TA2_Test_Run_Time_HW_Status(DRAMC_CTX_T * p)
{
	U8 u1ChannelIdx = 0;
	U32 u4ErrorValue = 0;
	U32 u4Ta2LoopEn = 0;
	U32 u4loopcount = 0;
	U8 u1status = 0;
	U32 bit_error = 0;
	static U32 err_count = 0;
	static U32 pass_count = 0;
	DRAM_CHANNEL_T eOriChannel = p->channel;

	for (u1ChannelIdx = 0; u1ChannelIdx < p->support_channel_num; u1ChannelIdx++)
	{
		vSetPHY2ChannelMapping(p, u1ChannelIdx);
		u4Ta2LoopEn = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A0), TEST2_A0_TA2_LOOP_EN);
		//mcSHOW_DBG_MSG(("### u4Ta2LoopEn:%d ### \n", u4Ta2LoopEn));

		if(u4Ta2LoopEn)
		{
			u4loopcount = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3), TEST2_A3_TESTCNT);
			if (u4loopcount == 1)
				u1status = 3; //RK0/1

			vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A0), 0, TEST2_A0_LOOP_NV_END);//cancel NV_END
			DramcEngine2CheckComplete(p, u1status);//Wait for complete
			//mcSHOW_DBG_MSG(("TESTRPT_TESTSTAT:%x\n", u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TESTRPT), TESTRPT_TESTSTAT)));//check TESTRPT_TESTSTAT
			u4ErrorValue = (u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_TESTRPT)) >> 4) & 0x3; //CMP_ERR_RK0/1
		}
		else
			u4ErrorValue = DramcEngine2Compare(p, TE_OP_WRITE_READ_CHECK);

		if (u4ErrorValue & 0x3) //RK0 or RK1 test fail
		{
			mcSHOW_DBG_MSG(("=== HW channel(%d) u4ErrorValue: 0x%x, bit error: 0x%x\n", u1ChannelIdx, u4ErrorValue, u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_CMP_ERR))));
#if defined(SLT)
			mcSHOW_ERR_MSG(("[dramc] DRAM_FATAL_ERR_FLAG = 0x80000000, line: %d\n",__LINE__));
			while (1);
#endif
		}
		TA2_Show_Cnt(p, u4ErrorValue);

		bit_error |= u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_CMP_ERR));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3), P_Fld(0, TEST2_A3_TEST2W) | P_Fld(0, TEST2_A3_TEST2R) | P_Fld(0, TEST2_A3_TEST1));
	}
	vSetPHY2ChannelMapping(p, eOriChannel);

	return bit_error;
}
#endif

void TA2_Test_Run_Time_HW(DRAMC_CTX_T * p)
{
	DRAM_CHANNEL_T channel_bak = p->channel;
	DRAM_RANK_T rank_bak = p->rank;

	TA2_Test_Run_Time_HW_Presetting(p, 0x10000, TA2_RKSEL_HW);	//TEST2_2_TEST2_OFF = 0x400
	TA2_Test_Run_Time_Pat_Setting(p, TA2_PAT_SWITCH_OFF);
	TA2_Test_Run_Time_HW_Write(p, ENABLE);
	//mcDELAY_MS(1);
	TA2_Test_Run_Time_HW_Status(p);

	p->channel = channel_bak;
	p->rank = rank_bak;
	return;
}

void Temp_TA2_Test_After_K(DRAMC_CTX_T * p)
{
	DRAM_CHANNEL_T channel_bak = p->channel;
	DRAM_RANK_T rank_bak = p->rank;

	do {
		TA2_Test_Run_Time_Pat_Setting(p, TA2_PAT_SWITCH_ON);
		TA2_Test_Run_Time_HW_Presetting(p, 0x200000, TA2_RKSEL_HW);
		TA2_Test_Run_Time_HW_Write(p, ENABLE);//TA2 trigger W
		TA2_Test_Run_Time_HW_Status(p);
	   }while(1);

	p->channel = channel_bak;
	p->rank = rank_bak;
	return;
}

static U8 *DramcFetchGlobalMR(DRAMC_CTX_T *p, U8 mr_idx)
{
	U8 *pMRGlobalValue = NULL;

	switch (mr_idx)
	{
		case 13: pMRGlobalValue = &u1MR13Value[p->rank]; break;
		case 26: pMRGlobalValue = &u1MR26Value[p->rank]; break;
		case 30: pMRGlobalValue = &u1MR30Value[p->rank]; break;
		default:
			mcSHOW_ERR_MSG(("%s NULL\n", __func__));
			#if __ETT__
			while(1);
			#endif
			break;
	}

	return pMRGlobalValue;
}

#if MRW_BACKUP
U8 DramcMRWriteBackup(DRAMC_CTX_T *p, U8 u1MRIdx, U8 u1Rank)
{
	U8 u1Value=0xff;
	U8 u1Fsp;
	U8 u1MRBackup_ERR_Flag=0, u1backupRK=p->rank;
	U16 u2Offset=0x0;
	REG_TRANSFER_T TransferReg;

	u1Fsp = FSP_0;

	#if (__LP5_COMBO__ == TRUE)
	if (is_lp5_family(p))
	{
		switch(u1MRIdx)
		{
			case 1:
			case 2:
			case 3:
			case 10:
			case 11:
			case 12:
			case 14:
			case 15:
			case 17:
			case 18:
			case 19:
			case 20:
			case 24:
			case 30:
			case 41:
				u1Fsp = gFSPWR_Flag[u1Rank];
				break;
		}
	}
	else
	#endif
	{
		switch(u1MRIdx)
	   {
			case 1:
			case 2:
			case 3:
			case 11:
			case 12:
			case 14:
			case 22:
				u1Fsp = gFSPWR_Flag[u1Rank];
				break;
		}
	}
	if (u1Fsp == FSP_0) /* All MR */
	{
		switch (u1MRIdx)
		{
			case 1:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_00_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_00_RK0_FSP0_MRWBK_RK0_FSP0_MR1;
				break;
			case 2:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_00_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_00_RK0_FSP0_MRWBK_RK0_FSP0_MR2;
				break;
			case 3:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_00_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_00_RK0_FSP0_MRWBK_RK0_FSP0_MR3;
				break;
			case 4:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_00_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_00_RK0_FSP0_MRWBK_RK0_FSP0_MR4;
				break;
			case 9:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_01_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_01_RK0_FSP0_MRWBK_RK0_FSP0_MR9;
				break;
			case 10:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_01_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_01_RK0_FSP0_MRWBK_RK0_FSP0_MR10;
				break;
			case 11:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_01_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_01_RK0_FSP0_MRWBK_RK0_FSP0_MR11;
				break;
			case 12:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_01_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_01_RK0_FSP0_MRWBK_RK0_FSP0_MR12;
				break;
			case 13:
				//MR13(LP4) work around, two RG is not synchronized
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_02_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_02_RK0_FSP0_MRWBK_RK0_FSP0_MR13;
				break;
			case 14:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_02_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_02_RK0_FSP0_MRWBK_RK0_FSP0_MR14;
				break;
			case 15:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_02_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_02_RK0_FSP0_MRWBK_RK0_FSP0_MR15;
				break;
			case 16:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_02_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_02_RK0_FSP0_MRWBK_RK0_FSP0_MR16;
				break;
			case 17:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_03_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_03_RK0_FSP0_MRWBK_RK0_FSP0_MR17;
				break;
			case 18:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_03_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_03_RK0_FSP0_MRWBK_RK0_FSP0_MR18;
				break;
			case 19:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_03_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_03_RK0_FSP0_MRWBK_RK0_FSP0_MR19;
				break;
			case 20:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_03_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_03_RK0_FSP0_MRWBK_RK0_FSP0_MR20;
				break;
			case 21:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_04_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_04_RK0_FSP0_MRWBK_RK0_FSP0_MR21;
				break;
			case 22:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_04_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_04_RK0_FSP0_MRWBK_RK0_FSP0_MR22;
				break;
			case 23:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_04_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_04_RK0_FSP0_MRWBK_RK0_FSP0_MR23;
				break;
			case 24:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_04_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_04_RK0_FSP0_MRWBK_RK0_FSP0_MR24;
				break;
			case 25:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_05_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_05_RK0_FSP0_MRWBK_RK0_FSP0_MR25;
				break;
			case 26:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_05_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_05_RK0_FSP0_MRWBK_RK0_FSP0_MR26;
				break;
			case 27:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_05_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_05_RK0_FSP0_MRWBK_RK0_FSP0_MR27;
				break;
			case 28:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_05_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_05_RK0_FSP0_MRWBK_RK0_FSP0_MR28;
				break;
			case 30:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_06_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_06_RK0_FSP0_MRWBK_RK0_FSP0_MR30;
				break;
			case 31:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_06_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_06_RK0_FSP0_MRWBK_RK0_FSP0_MR31;
				break;
			case 32:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_06_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_06_RK0_FSP0_MRWBK_RK0_FSP0_MR32;
				break;
			case 33:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_06_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_06_RK0_FSP0_MRWBK_RK0_FSP0_MR33;
				break;
			case 34:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_07_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_07_RK0_FSP0_MRWBK_RK0_FSP0_MR34;
				break;
			case 37:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_07_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_07_RK0_FSP0_MRWBK_RK0_FSP0_MR37;
				break;
			case 39:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_07_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_07_RK0_FSP0_MRWBK_RK0_FSP0_MR39;
				break;
			case 40:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_07_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_07_RK0_FSP0_MRWBK_RK0_FSP0_MR40;
				break;
			case 41:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_08_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_08_RK0_FSP0_MRWBK_RK0_FSP0_MR41;
				break;
			case 42:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_08_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_08_RK0_FSP0_MRWBK_RK0_FSP0_MR42;
				break;
			case 46:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_08_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_08_RK0_FSP0_MRWBK_RK0_FSP0_MR46;
				break;
			case 48:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_08_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_08_RK0_FSP0_MRWBK_RK0_FSP0_MR48;
				break;
			case 51:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_09_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_09_RK0_FSP0_MRWBK_RK0_FSP0_MR51;
				break;
			case 63:
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_09_RK0_FSP0;
				TransferReg.u4Fld = MR_BACKUP_09_RK0_FSP0_MRWBK_RK0_FSP0_MR63;
				break;
		}
	}
	else if (u1MRIdx == 21 || u1MRIdx == 22) /* MR only in FSP0/FSP1 */
	{
		if (u1MRIdx == 21)
		{
			TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_04_RK0_FSP1;
			TransferReg.u4Fld = MR_BACKUP_04_RK0_FSP1_MRWBK_RK0_FSP1_MR21;
	}
		else
		{
			TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_03_RK0_FSP1;
			TransferReg.u4Fld = MR_BACKUP_03_RK0_FSP1_MRWBK_RK0_FSP1_MR22;
		}
	}
	else /* MR in FSP0/FSP1/FSP2 */
	{
		if (u1MRIdx <= 20)
		{
			if (u1MRIdx <= 10)
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_00_RK0_FSP1;
			else if (u1MRIdx <= 15)
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_01_RK0_FSP1;
			else
				TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_03_RK0_FSP1;
			TransferReg.u4Addr += ((u1Fsp - FSP_1) * 0x30);
			if (u1MRIdx == 1 || u1MRIdx == 11 || u1MRIdx == 17)
				TransferReg.u4Fld = MR_BACKUP_00_RK0_FSP1_MRWBK_RK0_FSP1_MR1;
			else if (u1MRIdx == 2 || u1MRIdx == 12 || u1MRIdx == 18)
				TransferReg.u4Fld = MR_BACKUP_00_RK0_FSP1_MRWBK_RK0_FSP1_MR2;
			else if (u1MRIdx == 3 || u1MRIdx == 14 || u1MRIdx == 19)
				TransferReg.u4Fld = MR_BACKUP_00_RK0_FSP1_MRWBK_RK0_FSP1_MR3;
			else
				TransferReg.u4Fld = MR_BACKUP_00_RK0_FSP1_MRWBK_RK0_FSP1_MR10;
		}
		else if (u1Fsp == FSP_2 && u1MRIdx == 24)
		{
			TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_03_RK0_FSP2;
			TransferReg.u4Fld = MR_BACKUP_03_RK0_FSP2_MRWBK_RK0_FSP2_MR24;
		}
		else if (u1Fsp == FSP_1 && u1MRIdx == 41)
		{
			TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_03_RK0_FSP1;
			TransferReg.u4Fld = MR_BACKUP_03_RK0_FSP1_MRWBK_RK0_FSP1_MR41;
		}
		else
		{
			TransferReg.u4Addr = DRAMC_REG_MR_BACKUP_03_RK0_FSP1 + ((u1Fsp - FSP_1) * 0x30);
			if ((u1Fsp == FSP_1 && u1MRIdx == 24) || (u1Fsp == FSP_2 && u1MRIdx == 30))
			{
				TransferReg.u4Fld = MR_BACKUP_03_RK0_FSP1_MRWBK_RK0_FSP1_MR24;
			}
			else
			{
				TransferReg.u4Fld = MR_BACKUP_03_RK0_FSP1_MRWBK_RK0_FSP1_MR30;
			}
		}
	}
	vSetRank(p, u1Rank);
	if (u1MRBackup_ERR_Flag==0)
	{
		u1Value=u4IO32ReadFldAlign(DRAMC_REG_ADDR(TransferReg.u4Addr), TransferReg.u4Fld);
		mcSHOW_MRW_MSG(("  [MRW Backup] Rank%d FSP%d MR%d=0x%x\n",u1Rank, gFSPWR_Flag[u1Rank], u1MRIdx, u1Value));
	}
	vSetRank(p, u1backupRK);

	return u1Value;

}
#endif

void DramcMRWriteFldMsk(DRAMC_CTX_T *p, U8 mr_idx, U8 listVal8, U8 msk8, U8 UpdateMode)
{
	U8 *pMRGlobalValue = DramcFetchGlobalMR(p, mr_idx);

	// ASSERT (pMRGlobalValue != NULL)

	*pMRGlobalValue = ((*pMRGlobalValue & ~msk8) | listVal8);

	if (UpdateMode == TO_MR)
		DramcModeRegWriteByRank(p, p->rank, mr_idx, *pMRGlobalValue);
}

void DramcMRWriteFldAlign(DRAMC_CTX_T *p, U8 mr_idx, U8 value, U32 mr_fld, U8 UpdateMode)
{
	U8 *pMRGlobalValue = DramcFetchGlobalMR(p, mr_idx);

	// ASSERT (pMRGlobalValue != NULL)

	*pMRGlobalValue &= ~(Fld2Msk32(mr_fld));
	*pMRGlobalValue |= (value << Fld_shft(mr_fld));

	if (UpdateMode == TO_MR)
		DramcModeRegWriteByRank(p, p->rank, mr_idx, *pMRGlobalValue);
}

void DramcModeRegRead(DRAMC_CTX_T *p, U8 u1MRIdx, U16 *u2pValue)
{
	U32 u4MRValue;
#ifdef DUMP_INIT_RG_LOG_TO_DE
	gDUMP_INIT_RG_LOG_TO_DE_RG_log_flag=0;
#endif
	//vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), p->rank, SWCMD_CTRL0_MRRRK);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), u1MRIdx, SWCMD_CTRL0_MRSMA);

	// MRR command will be fired when MRREN 0->1
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 1, SWCMD_EN_MRREN);

	// wait MRR command fired.
	while (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), SPCMDRESP_MRR_RESPONSE) == 0)
	{
		mcDELAY_US(1);
	}

	// Since LP3 does not support CG condition, LP3 can not use MRR_STATUS_MRR_SW_REG to do sw mrr.
	// After fix HW CG condition, LP3 will use MRR_STATUS_MRR_SW_REG to do sw mrr.
	U32 u4MRRReg;
	if (u1IsLP4Family(p->dram_type))
		u4MRRReg = MRR_STATUS_MRR_SW_REG;
	else
		u4MRRReg = MRR_STATUS_MRR_REG;

	// Read out mode register value
	u4MRValue = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MRR_STATUS), u4MRRReg);
	*u2pValue = (U16)u4MRValue;

	// Set MRREN =0 for next time MRR.
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0, SWCMD_EN_MRREN);

#ifdef DUMP_INIT_RG_LOG_TO_DE
	gDUMP_INIT_RG_LOG_TO_DE_RG_log_flag=1;
#endif

	mcSHOW_DBG_MSG3(("Read MR%d =0x%x\n", u1MRIdx, u4MRValue));
}


void DramcModeRegReadByRank(DRAMC_CTX_T *p, U8 u1Rank, U8 u1MRIdx, U16 *u2pValue)
{
	U16 u2Value = 0;
	U8 u1RankBak;

	/* Since, TMRRI design changed (2 kinds of modes depending on value of R_DMRK_SCINPUT_OPT)
	 * DE: Jouling, Berson
	 * To specify SW_MRR rank -> new mode(scinput_opt == 0): MRSRK
	 *							 old mode(scinput_opt == 1): MRRRK
	 * Note: MPCRK is not used by SW to control rank anymore
	 */
	//Backup & set rank
	u1RankBak = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), SWCMD_CTRL0_MRSRK); //backup rank
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), u1Rank, SWCMD_CTRL0_MRSRK); //set rank

	//Mode reg read
	DramcModeRegRead(p, u1MRIdx, &u2Value);
	*u2pValue = u2Value;

	//Restore rank
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), u1RankBak, SWCMD_CTRL0_MRSRK);

}

void DramcModeRegWriteByRank_RTMRW(DRAMC_CTX_T *p, U8 *u1Rank, U8 *u1MRIdx, U8 *u1Value, U8 u1Len)
{
	U32 u4Response, u4TimeCnt, ii;
	U8 u1MRRK[6] = {0}, u1MRMA[6] = {0}, u1MROP[6] = {0};

	if (u1Len > 6 || u1Len == 0)
		return;

	for (ii = 0;ii < u1Len;ii++)
	{
		u1MRRK[ii] = u1Rank[ii];
		u1MRMA[ii] = u1MRIdx[ii];
		u1MROP[ii] = u1Value[ii];
	}

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RTMRW_CTRL0),
			P_Fld(3, RTMRW_CTRL0_RTMRW_LAT) |
			P_Fld(0x20, RTMRW_CTRL0_RTMRW_AGE) |
			P_Fld(u1Len - 1, RTMRW_CTRL0_RTMRW_LEN) |
			P_Fld(u1MRRK[0], RTMRW_CTRL0_RTMRW0_RK) |
			P_Fld(u1MRRK[1], RTMRW_CTRL0_RTMRW1_RK) |
			P_Fld(u1MRRK[2], RTMRW_CTRL0_RTMRW2_RK) |
			P_Fld(u1MRRK[3], RTMRW_CTRL0_RTMRW3_RK) |
			P_Fld(u1MRRK[4], RTMRW_CTRL0_RTMRW4_RK) |
			P_Fld(u1MRRK[5], RTMRW_CTRL0_RTMRW5_RK));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RTMRW_CTRL1),
			P_Fld(u1MRMA[0], RTMRW_CTRL1_RTMRW0_MA) |
			P_Fld(u1MRMA[1], RTMRW_CTRL1_RTMRW1_MA) |
			P_Fld(u1MRMA[2], RTMRW_CTRL1_RTMRW2_MA) |
			P_Fld(u1MRMA[3], RTMRW_CTRL1_RTMRW3_MA));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RTMRW_CTRL2),
			P_Fld(u1MROP[0], RTMRW_CTRL2_RTMRW0_OP) |
			P_Fld(u1MROP[1], RTMRW_CTRL2_RTMRW1_OP) |
			P_Fld(u1MROP[2], RTMRW_CTRL2_RTMRW2_OP) |
			P_Fld(u1MROP[3], RTMRW_CTRL2_RTMRW3_OP));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RTMRW_CTRL3),
			P_Fld(u1MRMA[4], RTMRW_CTRL3_RTMRW4_MA) |
			P_Fld(u1MRMA[5], RTMRW_CTRL3_RTMRW5_MA) |
			P_Fld(u1MROP[4], RTMRW_CTRL3_RTMRW4_OP) |
			P_Fld(u1MROP[5], RTMRW_CTRL3_RTMRW5_OP));
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MPC_CTRL),
			1, MPC_CTRL_RTMRW_HPRI_EN);
	mcDELAY_US(5);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN),
			1, SWCMD_EN_RTMRWEN);

#if __LP5_COMBO__
#if WORKAROUND_LP5_HEFF
	if (is_heff_mode(p))
	{
		mcDELAY_US(1);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
				1, CKECTRL_CKE2RANK_OPT6);
	}
#endif
#endif

	u4TimeCnt = TIME_OUT_CNT;

	do {
		u4Response = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP),
				SPCMDRESP_RTMRW_RESPONSE);
		u4TimeCnt--;
		mcDELAY_US(5);
	} while ((u4Response == 0) && (u4TimeCnt > 0));

	if (u4TimeCnt == 0)//time out
	{
		mcSHOW_DBG_MSG(("[LP5 RT MRW ] Resp fail (time out) Rank=%d, MR%d=0x%x\n", u1Rank[0], u1MRIdx[0], u1Value[0]));
	}

#if __LP5_COMBO__
#if WORKAROUND_LP5_HEFF
	if (is_heff_mode(p))
	{
		mcDELAY_US(1);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
				0, CKECTRL_CKE2RANK_OPT6);
	}
#endif
#endif

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN),
			0, SWCMD_EN_RTMRWEN);
}

static void DramcModeRegWriteByRank_SCSM(DRAMC_CTX_T *p, U8 u1Rank, U8 u1MRIdx, U8 u1Value)
{
	U32 counter = 0;
	U32 u4RabnkBackup;
	U32 u4register_024;

	// backup rank
	u4RabnkBackup = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), SWCMD_CTRL0_MRSRK);

	//backup register of CKE fix on/off
	u4register_024 = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL));

	// set rank
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), u1Rank, SWCMD_CTRL0_MRSRK);

	//CKE must be fix on when doing MRW
	CKEFixOnOff(p, u1Rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), u1MRIdx, SWCMD_CTRL0_MRSMA);
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), u1Value, SWCMD_CTRL0_MRSOP);

	// MRW command will be fired when MRWEN 0->1
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 1, SWCMD_EN_MRWEN);

	// wait MRW command fired.
	while (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), SPCMDRESP_MRW_RESPONSE) == 0)
	{
		counter++;
		mcSHOW_DBG_MSG2(("wait MRW command Rank%d MR%d =0x%x fired (%d)\n", u1Rank, u1MRIdx, u1Value, counter));
		mcDELAY_US(1);
	}

	// Set MRWEN =0 for next time MRW.
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0, SWCMD_EN_MRWEN);

	// restore CKEFIXON value
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), u4register_024);

	// restore rank
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_CTRL0), u4RabnkBackup, SWCMD_CTRL0_MRSRK);
}

void DramcModeRegWriteByRank(DRAMC_CTX_T *p, U8 u1Rank, U8 u1MRIdx, U8 u1Value)
{
	mcSHOW_DBG_MSG2(("MRW RK%d MR#%d = 0x%x\n", u1Rank,u1MRIdx, u1Value));

#ifndef ENABLE_POST_PACKAGE_REPAIR
	if ((u1MRIdx == 0x04) && (u1Value & 0x10))
	{
		mcSHOW_ERR_MSG(("ERROR MRW RK%d MR#%d = 0x%x\n", u1Rank,u1MRIdx, u1Value));
		while(1);
	}
#endif
	#if (fcFOR_CHIP_ID == fcA60868)
	// RTMRW & RTSWCMD-MRW can not be used in runtime

		if (u1EnterRuntime)
		{
			DramcModeRegWriteByRank_SCSM(p, u1Rank, u1MRIdx, u1Value);
		}
	else
#endif
		{
		#if (__LP5_COMBO__ == TRUE)
			if (is_lp5_family(p))
			{
			#if ENABLE_RUNTIME_MRW_FOR_LP5
				DramcModeRegWriteByRank_RTMRW(p, &u1Rank, &u1MRIdx, &u1Value, 1);
			#else
				DramcModeRegWriteByRank_RTSWCMD_MRW(p, u1Rank, u1MRIdx, u1Value);
			#endif
			}
			else
		#endif
			{
				DramcModeRegWriteByRank_SCSM(p, u1Rank, u1MRIdx, u1Value);
			}
		}

	#if MRW_CHECK_ONLY
	u1PrintModeRegWrite = 1;
	U8 u1Backup_Rank;
	U8 u1RankIdx, u1RankNum, u1RankStart;
	U8 u1FSPMRIdx;

	u1Backup_Rank=p->rank;

	if (u1Rank==3)
	{
		u1RankNum = 2;
		u1RankStart = 0;
	}
	else
	{
		u1RankNum = 1;
		u1RankStart = u1Rank;
	}

	#if (__LP5_COMBO__ == TRUE)
	if (is_lp5_family(p))
		u1FSPMRIdx=16;
	else
	#endif
		u1FSPMRIdx=13;

	for (u1RankIdx=u1RankStart;u1RankIdx<u1RankStart+u1RankNum;u1RankIdx++)
	{
		vSetRank(p, u1RankIdx);
		if (u1MRIdx==u1FSPMRIdx)
		{
			u2MRRecord[p->channel][u1RankIdx][FSP_0][u1FSPMRIdx] =u1Value;
			u2MRRecord[p->channel][u1RankIdx][FSP_1][u1FSPMRIdx] =u1Value;
		}
		else
			u2MRRecord[p->channel][u1RankIdx][gFSPWR_Flag[u1RankIdx]][u1MRIdx] = u1Value;

		if(u1PrintModeRegWrite)
		{
			#if VENDER_JV_LOG
			mcSHOW_JV_LOG_MSG(("Write Rank%d MR%d =0x%x\n", u1RankIdx, u1MRIdx, u1Value));
			#endif
			#if MRW_CHECK_ONLY
			mcSHOW_MRW_MSG(("MRW CH%d Rank%d FSP%d MR%d =0x%x\n", p->channel, u1RankIdx, gFSPWR_Flag[u1RankIdx], u1MRIdx, u1Value));
			#endif
			mcSHOW_DBG_MSG2(("Write Rank%d MR%d =0x%x\n", u1RankIdx, u1MRIdx, u1Value));
			mcDUMP_REG_MSG(("Write Rank%d MR%d =0x%x\n", u1RankIdx, u1MRIdx, u1Value));
		}
		#if MRW_BACKUP
		U8 MR_backup;

			MR_backup=DramcMRWriteBackup(p, u1MRIdx, u1RankIdx);
			if (MR_backup!=0xff)
			mcSHOW_MRW_MSG(("  [MRW Check] Rank%d FSP%d Backup_MR%d= 0x%x MR%d= 0x%x ==>%s\n", u1RankIdx, gFSPWR_Flag[u1RankIdx], u1MRIdx, MR_backup, u1MRIdx, u1Value, (u1Value==MR_backup?"PASS":"FAIL")));
		#endif

		#if (__LP5_COMBO__ == TRUE)
			if (is_lp5_family(p))
			{
				if (u1MRIdx==u1FSPMRIdx)
					gFSPWR_Flag[u1RankIdx] = u1Value & 0x3;
			}
			else
		#endif
			{
				if (u1MRIdx==u1FSPMRIdx)
					gFSPWR_Flag[u1RankIdx] = (u1Value>> 6) & 0x1;
			}
	}
	vSetRank(p, u1Backup_Rank);
	#endif
}

#ifdef __ETT__
static U8 u1gpRegBackup;
#endif
U32 u4gpRegBackupVlaue[100];
void DramcBackupRegisters(DRAMC_CTX_T *p, U32 *backup_addr, U32 backup_num)
{
	U32 u4RegIdx;

#ifdef __ETT__
	if (backup_num > 100 || u1gpRegBackup)
	{
		mcSHOW_ERR_MSG(("[DramcBackupRegisters] backup number over 64!!!\n"));
		while (1);
	}

	u1gpRegBackup++;
#endif

	for (u4RegIdx = 0; u4RegIdx < backup_num; u4RegIdx++)
	{
		u4gpRegBackupVlaue[u4RegIdx] = u4IO32Read4B(backup_addr[u4RegIdx]);
		//mcSHOW_DBG_MSG(("Backup Reg(0x%X) = 0x%X\n", backup_addr[u4RegIdx], u4gpRegBackupVlaue[u4RegIdx]));
	}
}

void DramcRestoreRegisters(DRAMC_CTX_T *p, U32 *restore_addr, U32 restore_num)
{
	U32 u4RegIdx;

#ifdef __ETT__
	if (u1gpRegBackup == 0)
	{
		mcSHOW_ERR_MSG(("[DramcRestoreRegisters] Need to call backup first\n"));
	}

	u1gpRegBackup--;
#endif

	for (u4RegIdx = 0; u4RegIdx < restore_num; u4RegIdx++)
	{
		vIO32Write4B(restore_addr[u4RegIdx], u4gpRegBackupVlaue[u4RegIdx]);
		//mcSHOW_DBG_MSG(("Restore Reg(0x%X) = 0x%X\n", restore_addr[u4RegIdx], u4gpRegBackupVlaue[u4RegIdx]));
	}
}

#if 0
//#if defined(DDR_INIT_TIME_PROFILING) || (__ETT__ && SUPPORT_SAVE_TIME_FOR_CALIBRATION)
void DramcConfInfraReset(DRAMC_CTX_T *p)
{
#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
		vIO32WriteFldMulti_All(DDRPHY_REG_MISC_CLK_CTRL, P_Fld(0, MISC_CLK_CTRL_DVFS_CLK_MEM_SEL)
													| P_Fld(0, MISC_CLK_CTRL_DVFS_MEM_CK_MUX_UPDATE_EN));

		vIO32WriteFldMulti_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL0), P_Fld(0, MISC_CG_CTRL0_CLK_MEM_SEL)
																| P_Fld(1, MISC_CG_CTRL0_W_CHG_MEM));
		mcDELAY_XNS(100);//reserve 100ns period for clock mute and latch the rising edge sync condition for BCLK
		vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_REG_MISC_CG_CTRL0), 0, MISC_CG_CTRL0_W_CHG_MEM);

#if (fcFOR_CHIP_ID == fcLafite)
		// 26M
		vIO32WriteFldMulti_All(DDRPHY_CKMUX_SEL, P_Fld(0x1, CKMUX_SEL_R_PHYCTRLMUX)  //move CKMUX_SEL_R_PHYCTRLMUX to here (it was originally between MISC_CG_CTRL0_CLK_MEM_SEL and MISC_CTRL0_R_DMRDSEL_DIV2_OPT)
									| P_Fld(0x1, CKMUX_SEL_R_PHYCTRLDCM)); // PHYCTRLDCM 1: follow DDRPHY_conf DCM settings, 0: follow infra DCM settings
		vIO32WriteFldMulti_All(DDRPHY_MISC_CG_CTRL0, P_Fld(0, MISC_CG_CTRL0_W_CHG_MEM)
					| P_Fld(0, MISC_CG_CTRL0_CLK_MEM_SEL));//[5:4] mem_ck mux: 2'b00: 26MHz, [0]: change memory clock
		vIO32WriteFldAlign_All(DDRPHY_MISC_CG_CTRL0, 1, MISC_CG_CTRL0_W_CHG_MEM);//change clock freq
		mcDELAY_US(1);
		vIO32WriteFldAlign_All(DDRPHY_MISC_CG_CTRL0, 0, MISC_CG_CTRL0_W_CHG_MEM);//disable memory clock change

		// dramc conf reset
		//mcSHOW_TIME_MSG(("Before infra reset, 0x10001148:%x\n", *(volatile unsigned *)(0x10001148)));
		*(volatile unsigned *)(0x10001140) = (0x1 << 15);
		//mcSHOW_TIME_MSG(("After infra reset, 0x10001148:%x\n",  *(volatile unsigned *)(0x10001148)));
		__asm__ __volatile__ ("dsb" : : : "memory");
		mcDELAY_US(200);
		//mcSHOW_TIME_MSG(("Before infra clear, 0x10001148:%x\n",  *(volatile unsigned *)(0x10001148)));
		*(volatile unsigned *)(0x10001144) = (0x1 << 15);
		//mcSHOW_TIME_MSG(("After infra clear, 0x10001148:%x\n",  *(volatile unsigned *)(0x10001148)));

		#if 0
		mcDELAY_US(200);
		*(volatile unsigned *)(0x10007018) = 0x88000040;
		mcDELAY_US(200);
		*(volatile unsigned *)(0x10007018) = 0x88000000;
		mcDELAY_US(200);
		#endif

		//DDRPHY Reset
		vIO32WriteFldAlign_All(DDRPHY_B0_DQ3, 0x0, B0_DQ3_RG_ARDQ_RESETB_B0);
		vIO32WriteFldAlign_All(DDRPHY_B0_DLL_ARPI0, 0x0, B0_DLL_ARPI0_RG_ARPI_RESETB_B0);
		vIO32WriteFldAlign_All(DDRPHY_B1_DQ3, 0x0, B1_DQ3_RG_ARDQ_RESETB_B1);
		vIO32WriteFldAlign_All(DDRPHY_B1_DLL_ARPI0, 0x0, B1_DLL_ARPI0_RG_ARPI_RESETB_B1);
		vIO32WriteFldAlign_All(DDRPHY_CA_CMD3, 0x0, CA_CMD3_RG_ARCMD_RESETB);
		vIO32WriteFldAlign_All(DDRPHY_CA_DLL_ARPI0, 0x0, CA_DLL_ARPI0_RG_ARPI_RESETB_CA);
		vIO32WriteFldAlign(DDRPHY_PLL4, 0x0, PLL4_RG_RPHYPLL_RESETB);//Since there is only 1 PLL, only control CHA
		mcDELAY_US(200);
		vIO32WriteFldAlign_All(DDRPHY_B0_DQ3, 0x1, B0_DQ3_RG_ARDQ_RESETB_B0);
		vIO32WriteFldAlign_All(DDRPHY_B0_DLL_ARPI0, 0x1, B0_DLL_ARPI0_RG_ARPI_RESETB_B0);
		vIO32WriteFldAlign_All(DDRPHY_B1_DQ3, 0x1, B1_DQ3_RG_ARDQ_RESETB_B1);
		vIO32WriteFldAlign_All(DDRPHY_B1_DLL_ARPI0, 0x1, B1_DLL_ARPI0_RG_ARPI_RESETB_B1);
		vIO32WriteFldAlign_All(DDRPHY_CA_CMD3, 0x1, CA_CMD3_RG_ARCMD_RESETB);
		vIO32WriteFldAlign_All(DDRPHY_CA_DLL_ARPI0, 0x1, CA_DLL_ARPI0_RG_ARPI_RESETB_CA);
		vIO32WriteFldAlign(DDRPHY_PLL4, 0x1, PLL4_RG_RPHYPLL_RESETB);//Since there is only 1 PLL, only control CHA

		//Disable SPM control
		vIO32WriteFldMulti(SPM_POWERON_CONFIG_EN, P_Fld(0xB16, POWERON_CONFIG_EN_PROJECT_CODE) | P_Fld(0, POWERON_CONFIG_EN_BCLK_CG_EN));

		//For FMeter after dcm enable
		vIO32WriteFldAlign_All(DDRPHY_MISC_CG_CTRL2, 0x0, MISC_CG_CTRL2_RG_MEM_DCM_DCM_EN);
		vIO32WriteFldAlign_All(DDRPHY_MISC_CG_CTRL2, 0x1, MISC_CG_CTRL2_RG_MEM_DCM_FORCE_ON);
#endif
#endif
}
#endif

#define PATTERN1 0x5A5A5A5A
#define PATTERN2 0xA5A5A5A5

#if defined(DDR_INIT_TIME_PROFILING) || ENABLE_APB_MASK_WRITE
U32 l_low_tick0, l_high_tick0, l_low_tick1, l_high_tick1;
void TimeProfileBegin(void)
{
#if __ETT__
	l_low_tick0 = GPT_GetTickCount(&l_high_tick0);
#else
	l_low_tick0 = get_timer(0);
#endif
}

U32 TimeProfileEnd(void)
{
#if __ETT__
	l_low_tick1 = GPT_GetTickCount(&l_high_tick1);

	//mcSHOW_TIME_MSG(("Time0 %u %u\n", l_high_tick0, l_low_tick0));
	//mcSHOW_TIME_MSG(("Time1 %u %u\n", l_high_tick1, l_low_tick1));
	return ((l_low_tick1 - l_low_tick0) * 76) / 1000;
#else
	l_low_tick1 = get_timer(l_low_tick0);
	return l_low_tick1 * 1000;
#endif
}
#endif

#if QT_GUI_Tool
void TA2_Test_Run_Time_SW_Presetting(DRAMC_CTX_T *p, U32 test2_1, U32 test2_2, U8 u1TestPat, U8 u1LoopCnt)
{
		u1TestPat = u1TestPat & 0x7f;

		DramcSetRankEngine2(p, p->rank);

		uiReg0D0h=u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_DUMMY_RD));
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_DUMMY_RD), P_Fld(0, DUMMY_RD_DQSG_DMYRD_EN) | P_Fld(0, DUMMY_RD_DQSG_DMYWR_EN) | P_Fld(0, DUMMY_RD_DUMMY_RD_EN) | P_Fld(0, DUMMY_RD_SREF_DMYRD_EN) | P_Fld(0, DUMMY_RD_DMY_RD_DBG) | P_Fld(0, DUMMY_RD_DMY_WR_DBG));  //must close dummy read when do test agent

		//vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TESTCHIP_DMA1), 0, TESTCHIP_DMA1_DMA_LP4MATAB_OPT);//Eddie
		// disable self test engine1 and self test engine2
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A3), P_Fld(0, TEST2_A3_TEST2W) | P_Fld(0, TEST2_A3_TEST2R) | P_Fld(0, TEST2_A3_TEST1));

		// 1.set pattern ,base address ,offset address
		// 2.select  ISI pattern or audio pattern or xtalk pattern
		// 3.set loop number
		// 4.enable read or write
		// 5.loop to check DM_CMP_CPT
		// 6.return CMP_ERR
		// currently only implement ucengine_status = 1, others are left for future extension

		// 1
		vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A0), P_Fld(test2_1>>24,TEST2_A0_TEST2_PAT0)|P_Fld(test2_2>>24,TEST2_A0_TEST2_PAT1));

	#if (FOR_DV_SIMULATION_USED==1 || SW_CHANGE_FOR_SIMULATION==1)
		//DV sim memory 0~0x100 has values, can't used
		//vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A1), (test2_1+0x100) & 0x00ffffff, TEST2_A1_TEST2_BASE);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RK_TEST2_A1),  0x10000, RK_TEST2_A1_TEST2_BASE); //LPDDR4 Setting
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RK_TEST2_A1),  0x0, RK_TEST2_A1_TEST2_BASE); //Eddie Change to 0 for LP5
	#else
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_RK_TEST2_A1), 0, RK_TEST2_A1_TEST2_BASE);
	#endif
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TEST2_A2), 0x2, TEST2_A2_TEST2_OFF);//Eddie

	return;
}
#endif
