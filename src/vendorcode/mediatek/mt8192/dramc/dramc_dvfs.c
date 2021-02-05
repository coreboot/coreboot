/* SPDX-License-Identifier: BSD-3-Clause */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "dramc_common.h"
#include "dramc_int_global.h"
#include "x_hal_io.h"


#define SHUFFLE_GROUP	4				//SHU1~4

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

U8 _MappingFreqArray[DRAM_DFS_SRAM_MAX] = {0};

U8 gDVFSCtrlSel = 0;
U32 gu4Ddrphy0SPMCtrl0;
U32 gu4Ddrphy1SPMCtrl0;
U32 gu4Ddrphy0SPMCtrl2;
U32 gu4Ddrphy1SPMCtrl2;


void vSetDFSTable(DRAMC_CTX_T *p, DRAM_DFS_FREQUENCY_TABLE_T *pFreqTable)
{
	p->pDFSTable = pFreqTable;
}

void vSetDFSFreqSelByTable(DRAMC_CTX_T *p, DRAM_DFS_FREQUENCY_TABLE_T *pFreqTable)
{
	vSetDFSTable(p, pFreqTable);
	DDRPhyFreqSel(p, p->pDFSTable->freq_sel);
#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
	vSetVcoreByFreq(p);
#endif
}

void vInitMappingFreqArray(DRAMC_CTX_T *p)
{
	U8 u1ShuffleIdx = 0;
	DRAM_DFS_FREQUENCY_TABLE_T *pFreqTbl;

	pFreqTbl = gFreqTbl;

	for (u1ShuffleIdx = DRAM_DFS_SHUFFLE_1; u1ShuffleIdx < DRAM_DFS_SHUFFLE_MAX; u1ShuffleIdx++)
	{
		_MappingFreqArray[pFreqTbl[u1ShuffleIdx].shuffleIdx] = pFreqTbl[u1ShuffleIdx].shuffleIdx;
	}
}

DRAM_DFS_FREQUENCY_TABLE_T* get_FreqTbl_by_shuffleIndex(DRAMC_CTX_T *p, U8 index)
{
	U8 u1ShuffleIdx = 0;
	DRAM_DFS_FREQUENCY_TABLE_T *pFreqTbl;

	pFreqTbl = gFreqTbl;

	for (u1ShuffleIdx = DRAM_DFS_SHUFFLE_1; u1ShuffleIdx < DRAM_DFS_SHUFFLE_MAX; u1ShuffleIdx++)
	{
		if (pFreqTbl[u1ShuffleIdx].shuffleIdx == index)
		{
			return & pFreqTbl[u1ShuffleIdx];
		}
	}
	return NULL;
}

U8 get_shuffleIndex_by_Freq(DRAMC_CTX_T *p)
{
	U8 u1ShuffleIdx = 0;
	DRAM_DFS_FREQUENCY_TABLE_T *pFreqTbl;
	DRAM_DFS_SRAM_SHU_T eCurr_shu_level = vGet_Current_ShuLevel(p);

	pFreqTbl = gFreqTbl;

	for (u1ShuffleIdx = DRAM_DFS_SHUFFLE_1; u1ShuffleIdx < DRAM_DFS_SHUFFLE_MAX; u1ShuffleIdx++)
	{
		if (pFreqTbl[u1ShuffleIdx].shuffleIdx == eCurr_shu_level)
		{
			return _MappingFreqArray[pFreqTbl[u1ShuffleIdx].shuffleIdx];
		}
	}

	return 0;
}

#if 0 //@Darren, debug codes
void DramcWriteShuffleSRAMRange(DRAMC_CTX_T *p, U32 u4StartAddr, U32 u4EndAddr, u32 u4Data)
{
	U32 ii, u4tmp, u4Offset=0;

	for (ii = u4StartAddr; ii <= u4EndAddr; ii += 4)
	{
		vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0+u4Offset+SHU_GRP_DRAMC_OFFSET), u4Data);
		u4Offset += 4;
		mcDELAY_MS(1);
	}
}

void DdrphyWriteShuffleSRAMRange(DRAMC_CTX_T *p, U32 u4StartAddr, U32 u4EndAddr, u32 u4Data)
{
	U32 ii, u4tmp, u4Offset=0;

	for (ii = u4StartAddr; ii <= u4EndAddr; ii += 4)
	{
		vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_PHYPLL0+u4Offset+SHU_GRP_DDRPHY_OFFSET), u4Data);
		u4Offset += 4;
		mcDELAY_MS(1);
	}
}

void FullRGDump(DRAMC_CTX_T *p, U8 step, U32 u4ShuOffset)
{
	U8 u1RankIdx=0;

	mcSHOW_DBG_MSG(("[FullRGDump] STEP%d\n", step));
	//Darren-DumpAoNonShuReg();
	for (u1RankIdx=RANK_0; u1RankIdx<p->support_rank_num; u1RankIdx++)
	{
		vSetRank(p, u1RankIdx);
		mcSHOW_DBG_MSG(("[FullRGDump] RANK%d\n", u1RankIdx));
		DumpAoShuReg(u4ShuOffset, u4ShuOffset);
	}
	//Darren-DumpNaoReg();
}

U32 SramDebugModeRead(DRAMC_CTX_T *p, U8 sram_shu_level, U32 u4Reg)
{
	U32 u4Value=0;
	U32 u4RegBackupAddress[] =
	{
		(DDRPHY_REG_MISC_SRAM_DMA0),
		(DDRPHY_REG_MISC_SRAM_DMA0 + SHIFT_TO_CHB_ADDR),
		(DDRPHY_REG_MISC_SRAM_DMA1),
		(DDRPHY_REG_MISC_SRAM_DMA1 + SHIFT_TO_CHB_ADDR),
#if (CHANNEL_NUM==4)
		(DDRPHY_REG_MISC_SRAM_DMA0 + SHIFT_TO_CHC_ADDR),
		(DDRPHY_REG_MISC_SRAM_DMA0 + SHIFT_TO_CHD_ADDR),
		(DDRPHY_REG_MISC_SRAM_DMA1 + SHIFT_TO_CHC_ADDR),
		(DDRPHY_REG_MISC_SRAM_DMA1 + SHIFT_TO_CHD_ADDR),
#endif
	};

	//Backup regs
	DramcBackupRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));

	//vIO32Write4B(DRAMC_REG_ADDR(u4Reg), u4Data); // SHU1
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA0, 0x0, MISC_SRAM_DMA0_APB_SLV_SEL);
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA1, 0x1, MISC_SRAM_DMA1_R_APB_DMA_DBG_ACCESS);
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA1, sram_shu_level, MISC_SRAM_DMA1_R_APB_DMA_DBG_LEVEL); // SHU8
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA0, 0x1, MISC_SRAM_DMA0_APB_SLV_SEL);

	p->ShuRGAccessIdx = DRAM_DFS_REG_SHU1;
	u4Value = u4IO32Read4B(DRAMC_REG_ADDR(u4Reg));// SHU1
	DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));
	p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
	mcSHOW_DBG_MSG(("[SramDebugModeRead] RK%d Reg=0x%x, Value=0x%x\n", p->rank, u4Reg, u4Value));

	//vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA0, 0x0, MISC_SRAM_DMA0_APB_SLV_SEL);
	//vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA1, 0x0, MISC_SRAM_DMA1_R_APB_DMA_DBG_ACCESS);

	return u4Value;
}

void SramDebugModeWrite(DRAMC_CTX_T *p, U8 sram_shu_level, U32 u4Reg, U32 u4Data)
{
	U32 u4RegBackupAddress[] =
	{
		(DDRPHY_REG_MISC_SRAM_DMA0),
		(DDRPHY_REG_MISC_SRAM_DMA0 + SHIFT_TO_CHB_ADDR),
		(DDRPHY_REG_MISC_SRAM_DMA1),
		(DDRPHY_REG_MISC_SRAM_DMA1 + SHIFT_TO_CHB_ADDR),
#if (CHANNEL_NUM==4)
		(DDRPHY_REG_MISC_SRAM_DMA0 + SHIFT_TO_CHC_ADDR),
		(DDRPHY_REG_MISC_SRAM_DMA0 + SHIFT_TO_CHD_ADDR),
		(DDRPHY_REG_MISC_SRAM_DMA1 + SHIFT_TO_CHC_ADDR),
		(DDRPHY_REG_MISC_SRAM_DMA1 + SHIFT_TO_CHD_ADDR),
#endif
	};

	//Backup regs
	DramcBackupRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));

	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA0, 0x0, MISC_SRAM_DMA0_APB_SLV_SEL);
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA1, 0x1, MISC_SRAM_DMA1_R_APB_DMA_DBG_ACCESS);
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA1, sram_shu_level, MISC_SRAM_DMA1_R_APB_DMA_DBG_LEVEL); // SHU8
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA0, 0x1, MISC_SRAM_DMA0_APB_SLV_SEL);

	p->ShuRGAccessIdx = DRAM_DFS_REG_SHU1;
	vIO32Write4B(DRAMC_REG_ADDR(u4Reg), u4Data); // SHU1
	DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));
	p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
	mcSHOW_DBG_MSG(("[SramDebugModeWrite] RK%d Reg=0x%x, Value=0x%x\n", p->rank, u4Reg, u4Data));

	//vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA0, 0x0, MISC_SRAM_DMA0_APB_SLV_SEL);
	//vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA1, 0x0, MISC_SRAM_DMA1_R_APB_DMA_DBG_ACCESS);
}

void DramcCopyShu0toShu1(DRAMC_CTX_T *p, U32 u4StartAddr, U32 u4EndAddr)
{
	U32 ii, u4tmp, u4Offset=0;
	DRAM_DFS_REG_SHU_T ShuRGAccessIdxBackup = p->ShuRGAccessIdx; // SHU1 need use p->ShuRGAccessIdx=DRAM_DFS_REG_SHU1 for RK1

	for (ii = u4StartAddr; ii <= u4EndAddr; ii += 4)
	{
		u4tmp = u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0+u4Offset));
		mcDELAY_MS(1);
		p->ShuRGAccessIdx = DRAM_DFS_REG_SHU1;
		vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SHURK_SELPH_DQ0+u4Offset), u4tmp);
		p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
		u4Offset += 4;
		mcDELAY_MS(1);
	}

	p->ShuRGAccessIdx = ShuRGAccessIdxBackup;
}

void DdrphyCopyShu0toShu1(DRAMC_CTX_T *p, U32 u4StartAddr, U32 u4EndAddr)
{
	U32 ii, u4tmp, u4Offset=0;
	DRAM_DFS_REG_SHU_T ShuRGAccessIdxBackup = p->ShuRGAccessIdx; // SHU1 need use p->ShuRGAccessIdx=DRAM_DFS_REG_SHU1 for RK1

	for (ii = u4StartAddr; ii <= u4EndAddr; ii += 4)
	{
		u4tmp = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_PHYPLL0+u4Offset));
		mcDELAY_MS(1);
		p->ShuRGAccessIdx = DRAM_DFS_REG_SHU1;
		vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_PHYPLL0+u4Offset), u4tmp);
		p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
		u4Offset += 4;
		mcDELAY_MS(1);
	}

	p->ShuRGAccessIdx = ShuRGAccessIdxBackup;
}
#endif

#if ENABLE_LP4Y_WA
void CmdBusTrainingLP4YWA(DRAMC_CTX_T *p, U8 u1OnOff)
{
	U8 u1MR51 = 0;

	if (p->frequency > 800) // skip DDR1600 up
		return;

	if (u1OnOff == DISABLE)
		u1MR51Value[p->dram_fsp] = u1MR51Value[p->dram_fsp] & ~(1 << 3); // disable CLK SE mode
	else
		u1MR51Value[p->dram_fsp] = u1MR51Value[p->dram_fsp] | (1 << 3); // enable CLK SE mode

	DramcModeRegWriteByRank(p, p->rank, 51, u1MR51Value[p->dram_fsp]);

	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD13), P_Fld( u1OnOff , SHU_CA_CMD13_RG_TX_ARCLKB_OE_TIE_SEL_CA ) \
															  | P_Fld( u1OnOff		 , SHU_CA_CMD13_RG_TX_ARCLKB_OE_TIE_EN_CA  ));
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD7) , P_Fld( u1OnOff , SHU_CA_CMD7_R_LP4Y_SDN_MODE_CLK		 ));
}
#endif

#if ENABLE_DFS_RUNTIME_MRW
void DFSRuntimeFspMRW(DRAMC_CTX_T *p)
{
	vIO32WriteFldAlign_All(DRAMC_REG_SA_RESERVE, p->dram_fsp, SA_RESERVE_DFS_FSP_RTMRW);
}

void DFSRuntimeMRW_preset(DRAMC_CTX_T *p, U8 sram_shu_level)
{
	U8 u1ChIdx = 0, u1RankIdx = 0;
	U8 u1MR03_Value = 0;
	U8 ch_start = 0, ch_end = 0;

		ch_start = CHANNEL_A;
		ch_end = CHANNEL_B;
#if (CHANNEL_NUM > 2)
		ch_end = CHANNEL_D;
#endif

	//Darren-mcSHOW_DBG_MSG(("[DFSRuntimeMRW_preset] FSP%d\n", p->dram_fsp));
	//! save mr13
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_15_0, u1MR13Value[RANK_0], LPIF_MR_OP_STORE_SHU_15_0_MR_OP_SET_SHU_15_0);
	//Darren-mcSHOW_DBG_MSG(("\tMR13 = 0x%x\n", u1MR13Value[RANK_0]));

#if ENABLE_READ_DBI
	u1MR03_Value = ((u1MR03Value[p->dram_fsp] & 0xbf) | (p->DBI_R_onoff[p->dram_fsp] << 6));
#endif

#if ENABLE_WRITE_DBI
	u1MR03_Value = ((u1MR03Value[p->dram_fsp] & 0x7F) | (p->DBI_W_onoff[p->dram_fsp] << 7));
#endif

	//! save shux mr1/mr2/mr3/mr11
	vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_0 + (sram_shu_level << 4),
		P_Fld(u1MR01Value[p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_0) |
		P_Fld(u1MR02Value[p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_1) |
		P_Fld(u1MR03_Value, LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_2) |
		P_Fld(u1MR11Value[p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_3));
		//Darren-mcSHOW_DBG_MSG(("\tMR01 = 0x%x, MR02 = 0x%x, MR03 = 0x%x, MR1 = 0x%x\n", u1MR01Value[p->dram_fsp], u1MR02Value[p->dram_fsp], u1MR03Value[p->dram_fsp], u1MR11Value[p->dram_fsp]));

	//! save shux mr22/mr51
	vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_1 + (sram_shu_level << 4),
		P_Fld(u1MR21Value[p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_1_MR_OP_SET_SHU_0_5) |
		P_Fld(u1MR22Value[p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_1_MR_OP_SET_SHU_0_6) |
		P_Fld(u1MR51Value[p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_1_MR_OP_SET_SHU_0_7));
		//Darren-mcSHOW_DBG_MSG(("\tMR22 = 0x%x, MR51 = 0x%x\n", u1MR22Value[p->dram_fsp], u1MR51Value[p->dram_fsp]));

	//! save shux mr12/mr14
	vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_7_0 + (sram_shu_level << 4),
		P_Fld(u1MR12Value[ch_start][RANK_0][p->dram_fsp], LPIF_MR_OP_STORE_SHU_7_0_MR_OP_SET_SHU_7_0) |
		P_Fld(u1MR12Value[ch_start][RANK_1][p->dram_fsp], LPIF_MR_OP_STORE_SHU_7_0_MR_OP_SET_SHU_7_1) |
		P_Fld(u1MR12Value[ch_end][RANK_0][p->dram_fsp], LPIF_MR_OP_STORE_SHU_7_0_MR_OP_SET_SHU_7_2) |
		P_Fld(u1MR12Value[ch_end][RANK_1][p->dram_fsp], LPIF_MR_OP_STORE_SHU_7_0_MR_OP_SET_SHU_7_3));

	vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_7_1 + (sram_shu_level << 4),
		P_Fld(u1MR14Value[ch_start][RANK_0][p->dram_fsp], LPIF_MR_OP_STORE_SHU_7_1_MR_OP_SET_SHU_7_4) |
		P_Fld(u1MR14Value[ch_start][RANK_1][p->dram_fsp], LPIF_MR_OP_STORE_SHU_7_1_MR_OP_SET_SHU_7_5) |
		P_Fld(u1MR14Value[ch_end][RANK_0][p->dram_fsp], LPIF_MR_OP_STORE_SHU_7_1_MR_OP_SET_SHU_7_6) |
		P_Fld(u1MR14Value[ch_end][RANK_1][p->dram_fsp], LPIF_MR_OP_STORE_SHU_7_1_MR_OP_SET_SHU_7_7));

#if 0
	for (u1ChIdx = CHANNEL_A; u1ChIdx < p->support_channel_num; u1ChIdx++)
	{
		for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
		{
			mcSHOW_DBG_MSG(("\tCH%d, RK%d, MR12 = 0x%x, MR14 = 0x%x\n", u1ChIdx, u1RankIdx,u1MR12Value[u1ChIdx][u1RankIdx][p->dram_fsp], u1MR14Value[u1ChIdx][u1RankIdx][p->dram_fsp]));
		}
	}
#endif
}

static void TriggerRTMRW_SingleChannel(DRAMC_CTX_T *p, U8 rtmrw_rank_sel, U8 u1MR1, U8 u1MR2, U8 u1MR3, U8 u1MR11, U8 u1MR12, U8 u1MR13, U8 u1MR14, U8 u1MR21, U8 u1MR22, U8 u1MR51)
{
	U8 rt_response_ack = 1, rt_ack = 0;
	U8 u1MRW_1ST_Num = 0x5; // MR13, MR1, MR2, MR3, MR11, MR12
	U8 u1MRW_2ND_Num = 0x2; // MR14, 22, 51

#if ENABLE_LP4Y_DFS
	u1MRW_2ND_Num++; // for LP4Y MR21
#endif

#if 1
	//! MR13, MR1, MR2, MR3, MR11, MR12
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RTMRW_CTRL0),
		P_Fld(rtmrw_rank_sel, RTMRW_CTRL0_RTMRW0_RK) |
		P_Fld(rtmrw_rank_sel, RTMRW_CTRL0_RTMRW1_RK) |
		P_Fld(rtmrw_rank_sel, RTMRW_CTRL0_RTMRW2_RK) |
		P_Fld(rtmrw_rank_sel, RTMRW_CTRL0_RTMRW3_RK) |
		P_Fld(rtmrw_rank_sel, RTMRW_CTRL0_RTMRW4_RK) |
		P_Fld(rtmrw_rank_sel, RTMRW_CTRL0_RTMRW5_RK) |
		P_Fld(u1MRW_1ST_Num, RTMRW_CTRL0_RTMRW_LEN) |
		P_Fld(0x0, RTMRW_CTRL0_RTMRW_AGE) |
		P_Fld(0x3, RTMRW_CTRL0_RTMRW_LAT));

	//! MA = 13, 1, 2, 3
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RTMRW_CTRL1),
		P_Fld(13, RTMRW_CTRL1_RTMRW0_MA) |
		P_Fld(1, RTMRW_CTRL1_RTMRW1_MA) |
		P_Fld(2, RTMRW_CTRL1_RTMRW2_MA) |
		P_Fld(3, RTMRW_CTRL1_RTMRW3_MA));

	//! OP13, OP1, OP2, OP3
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RTMRW_CTRL2),
		P_Fld(u1MR13, RTMRW_CTRL2_RTMRW0_OP) |
		P_Fld(u1MR1, RTMRW_CTRL2_RTMRW1_OP) |
		P_Fld(u1MR2, RTMRW_CTRL2_RTMRW2_OP) |
		P_Fld(u1MR3, RTMRW_CTRL2_RTMRW3_OP));

	//! MR11/MR12
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RTMRW_CTRL3),
		P_Fld(11, RTMRW_CTRL3_RTMRW4_MA) |
		P_Fld(12, RTMRW_CTRL3_RTMRW5_MA) |
		P_Fld(u1MR11, RTMRW_CTRL3_RTMRW4_OP) |
		P_Fld(u1MR12, RTMRW_CTRL3_RTMRW5_OP));

	//!runtime MRW trigger
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0x1, SWCMD_EN_RTMRWEN);

	do {
		rt_ack = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), SPCMDRESP_RTMRW_RESPONSE);
	} while(rt_response_ack != rt_ack);

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0x0, SWCMD_EN_RTMRWEN);
#endif

#if 1
   //! MR14/22/51
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RTMRW_CTRL0),
		P_Fld(rtmrw_rank_sel, RTMRW_CTRL0_RTMRW0_RK) |
		P_Fld(rtmrw_rank_sel, RTMRW_CTRL0_RTMRW1_RK) |
		P_Fld(rtmrw_rank_sel, RTMRW_CTRL0_RTMRW2_RK) |
#if ENABLE_LP4Y_DFS
		P_Fld(rtmrw_rank_sel, RTMRW_CTRL0_RTMRW3_RK) |
#endif
		P_Fld(u1MRW_2ND_Num, RTMRW_CTRL0_RTMRW_LEN) |
		P_Fld(0x0, RTMRW_CTRL0_RTMRW_AGE) |
		P_Fld(0x3, RTMRW_CTRL0_RTMRW_LAT));

	//! MA = 14, 22, 51
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RTMRW_CTRL1),
		P_Fld(14, RTMRW_CTRL1_RTMRW0_MA) |
#if ENABLE_LP4Y_DFS
		P_Fld(21, RTMRW_CTRL1_RTMRW3_MA) |
#endif
		P_Fld(22, RTMRW_CTRL1_RTMRW1_MA) |
		P_Fld(51, RTMRW_CTRL1_RTMRW2_MA));

	//! OP14, OP22, OP51
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RTMRW_CTRL2),
		P_Fld(u1MR14, RTMRW_CTRL2_RTMRW0_OP) |
#if ENABLE_LP4Y_DFS
		P_Fld(u1MR21, RTMRW_CTRL2_RTMRW3_OP) |
#endif
		P_Fld(u1MR22, RTMRW_CTRL2_RTMRW1_OP) |
		P_Fld(u1MR51, RTMRW_CTRL2_RTMRW2_OP));

	//!runtime MRW trigger
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0x1, SWCMD_EN_RTMRWEN);

	do {
		rt_ack = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), SPCMDRESP_RTMRW_RESPONSE);
	} while(rt_response_ack != rt_ack);

	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0x0, SWCMD_EN_RTMRWEN);
#endif
}

static void DFSRTMRW_HwsetWA(DRAMC_CTX_T *p, U8 cur_shu_mux_index, U8 pingpong_shu_level, U8 pingpong_fsp)
{
	U8 u1MR13_OP = 0, u1VRCG_OP = 0;
	U8 ch_start = 0, ch_end = 0, u1ChIdx = 0;
	U8 ch_bak = vGetPHY2ChannelMapping(p);

		ch_start = CHANNEL_A;
		ch_end = CHANNEL_B+1;
#if (CHANNEL_NUM > 2)
		ch_end = CHANNEL_D+1;
#endif

	for (u1ChIdx = ch_start; u1ChIdx < ch_end; u1ChIdx++)
	{
		vSetPHY2ChannelMapping(p, u1ChIdx);
		p->ShuRGAccessIdx = cur_shu_mux_index; // Currect
		u1MR13_OP = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_MR13), SHU_HWSET_MR13_HWSET_MR13_OP);
		p->ShuRGAccessIdx = pingpong_shu_level; // Next
		u1VRCG_OP = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_VRCG), SHU_HWSET_VRCG_HWSET_VRCG_OP);
		p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;

		if(pingpong_fsp == FSP_1)
		{
			if (cur_shu_mux_index == PHYPLL_MODE)
			{
				u1MR13_OP &= 0x3F; //! MR13 OP7 = 0, OP6 = 0, from PHYPLL to CLRPLL
				u1VRCG_OP &= 0x3F; //! MR13 OP7 = 0, OP6 = 0, from PHYPLL to CLRPLL
			}
			else
			{
				u1MR13_OP |= 0xC0; //! MR13 OP7 = 1, OP6 = 1, from CLRPLL to PHYPLL
				u1VRCG_OP |= 0xC0; //! MR13 OP7 = 1, OP6 = 1, from CLRPLL to PHYPLL
			}
		}
		else
		{
			if (cur_shu_mux_index == PHYPLL_MODE)
			{
				u1MR13_OP |= 0xC0; //! MR13 OP7 = 1, OP6 = 1, from CLRPLL to PHYPLL
				u1VRCG_OP |= 0xC0; //! MR13 OP7 = 1, OP6 = 1, from CLRPLL to PHYPLL
			}
			else
			{
				u1MR13_OP &= 0x3F; //! MR13 OP7 = 0, OP6 = 0, from PHYPLL to CLRPLL
				u1VRCG_OP &= 0x3F; //! MR13 OP7 = 0, OP6 = 0, from PHYPLL to CLRPLL
			}
		}
		p->ShuRGAccessIdx = cur_shu_mux_index; // Currect
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_MR13), u1MR13_OP, SHU_HWSET_MR13_HWSET_MR13_OP); // Current
		p->ShuRGAccessIdx = pingpong_shu_level; // Next
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_VRCG), u1VRCG_OP, SHU_HWSET_VRCG_HWSET_VRCG_OP); // Next
		p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
	}

	p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
	vSetPHY2ChannelMapping(p, ch_bak);
}

static void DFSRuntimeMRWEn(DRAMC_CTX_T *p, U8 cur_shu_mux_index, U8 nxt_shu_level, U8 pingpong_fsp)
{
	U8 rtmr13 = 0;
	U8 rtmr1 = 0, rtmr2 = 0, rtmr3 = 0, rtmr11 = 0;
	U8 rtmr12 = 0, rtmr14 = 0;
	U8 rtmr21 = 0, rtmr22 = 0, rtmr51 = 0;
	U8 md32_rtmrw_hpri_en_bk = 0;
	U32 bc_bak = 0, ch_bak = 0;
	U8 ch_start = 0, ch_end = 0;
	U8 u1ChIdx = 0, u1RankIdx = 0;
	U8 u1FldIdx = 0;

	bc_bak = GetDramcBroadcast();
	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
	ch_bak = vGetPHY2ChannelMapping(p);

		ch_start = CHANNEL_A;
		ch_end = CHANNEL_B+1;
#if (CHANNEL_NUM > 2)
		ch_end = CHANNEL_D+1;
#endif

	//! get mr13
	rtmr13 = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_15_0, LPIF_MR_OP_STORE_SHU_15_0_MR_OP_SET_SHU_15_0);
	//! get shux mr1/mr2/mr3/mr11
	rtmr1 = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_0 + (nxt_shu_level << 4), LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_0);
	rtmr2 = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_0 + (nxt_shu_level << 4), LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_1);
	rtmr3 = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_0 + (nxt_shu_level << 4), LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_2);
	rtmr11 = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_0 + (nxt_shu_level << 4), LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_3);
	//! get shux mr21/mr22/mr51
	rtmr21 = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_1 + (nxt_shu_level << 4), LPIF_MR_OP_STORE_SHU_0_1_MR_OP_SET_SHU_0_5);
	rtmr22 = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_1 + (nxt_shu_level << 4), LPIF_MR_OP_STORE_SHU_0_1_MR_OP_SET_SHU_0_6);
	rtmr51 = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_1 + (nxt_shu_level << 4), LPIF_MR_OP_STORE_SHU_0_1_MR_OP_SET_SHU_0_7);

	rtmr13 &= 0x3F;
	if (pingpong_fsp == FSP_1)
	{
		if(cur_shu_mux_index == PHYPLL_MODE)
			rtmr13 |= (0x1 << 7); //! MR13 OP7 = 1, OP6 = 0, from PHYPLL to CLRPLL
		else
			rtmr13 |= (0x1 << 6); //! MR13 OP7 = 0, OP6 = 1, from CLRPLL to PHYPLL
	}
	else
	{
		if(cur_shu_mux_index == PHYPLL_MODE)
			rtmr13 |= (0x1 << 6); //! MR13 OP7 = 0, OP6 = 1, from CLRPLL to PHYPLL
		else
			rtmr13 |= (0x1 << 7); //! MR13 OP7 = 1, OP6 = 0, from PHYPLL to CLRPLL
	}

#if 0 // @Darren-
	if (p->support_rank_num == RANK_DUAL)
		md32_rtmrw_rank = 0x3; //! dual rank
	else
		md32_rtmrw_rank = 0x1; //! single rank
#endif

	//Darren-mcSHOW_DBG_MSG(("[DFSRuntimeMRWEn]\n"));
	u1FldIdx = 0; // shift 8-bits field
	for (u1ChIdx = ch_start; u1ChIdx < ch_end; u1ChIdx++)
	{
		vSetPHY2ChannelMapping(p, u1ChIdx);
		md32_rtmrw_hpri_en_bk = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MPC_CTRL), MPC_CTRL_RTMRW_HPRI_EN);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MPC_CTRL), 0x1, MPC_CTRL_RTMRW_HPRI_EN);

		for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
		{
			//Darren-mcSHOW_DBG_MSG(("CH%d RK%d\n", u1ChIdx, u1RankIdx));
			//! get shux mr12/mr14/
			rtmr12 = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_7_0 + (nxt_shu_level << 4),  Fld(8, u1FldIdx*8));
			rtmr14 = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_7_1 + (nxt_shu_level << 4),  Fld(8, u1FldIdx*8));
			//Darren-mcSHOW_DBG_MSG(("\tMR1=0x%x, MR2=0x%x, MR3=0x%x, MR11=0x%x\n", rtmr1, rtmr2, rtmr3, rtmr11));
			//Darren-mcSHOW_DBG_MSG(("\tMR12=0x%x, MR13=0x%x, MR14=0x%x, MR22=0x%x, MR51=0x%x\n", rtmr12, rtmr13, rtmr14, rtmr22, rtmr51));
			TriggerRTMRW_SingleChannel(p, u1RankIdx, rtmr1, rtmr2, rtmr3, rtmr11, rtmr12, rtmr13, rtmr14, rtmr21, rtmr22, rtmr51);
			u1FldIdx++; // shift 8-bits field
		}

		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MPC_CTRL), md32_rtmrw_hpri_en_bk, MPC_CTRL_RTMRW_HPRI_EN);
	}

	vSetPHY2ChannelMapping(p, ch_bak);
	DramcBroadcastOnOff(bc_bak);
}
#endif

static void DFSHwSetWA(DRAMC_CTX_T *p, U8 cur_shu_mux_index, U8 nxt_shu_level)
{
	U8 u1MR13_OP = 0;
	U8 ch_start = 0, ch_end = 0, u1ChIdx = 0;
	U8 ch_bak = vGetPHY2ChannelMapping(p);
	ch_start = CHANNEL_A;
	ch_end = CHANNEL_B+1;
#if (CHANNEL_NUM > 2)
	ch_end = CHANNEL_D+1;
#endif
	for (u1ChIdx = ch_start; u1ChIdx < ch_end; u1ChIdx++)
	{
		vSetPHY2ChannelMapping(p, u1ChIdx);
		p->ShuRGAccessIdx = cur_shu_mux_index; // NOTE: Currect shuffle
		u1MR13_OP = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_MR13), SHU_HWSET_MR13_HWSET_MR13_OP);
		if ((nxt_shu_level == SRAM_SHU0) || (nxt_shu_level == SRAM_SHU1)) // for term shuffle level
			u1MR13_OP |= 0xC0; //! MR13 OP7 = 1, OP6 = 1, from CLRPLL to PHYPLL
		else
			u1MR13_OP &= 0x3F; //! MR13 OP7 = 0, OP6 = 0, from PHYPLL to CLRPLL
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_MR13), u1MR13_OP, SHU_HWSET_MR13_HWSET_MR13_OP); // Current
	}
	p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
	vSetPHY2ChannelMapping(p, ch_bak);
}
#if ENABLE_CONFIG_MCK_4TO1_MUX
void ConfigMCK4To1MUX(DRAMC_CTX_T *p, CLK_MUX_T eClkMux)
{
	U8 u1DVFS_52M_104M_SEL, u1DVFS_104M_208M_SEL;

	if (eClkMux == CLK_MUX_208M)
	{
		u1DVFS_52M_104M_SEL = 1;
		u1DVFS_104M_208M_SEL = 1;
	}
	else if (eClkMux == CLK_MUX_104M)
	{
		u1DVFS_52M_104M_SEL = 1;
		u1DVFS_104M_208M_SEL = 0;
	}
	else
	{
		u1DVFS_52M_104M_SEL = 0;
		u1DVFS_104M_208M_SEL = 0;
	}

	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_CKMUX_SEL, P_Fld(u1DVFS_52M_104M_SEL, MISC_CKMUX_SEL_RG_52M_104M_SEL)
						| P_Fld(u1DVFS_104M_208M_SEL, MISC_CKMUX_SEL_RG_104M_208M_SEL));

	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_CG_CTRL0, P_Fld(0x3, MISC_CG_CTRL0_CLK_MEM_SEL)
						| P_Fld(0x1, MISC_CG_CTRL0_W_CHG_MEM));

	mcDELAY_XNS(100);//reserve 100ns period for clock mute and latch the rising edge sync condition for BCLK

	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CG_CTRL0, 0x0, MISC_CG_CTRL0_W_CHG_MEM);
}
#endif

#if ENABLE_DFS_DEBUG_MODE
void WaitDFSDebugSM(DRAMC_CTX_T *p, U8 u1HangStatus)
{
	U8 u1Status[CHANNEL_NUM] = {0}, u1DvfsState[CHANNEL_NUM] = {0}, u1ChIdx = 0, u1ChStart = 0, u1ChEnd = 0;
	DRAM_CHANNEL_T eOriChannel = vGetPHY2ChannelMapping(p);
	U32 u4While1Cnt = 100;

		u1ChStart = CHANNEL_A;
		u1ChEnd = CHANNEL_B+1;
#if CHANNEL_NUM > 2
		u1ChEnd = CHANNEL_D+1;
#endif

	for (u1ChIdx = u1ChStart; u1ChIdx < u1ChEnd; u1ChIdx++)
	{
		vSetPHY2ChannelMapping(p, u1ChIdx);
		do {
			u1Status[u1ChIdx] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_DVFS_STATUS), DVFS_STATUS_CUT_PHY_ST_SHU);
			u1DvfsState[u1ChIdx] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MRR_STATUS2), MRR_STATUS2_DVFS_STATE);
			//mcSHOW_DBG_MSG(("[WaitDFSDebugSM] CH%d DFS debug mode state (0x%x, 0x%x), Dvfs State = 0x%x\n", u1ChIdx, u1Status[u1ChIdx], u1HangStatus, u1DvfsState[u1ChIdx]));
			if (u1Status[u1ChIdx] == u1HangStatus)
				break;

			if (u4While1Cnt == 0)
			{
				DDRPhyFreqMeter();
				while(1);
			}
			u4While1Cnt--;
		} while(1);
	}

	vSetPHY2ChannelMapping(p, eOriChannel);
}

void ExitDFSDebugMode(DRAMC_CTX_T *p, DFS_DBG_T eDbgMode)
{
	if ((eDbgMode == BEF_DFS_MODE) || (eDbgMode == AFT_DFS_MODE))
	{
		vIO32WriteFldMulti_All((DDRPHY_REG_MISC_DVFSCTL3), P_Fld(0x0, MISC_DVFSCTL3_RG_DFS_AFT_PHY_SHU_DBG_EN)
							| P_Fld(0x0, MISC_DVFSCTL3_RG_DFS_BEF_PHY_SHU_DBG_EN));
	}
	else if (eDbgMode == CHG_CLK_MODE)
	{
		vIO32WriteFldMulti_All((DDRPHY_REG_MISC_DVFSCTL3), P_Fld(0x0, MISC_DVFSCTL3_RG_PHY_ST_CHG_TO_BCLK_BY_LPC_EN)
							| P_Fld(0x0, MISC_DVFSCTL3_RG_PHY_ST_CHG_TO_MCLK_BY_LPC_EN));
		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CLK_CTRL, 0x0, MISC_CLK_CTRL_DVFS_MEM_CK_MUX_SEL_MODE); // HW mode
	}
	else
	{
		mcSHOW_ERR_MSG(("DFS debug mode err!\n"));
		#if __ETT__
		while (1);
		#endif
	}
}

void ChkDFSDebugMode(DRAMC_CTX_T *p, DFS_DBG_T eDbgMode)
{
	if (eDbgMode == BEF_DFS_MODE)
	{
		WaitDFSDebugSM(p, 0x1);
	}
	else if (eDbgMode == AFT_DFS_MODE)
	{
		WaitDFSDebugSM(p, 0x1d);

	}
	else if (eDbgMode == CHG_CLK_MODE)
	{
		WaitDFSDebugSM(p, 0x1e);

		// HW shuffle will switch clock to 208MHz and continue DFS
		vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_3, P_Fld(0xf, LPIF_LOW_POWER_CFG_3_DVFS_MEM_CK_MUX_SEL)
							| P_Fld(0x3, LPIF_LOW_POWER_CFG_3_DVFS_MEM_CK_MUX_UPDATE));
		mcDELAY_US(1); // Wait 1T 26MHz
		vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_3, P_Fld(0xf, LPIF_LOW_POWER_CFG_3_DVFS_MEM_CK_MUX_SEL)
							| P_Fld(0x0, LPIF_LOW_POWER_CFG_3_DVFS_MEM_CK_MUX_UPDATE));

		WaitDFSDebugSM(p, 0x1f);

		// HW shuffle will switch clock to MCK and continue DFS
		vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_3, P_Fld(0x5, LPIF_LOW_POWER_CFG_3_DVFS_MEM_CK_MUX_SEL)
							| P_Fld(0x3, LPIF_LOW_POWER_CFG_3_DVFS_MEM_CK_MUX_UPDATE));
		mcDELAY_US(1); // Wait 1T 26MHz
		vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_3, P_Fld(0x5, LPIF_LOW_POWER_CFG_3_DVFS_MEM_CK_MUX_SEL)
							| P_Fld(0x0, LPIF_LOW_POWER_CFG_3_DVFS_MEM_CK_MUX_UPDATE));

	}
	else
	{
		mcSHOW_ERR_MSG(("DFS debug mode err!\n"));
		#if __ETT__
		while (1);
		#endif
	}
}

void EntryDFSDebugMode(DRAMC_CTX_T *p, DFS_DBG_T eDbgMode)
{
	if (eDbgMode == BEF_DFS_MODE)
	{
		vIO32WriteFldMulti_All((DDRPHY_REG_MISC_DVFSCTL3), P_Fld(0x0, MISC_DVFSCTL3_RG_DFS_AFT_PHY_SHU_DBG_EN)
							| P_Fld(0x1, MISC_DVFSCTL3_RG_DFS_BEF_PHY_SHU_DBG_EN));
	}
	else if (eDbgMode == AFT_DFS_MODE)
	{
		vIO32WriteFldMulti_All((DDRPHY_REG_MISC_DVFSCTL3), P_Fld(0x1, MISC_DVFSCTL3_RG_DFS_AFT_PHY_SHU_DBG_EN)
							| P_Fld(0x0, MISC_DVFSCTL3_RG_DFS_BEF_PHY_SHU_DBG_EN));
	}
	else if (eDbgMode == CHG_CLK_MODE)
	{
		vIO32WriteFldMulti_All((DDRPHY_REG_MISC_DVFSCTL3), P_Fld(0x1, MISC_DVFSCTL3_RG_PHY_ST_CHG_TO_BCLK_BY_LPC_EN)
							| P_Fld(0x1, MISC_DVFSCTL3_RG_PHY_ST_CHG_TO_MCLK_BY_LPC_EN));
		// for MD32 RG mode
		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CLK_CTRL, 0x1, MISC_CLK_CTRL_DVFS_MEM_CK_MUX_SEL_MODE);
		// for PHY RG mode (no support)
		//Darren-vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CLK_CTRL, 0x1, MISC_CLK_CTRL_DVFS_MEM_CK_MUX_SEL_MODE);
	}
	else
	{
		mcSHOW_ERR_MSG(("DFS debug mode err!\n"));
		#if __ETT__
		while (1);
		#endif
	}

}
#endif

#if DFS_NOQUEUE_FLUSH_WA
U32 u4PERFCTL0_backup=0;

void EnableDFSNoQueueFlush(DRAMC_CTX_T *p)
{
	vIO32WriteFldMulti_All(DRAMC_REG_DVFS_CTRL0, P_Fld(0, DVFS_CTRL0_HWSET_WLRL)
		| P_Fld(0, DVFS_CTRL0_DVFS_RXFIFOST_SKIP) // sync MP settings
		| P_Fld(1, DVFS_CTRL0_DVFS_NOQUEFLUSH_EN)
		| P_Fld(0, DVFS_CTRL0_R_DMDVFSMRW_EN));
	vIO32WriteFldMulti_All(DRAMC_REG_SHUCTRL1, P_Fld(0, SHUCTRL1_FC_PRDCNT)
#if ENABLE_LP4Y_WA
		//@Berson, LP4Y tCKFSPE/X_SE violation at shuffle as DVFS noqueflush enable
		// LP4Y tCKFSPE/X_SE violation at shuffle from 7.5ns to 15ns
		| P_Fld(5, SHUCTRL1_CKFSPE_PRDCNT)
		| P_Fld(5, SHUCTRL1_VRCGEN_PRDCNT)
#else
		| P_Fld(0, SHUCTRL1_CKFSPE_PRDCNT)
		| P_Fld(0, SHUCTRL1_VRCGEN_PRDCNT)
#endif
		| P_Fld(0, SHUCTRL1_CKFSPX_PRDCNT));
	vIO32WriteFldAlign_All(DRAMC_REG_BYPASS_FSPOP, 0, BYPASS_FSPOP_BPFSP_OPT); // sync MP settings

#if ENABLE_DFS_RUNTIME_MRW // for Skip HW MR2
	vIO32WriteFldMulti_All(DRAMC_REG_DVFS_TIMING_CTRL3, P_Fld(0, DVFS_TIMING_CTRL3_RTMRW_MRW1_SKIP) // OP CHG & VRCG High
		| P_Fld(0, DVFS_TIMING_CTRL3_RTMRW_MRW2_SKIP) // VRCG Low
		| P_Fld(1, DVFS_TIMING_CTRL3_RTMRW_MRW3_SKIP)); // MR2 RL/WL (reduce 50ns)
#endif

#if ENABLE_DFS_NOQUEUE_FLUSH_DBG
	// for debug mode only (skip HW MRW)
	vIO32WriteFldMulti_All(DRAMC_REG_DVFS_TIMING_CTRL3, P_Fld(1, DVFS_TIMING_CTRL3_RTMRW_MRW1_PAUSE)
		| P_Fld(1, DVFS_TIMING_CTRL3_RTMRW_MRW2_PAUSE)
		| P_Fld(1, DVFS_TIMING_CTRL3_RTMRW_MRW3_PAUSE));
#endif
}

static void NoQueueFlushWA(DRAMC_CTX_T *p, U8 u1WA_enable)
{
	U32 bc_bak=0;

	if (p->support_channel_num > CHANNEL_SINGLE) //for dual single
	{
		bc_bak = GetDramcBroadcast();
		DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
	}

	if (u1WA_enable == ENABLE)
	{
		u4PERFCTL0_backup = (u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_PERFCTL0)) >> Fld_shft(PERFCTL0_RWAGEEN)) & 0x3;
		vIO32WriteFldMulti(DRAMC_REG_PERFCTL0, P_Fld(0, PERFCTL0_RWAGEEN)
				| P_Fld(0, PERFCTL0_EMILLATEN));
		//mcSHOW_DBG_MSG(("[NoQueueFlushWA] PERFCTL0[11:10] backup = 0x%x\n", u4PERFCTL0_backup));
	}
	else
	{
		vIO32WriteFldMulti(DRAMC_REG_PERFCTL0, P_Fld(u4PERFCTL0_backup & 0x1, PERFCTL0_RWAGEEN)
				| P_Fld((u4PERFCTL0_backup>>1) & 0x1, PERFCTL0_EMILLATEN));
	}

	if (p->support_channel_num > CHANNEL_SINGLE) //for dual single
		DramcBroadcastOnOff(bc_bak);
}
#endif

#if ENABLE_TIMING_TXSR_DFS_WA
static void TimingTxsrWA(DRAMC_CTX_T *p, U32 next_shu_level)
{
	U32 onoff=0, bc_bak=0;

	if (p->support_channel_num > CHANNEL_SINGLE) //for dual single
	{
		bc_bak = GetDramcBroadcast();
		DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
	}

	if ((next_shu_level == SRAM_SHU4) || (next_shu_level == SRAM_SHU5) || (next_shu_level == SRAM_SHU6))
		onoff = DISABLE;
	else
		onoff = ENABLE;

	vIO32WriteFldAlign(DRAMC_REG_REFCTRL1, onoff, REFCTRL1_REF_OVERHEAD_SLOW_REFPB_ENA);

	if (p->support_channel_num > CHANNEL_SINGLE)
		DramcBroadcastOnOff(bc_bak);
}
#endif

#if ENABLE_TX_REBASE_ODT_WA
void TxReadBaseODTWA(DRAMC_CTX_T *p, U8 next_shu_level)
{
	U32 termen_dis, bc_bak=0;

	if (p->support_channel_num > CHANNEL_SINGLE) //for dual single
	{
		bc_bak = GetDramcBroadcast();
		DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
	}

	if ((next_shu_level == SRAM_SHU0) || (next_shu_level == SRAM_SHU1)) // for DDR4266/DDR3200
		termen_dis = DISABLE; //term
	else
		termen_dis = ENABLE; // un-term

	//mcSHOW_DBG_MSG(("[TxReadBaseODTWA] SRAM SHU%d, termen_dis = %d\n", next_shu_level, termen_dis));
	vIO32WriteFldAlign(DDRPHY_REG_B0_DQ6, termen_dis, B0_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B0);
	vIO32WriteFldAlign(DDRPHY_REG_B1_DQ6, termen_dis, B1_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B1);
	vIO32WriteFldAlign(DDRPHY_REG_CA_CMD6, termen_dis, CA_CMD6_RG_TX_ARCMD_ODTEN_EXT_DIS);

	if (p->support_channel_num > CHANNEL_SINGLE) //for dual single
		DramcBroadcastOnOff(bc_bak);
}
#endif

#if ENABLE_TX_REBASE_WDQS_DQS_PI_WA
static void TxReBaseWDQSDqsPiWA(DRAMC_CTX_T *p, U8 pingpong_shu_level)
{
	U32 bc_bak=0;

	if (p->support_channel_num > CHANNEL_SINGLE) //for dual single
	{
		bc_bak = GetDramcBroadcast();
		DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
	}

	p->ShuRGAccessIdx = pingpong_shu_level;
	vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ13	, P_Fld(0, SHU_B0_DQ13_RG_TX_ARDQS_READ_BASE_EN_B0			 )
																| P_Fld(0, SHU_B0_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B0 		 ) );
	vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ13	, P_Fld(0, SHU_B1_DQ13_RG_TX_ARDQS_READ_BASE_EN_B1			 )
																| P_Fld(0, SHU_B1_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B1 		 ) );
	mcDELAY_US(1);
	vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ13	, P_Fld(1, SHU_B0_DQ13_RG_TX_ARDQS_READ_BASE_EN_B0			 )
																| P_Fld(1, SHU_B0_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B0 		 ) );
	vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ13	, P_Fld(1, SHU_B1_DQ13_RG_TX_ARDQS_READ_BASE_EN_B1			 )
																| P_Fld(1, SHU_B1_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B1 		 ) );
	p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;

	if (p->support_channel_num > CHANNEL_SINGLE) //for dual single
		DramcBroadcastOnOff(bc_bak);
}
#endif

#if ENABLE_SRAM_DMA_WA
/*#define DDRPHY_REG_SHU_B0_PHY_VREF_SEL						 (DDRPHY_AO_BASE_ADDRESS + 0x07B4)
	#define SHU_B0_PHY_VREF_SEL_RG_RX_ARDQ_VREF_SEL_LB_B0	   Fld(7, 0) //[6:0]
	#define SHU_B0_PHY_VREF_SEL_RG_RX_ARDQ_VREF_SEL_UB_B0	   Fld(7, 8) //[14:8]*/
U32 gSRAMBackupIdx[DRAM_DFS_SHUFFLE_MAX][4] = { // @Darren, LP5 don't use DDRPHY_REG_SHU_R0_CA_RXDLY6 !!
/*0*/	 {DDRPHY_REG_SHU_B0_DQ9, DDRPHY_REG_SHU_B1_DQ9},
/*1*/	 {DDRPHY_REG_SHU_B0_DQ9, DDRPHY_REG_SHU_B1_DQ9},
/*2*/	 {DDRPHY_REG_SHU_B0_DQ9, DDRPHY_REG_SHU_B1_DQ9},
/*3*/	 {DDRPHY_REG_SHU_B0_DQ9, DDRPHY_REG_SHU_B1_DQ9},
/*4*/	 {DDRPHY_REG_SHU_B0_DQ9, DDRPHY_REG_SHU_B1_DQ9},
/*5*/	 {DDRPHY_REG_SHU_B0_DQ9, DDRPHY_REG_SHU_B1_DQ9},
/*6*/	 {DDRPHY_REG_SHU_B0_DQ9, DDRPHY_REG_SHU_B1_DQ9},
};
void DPHYSaveToSRAMShuWA(DRAMC_CTX_T *p, U8 sram_shu_level)
{
	U8 u1ChannelIdx=0, u1RankIdx=0;
	U32 u4B0_DQ1=0, u4Offset=0;
	U8 u1Ch_backup = p->channel, u1Rk_backup = p->rank;
	U32 u4B0_PHY_VREF_SEL=0, u4B1_PHY_VREF_SEL=0, u4PHY_VREF_SEL=0;
	DRAM_DFS_REG_SHU_T ShuRGAccessIdxBackup = p->ShuRGAccessIdx;

	for (u1ChannelIdx=CHANNEL_A; u1ChannelIdx < (p->support_channel_num); u1ChannelIdx++)
	{
		vSetPHY2ChannelMapping(p, u1ChannelIdx);
		for (u1RankIdx = RANK_0; u1RankIdx < (p->support_rank_num); u1RankIdx++)
		{
			vSetRank(p, u1RankIdx);
			u4Offset = 0; // B0
			u4B0_PHY_VREF_SEL = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_PHY_VREF_SEL+u4Offset));
			u4Offset = DDRPHY_AO_B0_B1_OFFSET; // B1
			u4B1_PHY_VREF_SEL = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_PHY_VREF_SEL+u4Offset));

			u4PHY_VREF_SEL = (u4B1_PHY_VREF_SEL<<16) | u4B0_PHY_VREF_SEL;

			mcSHOW_DBG_MSG(("[DPHYSaveToSRAMShuWA] CH%d RK%d, B1B0_PHY_VREF_SEL=0x%x\n", u1ChannelIdx, u1RankIdx, u4PHY_VREF_SEL));
			p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
			vIO32Write4B(DRAMC_REG_ADDR(gSRAMBackupIdx[sram_shu_level][u1RankIdx]), u4PHY_VREF_SEL);
		}
	}

	p->ShuRGAccessIdx = ShuRGAccessIdxBackup;
	vSetPHY2ChannelMapping(p, u1Ch_backup);
	vSetRank(p, u1Rk_backup);
}

void DPHYSRAMShuWAToSHU1(DRAMC_CTX_T *p)
{
	U8 u1ChannelIdx=0, u1RankIdx=0;
	U32 u4B0_DQ1=0, u4Offset=0;
	U8 u1Ch_backup = p->channel, u1Rk_backup = p->rank;
	U32 u4B0_PHY_VREF_SEL=0, u4B1_PHY_VREF_SEL=0, u4PHY_VREF_SEL=0;
	DRAM_DFS_REG_SHU_T ShuRGAccessIdxBackup = p->ShuRGAccessIdx;

	for (u1ChannelIdx=CHANNEL_A; u1ChannelIdx < (p->support_channel_num); u1ChannelIdx++)
	{
		vSetPHY2ChannelMapping(p, u1ChannelIdx);
		for (u1RankIdx = RANK_0; u1RankIdx < (p->support_rank_num); u1RankIdx++)
		{
			vSetRank(p, u1RankIdx);
			u4Offset = 0; // B0
			u4B0_PHY_VREF_SEL = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_PHY_VREF_SEL+u4Offset));
			u4Offset = DDRPHY_AO_B0_B1_OFFSET; // B1
			u4B1_PHY_VREF_SEL = u4IO32Read4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_PHY_VREF_SEL+u4Offset));

			u4PHY_VREF_SEL = (u4B1_PHY_VREF_SEL<<16) | u4B0_PHY_VREF_SEL;
			mcSHOW_DBG_MSG(("[DPHYRxVrefWAToSHU1] CH%d RK%d, B1B0_PHY_VREF_SEL=0x%x\n", u1ChannelIdx, u1RankIdx, u4PHY_VREF_SEL));

			p->ShuRGAccessIdx = DRAM_DFS_REG_SHU1;
			u4Offset = 0; // B0
			vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_PHY_VREF_SEL+u4Offset), u4B0_PHY_VREF_SEL);
			u4Offset = DDRPHY_AO_B0_B1_OFFSET; // B1
			vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_PHY_VREF_SEL+u4Offset), u4B1_PHY_VREF_SEL);
			p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
		}
	}

	p->ShuRGAccessIdx = ShuRGAccessIdxBackup;
	vSetPHY2ChannelMapping(p, u1Ch_backup);
	vSetRank(p, u1Rk_backup);
}

void SRAMShuRestoreToDPHYWA(DRAMC_CTX_T *p, U8 sram_shu_level, U8 pingpong_shu_level)
{
	U8 u1ChannelIdx=0, u1RankIdx=0, u1ByteIdx=0;
	U32 u4Offset=0;
	U8 u1Ch_backup = p->channel, u1Rk_backup = p->rank;
	U32 u4Byte_PHY_VREF_SEL=0, u4PHY_VREF_SEL=0;
	DRAM_DFS_REG_SHU_T ShuRGAccessIdxBackup = p->ShuRGAccessIdx;

	for (u1ChannelIdx=CHANNEL_A; u1ChannelIdx < (p->support_channel_num); u1ChannelIdx++)
	{
		vSetPHY2ChannelMapping(p, u1ChannelIdx);
		for (u1RankIdx = RANK_0; u1RankIdx < (p->support_rank_num); u1RankIdx++)
		{
			vSetRank(p, u1RankIdx);
			p->ShuRGAccessIdx = pingpong_shu_level;
			u4PHY_VREF_SEL = u4IO32Read4B(DRAMC_REG_ADDR(gSRAMBackupIdx[sram_shu_level][u1RankIdx]));
			p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;

			for(u1ByteIdx=0; u1ByteIdx<DQS_NUMBER_LP4; u1ByteIdx++)
			{
				u4Offset = u1ByteIdx*DDRPHY_AO_B0_B1_OFFSET;
				u4Byte_PHY_VREF_SEL = (u4PHY_VREF_SEL >> (16*u1ByteIdx)) & 0xffff;

				//mcSHOW_DBG_MSG(("[SRAMShuRestoreToDPHYWA] CH%d RK%d B%d, u4Byte_PHY_VREF_SEL=0x%x\n", u1ChannelIdx, u1RankIdx, u1ByteIdx, u4Byte_PHY_VREF_SEL));

				p->ShuRGAccessIdx = pingpong_shu_level;
				vIO32Write4B(DRAMC_REG_ADDR(DDRPHY_REG_SHU_B0_PHY_VREF_SEL+u4Offset), u4Byte_PHY_VREF_SEL);
				p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
			}
		}
	}

	p->ShuRGAccessIdx = ShuRGAccessIdxBackup;
	vSetPHY2ChannelMapping(p, u1Ch_backup);
	vSetRank(p, u1Rk_backup);
}
#endif

void EnableDFSHwModeClk(DRAMC_CTX_T *p)
{
	//Shuffle HW mode for MCK/208M switch
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DVFSCTL3,
						P_Fld(0x3, MISC_DVFSCTL3_RG_DVFS_MEM_CK_SEL_DESTI) | // dvfs source clock selection when ddrphy shuffle
						P_Fld(0x1, MISC_DVFSCTL3_RG_DVFS_MEM_CK_SEL_SOURCE)); // dvfs destination clock selection when ddrphy shuffle
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_CLK_CTRL,
						P_Fld(0x1, MISC_CLK_CTRL_DVFS_MEM_CK_MUX_UPDATE_EN) | //M_CK clock mux selection update enable by shuffle
						P_Fld(0x1, MISC_CLK_CTRL_DVFS_CLK_MEM_SEL) | // by shuffle
						P_Fld(0x0, MISC_CLK_CTRL_DVFS_MEM_CK_MUX_SEL_MODE) | // HW mode by shuffle
						P_Fld(0x1, MISC_CLK_CTRL_DVFS_MEM_CK_MUX_SEL)); // 4-to-1 mux for PLLCK
}

void DVFSSettings(DRAMC_CTX_T *p)
{
	U8 u1DVFS_52M_104M_SEL = 1; // DVFS_SM freq: 0: 52Mhz 1:104Mhz
	U8 u1Master_DLL_Idle = 0x2b; // Master from MCK
	U8 u1Slave_DLL_Idle = 0x43; // Slave from MCK
#if (fcFOR_CHIP_ID == fcA60868) // @Darren, for A60868 only
	U8 u1ChClkIgnore[2] = {ENABLE, ENABLE}, u1Channel = 0; // 1=ignore
#endif
	U32 backup_broadcast = GetDramcBroadcast();
	DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);

	if (vGet_Div_Mode(p) == DIV16_MODE)
	{
		u1Master_DLL_Idle = 0x37; // Master from MCK
		u1Slave_DLL_Idle = 0x4D; // Slave from MCK
	}

	//DVFS debug enable - MRR_STATUS2_DVFS_STATE
	//@Lynx, A60868 HW always enable shuffle debug. remove RG: DVFSDLL_R_DDRPHY_SHUFFLE_DEBUG_ENABLE
	//vIO32WriteFldAlign_All(DRAMC_REG_DVFSDLL, 1, DVFSDLL_R_DDRPHY_SHUFFLE_DEBUG_ENABLE);

	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CKMUX_SEL, u1DVFS_52M_104M_SEL, MISC_CKMUX_SEL_RG_52M_104M_SEL); //Set DVFS_SM's clk
#if ENABLE_DFS_208M_CLOCK
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CKMUX_SEL, 0x1, MISC_CKMUX_SEL_RG_104M_208M_SEL); //Set DVFS_SM's clk to 208M
#endif
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_DVFSDLL, P_Fld(u1Master_DLL_Idle, MISC_SHU_DVFSDLL_R_DLL_IDLE)
		| P_Fld(u1Slave_DLL_Idle, MISC_SHU_DVFSDLL_R_2ND_DLL_IDLE));

	// @Darren, set current SRAM SHU index for SPM mode DFS latch/restore
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, vGet_Current_ShuLevel(p), MISC_RG_DFS_CTRL_RG_DR_SHU_LEVEL_SRAM);
	//mcSHOW_DBG_MSG(("[DVFSSettings] SHU_LEVEL_SRAM = %d\n", vGet_Current_ShuLevel(p)));

#if (fcFOR_CHIP_ID == fcA60868) // @Darren, for A60868 only
	for (u1Channel = CHANNEL_A; u1Channel < p->support_channel_num; u1Channel++)
		u1ChClkIgnore[u1Channel] = DISABLE;
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DVFSCTL2, P_Fld(u1ChClkIgnore[1], MISC_DVFSCTL2_RG_IGNORE_PHY_SH_CHG_CLK_RDY_CHB)
		| P_Fld(u1ChClkIgnore[0], MISC_DVFSCTL2_RG_IGNORE_PHY_SH_CHG_CLK_RDY_CHA));
#endif
	// DFS trigger by DDRPHY RG
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_SPM_DVFS_CONTROL_SEL); // DFS RG mode for calibration
	//vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_TX_TRACKING_DIS); // DFS RG mode for disable tx tracking
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_DVFSCTL2, 1, MISC_DVFSCTL2_RG_MRW_AFTER_DFS);

	vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_FSM_CFG_1, P_Fld(1, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL)
		| P_Fld(1, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL_2ND)
		| P_Fld(1, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL_FOR_PWR)
		| P_Fld(1, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL_FOR_PWR_2ND));
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_OPT, P_Fld(1, MISC_SHU_OPT_R_DQB0_SHU_PHY_GATING_RESETB_SPM_EN)
		| P_Fld(1, MISC_SHU_OPT_R_DQB1_SHU_PHY_GATING_RESETB_SPM_EN));
#if ENABLE_DFS_HW_SAVE_MASK
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_DVFSCTL2, 1, MISC_DVFSCTL2_DVFS_SYNC_MASK_FOR_PHY); // 0x1 = disable dfs hw save
#endif

#if ENABLE_DVFS_CDC_SYNCHRONIZER_OPTION
	//CDC option
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DVFSCTL2, P_Fld(1, MISC_DVFSCTL2_R_DVFS_CDC_OPTION) //Lewis@20170331: Not set SHUCTRL2_R_DVFS_CDC_OPTION to 1 since it will lead DDR reserve mode fail in DDR2400 and DDR1600
		| P_Fld(1, MISC_DVFSCTL2_R_CDC_MUX_SEL_OPTION));
#endif

#if 0 // @Darren, reserved from Mengru Dsim
	U8 u1MarginNew = (u1DVFS_52M_104M_SEL == 1) ? 0x3 : 0x1;
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SHU_OPT, 0x2, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN);
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DVFSCTL, P_Fld(u1MarginNew, MISC_DVFSCTL_R_DVFS_PICG_MARGIN_NEW)
			 | P_Fld(u1MarginNew, MISC_DVFSCTL_R_DVFS_PICG_MARGIN2_NEW)
		| P_Fld(u1MarginNew, MISC_DVFSCTL_R_DVFS_PICG_MARGIN3_NEW));
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CKMUX_SEL, 0x1, MISC_CKMUX_SEL_FMEM_CK_MUX);
	vIO32WriteFldMulti_All(DRAMC_REG_DVFS_CTRL0, P_Fld(0x1, DVFS_CTRL0_R_DRAMC_CHA)
			 | P_Fld(0x0, DVFS_CTRL0_DVFS_CKE_OPT)
			 | P_Fld(0x1, DVFS_CTRL0_SCARB_PRI_OPT)
		| P_Fld(0x0, DVFS_CTRL0_SHU_PHYRST_SEL));
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DVFSCTL2, P_Fld(0x1, MISC_DVFSCTL2_R_DVFS_PARK_N)
		| P_Fld(0x1, MISC_DVFSCTL2_R_DVFS_OPTION));
#endif

#if ENABLE_BLOCK_APHY_CLOCK_DFS_OPTION
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CG_CTRL7, 1, MISC_CG_CTRL7_ARMCTL_CK_OUT_CG_SEL);
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DVFSCTL, P_Fld(1, MISC_DVFSCTL_R_DVFS_PICG_POSTPONE)
		| P_Fld(1, MISC_DVFSCTL_R_DMSHUFFLE_CHANGE_FREQ_OPT));
#endif

#if ENABLE_REMOVE_MCK8X_UNCERT_DFS_OPTION // @Mazar
	//vIO32WriteFldAlign_All(DDRPHY_REG_MISC_DVFS_EMI_CLK, 1, MISC_DVFS_EMI_CLK_RG_DLL_SHUFFLE_DDRPHY);
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DVFSCTL, P_Fld(1, MISC_DVFSCTL_R_SHUFFLE_PI_RESET_ENABLE)
		| P_Fld(3, MISC_DVFSCTL_R_DVFS_MCK8X_MARGIN)
		| P_Fld(3, MISC_DVFSCTL_R_DVFS_PICG_MARGIN4_NEW));
#endif

#if RDSEL_TRACKING_EN
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA1, 0x1ffff, MISC_SRAM_DMA1_SPM_RESTORE_STEP_EN);
#endif

#if (fcFOR_CHIP_ID == fcMargaux) // @Darren, for Mar_gaux New setting for ddrphy shuffle (sync mode)
	vIO32WriteFldAlign(DDRPHY_REG_MISC_DVFSCTL2, 0, MISC_DVFSCTL2_R_DVFS_CLK_CHG_OK_SEL);
	vIO32WriteFldAlign(DDRPHY_REG_MISC_DVFSCTL2 + SHIFT_TO_CHB_ADDR, 1, MISC_DVFSCTL2_R_DVFS_CLK_CHG_OK_SEL);
#endif

	//Cann_on CDC options
	//DLL_SHUFFLE should be set enable before switch frequency
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_DVFS_EMI_CLK, 0, MISC_DVFS_EMI_CLK_RG_DLL_SHUFFLE_DDRPHY);
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_DVFSCTL2, 0, MISC_DVFSCTL2_RG_DLL_SHUFFLE);
	vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DVFSCTL2, P_Fld(0, MISC_DVFSCTL2_R_DVFS_OPTION)
		| P_Fld(0, MISC_DVFSCTL2_R_DVFS_PARK_N));

#if ENABLE_DFS_TIMING_ENLARGE
	DFSEnlargeTimingSettings(p);
#endif

	//EnableDFSHwModeClk(p); // @Darren, for DFS shuffle change

	DramcBroadcastOnOff(backup_broadcast);
}

#if ENABLE_DFS_SSC_WA
void DDRSSCSetting(DRAMC_CTX_T * p)
{
	U32 u4DELTA1 = 0;

	if (p->frequency == 1866)
	{
		u4DELTA1 = 0xE14;
	}
	else if (p->frequency == 1600)
	{
		u4DELTA1 = 0xC1C;
	}
	else if (p->frequency == 1200)
	{
		u4DELTA1 = 0x90F;
	}
	else
	{
		return;
	}

	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_PHYPLL1, 0x1, SHU_PHYPLL1_RG_RPHYPLL_SDM_FRA_EN);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_PHYPLL1, 0x1, SHU_PHYPLL1_RG_RPHYPLL_SDM_FRA_EN);

	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_PHYPLL6, 0x1, SHU_PHYPLL6_RG_RPHYPLL_SDM_SSC_PH_INIT);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CLRPLL6, 0x1, SHU_CLRPLL6_RG_RCLRPLL_SDM_SSC_PH_INIT);

	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_PHYPLL6, 0x0208, SHU_PHYPLL6_RG_RPHYPLL_SDM_SSC_PRD);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CLRPLL6, 0x0208, SHU_CLRPLL6_RG_RCLRPLL_SDM_SSC_PRD);

	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_PHYPLL7, 0x0, SHU_PHYPLL7_RG_RPHYPLL_SDM_SSC_DELTA);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CLRPLL7, 0x0, SHU_CLRPLL7_RG_RCLRPLL_SDM_SSC_DELTA);

	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_PHYPLL7, u4DELTA1, SHU_PHYPLL7_RG_RPHYPLL_SDM_SSC_DELTA1);
	vIO32WriteFldAlign_All(DDRPHY_REG_SHU_CLRPLL7, u4DELTA1, SHU_CLRPLL7_RG_RCLRPLL_SDM_SSC_DELTA1);

	//vIO32WriteFldAlign_All(DDRPHY_PLL1, 0x1, PLL1_RG_RPHYPLL_SDM_SSC_EN);
	//vIO32WriteFldAlign_All(DDRPHY_PLL2, 0x1, PLL2_RG_RCLRPLL_SDM_SSC_EN);
}

void DramcSSCHoppingOnOff(DRAMC_CTX_T *p, U8 cur_shu_level, U8 u1OnOff)
{
	if ((cur_shu_level == 0x0) || (cur_shu_level == 0x8) || (cur_shu_level == 0x9) || (cur_shu_level == 0x6) || (cur_shu_level == 0x5))
	{
		if (!(p->u1PLLMode == PHYPLL_MODE))
			vIO32WriteFldAlign(DDRPHY_REG_CLRPLL0, u1OnOff, CLRPLL0_RG_RCLRPLL_SDM_SSC_EN); // CLRPLL SSC
		else
			vIO32WriteFldAlign(DDRPHY_REG_PHYPLL0, u1OnOff, PHYPLL0_RG_RPHYPLL_SDM_SSC_EN); // PHYPLL SSC
	}
}
#endif


#if DVT_TEST_DUMMY_RD_SIDEBAND_FROM_SPM || ENABLE_DFS_SSC_WA
void DVS_DMY_RD_ENTR(DRAMC_CTX_T *p)
{
	/*TINFO="DRAM : SPM DVS DMY RD ENTR"*/

	/*TINFO="DRAM : set sc_ddrphy_fb_ck_en = 1"*/
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 1, LPIF_LOW_POWER_CFG_0_DDRPHY_FB_CK_EN);


	mcDELAY_US(1);

	/*TINFO="DRAM : set sc_dmyrd_en_mod_sel = 1"*/
	//! diff with WE
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 1, LPIF_LOW_POWER_CFG_1_DMY_EN_MOD_SEL);

	mcDELAY_US(1);

	/*TINFO="DRAM : set sc_dmyrd_intv_sel = 1"*/
	//! diff with WE
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 1, LPIF_LOW_POWER_CFG_1_DMYRD_INTV_SEL);

	mcDELAY_US(1);

	/*TINFO="DRAM : set sc_dmyrd_en = 1"*/
	//! diff with WE
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 1, LPIF_LOW_POWER_CFG_1_DMYRD_EN);

	mcDELAY_US(1);
}

void DVS_DMY_RD_EXIT(DRAMC_CTX_T *p)
{
	/*TINFO="DRAM : SPM DVS DMY RD EXIT"*/

	/*TINFO="DRAM : set sc_dmyrd_en = 0"*/
	//! diff with WE
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 0, LPIF_LOW_POWER_CFG_1_DMYRD_EN);

	mcDELAY_US(1);

	/*TINFO="DRAM : set sc_dmyrd_intv_sel = 0"*/
	//! diff with WE
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 0, LPIF_LOW_POWER_CFG_1_DMYRD_INTV_SEL);

	mcDELAY_US(1);

	/*TINFO="DRAM : set sc_dmyrd_en_mod_sel = 0"*/
	//! diff with WE
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 0, LPIF_LOW_POWER_CFG_1_DMY_EN_MOD_SEL);

	mcDELAY_US(1);

	/*TINFO="DRAM : set sc_ddrphy_fb_ck_en = 0"*/
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_DDRPHY_FB_CK_EN);

	mcDELAY_US(1);


	/*TINFO="DRAM : SPM DVS DMY RD EXIT end "*/
}
#endif


#if 1//(FOR_DV_SIMULATION_USED==0 && SW_CHANGE_FOR_SIMULATION==0)

void DPMEnableTracking(DRAMC_CTX_T *p, U32 u4Reg, U32 u4Field, U8 u1ShuIdx, U8 u1Enable)
{
	U32 val, fld;

	fld = Fld(1, (Fld_shft(u4Field) + u1ShuIdx));

	val = (u1Enable) ? 1 : 0;

	vIO32WriteFldAlign_All(u4Reg, u1Enable, fld);
}

void DPMInit(DRAMC_CTX_T *p)
{
	U8 u1SetVal;
	U8 u1Pll1Val, u1Pll2Val;
	U8 u1ShuSramVal;
	DRAM_DFS_SRAM_SHU_T u1CurrShuLevel = vGet_Current_ShuLevel(p);

	u1SetVal = (p->support_channel_num > 1) ? 0x3 : 0x1;

	// pre-setting DPM to dramc low power interface setting
	vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0,
			P_Fld(u1SetVal, LPIF_LOW_POWER_CFG_0_PHYPLL_EN) |	// both channel phy pll en
			P_Fld(u1SetVal, LPIF_LOW_POWER_CFG_0_DPY_DLL_EN) |	// both channel dpy pll en
			P_Fld(u1SetVal, LPIF_LOW_POWER_CFG_0_DPY_2ND_DLL_EN) |	// both channel dpy 2nd pll en
			P_Fld(u1SetVal, LPIF_LOW_POWER_CFG_0_DPY_DLL_CK_EN) |	// both channel dpy dll ck en
			P_Fld(u1SetVal, LPIF_LOW_POWER_CFG_0_DPY_VREF_EN));	// both channel dpy vref en

	vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_3,
			P_Fld(u1SetVal, LPIF_LOW_POWER_CFG_3_DPY_MCK8X_EN) |	// both channel mck8x en
			P_Fld(u1SetVal, LPIF_LOW_POWER_CFG_3_DPY_MIDPI_EN) |	// both channel midpi en
			P_Fld(u1SetVal, LPIF_LOW_POWER_CFG_3_DPY_PI_RESETB_EN));	// both channel dpy pi resetb en

	if (p->u1PLLMode == PHYPLL_MODE)
	{
		mcSHOW_DBG_MSG(("PHYPLL\n"));
		u1Pll1Val = u1SetVal;
		u1Pll2Val = 0;
	}
	else
	{
		mcSHOW_DBG_MSG(("CLRPLL\n"));
		u1Pll1Val = 0;
		u1Pll2Val = u1SetVal;
	}

	vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0,
			P_Fld(u1Pll1Val, LPIF_LOW_POWER_CFG_0_PHYPLL_SHU_EN) |
			P_Fld(u1Pll1Val, LPIF_LOW_POWER_CFG_0_PHYPLL_MODE_SW) |
			P_Fld(u1Pll2Val, LPIF_LOW_POWER_CFG_0_PHYPLL2_SHU_EN) |
			P_Fld(u1Pll2Val, LPIF_LOW_POWER_CFG_0_PHYPLL2_MODE_SW));

	// all by lpif fw mode
	vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_FSM_CFG_1,
			/* TBA set control mux in DV initial */
			P_Fld(0x0, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL) |				// 0: MD32, 1: SPM
			P_Fld(0x0, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL_2ND) |			// 0: MD32, 1: SPM
			P_Fld(0x0, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL_FOR_PWR) |		// 0: MD32, 1: SPM
			P_Fld(0x0, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL_FOR_PWR_2ND) |	// 0: MD32, 1: SPM
			P_Fld(0x1, LPIF_FSM_CFG_1_LPIF_OUTPUT_PATH_FROM_SW) |			// 0: MD32 SCU, 1: MD32 CFG
			P_Fld(0x1, LPIF_FSM_CFG_1_LPIF_OUTPUT_PATH_FROM_SW_2ND) |		// 0: MD32 SCU, 1: MD32 CFG
			P_Fld(0x1, LPIF_FSM_CFG_1_LPIF_POWER_CONTROL_SEL) | 			// 0: MD32 SCU, 1: MD32 CFG
			P_Fld(0x1, LPIF_FSM_CFG_1_LPIF_POWER_CONTROL_SEL_2ND)); 		// 0: MD32 SCU, 1: MD32 CFG

	vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_FSM_OUT_CTRL_0,
			P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_PHYPLL_EN) |
			P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_DLL_EN) |
			P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_2ND_DLL_EN) |
			P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_DLL_CK_EN) |
			P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_VREF_EN) |
			P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_PHYPLL_SHU_EN) | // @Darren, fix dfs phypll init
			P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_PHYPLL_MODE_SW));

	u1ShuSramVal = u1CurrShuLevel;

	if (p->support_channel_num > 1)
		u1ShuSramVal |= u1CurrShuLevel << 4;

	// NOTE: MD32 PST mode shuffle level = (LPIF_CTRL_CTRL1_LPIF_DRAMC_DR_SHU_LEVEL_SRAM | LPIF_LOW_POWER_CFG_1_DR_SHU_SRAM_LEVEL)
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, u1ShuSramVal, LPIF_LOW_POWER_CFG_1_DR_SHU_SRAM_LEVEL);

#if __ETT__
	/* internal test mode */
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_SSPM_CFGREG_GPR0, 0xE7700E77, SSPM_CFGREG_GPR0_GPR0);
#endif

	// for DFS
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0x0, MISC_RG_DFS_CTRL_SPM_DVFS_CONTROL_SEL);
	vIO32WriteFldAlign_All(DDRPHY_REG_PHYPLL0, 0x0, PHYPLL0_RG_RPHYPLL_EN);
	vIO32WriteFldAlign_All(DDRPHY_REG_CLRPLL0, 0x0, CLRPLL0_RG_RCLRPLL_EN);

	// enable DFD
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_DFD_DBUG_0 , 0x1, LPIF_DFD_DBUG_0_LPIF_DFD_DEBUG_ISO_EN);
}

//-------------------------------------------------------------------------
/** TransferPLLToSPMControl
 *	1. Enable DVFS to SPM control
 *	2. Configs SPM pinmux
 *	3. To control PLL between PHYPLL and CLRPLL via SPM
 *	4. set current SRAM SHU index for SPM mode DFS latch/restore
 */
//-------------------------------------------------------------------------
void TransferPLLToSPMControl(DRAMC_CTX_T *p, U32 MD32Offset)
{
	//U8 u1EnMd32Ch = 0, i;
	//U16 u2SramLevel = 0;
	//DRAM_DFS_SRAM_SHU_T u1CurrShuLevel = vGet_Current_ShuLevel(p);

	/*for (i = 0; i < DPM_CH_NUM; i++)
	{
		u1EnMd32Ch |= (0x1 << i);
		u2SramLevel |= (u1CurrShuLevel << (4*i));
	}*/

	/*TINFO="DRAM : enter SW DVFS"*/
	//! To DFS SPM mode after calibration
	// Enable DVFS to SPM control
	/*vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0x0, MISC_RG_DFS_CTRL_SPM_DVFS_CONTROL_SEL);

	vIO32WriteFldMulti(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0+MD32Offset, P_Fld(0x3, LPIF_LOW_POWER_CFG_0_PHYPLL_EN)	// both channel phy pll en
			 | P_Fld(0x3, LPIF_LOW_POWER_CFG_0_DPY_DLL_EN) // both channel dpy pll en
			 | P_Fld(0x3, LPIF_LOW_POWER_CFG_0_DPY_2ND_DLL_EN) // both channel dpy 2nd pll en
			 | P_Fld(0x3, LPIF_LOW_POWER_CFG_0_DPY_DLL_CK_EN) // both channel dpy dll ck en
			 | P_Fld(0x3, LPIF_LOW_POWER_CFG_0_DPY_VREF_EN) // both channel dpy vref en
			 | P_Fld(0x3, LPIF_LOW_POWER_CFG_0_PHYPLL_SHU_EN) // @Darren, fix dfs phypll init
			 | P_Fld(0x3, LPIF_LOW_POWER_CFG_0_PHYPLL_MODE_SW)); // bith channel phypll mode sw*/

	// DFS trigger by DRAMC MD32 RG
	/*vIO32WriteFldMulti(DDRPHY_MD32_REG_LPIF_FSM_CFG_1+MD32Offset, P_Fld(0, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL)
		| P_Fld(0, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL_2ND)
		| P_Fld(0, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL_FOR_PWR)
		| P_Fld(0, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL_FOR_PWR_2ND)
		| P_Fld(1, LPIF_FSM_CFG_1_LPIF_OUTPUT_PATH_FROM_SW) // 1: MD32 RG mode, 0: MD32 PST mode
		| P_Fld(1, LPIF_FSM_CFG_1_LPIF_OUTPUT_PATH_FROM_SW_2ND));*/ // 1: MD32 RG mode, 0: MD32 PST mode

	mcSHOW_DBG_MSG(("TransferPLLToSPMControl - MODE SW "));

	if (p->u1PLLMode == PHYPLL_MODE)
	{
		/*mcSHOW_DBG_MSG(("PHYPLL\n"));
		vIO32WriteFldAlign(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0+MD32Offset, 0, LPIF_LOW_POWER_CFG_0_PHYPLL2_SHU_EN);
		vIO32WriteFldAlign(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0+MD32Offset, u1EnMd32Ch, LPIF_LOW_POWER_CFG_0_PHYPLL_SHU_EN);  // PHYPLL for part of SHU RG
		vIO32WriteFldAlign(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0+MD32Offset, 0, LPIF_LOW_POWER_CFG_0_PHYPLL2_MODE_SW); // same as DRAMC_DPY_CLK_SW_CON2_SW_PHYPLL2_MODE_SW by MUX
		vIO32WriteFldAlign(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0+MD32Offset, u1EnMd32Ch, LPIF_LOW_POWER_CFG_0_PHYPLL_MODE_SW);*/ // same as DRAMC_DPY_CLK_SW_CON2_SW_PHYPLL_MODE_SW by MUX
		vIO32WriteFldMulti(DDRPHY_MD32_REG_LPIF_FSM_OUT_CTRL_0+MD32Offset,
				P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_PHYPLL_EN) |
				P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_DLL_EN) |
				P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_2ND_DLL_EN) |
				P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_DLL_CK_EN) |
				P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_VREF_EN) |
				P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_PHYPLL_SHU_EN) | // @Darren, fix dfs phypll init
				P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_PHYPLL_MODE_SW));
	}
	else
	{
		/*mcSHOW_DBG_MSG(("CLRPLL\n"));
		vIO32WriteFldAlign(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0+MD32Offset, 0, LPIF_LOW_POWER_CFG_0_PHYPLL_SHU_EN);
		vIO32WriteFldAlign(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0+MD32Offset, u1EnMd32Ch, LPIF_LOW_POWER_CFG_0_PHYPLL2_SHU_EN);  // CLRPLL for part of SHU RG
		vIO32WriteFldAlign(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0+MD32Offset, 0, LPIF_LOW_POWER_CFG_0_PHYPLL_MODE_SW); // same as DRAMC_DPY_CLK_SW_CON2_SW_PHYPLL2_MODE_SW by MUX
		vIO32WriteFldAlign(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0+MD32Offset, u1EnMd32Ch, LPIF_LOW_POWER_CFG_0_PHYPLL2_MODE_SW);*/ // same as DRAMC_DPY_CLK_SW_CON2_SW_PHYPLL_MODE_SW by MUX
		vIO32WriteFldMulti(DDRPHY_MD32_REG_LPIF_FSM_OUT_CTRL_0+MD32Offset,
				P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_PHYPLL_EN) |
				P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_DLL_EN) |
				P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_2ND_DLL_EN) |
				P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_DLL_CK_EN) |
				P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_VREF_EN) |
				P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_PHYPLL2_SHU_EN) | // @Darren, fix dfs clrpll init
				P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_PHYPLL2_MODE_SW));
	}
	mcDELAY_US(1);

	//Darren-vIO32WriteFldAlign_All(DDRPHY_REG_PHYPLL0, 0x0, PHYPLL0_RG_RPHYPLL_EN);
	//Darren-vIO32WriteFldAlign_All(DDRPHY_REG_CLRPLL0, 0x0, CLRPLL0_RG_RCLRPLL_EN);

	//set current SRAM SHU index for SPM mode DFS latch/restore
	// @Darren for MD32 RG mode only
	// MD32 PST mode shuffle level = (LPIF_CTRL_CTRL1_LPIF_DRAMC_DR_SHU_LEVEL_SRAM | LPIF_LOW_POWER_CFG_1_DR_SHU_SRAM_LEVEL)
	//Darren-vIO32WriteFldAlign(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1+MD32Offset, u2SramLevel, LPIF_LOW_POWER_CFG_1_DR_SHU_SRAM_LEVEL);
	//mcSHOW_DBG_MSG(("TransferPLLToSPMControl - Current SRAM SHU LEVEL = %d\n", u1CurrShuLevel));

#if DFS_NOQUEUE_FLUSH_WA
	// Enable Max cnt for latency measure from shu_en to shu_ack
	vIO32WriteFldAlign(DDRPHY_MD32_REG_LPIF_FSM_CFG+MD32Offset, 1, LPIF_FSM_CFG_DBG_LATENCY_CNT_EN);
#endif

	// MD32 clock is 208M
	vIO32WriteFldMulti(DDRPHY_MD32_REG_SSPM_MCLK_DIV+MD32Offset,
			P_Fld(0, SSPM_MCLK_DIV_MCLK_SRC) |
			P_Fld(0, SSPM_MCLK_DIV_MCLK_DIV));
}
#endif


#if ENABLE_DVFS_BYPASS_MR13_FSP
void DFSBypassMR13HwSet(DRAMC_CTX_T *p)
{
#if __A60868_TO_BE_PORTING__
	U8 u1ShuffleIdx, BFSP = 0, u1SramShuIdx = 0;
	REG_TRANSFER_T TransferReg;

	TransferReg.u4Addr = DRAMC_REG_BYPASS_FSPOP;
	TransferReg.u4Fld = BYPASS_FSPOP_BPFSP_SET_SHU3;
	for (u1ShuffleIdx = 0; u1ShuffleIdx < DRAM_DFS_SRAM_MAX; u1ShuffleIdx++)
	{
		u1SramShuIdx = gFreqTbl[u1ShuffleIdx].shuffleIdx;
		switch (u1SramShuIdx)
		{
			case 0:
				TransferReg.u4Fld = BYPASS_FSPOP_BPFSP_SET_SHU0;
				break;
			case 1:
				TransferReg.u4Fld = BYPASS_FSPOP_BPFSP_SET_SHU1;
				break;
			case 2:
				TransferReg.u4Fld = BYPASS_FSPOP_BPFSP_SET_SHU2;
				break;
			case 3:
				TransferReg.u4Fld = BYPASS_FSPOP_BPFSP_SET_SHU3;
				break;
			case 4:
				TransferReg.u4Fld = BYPASS_FSPOP_BPFSP_SET_SHU4;
				break;
			case 5:
				TransferReg.u4Fld = BYPASS_FSPOP_BPFSP_SET_SHU5;
				break;
			case 6:
				TransferReg.u4Fld = BYPASS_FSPOP_BPFSP_SET_SHU6;
				break;
			case 7:
				TransferReg.u4Fld = BYPASS_FSPOP_BPFSP_SET_SHU7;
				break;
			case 8:
				TransferReg.u4Fld = BYPASS_FSPOP_BPFSP_SET_SHU8;
				break;
			case 9:
				TransferReg.u4Fld = BYPASS_FSPOP_BPFSP_SET_SHU9;
				break;
			default:
				mcSHOW_ERR_MSG(("[DFSBypassMR13HwSet] fail at BPFSP_SHU%d incorrect !!!\n", u1SramShuIdx));
				break;
		}
		BFSP = (gFreqTbl[u1ShuffleIdx].freq_sel <= LP4_DDR2667)? 0x1: 0x0; //0x1 (Bypass), 0x0 (Not bypass)
		//mcSHOW_DBG_MSG(("[DFSBypassMR13HwSet] BPFSP_SHU%d = 0x%x\n", u1SramShuIdx, BFSP));
		vIO32WriteFldAlign_All(TransferReg.u4Addr, BFSP, TransferReg.u4Fld);
	}
	vIO32WriteFldAlign_All(DRAMC_REG_TX_FREQ_RATIO_OLD_MODE0, 0x1, TX_FREQ_RATIO_OLD_MODE0_SHUFFLE_LEVEL_MODE_SELECT); // 1: shuffle level = 10, 0: shuffle level =4
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CDC_CTRL, 0x0, MISC_CDC_CTRL_REG_CDC_BYPASS_DBG);
	vIO32WriteFldAlign_All(DRAMC_REG_BYPASS_FSPOP, 0x1, BYPASS_FSPOP_BPFSP_OPT);
#endif
}
#endif

#if FOR_DV_SIMULATION_USED
void DFSSwitchtoRGMode(DRAMC_CTX_T *p)
{
	vIO32WriteFldAlign(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_SPM_DVFS_CONTROL_SEL);
}
#endif

void DramcSaveToShuffleSRAM(DRAMC_CTX_T *p, DRAM_DFS_SHUFFLE_TYPE_T srcRG, DRAM_DFS_SHUFFLE_TYPE_T dstRG)
{
	U8 u1ChIdx;
	U8 u1value;
	DRAM_CHANNEL_T eOriChannel = vGetPHY2ChannelMapping(p);

	#if ENABLE_SRAM_DMA_WA
	DPHYSaveToSRAMShuWA(p, p->pDFSTable->shuffleIdx);
	#endif

	for (u1ChIdx = 0; u1ChIdx < p->support_channel_num; u1ChIdx++)
	{
		vSetPHY2ChannelMapping(p, u1ChIdx);

		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 0, MISC_SRAM_DMA0_SW_DMA_FIRE);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 0, MISC_SRAM_DMA0_APB_SLV_SEL);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 1, MISC_SRAM_DMA0_SW_MODE);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 1, MISC_SRAM_DMA0_SW_STEP_EN_MODE);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 1, MISC_SRAM_DMA0_SRAM_WR_MODE);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 0, MISC_SRAM_DMA0_APB_WR_MODE);

		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), srcRG, MISC_SRAM_DMA0_SW_SHU_LEVEL_APB);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), dstRG, MISC_SRAM_DMA0_SW_SHU_LEVEL_SRAM);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 1, MISC_SRAM_DMA0_SW_DMA_FIRE);
		do {
			u1value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DMA_DEBUG0), MISC_DMA_DEBUG0_SRAM_DONE);
			u1value |= (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DMA_DEBUG0), MISC_DMA_DEBUG0_APB_DONE) << 1);
			mcSHOW_DBG_MSG3(("\twait dramc to shuffle sram done.\n"));
		} while (u1value != 0x3);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 0, MISC_SRAM_DMA0_SW_DMA_FIRE);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 0, MISC_SRAM_DMA0_SW_STEP_EN_MODE);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 0, MISC_SRAM_DMA0_SW_MODE);
	}

	vSetPHY2ChannelMapping(p, eOriChannel);
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA0, 0, MISC_SRAM_DMA0_SRAM_WR_MODE); //MP setting:should disable WR MDOE
}

void LoadShuffleSRAMtoDramc(DRAMC_CTX_T *p, DRAM_DFS_SHUFFLE_TYPE_T srcRG, DRAM_DFS_SHUFFLE_TYPE_T dstRG)
{
	U8 u1ChIdx;
	U8 u1value;
	DRAM_CHANNEL_T eOriChannel = vGetPHY2ChannelMapping(p);

	for (u1ChIdx = 0; u1ChIdx < p->support_channel_num; u1ChIdx++)
	{
		vSetPHY2ChannelMapping(p, u1ChIdx);

		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 0, MISC_SRAM_DMA0_SW_DMA_FIRE);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 0, MISC_SRAM_DMA0_APB_SLV_SEL);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 1, MISC_SRAM_DMA0_SW_MODE);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 1, MISC_SRAM_DMA0_SW_STEP_EN_MODE);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 0, MISC_SRAM_DMA0_SRAM_WR_MODE); //diff with DramcSaveToShuffleSRAM
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 1, MISC_SRAM_DMA0_APB_WR_MODE); // diff with DramcSaveToShuffleSRAM

		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), dstRG, MISC_SRAM_DMA0_SW_SHU_LEVEL_APB);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), srcRG, MISC_SRAM_DMA0_SW_SHU_LEVEL_SRAM);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 1, MISC_SRAM_DMA0_SW_DMA_FIRE);
		do {
			u1value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DMA_DEBUG0), MISC_DMA_DEBUG0_SRAM_DONE);
			u1value |= (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DMA_DEBUG0), MISC_DMA_DEBUG0_APB_DONE) << 1);
			mcSHOW_DBG_MSG3(("\twait shuffle sram to dramc done.\n"));
		} while (u1value != 0x3);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 0, MISC_SRAM_DMA0_SW_DMA_FIRE);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 0, MISC_SRAM_DMA0_SW_STEP_EN_MODE);
		vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 0, MISC_SRAM_DMA0_SW_MODE);
	}

	vSetPHY2ChannelMapping(p, eOriChannel);
}

static U8 WaitChShuEnAck(DRAMC_CTX_T *p, U32 u4Addr, U32 u4Fld, U8 u1Status)
{
	U8 u1WaitShuAckState = 0, u1ChIdx = 0, u1AckDone = 0;
	DRAM_CHANNEL_T eOriChannel = vGetPHY2ChannelMapping(p);

	for (u1ChIdx = CHANNEL_A; u1ChIdx < p->support_channel_num; u1ChIdx++)
	{
		vSetPHY2ChannelMapping(p, u1ChIdx);

		do {
			u1WaitShuAckState = u4IO32ReadFldAlign(DRAMC_REG_ADDR(u4Addr), u4Fld);
			//mcSHOW_DBG_MSG(("[WaitChShuEnAck] Wait Shu Ack State = 0x%x\n", u1WaitShuAckState));
			if (u1WaitShuAckState == u1Status)
				break;
		} while(1);

		u1AckDone |= (0x1 << u1ChIdx);
	}
	vSetPHY2ChannelMapping(p, eOriChannel);

	return u1AckDone; // shu end
}

void DramcDFSDirectJump_SRAMShuRGMode(DRAMC_CTX_T *p, U8 shu_level)
{
	U8 u1ShuAck = 0;
	U8 i = 0;
	U8 u1ChkComplete = 1;

	if (p->u1PLLMode == PHYPLL_MODE)
	{
		mcSHOW_DBG_MSG3(("Disable CLRPLL\n"));
		vIO32WriteFldAlign_All(DDRPHY_REG_CLRPLL0, 0, CLRPLL0_RG_RCLRPLL_EN);
	}
	else
	{
		mcSHOW_DBG_MSG3(("Disable PHYPLL\n"));
		vIO32WriteFldAlign_All(DDRPHY_REG_PHYPLL0, 0, PHYPLL0_RG_RPHYPLL_EN);
	}

	for (i = 0; i < p->support_channel_num; i++)
	{
		u1ShuAck |= (0x1 << i);
	}

	if (p->u1PLLMode == PHYPLL_MODE)
	{
		mcSHOW_DBG_MSG3(("DFSDirectJump to CLRPLL, SHU_LEVEL=%d, ACK=%x\n", shu_level, u1ShuAck));
	}
	else
	{
		mcSHOW_DBG_MSG3(("DFSDirectJump to PHYPLL, SHU_LEVEL=%d, ACK=%x\n", shu_level, u1ShuAck));
	}

	/*TINFO="DRAM : set ddrphy_fb_ck_en=1"*/
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_DDRPHY_FB_CK_EN);

	// sram latch
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_DR_SHU_LEVEL_SRAM_LATCH);
	mcDELAY_US(1);
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_DR_SHU_LEVEL_SRAM_LATCH);

	if (p->u1PLLMode == PHYPLL_MODE)
	{
		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_PHYPLL_SHU_EN);
		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, !p->u1PLLMode, MISC_RG_DFS_CTRL_RG_DR_SHU_LEVEL);
		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_PHYPLL2_SHU_EN);
		mcSHOW_DBG_MSG3(("Enable CLRPLL\n"));
	}
	else
	{
		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_PHYPLL2_SHU_EN);
		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, !p->u1PLLMode, MISC_RG_DFS_CTRL_RG_DR_SHU_LEVEL);
		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_PHYPLL_SHU_EN);
		mcSHOW_DBG_MSG3(("Enable PHYPLL\n"));
	}
	mcDELAY_US(1);

#if 1 //Darren-
	//vIO32WriteFldMulti((DDRPHY_MISC_SPM_CTRL3), P_Fld(0, MISC_SPM_CTRL3_RG_DR_SHU_LEVEL_SRAM_CH1)
	//					  | P_Fld(0, MISC_SPM_CTRL3_RG_DR_SHU_LEVEL_SRAM_CH0));
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, shu_level, MISC_RG_DFS_CTRL_RG_DR_SHU_LEVEL_SRAM);

	//wait sram load ack.
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_DR_SRAM_LOAD);
	//while (!u4IO32ReadFldAlign(DDRPHY_MISC_DMA_DEBUG0, MISC_DMA_DEBUG0_SC_DR_SRAM_PLL_LOAD_ACK)); // wait SRAM PLL load ack
	while (WaitChShuEnAck(p, DDRPHY_REG_MISC_DMA_DEBUG0, MISC_DMA_DEBUG0_SC_DR_SRAM_LOAD_ACK, u1ChkComplete) != u1ShuAck)
	//while (!u4IO32ReadFldAlign(DDRPHY_REG_MISC_DMA_DEBUG0, MISC_DMA_DEBUG0_SC_DR_SRAM_LOAD_ACK))
	{
		mcSHOW_DBG_MSG3(("\twait sram load ack.\n"));
	}
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_DR_SRAM_LOAD);
#endif

	if (p->u1PLLMode == PHYPLL_MODE)
	{
		//vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 1, MISC_SPM_CTRL1_RG_PHYPLL2_MODE_SW);
		vIO32WriteFldAlign_All(DDRPHY_REG_CLRPLL0, 1, CLRPLL0_RG_RCLRPLL_EN); // Darren NOTE: Don't use PHYPLLx_MODE_SW and it will lock RCLRPLL_EN and RPHYPLL_EN control
	}
	else
	{
		//vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 1, MISC_SPM_CTRL1_RG_PHYPLL_MODE_SW);
		vIO32WriteFldAlign_All(DDRPHY_REG_PHYPLL0, 1, PHYPLL0_RG_RPHYPLL_EN); // Darren NOTE: Don't use PHYPLLx_MODE_SW and it will lock RCLRPLL_EN and RPHYPLL_EN control
	}

#if DFS_NOQUEUE_FLUSH_WA
	NoQueueFlushWA(p, ENABLE);
#endif

#if ENABLE_SRAM_DMA_WA
	SRAMShuRestoreToDPHYWA(p, shu_level, !p->u1PLLMode);
#endif

	#if 0//ENABLE_DFS_DEBUG_MODE
	EntryDFSDebugMode(p, CHG_CLK_MODE);
	#endif

#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
	mcDELAY_US(20); // for SRAM shuffle DV sim spec > 20us
#else
	mcDELAY_XUS(20); // for SRAM shuffle DV sim spec > 20us
#endif

#if 0
	mcSHOW_DBG_MSG3(("Enable SHORT-QUEUE\n"));
	vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 1, MISC_SPM_CTRL1_RG_DR_SHORT_QUEUE);

	mcSHOW_DBG_MSG3(("\twait 5us for short queue ack.\n"));
	mcDELAY_US(5);
#endif

	//mcSHOW_DBG_MSG(("Disable RX-Tracking\n"));
	//vIO32WriteFldAlign(SPM_SW_RSV_8, 0, SW_RSV_8_RX_TRACKING_EN);

	mcSHOW_DBG_MSG3(("SHUFFLE Start\n"));
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_DR_SHU_EN); // NOTE: from SHU_EN=1 to ACK, DV spec < 5.1us

#if DFS_NOQUEUE_FLUSH_WA && ENABLE_DFS_NOQUEUE_FLUSH_DBG
	WaitNoQueueFlushComplete(p); // for debug mode MRW skip
#endif

	// Fixed DV sim spec for DFS shu_en=1 < 5.1us and shu_en=0 < 120ns
#if 1//Darren-for test chip(FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
	//mcSHOW_DBG_MSG3(("\twait 5us for shu_en ack.\n"));
	//mcDELAY_US(5);
	#if 0//ENABLE_DFS_DEBUG_MODE
	ChkDFSDebugMode(p, CHG_CLK_MODE);
	#endif

	//while (WaitChShuEnAck(p, DRAMC_REG_MRR_STATUS2, MRR_STATUS2_DVFS_STATE, u1ShuAckState) != u1ShuAck) // SHUFFLE_END
	//@tg Fix RG mode can not recevie shuffle end ack.
	while ((u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4, LPIF_STATUS_4_DR_SHU_EN_ACK) & u1ShuAck) != u1ShuAck
#if CHANNEL_NUM > 2
	|| (u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4+SHIFT_TO_CHB_ADDR, LPIF_STATUS_4_DR_SHU_EN_ACK) & u1ShuAck) != u1ShuAck
#endif
	)
	{
		mcSHOW_DBG_MSG3(("\twait shu_en ack.\n"));
	}
#else
	while (u4IO32ReadFldAlign(DRAMC_REG_MRR_STATUS2, MRR_STATUS2_DVFS_STATE) != u1ShuAckState); // SHUFFLE_END
#endif

	#if 0//ENABLE_DFS_DEBUG_MODE
	ExitDFSDebugMode(p, CHG_CLK_MODE);
	#endif

#if ENABLE_TX_REBASE_WDQS_DQS_PI_WA
	TxReBaseWDQSDqsPiWA(p, !p->u1PLLMode);
#endif

#if ENABLE_TX_REBASE_ODT_WA
	TxReadBaseODTWA(p, shu_level);
#endif

	//vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 0, MISC_SPM_CTRL1_RG_DR_SHORT_QUEUE);
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_DR_SHU_EN); // NOTE: from ACK to SHU_EN=0, DV spec < 120ns
	mcSHOW_DBG_MSG3(("SHUFFLE End\n"));

	//if(shu_level == 0)//LP4-2CH
	//{
		//mcSHOW_DBG_MSG(("Enable RX-Tracking for shuffle-0\n"));
		//vIO32WriteFldAlign(SPM_SW_RSV_8, 3, SW_RSV_8_RX_TRACKING_EN);
	//}

	if (p->u1PLLMode == PHYPLL_MODE)
	{
		/*TINFO="DRAM : set sc_phypll_mode_sw=0"*/
		//vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 0, MISC_SPM_CTRL1_RG_PHYPLL_MODE_SW); // Disable PHYPLL
		vIO32WriteFldAlign_All(DDRPHY_REG_PHYPLL0, 0, PHYPLL0_RG_RPHYPLL_EN); // Darren NOTE: Don't use PHYPLLx_MODE_SW and it will lock RCLRPLL_EN and RPHYPLL_EN control
	}
	else
	{
		/*TINFO="DRAM : set sc_phypll2_mode_sw=0"*/
		//vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 0, MISC_SPM_CTRL1_RG_PHYPLL2_MODE_SW); // Disable CLRPLL
		vIO32WriteFldAlign_All(DDRPHY_REG_CLRPLL0, 0, CLRPLL0_RG_RCLRPLL_EN); // Darren NOTE: Don't use PHYPLLx_MODE_SW and it will lock RCLRPLL_EN and RPHYPLL_EN control
	}

#if 1 //Darren-
	//wait sram restore ack.
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_DR_SRAM_RESTORE);
	while (WaitChShuEnAck(p, DDRPHY_REG_MISC_DMA_DEBUG0, MISC_DMA_DEBUG0_SC_DR_SRAM_RESTORE_ACK, u1ChkComplete) != u1ShuAck)
	//while (!u4IO32ReadFldAlign(DDRPHY_REG_MISC_DMA_DEBUG0, MISC_DMA_DEBUG0_SC_DR_SRAM_RESTORE_ACK))
	{
		mcSHOW_DBG_MSG3(("\twait sram restore ack.\n"));
	}
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_DR_SRAM_RESTORE);

#if DFS_NOQUEUE_FLUSH_WA
	NoQueueFlushWA(p, DISABLE);
#endif

	/*TINFO="DRAM : set ddrphy_fb_ck_en=0"*/
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_DDRPHY_FB_CK_EN);
#endif

	#if ENABLE_TIMING_TXSR_DFS_WA
	TimingTxsrWA(p, shu_level);
	#endif

	mcSHOW_DBG_MSG3(("Shuffle flow complete\n"));

	p->u1PLLMode = !p->u1PLLMode;
	return;
}


void DramcDFSDirectJump_RGMode(DRAMC_CTX_T *p, U8 shu_level)
{
	U8 u1ShuAck = 0;
	U8 i = 0;
	U8 u1shu_level = 0;

	if (p->u1PLLMode == PHYPLL_MODE)
	{
		mcSHOW_DBG_MSG3(("Disable CLRPLL\n"));
		vIO32WriteFldAlign_All(DDRPHY_REG_CLRPLL0, 0, CLRPLL0_RG_RCLRPLL_EN);
	}
	else
	{
		mcSHOW_DBG_MSG3(("Disable PHYPLL\n"));
		vIO32WriteFldAlign_All(DDRPHY_REG_PHYPLL0, 0, PHYPLL0_RG_RPHYPLL_EN);
	}

	for (i = 0; i < p->support_channel_num; i++)
	{
		u1ShuAck |= (0x1 << i);
	}

	if (p->u1PLLMode == PHYPLL_MODE)
	{
		mcSHOW_DBG_MSG3(("DFSDirectJump_RGMode to CLRPLL, SHU_LEVEL=%d, ACK=%x\n", shu_level, u1ShuAck));
	}
	else
	{
		mcSHOW_DBG_MSG3(("DFSDirectJump_RGMode to PHYPLL, SHU_LEVEL=%d, ACK=%x\n", shu_level, u1ShuAck));
	}

	/*TINFO="DRAM : set ddrphy_fb_ck_en=1"*/
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_DDRPHY_FB_CK_EN);

	if (shu_level == DRAM_DFS_SHUFFLE_1)
		u1shu_level = shu_level; // Darren: shuffle to shu0 status (original calib flow.)
	else
		u1shu_level = 1; // Darren: Using shu1 for backup/restore, it diff with SPM mode

	if (p->u1PLLMode == PHYPLL_MODE)
	{
		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_PHYPLL_SHU_EN);
		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, u1shu_level, MISC_RG_DFS_CTRL_RG_DR_SHU_LEVEL);
		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_PHYPLL2_SHU_EN);
		mcSHOW_DBG_MSG3(("Enable CLRPLL\n"));
	}
	else
	{
		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_PHYPLL2_SHU_EN);
		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, u1shu_level, MISC_RG_DFS_CTRL_RG_DR_SHU_LEVEL);
		vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_PHYPLL_SHU_EN);
		mcSHOW_DBG_MSG3(("Enable PHYPLL\n"));
	}
	mcDELAY_US(1);

	if (p->u1PLLMode == PHYPLL_MODE)
	{
		//vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 1, MISC_SPM_CTRL1_RG_PHYPLL2_MODE_SW);
		vIO32WriteFldAlign_All(DDRPHY_REG_CLRPLL0, 1, CLRPLL0_RG_RCLRPLL_EN); // Darren NOTE: Don't use PHYPLLx_MODE_SW and it will lock RCLRPLL_EN and RPHYPLL_EN control
	}
	else
	{
		//vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 1, MISC_SPM_CTRL1_RG_PHYPLL_MODE_SW);
		vIO32WriteFldAlign_All(DDRPHY_REG_PHYPLL0, 1, PHYPLL0_RG_RPHYPLL_EN); // Darren NOTE: Don't use PHYPLLx_MODE_SW and it will lock RCLRPLL_EN and RPHYPLL_EN control
	}

#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
	mcDELAY_US(20); // for SRAM shuffle DV sim spec > 20us
#else
	mcDELAY_XUS(20); // for SRAM shuffle DV sim spec > 20us
#endif

#if 0
	mcSHOW_DBG_MSG3(("Enable SHORT-QUEUE\n"));
	vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 1, MISC_SPM_CTRL1_RG_DR_SHORT_QUEUE);

	mcSHOW_DBG_MSG3(("\twait 5us for short queue ack.\n"));
	mcDELAY_US(5);
#endif

	//mcSHOW_DBG_MSG(("Disable RX-Tracking\n"));
	//vIO32WriteFldAlign(SPM_SW_RSV_8, 0, SW_RSV_8_RX_TRACKING_EN);


	mcSHOW_DBG_MSG3(("SHUFFLE Start\n"));
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_DR_SHU_EN);

	//mcSHOW_DBG_MSG3(("\twait 5us for shu_en ack.\n"));
	//mcDELAY_US(5);
	//while (WaitChShuEnAck(p, DRAMC_REG_MRR_STATUS2, MRR_STATUS2_DVFS_STATE, u1ShuAckState) != u1ShuAck) // SHUFFLE_END
	//@tg Fix RG mode can not recevie shuffle end ack.
	while ((u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4, LPIF_STATUS_4_DR_SHU_EN_ACK) & u1ShuAck) != u1ShuAck
#if CHANNEL_NUM > 2
	|| (u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4+SHIFT_TO_CHB_ADDR, LPIF_STATUS_4_DR_SHU_EN_ACK) & u1ShuAck) != u1ShuAck
#endif
	)
	{
		mcSHOW_DBG_MSG3(("\twait shu_en ack.\n"));
	}

	//vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 0, MISC_SPM_CTRL1_RG_DR_SHORT_QUEUE);
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_DR_SHU_EN);
	mcSHOW_DBG_MSG3(("SHUFFLE End\n"));

	//if(shu_level == 0)//LP4-2CH
	//{
		//mcSHOW_DBG_MSG(("Enable RX-Tracking for shuffle-0\n"));
		//vIO32WriteFldAlign(SPM_SW_RSV_8, 3, SW_RSV_8_RX_TRACKING_EN);
	//}

	if (p->u1PLLMode == PHYPLL_MODE)
	{
		/*TINFO="DRAM : set sc_phypll_mode_sw=0"*/
		//vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 0, MISC_SPM_CTRL1_RG_PHYPLL_MODE_SW); // Disable PHYPLL
		vIO32WriteFldAlign_All(DDRPHY_REG_PHYPLL0, 0, PHYPLL0_RG_RPHYPLL_EN); // Darren NOTE: Don't use PHYPLLx_MODE_SW and it will lock RCLRPLL_EN and RPHYPLL_EN control
	}
	else
	{
		/*TINFO="DRAM : set sc_phypll2_mode_sw=0"*/
		//vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 0, MISC_SPM_CTRL1_RG_PHYPLL2_MODE_SW); // Disable CLRPLL
		vIO32WriteFldAlign_All(DDRPHY_REG_CLRPLL0, 0, CLRPLL0_RG_RCLRPLL_EN); // Darren NOTE: Don't use PHYPLLx_MODE_SW and it will lock RCLRPLL_EN and RPHYPLL_EN control
	}

	/*TINFO="DRAM : set ddrphy_fb_ck_en=0"*/
	vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_DDRPHY_FB_CK_EN);

	mcSHOW_DBG_MSG3(("Shuffle flow complete\n"));

	p->u1PLLMode = !p->u1PLLMode;
	return;
}

static void DramcDFSDirectJump_SPMMode(DRAMC_CTX_T *p, U8 shu_level)
{
	U8 u1ShuAck = 0, u1EnMd32Ch = 0;
	U8 i = 0;
	U8 pingpong_shu_level = 0; // for shu0/1
	U8 u1PingPong = 0;
	U16 u2SramLevel = 0;
#if ENABLE_DFS_RUNTIME_MRW
	U8 cur_fsp = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SA_RESERVE), SA_RESERVE_DFS_FSP_RTMRW);
#endif

	for (i = 0; i < DPM_CH_NUM; i++)
	{
		u1ShuAck |= (0x1 << i);
		u1EnMd32Ch |= (0x1 << i);
	}

	if (p->u1PLLMode == PHYPLL_MODE)
	{
		mcSHOW_DBG_MSG3(("DramcDFSDirectJump_SPMMode to CLRPLL, SHU_LEVEL=%d, ACK=%x\n", shu_level, u1ShuAck));
	}
	else
	{
		mcSHOW_DBG_MSG3(("DramcDFSDirectJump_SPMMode to PHYPLL, SHU_LEVEL=%d, ACK=%x\n", shu_level, u1ShuAck));
	}

	//vIO32WriteFldAlign(DDRPHY_REG_MISC_STBCAL2, 0x1, MISC_STBCAL2_STB_DBG_STATUS); // HJ Huang
	/*TINFO="DRAM : set ddrphy_fb_ck_en=1"*/
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnMd32Ch, LPIF_LOW_POWER_CFG_0_DDRPHY_FB_CK_EN);
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_2, u1EnMd32Ch, LPIF_LOW_POWER_CFG_2_DR_SHU_LEVEL_SRAM_LATCH);
	mcDELAY_US(1);
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_2, 0, LPIF_LOW_POWER_CFG_2_DR_SHU_LEVEL_SRAM_LATCH);

	//LPIF_STATUS_10_DRAMC_DR_SHU_LEVEL[1:0] for CHA
	//LPIF_STATUS_10_DRAMC_DR_SHU_LEVEL[3:2] for CHB
	pingpong_shu_level = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_10, LPIF_STATUS_10_DRAMC_DR_SHU_LEVEL); // read shuffle level for dramc conf0/1
	mcSHOW_DBG_MSG3(("Ping-pong CONF%d\n", (pingpong_shu_level & 0x1)));
	for (i = 0; i < DPM_CH_NUM; i++)
	{
		u2SramLevel |= (shu_level << (i*4));
		u1PingPong |= (!((pingpong_shu_level >> (i*2)) & 0x1)) << (i*2);
	}
	pingpong_shu_level = u1PingPong;

	if (p->u1PLLMode == PHYPLL_MODE)
	{
		vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_PHYPLL_SHU_EN);
		vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, pingpong_shu_level, LPIF_LOW_POWER_CFG_1_DR_SHU_LEVEL);
		vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnMd32Ch, LPIF_LOW_POWER_CFG_0_PHYPLL2_SHU_EN);
		mcSHOW_DBG_MSG3(("Enable CLRPLL (0x%x 0x%x)\n", pingpong_shu_level, u2SramLevel));
	}
	else
	{
		vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_PHYPLL2_SHU_EN);
		vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, pingpong_shu_level, LPIF_LOW_POWER_CFG_1_DR_SHU_LEVEL);
		vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnMd32Ch, LPIF_LOW_POWER_CFG_0_PHYPLL_SHU_EN);
		mcSHOW_DBG_MSG3(("Enable PHYPLL (0x%x 0x%x)\n", pingpong_shu_level, u2SramLevel));
	}
	mcDELAY_US(1);

#if ENABLE_DFS_RUNTIME_MRW
	DFSRuntimeMRWEn(p, p->u1PLLMode, shu_level, cur_fsp);
#endif

#if 0 //Darren test+
	vIO32WriteFldAlign(SPM_SPM_POWER_ON_VAL0, 0, SPM_POWER_ON_VAL0_SC_DR_SHU_LEVEL);
	vIO32WriteFldAlign(SPM_SPM_POWER_ON_VAL0, shu_level, SPM_POWER_ON_VAL0_SC_DR_SHU_LEVEL);
#else
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, u2SramLevel, LPIF_LOW_POWER_CFG_1_DR_SHU_SRAM_LEVEL);
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, u1EnMd32Ch, LPIF_LOW_POWER_CFG_1_DR_SRAM_LOAD);
	while ((u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4, LPIF_STATUS_4_DR_SRAM_LOAD_ACK) & u1ShuAck) != u1ShuAck
#if CHANNEL_NUM > 2
	|| (u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4+SHIFT_TO_CHB_ADDR, LPIF_STATUS_4_DR_SRAM_LOAD_ACK) & u1ShuAck) != u1ShuAck
#endif
	)
	{
		mcSHOW_DBG_MSG3(("\twait sram load ack.\n"));
	}
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 0, LPIF_LOW_POWER_CFG_1_DR_SRAM_LOAD);
#endif

	//vIO32WriteFldAlign(DDRPHY_REG_MISC_STBCAL2, 0x2, MISC_STBCAL2_STB_DBG_STATUS);

#if ENABLE_DFS_SSC_WA
	DVS_DMY_RD_ENTR(p);
#endif

	if (p->u1PLLMode == PHYPLL_MODE)
		vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnMd32Ch, LPIF_LOW_POWER_CFG_0_PHYPLL2_MODE_SW);
	else
		vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnMd32Ch, LPIF_LOW_POWER_CFG_0_PHYPLL_MODE_SW);

#if ENABLE_DFS_SSC_WA
	DramcSSCHoppingOnOff(p, cur_shu_level, ENABLE);
#endif

#if DFS_NOQUEUE_FLUSH_WA
	NoQueueFlushWA(p, ENABLE);
#endif

#if ENABLE_SRAM_DMA_WA
	SRAMShuRestoreToDPHYWA(p, shu_level, !p->u1PLLMode);
#endif

#if ENABLE_DFS_RUNTIME_MRW
	DFSRTMRW_HwsetWA(p, p->u1PLLMode, !p->u1PLLMode, cur_fsp);
#endif

	DFSHwSetWA(p, p->u1PLLMode, shu_level);
	#if ENABLE_DFS_DEBUG_MODE
	EntryDFSDebugMode(p, CHG_CLK_MODE);
	#endif

	mcDELAY_US(20);

	/*TINFO="DRAM : set ddrphy_fb_ck_en=0"*/
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_DDRPHY_FB_CK_EN);

	//EnableDramcTrackingByShuffle(p, shu_level, DISABLE);

	/*TINFO="DRAM : set ddrphy_fb_ck_en=1"*/
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnMd32Ch, LPIF_LOW_POWER_CFG_0_DDRPHY_FB_CK_EN);

	//func_imp_tracking_value_backup();
	//func_imp_tracking_off();
	//func_force_mm_ultra();

#if ENABLE_DFS_SSC_WA
	DVS_DMY_RD_EXIT(p);
	//DramcSSCHoppingOnOff(p, cur_shu_level, ENABLE); // for waveform measure
	//mcDELAY_US(10); // for waveform measure
#endif

#if ENABLE_DDR800_OPEN_LOOP_MODE_OPTION
	DDR800semiPowerSavingOn(p, shu_level, DISABLE);
#endif

#if (ENABLE_TX_TRACKING && TX_RETRY_ENABLE)
	SPMTx_Track_Retry_OnOff(p, shu_level, ENABLE);
#endif

	mcSHOW_DBG_MSG3(("SHUFFLE Start\n"));
	//vIO32WriteFldAlign(DDRPHY_REG_MISC_STBCAL2, 0x3, MISC_STBCAL2_STB_DBG_STATUS);
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnMd32Ch, LPIF_LOW_POWER_CFG_0_DR_SHU_EN);

	#if ENABLE_DFS_DEBUG_MODE
	ChkDFSDebugMode(p, CHG_CLK_MODE);
	// Add WA at here
	ExitDFSDebugMode(p, CHG_CLK_MODE);
	#endif

	while ((u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4, LPIF_STATUS_4_DR_SHU_EN_ACK) & u1ShuAck) != u1ShuAck
#if CHANNEL_NUM > 2
	|| (u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4+SHIFT_TO_CHB_ADDR, LPIF_STATUS_4_DR_SHU_EN_ACK) & u1ShuAck) != u1ShuAck
#endif
	)
	{
		mcSHOW_DBG_MSG3(("\twait shu_en ack.\n"));
	}

#if DFS_NOQUEUE_FLUSH_LATENCY_CNT
	U8 MaxCnt = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_RESERVED_6, LPIF_RESERVED_6_MAX_CNT_SHU_EN_HIGH_TO_ACK); // show chx max cnt
	// cnt * 8 * 4.8ns (208M)
	mcSHOW_DBG_MSG(("\tMAX CNT = %d\n", MaxCnt));
#endif

#if ENABLE_TX_REBASE_WDQS_DQS_PI_WA
	TxReBaseWDQSDqsPiWA(p, !p->u1PLLMode);
#endif

#if ENABLE_TX_REBASE_ODT_WA
	TxReadBaseODTWA(p, shu_level);
#endif
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_DR_SHU_EN);
	//vIO32WriteFldAlign(DDRPHY_REG_MISC_STBCAL2, 0x4, MISC_STBCAL2_STB_DBG_STATUS);
	mcSHOW_DBG_MSG3(("SHUFFLE End\n"));

	if (p->u1PLLMode == PHYPLL_MODE)
		vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_PHYPLL_MODE_SW); // PHYPLL off
	else
		vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_PHYPLL2_MODE_SW); // CLRPLL off

#if ENABLE_DDR800_OPEN_LOOP_MODE_OPTION
	DDR800semiPowerSavingOn(p, shu_level, ENABLE);
#endif

#if (ENABLE_TX_TRACKING && TX_RETRY_ENABLE)
	SPMTx_Track_Retry_OnOff(p, shu_level, DISABLE);
#endif

#if ENABLE_DFS_SSC_WA
	DramcSSCHoppingOnOff(p, cur_shu_level, DISABLE);
#endif

	//func_imp_tracking_on();
#if 1 //Darren test+
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_2, u1EnMd32Ch, LPIF_LOW_POWER_CFG_2_DR_SRAM_RESTORE);
	while ((u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4, LPIF_STATUS_4_DR_SRAM_RESTORE_ACK) & u1ShuAck) != u1ShuAck
#if CHANNEL_NUM > 2
	|| (u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4+SHIFT_TO_CHB_ADDR, LPIF_STATUS_4_DR_SRAM_RESTORE_ACK) & u1ShuAck) != u1ShuAck
#endif
	)
	{
		mcSHOW_DBG_MSG3(("\twait sram restore ack.\n"));
	}
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_2, 0, LPIF_LOW_POWER_CFG_2_DR_SRAM_RESTORE);
#endif

#if DFS_NOQUEUE_FLUSH_WA
	NoQueueFlushWA(p, DISABLE);
#endif

	/*TINFO="DRAM : set ddrphy_fb_ck_en=0"*/
	vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_DDRPHY_FB_CK_EN);
	//Darren-EnableDramcTrackingByShuffle(p, shu_level, ENABLE);

	//-----------------------------------
	//	   TRIGGER DRAM GATING ERROR
	//-----------------------------------
	//func_dram_dummy_read_on();
	//mcDELAY_US(2);
	//func_dram_dummy_read_off();

	p->u1PLLMode = !p->u1PLLMode;

	//vIO32WriteFldAlign(DDRPHY_REG_MISC_STBCAL2, 0x5, MISC_STBCAL2_STB_DBG_STATUS);
	mcSHOW_DBG_MSG3(("Shuffle flow complete\n"));

	return;
}

void DramcDFSDirectJump(DRAMC_CTX_T *p, U8 shu_level)
{
#if (DRAMC_DFS_MODE == 2)
	gDVFSCtrlSel = 2; // SRAM RG mode
#elif (DRAMC_DFS_MODE == 1)
	gDVFSCtrlSel = 1; // MD32
#elif (DRAMC_DFS_MODE == 0)
	gDVFSCtrlSel = 0; // Legacy mode
#endif

	if (gDVFSCtrlSel == 0)
	{
		if (shu_level == SRAM_SHU0) // DDR4266
			DramcDFSDirectJump_RGMode(p, 0); // Legacy mode for CONF0
		else
			DramcDFSDirectJump_RGMode(p, 1); // Legacy mode for CONF1
	}
	else if (gDVFSCtrlSel == 1)
	{
		DramcDFSDirectJump_SPMMode(p, shu_level);
	}
	else
	{
		DramcDFSDirectJump_SRAMShuRGMode(p, shu_level);
	}
}

void ShuffleDfsToFSP1(DRAMC_CTX_T *p)
{
	U8 operating_fsp = p->dram_fsp;

	// Support single rank and dual ranks
	// Double confirm from CLRPLL to PHYPLL
	if (operating_fsp == FSP_1)
	{
		cbt_dfs_mr13_global(p, CBT_HIGH_FREQ);
		cbt_switch_freq(p, CBT_HIGH_FREQ);
	}
}

