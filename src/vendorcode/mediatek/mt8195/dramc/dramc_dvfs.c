/* SPDX-License-Identifier: BSD-3-Clause */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "dramc_common.h"
#include "dramc_int_global.h"
#include "x_hal_io.h"
#include "dramc_top.h"

#define SHUFFLE_GROUP   4               //SHU1~4

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

#if DUMP_ALLSUH_RG
    mcSHOW_DBG_MSG(("\n[dumpRG] vSetDFSFreqSelByTable Freq= %d, SRAM SHU%d\n", p->frequency, p->pDFSTable->SRAMIdx));
    //mcSHOW_DBG_MSG3(("\n[dumpRG] vSetDFSFreqSelByTable Freq= %d, SRAM SHU%d\n", p->frequency, p->pDFSTable->shuffleIdx));
#endif
}

DRAM_DFS_FREQUENCY_TABLE_T* get_FreqTbl_by_SRAMIndex(DRAMC_CTX_T *p, DRAM_DFS_SRAM_SHU_T SRAMIdx)
{
    U8 u1ShuffleIdx = 0;
    DRAM_DFS_FREQUENCY_TABLE_T *pFreqTbl;

    pFreqTbl = gFreqTbl;

    for (u1ShuffleIdx = 0; u1ShuffleIdx < DRAM_DFS_SRAM_MAX; u1ShuffleIdx++)
    {
        if (pFreqTbl[u1ShuffleIdx].SRAMIdx == SRAMIdx)
            break;
    }

    if (u1ShuffleIdx == DRAM_DFS_SRAM_MAX)
        {
        mcSHOW_ERR_MSG(("\n get_FreqTbl_by_shuffleIndex is NULL \n"));
        #if (__ETT__)
            while(1);
        #else
        ASSERT(0);
        #endif
    }
    return &pFreqTbl[u1ShuffleIdx];
}

#if 0
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

    for (u1RankIdx=RANK_0; u1RankIdx<p->support_rank_num; u1RankIdx++)
    {
        vSetRank(p, u1RankIdx);
        mcSHOW_DBG_MSG(("[FullRGDump] RANK%d\n", u1RankIdx));
        DumpAoShuReg(u4ShuOffset, u4ShuOffset);
    }

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


    DramcBackupRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));

    //vIO32Write4B(DRAMC_REG_ADDR(u4Reg), u4Data);
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA0, 0x0, MISC_SRAM_DMA0_APB_SLV_SEL);
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA1, 0x1, MISC_SRAM_DMA1_R_APB_DMA_DBG_ACCESS);
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA1, sram_shu_level, MISC_SRAM_DMA1_R_APB_DMA_DBG_LEVEL);
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA0, 0x1, MISC_SRAM_DMA0_APB_SLV_SEL);

    p->ShuRGAccessIdx = DRAM_DFS_REG_SHU1;
    u4Value = u4IO32Read4B(DRAMC_REG_ADDR(u4Reg));
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


    DramcBackupRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));

    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA0, 0x0, MISC_SRAM_DMA0_APB_SLV_SEL);
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA1, 0x1, MISC_SRAM_DMA1_R_APB_DMA_DBG_ACCESS);
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA1, sram_shu_level, MISC_SRAM_DMA1_R_APB_DMA_DBG_LEVEL);
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA0, 0x1, MISC_SRAM_DMA0_APB_SLV_SEL);

    p->ShuRGAccessIdx = DRAM_DFS_REG_SHU1;
    vIO32Write4B(DRAMC_REG_ADDR(u4Reg), u4Data);
    DramcRestoreRegisters(p, u4RegBackupAddress, sizeof(u4RegBackupAddress) / sizeof(U32));
    p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
    mcSHOW_DBG_MSG(("[SramDebugModeWrite] RK%d Reg=0x%x, Value=0x%x\n", p->rank, u4Reg, u4Data));

    //vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA0, 0x0, MISC_SRAM_DMA0_APB_SLV_SEL);
    //vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA1, 0x0, MISC_SRAM_DMA1_R_APB_DMA_DBG_ACCESS);
}

void DramcCopyShu0toShu1(DRAMC_CTX_T *p, U32 u4StartAddr, U32 u4EndAddr)
{
    U32 ii, u4tmp, u4Offset=0;
    DRAM_DFS_REG_SHU_T ShuRGAccessIdxBackup = p->ShuRGAccessIdx;

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
    DRAM_DFS_REG_SHU_T ShuRGAccessIdxBackup = p->ShuRGAccessIdx;

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

#if ENABLE_LP4Y_WA && LP4Y_BACKUP_SOLUTION
static void ClkSingleEndRGEnable (DRAMC_CTX_T *p, U8 u1OnOff)
{
    vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD13), P_Fld( u1OnOff, SHU_CA_CMD13_RG_TX_ARCLKB_OE_TIE_SEL_CA) \
                                                      | P_Fld( u1OnOff, SHU_CA_CMD13_RG_TX_ARCLKB_OE_TIE_EN_CA));
    vIO32WriteFldMulti(DRAMC_REG_ADDR(DDRPHY_REG_SHU_CA_CMD7) , P_Fld( u1OnOff, SHU_CA_CMD7_R_LP4Y_SDN_MODE_CLK));
}

void CmdBusTrainingLP4YWA(DRAMC_CTX_T *p, U8 u1OnOff)
{
    U8 u1MR51 = 0;

    if ((p->frequency > 800) && (p->dram_fsp==FSP_0))
        return;

    if (p->dram_fsp==FSP_1)
    {
        if (u1OnOff==DISABLE)
        {
            p->ShuRGAccessIdx = DRAM_DFS_REG_SHU1;


            ClkSingleEndRGEnable ( p, u1OnOff);

            p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
        }
    }
    else
    {
        if (u1OnOff == DISABLE)
            u1MR51Value[p->dram_fsp] = u1MR51Value[p->dram_fsp] & ~(1 << 3);
        else
            u1MR51Value[p->dram_fsp] = u1MR51Value[p->dram_fsp] | (1 << 3);

        DramcModeRegWriteByRank(p, p->rank, 51, u1MR51Value[p->dram_fsp]);


        ClkSingleEndRGEnable ( p, u1OnOff);
    }
}
#endif

#if ENABLE_DFS_RUNTIME_MRW
void DFSRuntimeFspMRW(DRAMC_CTX_T *p)
{
    p->boot_fsp = p->dram_fsp;
    vIO32WriteFldAlign_All(DRAMC_REG_SA_RESERVE, p->boot_fsp, SA_RESERVE_DFS_FSP_RTMRW);
}

void DFSRuntimeMRW_preset_BeforeK(DRAMC_CTX_T *p, U8 sram_shu_level)
{
    U8 u1ChIdx = 0, u1RankIdx = 0;
    U8 u1MR51_Value = u1MR51Value[p->dram_fsp];
    U32 u4DPMOffset = 0;
    #if ENABLE_RTMRW_DEBUG_LOG
    mcSHOW_DBG_MSG(("[DFSRuntimeMRW_preset_BeforeK] FSP%d\n", p->dram_fsp));
    #endif

#if ENABLE_LP4Y_DFS && (LP4Y_BACKUP_SOLUTION == 1)
    if (p->frequency <=800)
    {
        u1MR51_Value = u1MR51Value[p->dram_fsp] | 0xe;
    }
    else
    {
        u1MR51_Value = u1MR51Value[p->dram_fsp] & 0xf1;
    }
#endif


    vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_0 + (sram_shu_level << 4),
        P_Fld(u1MR01Value[p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_0) |
        P_Fld(u1MR02Value[p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_1) |
        P_Fld(u1MR03Value[p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_2) |
        P_Fld(u1MR11Value[p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_3));
    #if ENABLE_RTMRW_DEBUG_LOG
    mcSHOW_DBG_MSG(("\tMR01 = 0x%x, MR02 = 0x%x, MR03 = 0x%x, MR11 = 0x%x\n", u1MR01Value[p->dram_fsp], u1MR02Value[p->dram_fsp], u1MR03Value[p->dram_fsp], u1MR11Value[p->dram_fsp]));
    #endif

    vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_1 + (sram_shu_level << 4),
        P_Fld(u1MR21Value[p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_1_MR_OP_SET_SHU_0_5) |
        P_Fld(u1MR22Value[p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_1_MR_OP_SET_SHU_0_6) |
        P_Fld(u1MR51_Value, LPIF_MR_OP_STORE_SHU_0_1_MR_OP_SET_SHU_0_7));
    #if ENABLE_RTMRW_DEBUG_LOG
    mcSHOW_DBG_MSG(("\tMR22 = 0x%x, MR51 = 0x%x\n", u1MR22Value[p->dram_fsp], u1MR51_Value));
    #endif

    for (u1ChIdx = CHANNEL_A; u1ChIdx < p->support_channel_num; u1ChIdx ++)
    {
#if CHANNEL_NUM > 2
	if (channel_num_auxadc > 2) {
        if (u1ChIdx >= CHANNEL_C)
            u4DPMOffset = ((u1ChIdx >> 1) << POS_BANK_NUM);
		}
#endif

        vIO32WriteFldMulti(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_2 + ((u1ChIdx%2)*4) + u4DPMOffset + (sram_shu_level << 4),
            P_Fld(u1MR12Value[u1ChIdx][RANK_0][p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_2_MR_OP_SET_SHU_0_8) |
            P_Fld(u1MR12Value[u1ChIdx][RANK_1][p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_2_MR_OP_SET_SHU_0_9) |
            P_Fld(u1MR14Value[u1ChIdx][RANK_0][p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_2_MR_OP_SET_SHU_0_10) |
            P_Fld(u1MR14Value[u1ChIdx][RANK_1][p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_2_MR_OP_SET_SHU_0_11));
    }

#if ENABLE_RTMRW_DEBUG_LOG
    for (u1ChIdx = CHANNEL_A; u1ChIdx < p->support_channel_num; u1ChIdx++)
    {
        for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
        {
            mcSHOW_DBG_MSG(("\tCH%d, RK%d, MR12 = 0x%x, MR14 = 0x%x\n", u1ChIdx, u1RankIdx,u1MR12Value[u1ChIdx][u1RankIdx][p->dram_fsp], u1MR14Value[u1ChIdx][u1RankIdx][p->dram_fsp]));
        }
    }
#endif
}

void DFSRuntimeMRW_preset_AfterK(DRAMC_CTX_T *p, U8 sram_shu_level)
{
    U8 u1ChIdx = 0, u1RankIdx = 0;
    U8 u1MR03_Value = u1MR03Value[p->dram_fsp], u1MR51_Value = u1MR51Value[p->dram_fsp];
    U32 u4DPMOffset = 0;
#if ENABLE_RTMRW_DEBUG_LOG
    mcSHOW_DBG_MSG(("[DFSRuntimeMRW_preset_AfterK] FSP%d\n", p->dram_fsp));
#endif

#if ENABLE_READ_DBI
    u1MR03_Value = ((u1MR03Value[p->dram_fsp] & 0xbf) | (p->DBI_R_onoff[p->dram_fsp] << 6));
#endif

#if ENABLE_WRITE_DBI
    u1MR03_Value = ((u1MR03Value[p->dram_fsp] & 0x7F) | (p->DBI_W_onoff[p->dram_fsp] << 7));
#endif

#if ENABLE_LP4Y_DFS && (LP4Y_BACKUP_SOLUTION == 0)
    if (p->frequency <=800)
    {
        u1MR51_Value = u1MR51Value[p->dram_fsp] | 0xe;
    }
    else
    {
        u1MR51_Value = u1MR51Value[p->dram_fsp] & 0xf1;
    }
#endif


    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_0 + (sram_shu_level << 4),
        u1MR03_Value, LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_2);
    #if ENABLE_RTMRW_DEBUG_LOG
    mcSHOW_DBG_MSG(("\tMR01 = 0x%x, MR02 = 0x%x, MR03 = 0x%x, MR11 = 0x%x\n", u1MR01Value[p->dram_fsp], u1MR02Value[p->dram_fsp], u1MR03_Value, u1MR11Value[p->dram_fsp]));
    #endif

    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_1 + (sram_shu_level << 4),
        u1MR51_Value, LPIF_MR_OP_STORE_SHU_0_1_MR_OP_SET_SHU_0_7);
    #if ENABLE_RTMRW_DEBUG_LOG
    mcSHOW_DBG_MSG(("\tMR22 = 0x%x, MR51 = 0x%x\n", u1MR22Value[p->dram_fsp], u1MR51_Value));
    #endif

    for (u1ChIdx = CHANNEL_A; u1ChIdx < p->support_channel_num; u1ChIdx ++)
    {
#if CHANNEL_NUM > 2
	if (channel_num_auxadc > 2) {
        if (u1ChIdx >= CHANNEL_C)
            u4DPMOffset = ((u1ChIdx >> 1) << POS_BANK_NUM);
		}
#endif

        vIO32WriteFldMulti(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_2 + ((u1ChIdx%2)*4) + u4DPMOffset + (sram_shu_level << 4),
            P_Fld(u1MR12Value[u1ChIdx][RANK_0][p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_2_MR_OP_SET_SHU_0_8) |
            P_Fld(u1MR12Value[u1ChIdx][RANK_1][p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_2_MR_OP_SET_SHU_0_9) |
            P_Fld(u1MR14Value[u1ChIdx][RANK_0][p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_2_MR_OP_SET_SHU_0_10) |
            P_Fld(u1MR14Value[u1ChIdx][RANK_1][p->dram_fsp], LPIF_MR_OP_STORE_SHU_0_2_MR_OP_SET_SHU_0_11));
    }

#if ENABLE_RTMRW_DEBUG_LOG
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
    U8 u1MRW_1ST_Num = 0x5;
    U8 u1MRW_2ND_Num = 0x2;

#if ENABLE_LP4Y_DFS
    u1MRW_2ND_Num++;
#endif

#if 1

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


    vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RTMRW_CTRL1),
        P_Fld(13, RTMRW_CTRL1_RTMRW0_MA) |
        P_Fld(1, RTMRW_CTRL1_RTMRW1_MA) |
        P_Fld(2, RTMRW_CTRL1_RTMRW2_MA) |
        P_Fld(3, RTMRW_CTRL1_RTMRW3_MA));


    vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RTMRW_CTRL2),
        P_Fld(u1MR13, RTMRW_CTRL2_RTMRW0_OP) |
        P_Fld(u1MR1, RTMRW_CTRL2_RTMRW1_OP) |
        P_Fld(u1MR2, RTMRW_CTRL2_RTMRW2_OP) |
        P_Fld(u1MR3, RTMRW_CTRL2_RTMRW3_OP));


    vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RTMRW_CTRL3),
        P_Fld(11, RTMRW_CTRL3_RTMRW4_MA) |
        P_Fld(12, RTMRW_CTRL3_RTMRW5_MA) |
        P_Fld(u1MR11, RTMRW_CTRL3_RTMRW4_OP) |
        P_Fld(u1MR12, RTMRW_CTRL3_RTMRW5_OP));


    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0x1, SWCMD_EN_RTMRWEN);

    do {
        rt_ack = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), SPCMDRESP_RTMRW_RESPONSE);
    } while(rt_response_ack != rt_ack);

    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0x0, SWCMD_EN_RTMRWEN);
#endif

#if 1

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


    vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RTMRW_CTRL1),
        P_Fld(14, RTMRW_CTRL1_RTMRW0_MA) |
#if ENABLE_LP4Y_DFS
        P_Fld(21, RTMRW_CTRL1_RTMRW3_MA) |
#endif
        P_Fld(22, RTMRW_CTRL1_RTMRW1_MA) |
        P_Fld(51, RTMRW_CTRL1_RTMRW2_MA));


    vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_RTMRW_CTRL2),
        P_Fld(u1MR14, RTMRW_CTRL2_RTMRW0_OP) |
#if ENABLE_LP4Y_DFS
        P_Fld(u1MR21, RTMRW_CTRL2_RTMRW3_OP) |
#endif
        P_Fld(u1MR22, RTMRW_CTRL2_RTMRW1_OP) |
        P_Fld(u1MR51, RTMRW_CTRL2_RTMRW2_OP));


    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0x1, SWCMD_EN_RTMRWEN);

    do {
        rt_ack = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), SPCMDRESP_RTMRW_RESPONSE);
    } while(rt_response_ack != rt_ack);

    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SWCMD_EN), 0x0, SWCMD_EN_RTMRWEN);
#endif
}

static void DFSRTMRW_HwsetWA(DRAMC_CTX_T *p, U8 cur_shu_mux_index, U8 pingpong_shu_level, U8 CH_num)
{
    U8 u1MR13_OP = 0, u1VRCG_OP = 0;
    U8 u1ChIdx = 0;
    U8 bc_bak = DRAMC_BROADCAST_OFF;
    #if ENABLE_RTMRW_DEBUG_LOG
    mcSHOW_DBG_MSG(("[DFSRTMRW_HwsetWA] \n"));
    #endif

        p->ShuRGAccessIdx = cur_shu_mux_index;
        u1MR13_OP = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_MR13), SHU_HWSET_MR13_HWSET_MR13_OP);
        p->ShuRGAccessIdx = pingpong_shu_level;
        u1VRCG_OP = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_VRCG), SHU_HWSET_VRCG_HWSET_VRCG_OP);
        p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;

        if(p->boot_fsp == FSP_1)
        {
            if (cur_shu_mux_index == PHYPLL_MODE)
            {
                u1MR13_OP &= 0x3F;
                u1VRCG_OP &= 0x3F;
            }
            else
            {
                u1MR13_OP |= 0xC0;
                u1VRCG_OP |= 0xC0;
            }
        }
        else
        {
            if (cur_shu_mux_index == PHYPLL_MODE)
            {
                u1MR13_OP |= 0xC0;
                u1VRCG_OP |= 0xC0;
            }
            else
            {
                u1MR13_OP &= 0x3F;
                u1VRCG_OP &= 0x3F;
            }
        }

    #if ENABLE_RTMRW_DEBUG_LOG
    mcSHOW_DBG_MSG(("HWSET_MR13_OP = 0x%x\n", u1MR13_OP));
    mcSHOW_DBG_MSG(("HWSET_VRCG_OP = 0x%x\n", u1VRCG_OP));
    #endif

    if (CH_num > CHANNEL_SINGLE)
    {
        bc_bak = GetDramcBroadcast();
        DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
    }
        p->ShuRGAccessIdx = cur_shu_mux_index;
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_MR13), u1MR13_OP, SHU_HWSET_MR13_HWSET_MR13_OP);
        p->ShuRGAccessIdx = pingpong_shu_level;
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_VRCG), u1VRCG_OP, SHU_HWSET_VRCG_HWSET_VRCG_OP);
        p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
    if (CH_num > CHANNEL_SINGLE)
        DramcBroadcastOnOff(bc_bak);

}

static void DFSRuntimeMRWEn(DRAMC_CTX_T *p, U8 cur_shu_mux_index, U8 nxt_shu_level)
{
    U8 rtmr13 = 0;
    U8 rtmr1 = 0, rtmr2 = 0, rtmr3 = 0, rtmr11 = 0;
    U8 rtmr12 = 0, rtmr14 = 0;
    U8 rtmr21 = 0, rtmr22 = 0, rtmr51 = 0;
    U8 dpm_rtmrw_hpri_en_bk = 0;
    U32 bc_bak = 0, ch_bak = 0;
    U8 u1ChIdx = 0, u1RankIdx = 0;
    U32 DPMOffset;

    bc_bak = GetDramcBroadcast();
    DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);
    ch_bak = vGetPHY2ChannelMapping(p);


    rtmr1 = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_0 + (nxt_shu_level << 4), LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_0);
    rtmr2 = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_0 + (nxt_shu_level << 4), LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_1);
    rtmr3 = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_0 + (nxt_shu_level << 4), LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_2);
    rtmr11 = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_0 + (nxt_shu_level << 4), LPIF_MR_OP_STORE_SHU_0_0_MR_OP_SET_SHU_0_3);

    rtmr21 = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_1 + (nxt_shu_level << 4), LPIF_MR_OP_STORE_SHU_0_1_MR_OP_SET_SHU_0_5);
    rtmr22 = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_1 + (nxt_shu_level << 4), LPIF_MR_OP_STORE_SHU_0_1_MR_OP_SET_SHU_0_6);
    rtmr51 = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_1 + (nxt_shu_level << 4), LPIF_MR_OP_STORE_SHU_0_1_MR_OP_SET_SHU_0_7);

    rtmr13 &= 0x3F;
    if (p->boot_fsp== FSP_1)
    {
        if(cur_shu_mux_index == PHYPLL_MODE)
            rtmr13 |= (0x1 << 7);
        else
            rtmr13 |= (0x1 << 6);
    }
    else
    {
        if(cur_shu_mux_index == PHYPLL_MODE)
            rtmr13 |= (0x1 << 6);
        else
            rtmr13 |= (0x1 << 7);
    }
#if ENABLE_RTMRW_DEBUG_LOG
    mcSHOW_DBG_MSG(("[DFSRuntimeMRWEn]\n"));
#endif
    for (u1ChIdx = CHANNEL_A; u1ChIdx < p->support_channel_num; u1ChIdx++)
    {
        vSetPHY2ChannelMapping(p, u1ChIdx);
        dpm_rtmrw_hpri_en_bk = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MPC_CTRL), MPC_CTRL_RTMRW_HPRI_EN);
        if (dpm_rtmrw_hpri_en_bk == 0)
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MPC_CTRL), 0x1, MPC_CTRL_RTMRW_HPRI_EN);

#if (CHANNEL_NUM > 2)
	if (channel_num_auxadc > 2) {
        if (u1ChIdx >= CHANNEL_C)
            DPMOffset = ((u1ChIdx >> 1) << POS_BANK_NUM);
        else
		}
#endif
            DPMOffset = 0;

        for (u1RankIdx = RANK_0; u1RankIdx < p->support_rank_num; u1RankIdx++)
        {

            rtmr12 = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_2 + ((u1ChIdx%2)*4) + DPMOffset + (nxt_shu_level << 4),  Fld(8, u1RankIdx*8));
            rtmr14 = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_MR_OP_STORE_SHU_0_2 + ((u1ChIdx%2)*4) + DPMOffset + (nxt_shu_level << 4),  Fld(8, (u1RankIdx*8)+16));
            #if ENABLE_RTMRW_DEBUG_LOG
            mcSHOW_DBG_MSG(("CH%d RK%d\n", u1ChIdx, u1RankIdx));
            mcSHOW_DBG_MSG(("\tMR1=0x%x, MR2=0x%x, MR3=0x%x, MR11=0x%x\n", rtmr1, rtmr2, rtmr3, rtmr11));
            mcSHOW_DBG_MSG(("\tMR12=0x%x, MR13=0x%x, MR14=0x%x, MR22=0x%x, MR51=0x%x\n", rtmr12, rtmr13, rtmr14, rtmr22, rtmr51));
            #endif
            TriggerRTMRW_SingleChannel(p, u1RankIdx, rtmr1, rtmr2, rtmr3, rtmr11, rtmr12, rtmr13, rtmr14, rtmr21, rtmr22, rtmr51);
        }

        if (dpm_rtmrw_hpri_en_bk == 0)
            vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MPC_CTRL), dpm_rtmrw_hpri_en_bk, MPC_CTRL_RTMRW_HPRI_EN);
    }

    vSetPHY2ChannelMapping(p, ch_bak);
    DramcBroadcastOnOff(bc_bak);
}
#endif

static void DFSHwSetWA(DRAMC_CTX_T *p, U8 cur_shu_mux_index, U8 nxt_shu_level, U8 CH_num)
{
    U8 u1MR13_OP = 0;
    U8 bc_bak = DRAMC_BROADCAST_OFF;
    #if ENABLE_RTMRW_DEBUG_LOG
    mcSHOW_DBG_MSG(("[DFSHwSetWA] \n"));
    #endif
    p->ShuRGAccessIdx = cur_shu_mux_index;
    u1MR13_OP = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_MR13), SHU_HWSET_MR13_HWSET_MR13_OP);

    if ((nxt_shu_level == SRAM_SHU0) || (nxt_shu_level == SRAM_SHU1))
        u1MR13_OP |= 0xC0;
    else
        u1MR13_OP &= 0x3F;

    if (CH_num > CHANNEL_SINGLE)
    {
        bc_bak = GetDramcBroadcast();
        DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
    }

    #if ENABLE_RTMRW_DEBUG_LOG
    mcSHOW_DBG_MSG(("HWSET_MR13_OP = 0x%x\n", u1MR13_OP));
    #endif

    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_MR13), u1MR13_OP, SHU_HWSET_MR13_HWSET_MR13_OP);
    if (CH_num > CHANNEL_SINGLE)
        DramcBroadcastOnOff(bc_bak);

    p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;
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

    mcDELAY_XNS(100);

    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CG_CTRL0, 0x0, MISC_CG_CTRL0_W_CHG_MEM);
}
#endif

#if ENABLE_DFS_DEBUG_MODE
static void WaitDFSDebugSM(DRAMC_CTX_T *p, U8 u1HangStatus)
{
    U8 u1Status[CHANNEL_NUM] = {0}, u1DvfsState[CHANNEL_NUM] = {0}, u1ChIdx = 0;
    DRAM_CHANNEL_T eOriChannel = vGetPHY2ChannelMapping(p);
    U32 u4While1Cnt = 100;

    for (u1ChIdx = CHANNEL_A; u1ChIdx < p->support_channel_num; u1ChIdx++)
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
                DDRPhyFreqMeter(p);
                while(1);
            }
            u4While1Cnt--;
        } while(1);
    }

    vSetPHY2ChannelMapping(p, eOriChannel);
}

static void ExitDFSDebugMode(DRAMC_CTX_T *p, DFS_DBG_T eDbgMode)
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

static void ChkDFSDebugMode(DRAMC_CTX_T *p, DFS_DBG_T eDbgMode)
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


        vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_3, P_Fld(0xf, LPIF_LOW_POWER_CFG_3_DVFS_MEM_CK_MUX_SEL)
                            | P_Fld(0x3, LPIF_LOW_POWER_CFG_3_DVFS_MEM_CK_MUX_UPDATE));
        mcDELAY_US(1);
        vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_3, P_Fld(0xf, LPIF_LOW_POWER_CFG_3_DVFS_MEM_CK_MUX_SEL)
                            | P_Fld(0x0, LPIF_LOW_POWER_CFG_3_DVFS_MEM_CK_MUX_UPDATE));

        WaitDFSDebugSM(p, 0x1f);


        vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_3, P_Fld(0x5, LPIF_LOW_POWER_CFG_3_DVFS_MEM_CK_MUX_SEL)
                            | P_Fld(0x3, LPIF_LOW_POWER_CFG_3_DVFS_MEM_CK_MUX_UPDATE));
        mcDELAY_US(1);
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

static void EntryDFSDebugMode(DRAMC_CTX_T *p, DFS_DBG_T eDbgMode)
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

        vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CLK_CTRL, 0x1, MISC_CLK_CTRL_DVFS_MEM_CK_MUX_SEL_MODE);
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

#if DFS_NOQUEUE_FLUSH_ENABLE
U32 u4PERFCTL0_backup=0;

void EnableDFSNoQueueFlush(DRAMC_CTX_T *p)
{
    vIO32WriteFldMulti_All(DRAMC_REG_DVFS_CTRL0, P_Fld(0, DVFS_CTRL0_HWSET_WLRL)
        | P_Fld(0, DVFS_CTRL0_DVFS_RXFIFOST_SKIP)
        | P_Fld(1, DVFS_CTRL0_DVFS_NOQUEFLUSH_EN)
        | P_Fld(0, DVFS_CTRL0_R_DMDVFSMRW_EN));
    vIO32WriteFldMulti_All(DRAMC_REG_SHUCTRL1, P_Fld(0, SHUCTRL1_FC_PRDCNT)
#if ENABLE_LP4Y_WA
        | P_Fld(5, SHUCTRL1_CKFSPE_PRDCNT)
        | P_Fld(5, SHUCTRL1_VRCGEN_PRDCNT)
#else
        | P_Fld(0, SHUCTRL1_CKFSPE_PRDCNT)
        | P_Fld(0, SHUCTRL1_VRCGEN_PRDCNT)
#endif
        | P_Fld(0, SHUCTRL1_CKFSPX_PRDCNT));
    vIO32WriteFldAlign_All(DRAMC_REG_BYPASS_FSPOP, 0, BYPASS_FSPOP_BPFSP_OPT);

#if ENABLE_DFS_RUNTIME_MRW
    vIO32WriteFldMulti_All(DRAMC_REG_DVFS_TIMING_CTRL3, P_Fld(0, DVFS_TIMING_CTRL3_RTMRW_MRW1_SKIP)
        | P_Fld(0, DVFS_TIMING_CTRL3_RTMRW_MRW2_SKIP)
        | P_Fld(1, DVFS_TIMING_CTRL3_RTMRW_MRW3_SKIP));
#endif

#if ENABLE_DFS_NOQUEUE_FLUSH_DBG

    vIO32WriteFldMulti_All(DRAMC_REG_DVFS_TIMING_CTRL3, P_Fld(1, DVFS_TIMING_CTRL3_RTMRW_MRW1_PAUSE)
        | P_Fld(1, DVFS_TIMING_CTRL3_RTMRW_MRW2_PAUSE)
        | P_Fld(1, DVFS_TIMING_CTRL3_RTMRW_MRW3_PAUSE));
#endif

    vIO32WriteFldAlign_All(DRAMC_REG_MISCTL0, 1, MISCTL0_GROUP_A_REV);
}
#if 0
static void WaitNoQueueFlushComplete(DRAMC_CTX_T *p)
{

    U8 u1tCKFSPe_OK[CHANNEL_NUM] = {0};
    U8 u1tVRCGDis_OK[CHANNEL_NUM] = {0};
    U8 u1ChIdx = 0;
    U16 u2Cnt = 0, u2PrintCnt = 1000;
    DRAM_CHANNEL_T eOriChannel = vGetPHY2ChannelMapping(p);

    for (u1ChIdx = CHANNEL_A; u1ChIdx < p->support_channel_num; u1ChIdx++)
    {
        vSetPHY2ChannelMapping(p, u1ChIdx);
        do {
            u1tCKFSPe_OK[u1ChIdx] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MRR_STATUS2), MRR_STATUS2_TCKFSPE_OK);
            if ((u2Cnt++) == u2PrintCnt)
            {
                u2Cnt = 0;
                mcSHOW_ERR_MSG(("MRW1_SKIP FAIL!\n"));
                while(1);
            }
        } while(u1tCKFSPe_OK[u1ChIdx] != 0);

        vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DVFS_TIMING_CTRL3), 1, DVFS_TIMING_CTRL3_RTMRW_MRW1_SKIP);

        do {
            u1tCKFSPe_OK[u1ChIdx] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MRR_STATUS2), MRR_STATUS2_TCKFSPE_OK);
            u1tVRCGDis_OK[u1ChIdx] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MRR_STATUS2), MRR_STATUS2_TVRCG_DIS_OK);
            if ((u2Cnt++) == u2PrintCnt)
            {
                u2Cnt = 0;
                mcSHOW_ERR_MSG(("MRW2_SKIP FAIL!\n"));
                while(1);
            }
        } while((u1tCKFSPe_OK[u1ChIdx] != 1) && (u1tVRCGDis_OK[u1ChIdx] != 0));

        vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DVFS_TIMING_CTRL3), 1, DVFS_TIMING_CTRL3_RTMRW_MRW2_SKIP);

        do {
            u1tVRCGDis_OK[u1ChIdx] = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MRR_STATUS2), MRR_STATUS2_TVRCG_DIS_OK);
            if ((u2Cnt++) == u2PrintCnt)
            {
                u2Cnt = 0;
                mcSHOW_ERR_MSG(("MRW3_SKIP FAIL!\n"));
                while(1);
            }
        } while(u1tVRCGDis_OK[u1ChIdx] != 1);

        vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DVFS_TIMING_CTRL3), 1, DVFS_TIMING_CTRL3_RTMRW_MRW3_SKIP);
    }

    vSetPHY2ChannelMapping(p, eOriChannel);
}
#endif
#endif

#if ENABLE_TIMING_TXSR_DFS_WA
static void TimingTxsrWA(DRAMC_CTX_T *p, U32 next_shu_level)
{
    U32 onoff=0, bc_bak=0;

    if (p->support_channel_num > CHANNEL_SINGLE)
    {
        bc_bak = GetDramcBroadcast();
        DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
    }

    if ((next_shu_level == SRAM_SHU4) || (next_shu_level == SRAM_SHU5)
        || (next_shu_level == SRAM_SHU6)
#if ENABLE_DDR400_OPEN_LOOP_MODE_OPTION
        || (next_shu_level == SRAM_SHU7))
#else
        )
#endif
    {
        onoff = DISABLE;
    }
    else
    {
        onoff = ENABLE;
    }

    vIO32WriteFldAlign(DRAMC_REG_REFCTRL1, onoff, REFCTRL1_REF_OVERHEAD_SLOW_REFPB_ENA);

    if (p->support_channel_num > CHANNEL_SINGLE)
        DramcBroadcastOnOff(bc_bak);
}
#endif

#if ENABLE_TX_REBASE_ODT_WA
void TxReadBaseODTWA(DRAMC_CTX_T *p, U8 next_shu_level)
{
    U32 termen_dis, bc_bak=0;

    if (p->support_channel_num > CHANNEL_SINGLE)
    {
        bc_bak = GetDramcBroadcast();
        DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
    }

    if ((next_shu_level == SRAM_SHU0) || (next_shu_level == SRAM_SHU1))
        termen_dis = DISABLE;
    else
        termen_dis = ENABLE;

    //mcSHOW_DBG_MSG(("[TxReadBaseODTWA] SRAM SHU%d, termen_dis = %d\n", next_shu_level, termen_dis));
    vIO32WriteFldAlign(DDRPHY_REG_B0_DQ6, termen_dis, B0_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B0);
    vIO32WriteFldAlign(DDRPHY_REG_B1_DQ6, termen_dis, B1_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B1);
    vIO32WriteFldAlign(DDRPHY_REG_CA_CMD6, termen_dis, CA_CMD6_RG_TX_ARCMD_ODTEN_EXT_DIS);

    if (p->support_channel_num > CHANNEL_SINGLE)
        DramcBroadcastOnOff(bc_bak);
}
#endif

#if ENABLE_TX_REBASE_WDQS_DQS_PI_WA
static void TxReBaseWDQSDqsPiWA(DRAMC_CTX_T *p, U8 pingpong_shu_level)
{
    U32 bc_bak=0;

    if (p->support_channel_num > CHANNEL_SINGLE)
    {
        bc_bak = GetDramcBroadcast();
        DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
    }

    p->ShuRGAccessIdx = pingpong_shu_level;
    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ13   , P_Fld(0, SHU_B0_DQ13_RG_TX_ARDQS_READ_BASE_EN_B0           )
                                                                | P_Fld(0, SHU_B0_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B0          ) );
    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ13   , P_Fld(0, SHU_B1_DQ13_RG_TX_ARDQS_READ_BASE_EN_B1           )
                                                                | P_Fld(0, SHU_B1_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B1          ) );
    mcDELAY_US(1);
    vIO32WriteFldMulti(DDRPHY_REG_SHU_B0_DQ13   , P_Fld(1, SHU_B0_DQ13_RG_TX_ARDQS_READ_BASE_EN_B0           )
                                                                | P_Fld(1, SHU_B0_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B0          ) );
    vIO32WriteFldMulti(DDRPHY_REG_SHU_B1_DQ13   , P_Fld(1, SHU_B1_DQ13_RG_TX_ARDQS_READ_BASE_EN_B1           )
                                                                | P_Fld(1, SHU_B1_DQ13_RG_TX_ARDQSB_READ_BASE_EN_B1          ) );
    p->ShuRGAccessIdx = DRAM_DFS_REG_SHU0;

    if (p->support_channel_num > CHANNEL_SINGLE)
        DramcBroadcastOnOff(bc_bak);
}
#endif

#if ENABLE_DDR800_SOPEN_DSC_WA
static void DDR800_SOPEN_DSC_WA(DRAMC_CTX_T *p, U8 next_shu_level, U8 u1OnOff)
{
    U8 MPDIV_CG, bc_bak = 0;

    if (p->DRAMPinmux != PINMUX_DSC)
        return;

    if (p->support_channel_num> CHANNEL_SINGLE)
    {
        bc_bak = GetDramcBroadcast();
        DramcBroadcastOnOff(DRAMC_BROADCAST_ON);
    }

    if ((next_shu_level == SRAM_SHU6) && (u1OnOff==DISABLE))
    {
        MPDIV_CG = DISABLE;
        vIO32WriteFldAlign(DDRPHY_REG_MISC_CTRL4, MPDIV_CG, MISC_CTRL4_R_OPT2_MPDIV_CG);
    }
    else if ((next_shu_level != SRAM_SHU6) && (u1OnOff==ENABLE))
    {
        MPDIV_CG = ENABLE;
        vIO32WriteFldAlign(DDRPHY_REG_MISC_CTRL4, MPDIV_CG, MISC_CTRL4_R_OPT2_MPDIV_CG);
    }

    if (p->support_channel_num > CHANNEL_SINGLE)
        DramcBroadcastOnOff(bc_bak);
}
#endif

void EnableDFSHwModeClk(DRAMC_CTX_T *p)
{

    vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DVFSCTL3,
                        P_Fld(0x3, MISC_DVFSCTL3_RG_DVFS_MEM_CK_SEL_DESTI) |
                        P_Fld(0x1, MISC_DVFSCTL3_RG_DVFS_MEM_CK_SEL_SOURCE));
    vIO32WriteFldMulti_All(DDRPHY_REG_MISC_CLK_CTRL,
                        P_Fld(0x1, MISC_CLK_CTRL_DVFS_MEM_CK_MUX_UPDATE_EN) |
                        P_Fld(0x1, MISC_CLK_CTRL_DVFS_CLK_MEM_SEL) |
                        P_Fld(0x0, MISC_CLK_CTRL_DVFS_MEM_CK_MUX_SEL_MODE) |
                        P_Fld(0x1, MISC_CLK_CTRL_DVFS_MEM_CK_MUX_SEL));
}
#if 0
static void DFSEnlargeTimingSettings(DRAMC_CTX_T *p)
{
    vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_DVFSDLL, P_Fld(0x7f, MISC_SHU_DVFSDLL_R_DLL_IDLE)
             | P_Fld(0x7f, MISC_SHU_DVFSDLL_R_2ND_DLL_IDLE)
             | P_Fld(0, MISC_SHU_DVFSDLL_R_BYPASS_1ST_DLL)
        | P_Fld(0, MISC_SHU_DVFSDLL_R_BYPASS_2ND_DLL));
    vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DVFSCTL3, P_Fld(1, MISC_DVFSCTL3_RG_PHY_ST_DELAY_AFT_CHG_TO_MCLK)
             | P_Fld(0xf, MISC_DVFSCTL3_RG_CNT_PHY_ST_DELAY_BEF_CHG_TO_BCLK)
             | P_Fld(0xf, MISC_DVFSCTL3_RG_CNT_PHY_ST_DELAY_BEF_CHG_TO_MCLK)
        | P_Fld(0x20, MISC_DVFSCTL3_RG_CNT_PHY_ST_DELAY_AFT_CHG_TO_MCLK));
    vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DVFSCTL, P_Fld(0xf, MISC_DVFSCTL_R_DVFS_PICG_MARGIN_NEW)
             | P_Fld(0xf, MISC_DVFSCTL_R_DVFS_PICG_MARGIN2_NEW)
             | P_Fld(0xf, MISC_DVFSCTL_R_DVFS_PICG_MARGIN3_NEW)
             | P_Fld(0xf, MISC_DVFSCTL_R_DVFS_PICG_MARGIN4_NEW)
        | P_Fld(0xf, MISC_DVFSCTL_R_DVFS_MCK8X_MARGIN));
}
#endif
void DVFSSettings(DRAMC_CTX_T *p)
{
    U8 u1DVFS_52M_104M_SEL = 1;
    U8 u1Master_DLL_Idle = 0x2b;
    U8 u1Slave_DLL_Idle = 0x43;
#if (fcFOR_CHIP_ID == fcA60868)
    U8 u1ChClkIgnore[2] = {ENABLE, ENABLE}, u1Channel = 0;
#endif
    U32 backup_broadcast = GetDramcBroadcast();
    DramcBroadcastOnOff(DRAMC_BROADCAST_OFF);

    if (vGet_Div_Mode(p) == DIV16_MODE)
    {
        u1Master_DLL_Idle = 0x37;
        u1Slave_DLL_Idle = 0x4D;
    }



    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CKMUX_SEL, u1DVFS_52M_104M_SEL, MISC_CKMUX_SEL_RG_52M_104M_SEL);
#if ENABLE_DFS_208M_CLOCK
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_CKMUX_SEL, 0x1, MISC_CKMUX_SEL_RG_104M_208M_SEL);
#endif
    vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_DVFSDLL, P_Fld(u1Master_DLL_Idle, MISC_SHU_DVFSDLL_R_DLL_IDLE)
        | P_Fld(u1Slave_DLL_Idle, MISC_SHU_DVFSDLL_R_2ND_DLL_IDLE));

    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, vGet_Current_SRAMIdx(p), MISC_RG_DFS_CTRL_RG_DR_SHU_LEVEL_SRAM);
    //mcSHOW_DBG_MSG(("[DVFSSettings] SHU_LEVEL_SRAM = %d\n", vGet_Current_SRAMIdx(p)));

#if (fcFOR_CHIP_ID == fcA60868) // @Darren, for A60868 only
    for (u1Channel = CHANNEL_A; u1Channel < p->support_channel_num; u1Channel++)
        u1ChClkIgnore[u1Channel] = DISABLE;
    vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DVFSCTL2, P_Fld(u1ChClkIgnore[1], MISC_DVFSCTL2_RG_IGNORE_PHY_SH_CHG_CLK_RDY_CHB)
        | P_Fld(u1ChClkIgnore[0], MISC_DVFSCTL2_RG_IGNORE_PHY_SH_CHG_CLK_RDY_CHA));
#endif

    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_SPM_DVFS_CONTROL_SEL);
    //vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_TX_TRACKING_DIS);
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_DVFSCTL2, 1, MISC_DVFSCTL2_RG_MRW_AFTER_DFS);

    vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_FSM_CFG_1, P_Fld(1, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL)
        | P_Fld(1, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL_2ND)
        | P_Fld(1, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL_FOR_PWR)
        | P_Fld(1, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL_FOR_PWR_2ND));
    vIO32WriteFldMulti_All(DDRPHY_REG_MISC_SHU_OPT, P_Fld(1, MISC_SHU_OPT_R_DQB0_SHU_PHY_GATING_RESETB_SPM_EN)
        | P_Fld(1, MISC_SHU_OPT_R_DQB1_SHU_PHY_GATING_RESETB_SPM_EN));
#if ENABLE_DFS_HW_SAVE_MASK
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_DVFSCTL2, 1, MISC_DVFSCTL2_DVFS_SYNC_MASK_FOR_PHY);
#endif

#if 0
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

#if ENABLE_REMOVE_MCK8X_UNCERT_DFS_OPTION
    //vIO32WriteFldAlign_All(DDRPHY_REG_MISC_DVFS_EMI_CLK, 1, MISC_DVFS_EMI_CLK_RG_DLL_SHUFFLE_DDRPHY);
    vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DVFSCTL, P_Fld(1, MISC_DVFSCTL_R_SHUFFLE_PI_RESET_ENABLE)
        | P_Fld(3, MISC_DVFSCTL_R_DVFS_MCK8X_MARGIN)
        | P_Fld(3, MISC_DVFSCTL_R_DVFS_PICG_MARGIN4_NEW));
#endif

#if (fcFOR_CHIP_ID == fc8195)
    vIO32WriteFldAlign(DDRPHY_REG_MISC_DVFSCTL2, 0, MISC_DVFSCTL2_R_DVFS_CLK_CHG_OK_SEL);
    vIO32WriteFldAlign(DDRPHY_REG_MISC_DVFSCTL2 + SHIFT_TO_CHB_ADDR, 1, MISC_DVFSCTL2_R_DVFS_CLK_CHG_OK_SEL);
#if CHANNEL_NUM > 2
	if (channel_num_auxadc > 2) {
	    vIO32WriteFldAlign(DDRPHY_REG_MISC_DVFSCTL2 + SHIFT_TO_CHC_ADDR, 0, MISC_DVFSCTL2_R_DVFS_CLK_CHG_OK_SEL);
    	vIO32WriteFldAlign(DDRPHY_REG_MISC_DVFSCTL2 + SHIFT_TO_CHD_ADDR, 1, MISC_DVFSCTL2_R_DVFS_CLK_CHG_OK_SEL);
		}
#endif
#endif


    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_DVFS_EMI_CLK, 0, MISC_DVFS_EMI_CLK_RG_DLL_SHUFFLE_DDRPHY);
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_DVFSCTL2, 0, MISC_DVFSCTL2_RG_DLL_SHUFFLE);
    vIO32WriteFldMulti_All(DDRPHY_REG_MISC_DVFSCTL2, P_Fld(0, MISC_DVFSCTL2_R_DVFS_OPTION)
        | P_Fld(0, MISC_DVFSCTL2_R_DVFS_PARK_N));

#if 0
    if (vGet_DDR_Loop_Mode(p) == OPEN_LOOP_MODE)
    {
        vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SHU_DVFSDLL, 1, MISC_SHU_DVFSDLL_R_BYPASS_2ND_DLL);
    }
#endif

#if ENABLE_DFS_TIMING_ENLARGE
    DFSEnlargeTimingSettings(p);
#endif

    //EnableDFSHwModeClk(p);

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

static void DramcSSCHoppingOnOff(DRAMC_CTX_T *p, U8 cur_shu_level, U8 u1OnOff)
{
    if ((cur_shu_level == 0x0) || (cur_shu_level == 0x8) || (cur_shu_level == 0x9) || (cur_shu_level == 0x6) || (cur_shu_level == 0x5))
    {
        if (!(p->u1PLLMode == PHYPLL_MODE))
            vIO32WriteFldAlign(DDRPHY_REG_CLRPLL0, u1OnOff, CLRPLL0_RG_RCLRPLL_SDM_SSC_EN);
        else
            vIO32WriteFldAlign(DDRPHY_REG_PHYPLL0, u1OnOff, PHYPLL0_RG_RPHYPLL_SDM_SSC_EN);
    }
}
#endif


#if DVT_TEST_DUMMY_RD_SIDEBAND_FROM_SPM || ENABLE_DFS_SSC_WA
void DVS_DMY_RD_ENTR(DRAMC_CTX_T *p)
{

    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 1, LPIF_LOW_POWER_CFG_0_DDRPHY_FB_CK_EN);


    mcDELAY_US(1);


    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 1, LPIF_LOW_POWER_CFG_1_DMY_EN_MOD_SEL);

    mcDELAY_US(1);


    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 1, LPIF_LOW_POWER_CFG_1_DMYRD_INTV_SEL);

    mcDELAY_US(1);


    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 1, LPIF_LOW_POWER_CFG_1_DMYRD_EN);

    mcDELAY_US(1);
}

void DVS_DMY_RD_EXIT(DRAMC_CTX_T *p)
{

    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 0, LPIF_LOW_POWER_CFG_1_DMYRD_EN);

    mcDELAY_US(1);


    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 0, LPIF_LOW_POWER_CFG_1_DMYRD_INTV_SEL);

    mcDELAY_US(1);


    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 0, LPIF_LOW_POWER_CFG_1_DMY_EN_MOD_SEL);

    mcDELAY_US(1);


    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_DDRPHY_FB_CK_EN);

    mcDELAY_US(1);



}
#endif


#if 1//(FOR_DV_SIMULATION_USED==0 && SW_CHANGE_FOR_SIMULATION==0)
static void EnableDramcTrackingByShuffle(DRAMC_CTX_T *p, U8 u1EnDPMCh, U8 u1OnOff)
{
    if (u1OnOff == ENABLE)
    {
    #if ENABLE_RX_TRACKING
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, u1EnDPMCh, LPIF_LOW_POWER_CFG_1_DPHY_RXDLY_TRACK_EN);

    #endif
    #if ENABLE_TX_TRACKING
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 0, LPIF_LOW_POWER_CFG_1_TX_TRACKING_DIS);

    #endif
    }
    else if (u1OnOff == DISABLE)
    {
    #if ENABLE_RX_TRACKING
        mcSHOW_DBG_MSG4(("Disable RX-Tracking\n"));
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 0, LPIF_LOW_POWER_CFG_1_DPHY_RXDLY_TRACK_EN);
    #endif
    #if ENABLE_TX_TRACKING
        mcSHOW_DBG_MSG4(("Disable TX-Tracking\n"));
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, u1EnDPMCh, LPIF_LOW_POWER_CFG_1_TX_TRACKING_DIS);
    #endif
    }
}

#if 0
static void EnableDramcTrackingBySPMControl(DRAMC_CTX_T *p)
{
    if (p->frequency >= 1600)
        vIO32WriteFldAlign(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 1, LPIF_LOW_POWER_CFG_1_DPHY_RXDLY_TRACK_EN);
}

static void TransferToRegControl(void)
{

}


static void TransferToSPMControl(DRAMC_CTX_T *p)
{
#if ENABLE_TX_TRACKING
    vIO32WriteFldAlign_All(DRAMC_REG_TX_TRACKING_SET0, 0x0, TX_TRACKING_SET0_TX_TRACKING_OPT);
#endif
#if 0
    vIO32WriteFldAlign_All(DDRPHY_MISC_SPM_CTRL0, 0xfbffefff, MISC_SPM_CTRL0_PHY_SPM_CTL0);
    vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL2, 0xffffffef, MISC_SPM_CTRL2_PHY_SPM_CTL2);
    vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL2 + SHIFT_TO_CHB_ADDR, 0x7fffffef, MISC_SPM_CTRL2_PHY_SPM_CTL2);
#endif

    return;
}
#endif
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
    DRAM_DFS_SRAM_SHU_T u1CurrShuLevel = vGet_Current_SRAMIdx(p);

    u1SetVal = (p->support_channel_num > 1) ? 0x3 : 0x1;


    vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0,
            P_Fld(u1SetVal, LPIF_LOW_POWER_CFG_0_PHYPLL_EN) |
            P_Fld(u1SetVal, LPIF_LOW_POWER_CFG_0_DPY_DLL_EN) |
            P_Fld(u1SetVal, LPIF_LOW_POWER_CFG_0_DPY_2ND_DLL_EN) |
            P_Fld(u1SetVal, LPIF_LOW_POWER_CFG_0_DPY_DLL_CK_EN) |
            P_Fld(u1SetVal, LPIF_LOW_POWER_CFG_0_DPY_VREF_EN));

    vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_3,
            P_Fld(u1SetVal, LPIF_LOW_POWER_CFG_3_DPY_MCK8X_EN) |
            P_Fld(u1SetVal, LPIF_LOW_POWER_CFG_3_DPY_MIDPI_EN) |
            P_Fld(u1SetVal, LPIF_LOW_POWER_CFG_3_DPY_PI_RESETB_EN));

    if (p->u1PLLMode == PHYPLL_MODE)
    {
        mcSHOW_DBG_MSG2(("PHYPLL\n"));
        u1Pll1Val = u1SetVal;
        u1Pll2Val = 0;
    }
    else
    {
        mcSHOW_DBG_MSG2(("CLRPLL\n"));
        u1Pll1Val = 0;
        u1Pll2Val = u1SetVal;
    }

    vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0,
            P_Fld(u1Pll1Val, LPIF_LOW_POWER_CFG_0_PHYPLL_SHU_EN) |
            P_Fld(u1Pll1Val, LPIF_LOW_POWER_CFG_0_PHYPLL_MODE_SW) |
            P_Fld(u1Pll2Val, LPIF_LOW_POWER_CFG_0_PHYPLL2_SHU_EN) |
            P_Fld(u1Pll2Val, LPIF_LOW_POWER_CFG_0_PHYPLL2_MODE_SW));


    vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_FSM_CFG_1,

            P_Fld(0x0, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL) |
            P_Fld(0x0, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL_2ND) |
            P_Fld(0x0, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL_FOR_PWR) |
            P_Fld(0x0, LPIF_FSM_CFG_1_LPIF_LEGACY_CONTROL_FOR_PWR_2ND) |
            P_Fld(0x1, LPIF_FSM_CFG_1_LPIF_OUTPUT_PATH_FROM_SW) |
            P_Fld(0x1, LPIF_FSM_CFG_1_LPIF_OUTPUT_PATH_FROM_SW_2ND) |
            P_Fld(0x1, LPIF_FSM_CFG_1_LPIF_POWER_CONTROL_SEL) |
            P_Fld(0x1, LPIF_FSM_CFG_1_LPIF_POWER_CONTROL_SEL_2ND));

    vIO32WriteFldMulti_All(DDRPHY_MD32_REG_LPIF_FSM_OUT_CTRL_0,
            P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_PHYPLL_EN) |
            P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_DLL_EN) |
            P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_2ND_DLL_EN) |
            P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_DLL_CK_EN) |
            P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_DPY_VREF_EN) |
            P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_PHYPLL_SHU_EN) |
            P_Fld(0x1, LPIF_FSM_OUT_CTRL_0_LOG_OPT_PHYPLL_MODE_SW));

    u1ShuSramVal = u1CurrShuLevel;

    if (p->support_channel_num > 1)
        u1ShuSramVal |= u1CurrShuLevel << 4;


    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, u1ShuSramVal, LPIF_LOW_POWER_CFG_1_DR_SHU_SRAM_LEVEL);

#if __ETT__
    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_SSPM_CFGREG_GPR0, 0xE7700E77, SSPM_CFGREG_GPR0_GPR0);
#endif

    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0x0, MISC_RG_DFS_CTRL_SPM_DVFS_CONTROL_SEL);
    vIO32WriteFldAlign_All(DDRPHY_REG_PHYPLL0, 0x0, PHYPLL0_RG_RPHYPLL_EN);
    vIO32WriteFldAlign_All(DDRPHY_REG_CLRPLL0, 0x0, CLRPLL0_RG_RCLRPLL_EN);


    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_DFD_DBUG_0 , 0x1, LPIF_DFD_DBUG_0_LPIF_DFD_DEBUG_ISO_EN);
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
        u1SramShuIdx = gFreqTbl[u1ShuffleIdx].SRAMIdx;
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
        BFSP = (gFreqTbl[u1ShuffleIdx].freq_sel <= LP4_DDR2667)? 0x1: 0x0;
        //mcSHOW_DBG_MSG(("[DFSBypassMR13HwSet] BPFSP_SHU%d = 0x%x\n", u1SramShuIdx, BFSP));
        vIO32WriteFldAlign_All(TransferReg.u4Addr, BFSP, TransferReg.u4Fld);
    }
    vIO32WriteFldAlign_All(DRAMC_REG_TX_FREQ_RATIO_OLD_MODE0, 0x1, TX_FREQ_RATIO_OLD_MODE0_SHUFFLE_LEVEL_MODE_SELECT);
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
#if 0
static U8 vGet_Current_SRAMShuLevel(DRAMC_CTX_T *p)
{
    U8 curr_shu_level;

    curr_shu_level = u4IO32ReadFldAlign(DDRPHY_REG_MISC_RG_DFS_CTRL, MISC_RG_DFS_CTRL_RG_DR_SHU_LEVEL_SRAM);

    return curr_shu_level;
}
#endif
void DramcSaveToShuffleSRAM(DRAMC_CTX_T *p, DRAM_DFS_REG_SHU_T srcRG, DRAM_DFS_SRAM_SHU_T dstRG)
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
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 1, MISC_SRAM_DMA0_SRAM_WR_MODE);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 0, MISC_SRAM_DMA0_APB_WR_MODE);

        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), srcRG, MISC_SRAM_DMA0_SW_SHU_LEVEL_APB);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), dstRG, MISC_SRAM_DMA0_SW_SHU_LEVEL_SRAM);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 1, MISC_SRAM_DMA0_SW_DMA_FIRE);
        do {
            u1value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DMA_DEBUG0), MISC_DMA_DEBUG0_SRAM_DONE);
            u1value |= (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DMA_DEBUG0), MISC_DMA_DEBUG0_APB_DONE) << 1);
            mcSHOW_DBG_MSG4(("\twait dramc to shuffle sram done.\n"));
        } while (u1value != 0x3);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 0, MISC_SRAM_DMA0_SW_DMA_FIRE);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 0, MISC_SRAM_DMA0_SW_STEP_EN_MODE);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 0, MISC_SRAM_DMA0_SW_MODE);
    }

    vSetPHY2ChannelMapping(p, eOriChannel);
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_SRAM_DMA0, 0, MISC_SRAM_DMA0_SRAM_WR_MODE);
}

void LoadShuffleSRAMtoDramc(DRAMC_CTX_T *p, DRAM_DFS_SRAM_SHU_T srcRG, DRAM_DFS_REG_SHU_T dstRG)
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
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 0, MISC_SRAM_DMA0_SRAM_WR_MODE);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 1, MISC_SRAM_DMA0_APB_WR_MODE);

        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), dstRG, MISC_SRAM_DMA0_SW_SHU_LEVEL_APB);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), srcRG, MISC_SRAM_DMA0_SW_SHU_LEVEL_SRAM);
        vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_SRAM_DMA0), 1, MISC_SRAM_DMA0_SW_DMA_FIRE);
        do {
            u1value = u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DMA_DEBUG0), MISC_DMA_DEBUG0_SRAM_DONE);
            u1value |= (u4IO32ReadFldAlign(DRAMC_REG_ADDR(DDRPHY_REG_MISC_DMA_DEBUG0), MISC_DMA_DEBUG0_APB_DONE) << 1);
            mcSHOW_DBG_MSG4(("\twait shuffle sram to dramc done.\n"));
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

    return u1AckDone;
}

void DramcDFSDirectJump_SRAMShuRGMode(DRAMC_CTX_T *p, DRAM_DFS_SRAM_SHU_T shu_level)
{
    U8 u1ShuAck = 0, u1SramAck = 0;
    U8 i = 0;
    U8 u1ChkComplete = 1;

    if (p->u1PLLMode == PHYPLL_MODE)
    {
        mcSHOW_DBG_MSG4(("Disable CLRPLL\n"));
        vIO32WriteFldAlign_All(DDRPHY_REG_CLRPLL0, 0, CLRPLL0_RG_RCLRPLL_EN);
    }
    else
    {
        mcSHOW_DBG_MSG4(("Disable PHYPLL\n"));
        vIO32WriteFldAlign_All(DDRPHY_REG_PHYPLL0, 0, PHYPLL0_RG_RPHYPLL_EN);
    }

    for (i = 0; i < p->support_channel_num; i++)
    {
        u1ShuAck |= (0x1 << i);
    }


    u1SramAck = u1ShuAck;
    u1ShuAck &= 0x3;

    if (p->u1PLLMode == PHYPLL_MODE)
    {
        mcSHOW_DBG_MSG4(("DFSDirectJump to CLRPLL, SHU_LEVEL=%d, ACK=%x\n", shu_level, u1ShuAck));
    }
    else
    {
        mcSHOW_DBG_MSG4(("DFSDirectJump to PHYPLL, SHU_LEVEL=%d, ACK=%x\n", shu_level, u1ShuAck));
    }


    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_DDRPHY_FB_CK_EN);


    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_DR_SHU_LEVEL_SRAM_LATCH);
    mcDELAY_US(1);
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_DR_SHU_LEVEL_SRAM_LATCH);

    if (p->u1PLLMode == PHYPLL_MODE)
    {
        vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_PHYPLL_SHU_EN);
        vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, !p->u1PLLMode, MISC_RG_DFS_CTRL_RG_DR_SHU_LEVEL);
        vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_PHYPLL2_SHU_EN);
        mcSHOW_DBG_MSG4(("Enable CLRPLL\n"));
    }
    else
    {
        vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_PHYPLL2_SHU_EN);
        vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, !p->u1PLLMode, MISC_RG_DFS_CTRL_RG_DR_SHU_LEVEL);
        vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_PHYPLL_SHU_EN);
        mcSHOW_DBG_MSG4(("Enable PHYPLL\n"));
    }
    mcDELAY_US(1);

#if 1
    //vIO32WriteFldMulti((DDRPHY_MISC_SPM_CTRL3), P_Fld(0, MISC_SPM_CTRL3_RG_DR_SHU_LEVEL_SRAM_CH1)
    //                    | P_Fld(0, MISC_SPM_CTRL3_RG_DR_SHU_LEVEL_SRAM_CH0));
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, shu_level, MISC_RG_DFS_CTRL_RG_DR_SHU_LEVEL_SRAM);


    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_DR_SRAM_LOAD);
    //while (!u4IO32ReadFldAlign(DDRPHY_MISC_DMA_DEBUG0, MISC_DMA_DEBUG0_SC_DR_SRAM_PLL_LOAD_ACK));
    while (WaitChShuEnAck(p, DDRPHY_REG_MISC_DMA_DEBUG0, MISC_DMA_DEBUG0_SC_DR_SRAM_LOAD_ACK, u1ChkComplete) != u1SramAck)
    //while (!u4IO32ReadFldAlign(DDRPHY_REG_MISC_DMA_DEBUG0, MISC_DMA_DEBUG0_SC_DR_SRAM_LOAD_ACK))
    {
        mcSHOW_DBG_MSG4(("\twait sram load ack.\n"));
    }
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_DR_SRAM_LOAD);
#endif

    if (p->u1PLLMode == PHYPLL_MODE)
    {
        //vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 1, MISC_SPM_CTRL1_RG_PHYPLL2_MODE_SW);
        vIO32WriteFldAlign_All(DDRPHY_REG_CLRPLL0, 1, CLRPLL0_RG_RCLRPLL_EN);
    }
    else
    {
        //vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 1, MISC_SPM_CTRL1_RG_PHYPLL_MODE_SW);
        vIO32WriteFldAlign_All(DDRPHY_REG_PHYPLL0, 1, PHYPLL0_RG_RPHYPLL_EN);
    }

    #if 0
    EntryDFSDebugMode(p, CHG_CLK_MODE);
    #endif

#if ENABLE_DDR800_SOPEN_DSC_WA
    DDR800_SOPEN_DSC_WA(p, shu_level, DISABLE);
#endif

#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
    mcDELAY_US(20);
#else
    mcDELAY_XUS(20);
#endif

#if 0
    mcSHOW_DBG_MSG3(("Enable SHORT-QUEUE\n"));
    vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 1, MISC_SPM_CTRL1_RG_DR_SHORT_QUEUE);

    mcSHOW_DBG_MSG3(("\twait 5us for short queue ack.\n"));
    mcDELAY_US(5);
#endif

    //mcSHOW_DBG_MSG(("Disable RX-Tracking\n"));
    //vIO32WriteFldAlign(SPM_SW_RSV_8, 0, SW_RSV_8_RX_TRACKING_EN);

    mcSHOW_DBG_MSG4(("SHUFFLE Start\n"));
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_DR_SHU_EN);

#if DFS_NOQUEUE_FLUSH_ENABLE && ENABLE_DFS_NOQUEUE_FLUSH_DBG
    WaitNoQueueFlushComplete(p);
#endif


#if 1
    //mcSHOW_DBG_MSG3(("\twait 5us for shu_en ack.\n"));
    //mcDELAY_US(5);
    #if 0
    ChkDFSDebugMode(p, CHG_CLK_MODE);
    #endif


if (channel_num_auxadc <= 2) {
    while ((u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4, LPIF_STATUS_4_DR_SHU_EN_ACK) & u1ShuAck) != u1ShuAck)
    {
	mcSHOW_DBG_MSG4(("\twait shu_en ack.\n"));
    }
    }
else {
    while ((u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4, LPIF_STATUS_4_DR_SHU_EN_ACK) & u1ShuAck) != u1ShuAck
	    || (u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4+SHIFT_TO_CHB_ADDR, LPIF_STATUS_4_DR_SHU_EN_ACK) & u1ShuAck) != u1ShuAck)
    {
        mcSHOW_DBG_MSG4(("\twait shu_en ack.\n"));
    }
    }

#else
    while (u4IO32ReadFldAlign(DRAMC_REG_MRR_STATUS2, MRR_STATUS2_DVFS_STATE) != u1ShuAckState);
#endif

    #if 0
    ExitDFSDebugMode(p, CHG_CLK_MODE);
    #endif

#if ENABLE_TX_REBASE_WDQS_DQS_PI_WA
    TxReBaseWDQSDqsPiWA(p, !p->u1PLLMode);
#endif

#if ENABLE_TX_REBASE_ODT_WA
    TxReadBaseODTWA(p, shu_level);
#endif

    //vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 0, MISC_SPM_CTRL1_RG_DR_SHORT_QUEUE);
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_DR_SHU_EN);
    mcSHOW_DBG_MSG4(("SHUFFLE End\n"));

#if ENABLE_DDR800_SOPEN_DSC_WA
    DDR800_SOPEN_DSC_WA(p, shu_level, ENABLE);
#endif

    //if(shu_level == 0)//LP4-2CH
    //{
        //mcSHOW_DBG_MSG(("Enable RX-Tracking for shuffle-0\n"));
        //vIO32WriteFldAlign(SPM_SW_RSV_8, 3, SW_RSV_8_RX_TRACKING_EN);
    //}

    if (p->u1PLLMode == PHYPLL_MODE)
    {
        //vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 0, MISC_SPM_CTRL1_RG_PHYPLL_MODE_SW);
        vIO32WriteFldAlign_All(DDRPHY_REG_PHYPLL0, 0, PHYPLL0_RG_RPHYPLL_EN);
    }
    else
    {
        //vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 0, MISC_SPM_CTRL1_RG_PHYPLL2_MODE_SW);
        vIO32WriteFldAlign_All(DDRPHY_REG_CLRPLL0, 0, CLRPLL0_RG_RCLRPLL_EN);
    }

#if 1
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_DR_SRAM_RESTORE);
    while (WaitChShuEnAck(p, DDRPHY_REG_MISC_DMA_DEBUG0, MISC_DMA_DEBUG0_SC_DR_SRAM_RESTORE_ACK, u1ChkComplete) != u1SramAck)
    //while (!u4IO32ReadFldAlign(DDRPHY_REG_MISC_DMA_DEBUG0, MISC_DMA_DEBUG0_SC_DR_SRAM_RESTORE_ACK))
    {
        mcSHOW_DBG_MSG4(("\twait sram restore ack.\n"));
    }
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_DR_SRAM_RESTORE);


    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_DDRPHY_FB_CK_EN);
#endif

    #if ENABLE_TIMING_TXSR_DFS_WA
    TimingTxsrWA(p, shu_level);
    #endif

    p->u1PLLMode = !p->u1PLLMode;

    mcSHOW_DBG_MSG4(("Shuffle flow complete\n"));

    return;
}


void DramcDFSDirectJump_RGMode(DRAMC_CTX_T *p, DRAM_DFS_REG_SHU_T shu_level)
{
    U8 u1ShuAck = 0;
    U8 i = 0;
    U8 u1shu_level = 0;

    if (p->u1PLLMode == PHYPLL_MODE)
    {
        mcSHOW_DBG_MSG4(("Disable CLRPLL\n"));
        vIO32WriteFldAlign_All(DDRPHY_REG_CLRPLL0, 0, CLRPLL0_RG_RCLRPLL_EN);
    }
    else
    {
        mcSHOW_DBG_MSG4(("Disable PHYPLL\n"));
        vIO32WriteFldAlign_All(DDRPHY_REG_PHYPLL0, 0, PHYPLL0_RG_RPHYPLL_EN);
    }

    for (i = 0; i < p->support_channel_num; i++)
    {
        u1ShuAck |= (0x1 << i);
    }


    u1ShuAck &= 0x3;

    if (p->u1PLLMode == PHYPLL_MODE)
    {
        mcSHOW_DBG_MSG4(("DFSDirectJump_RGMode to CLRPLL, SHU_LEVEL=%d, ACK=%x\n", shu_level, u1ShuAck));
    }
    else
    {
        mcSHOW_DBG_MSG4(("DFSDirectJump_RGMode to PHYPLL, SHU_LEVEL=%d, ACK=%x\n", shu_level, u1ShuAck));
    }


    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_DDRPHY_FB_CK_EN);

    if (shu_level == DRAM_DFS_REG_SHU0)
        u1shu_level = shu_level;
    else
        u1shu_level = 1;

    if (p->u1PLLMode == PHYPLL_MODE)
    {
        vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_PHYPLL_SHU_EN);
        vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, u1shu_level, MISC_RG_DFS_CTRL_RG_DR_SHU_LEVEL);
        vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_PHYPLL2_SHU_EN);
        mcSHOW_DBG_MSG4(("Enable CLRPLL\n"));
    }
    else
    {
        vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_PHYPLL2_SHU_EN);
        vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, u1shu_level, MISC_RG_DFS_CTRL_RG_DR_SHU_LEVEL);
        vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_PHYPLL_SHU_EN);
        mcSHOW_DBG_MSG4(("Enable PHYPLL\n"));
    }
    mcDELAY_US(1);

    if (p->u1PLLMode == PHYPLL_MODE)
    {
        //vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 1, MISC_SPM_CTRL1_RG_PHYPLL2_MODE_SW);
        vIO32WriteFldAlign_All(DDRPHY_REG_CLRPLL0, 1, CLRPLL0_RG_RCLRPLL_EN);
    }
    else
    {
        //vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 1, MISC_SPM_CTRL1_RG_PHYPLL_MODE_SW);
        vIO32WriteFldAlign_All(DDRPHY_REG_PHYPLL0, 1, PHYPLL0_RG_RPHYPLL_EN);
    }

#if (FOR_DV_SIMULATION_USED == 0 && SW_CHANGE_FOR_SIMULATION == 0)
    mcDELAY_US(20);
#else
    mcDELAY_XUS(20);
#endif

#if 0
    mcSHOW_DBG_MSG3(("Enable SHORT-QUEUE\n"));
    vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 1, MISC_SPM_CTRL1_RG_DR_SHORT_QUEUE);

    mcSHOW_DBG_MSG3(("\twait 5us for short queue ack.\n"));
    mcDELAY_US(5);
#endif

    //mcSHOW_DBG_MSG(("Disable RX-Tracking\n"));
    //vIO32WriteFldAlign(SPM_SW_RSV_8, 0, SW_RSV_8_RX_TRACKING_EN);


    mcSHOW_DBG_MSG4(("SHUFFLE Start\n"));
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 1, MISC_RG_DFS_CTRL_RG_DR_SHU_EN);

    //mcSHOW_DBG_MSG3(("\twait 5us for shu_en ack.\n"));
    //mcDELAY_US(5);
    //while (WaitChShuEnAck(p, DRAMC_REG_MRR_STATUS2, MRR_STATUS2_DVFS_STATE, u1ShuAckState) != u1ShuAck)

if (channel_num_auxadc <= 2) {
    while ((u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4, LPIF_STATUS_4_DR_SHU_EN_ACK) & u1ShuAck) != u1ShuAck)
	{
        mcSHOW_DBG_MSG4(("\twait shu_en ack.\n"));
    }
    }
else {
    while ((u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4, LPIF_STATUS_4_DR_SHU_EN_ACK) & u1ShuAck) != u1ShuAck
//#if CHANNEL_NUM > 2
	    || (u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4+SHIFT_TO_CHB_ADDR, LPIF_STATUS_4_DR_SHU_EN_ACK) & u1ShuAck) != u1ShuAck)
//#endif
    {
        mcSHOW_DBG_MSG4(("\twait shu_en ack.\n"));
    }
	}

    //vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 0, MISC_SPM_CTRL1_RG_DR_SHORT_QUEUE);
    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_DR_SHU_EN);
    mcSHOW_DBG_MSG4(("SHUFFLE End\n"));

    //if(shu_level == 0)//LP4-2CH
    //{
        //mcSHOW_DBG_MSG(("Enable RX-Tracking for shuffle-0\n"));
        //vIO32WriteFldAlign(SPM_SW_RSV_8, 3, SW_RSV_8_RX_TRACKING_EN);
    //}

    if (p->u1PLLMode == PHYPLL_MODE)
    {

        //vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 0, MISC_SPM_CTRL1_RG_PHYPLL_MODE_SW);
        vIO32WriteFldAlign_All(DDRPHY_REG_PHYPLL0, 0, PHYPLL0_RG_RPHYPLL_EN);
    }
    else
    {

        //vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 0, MISC_SPM_CTRL1_RG_PHYPLL2_MODE_SW);
        vIO32WriteFldAlign_All(DDRPHY_REG_CLRPLL0, 0, CLRPLL0_RG_RCLRPLL_EN);
    }


    vIO32WriteFldAlign_All(DDRPHY_REG_MISC_RG_DFS_CTRL, 0, MISC_RG_DFS_CTRL_RG_DDRPHY_FB_CK_EN);

    mcSHOW_DBG_MSG4(("Shuffle flow complete\n"));

    p->u1PLLMode = !p->u1PLLMode;
    return;
}

void DramcDFSDirectJump_SPMMode(DRAMC_CTX_T *p, DRAM_DFS_SRAM_SHU_T shu_level)
{
    U8 u1ShuAck = 0, u1EnDPMCh = 0;
    U8 u1ChIdx = 0;
    U8 u1ChNum_dpm = (p->support_channel_num==CHANNEL_SINGLE)?0x1:0x2;
    U8 pingpong_shu_level = 0;
    U8 u1PingPong = 0;
    U16 u2SramLevel = 0;

    for (u1ChIdx = 0; u1ChIdx < u1ChNum_dpm; u1ChIdx++)
    {
        u1ShuAck |= (0x1 << u1ChIdx);
        u1EnDPMCh |= (0x1 << u1ChIdx);
    }

    if (p->u1PLLMode == PHYPLL_MODE)
    {
        mcSHOW_DBG_MSG4(("DramcDFSDirectJump_SPMMode to CLRPLL, SHU_LEVEL=%d, ACK=%x\n", shu_level, u1ShuAck));
    }
    else
    {
        mcSHOW_DBG_MSG4(("DramcDFSDirectJump_SPMMode to PHYPLL, SHU_LEVEL=%d, ACK=%x\n", shu_level, u1ShuAck));
    }

    //vIO32WriteFldAlign(DDRPHY_REG_MISC_STBCAL2, 0x1, MISC_STBCAL2_STB_DBG_STATUS);

    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnDPMCh, LPIF_LOW_POWER_CFG_0_DDRPHY_FB_CK_EN);
    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_2, u1EnDPMCh, LPIF_LOW_POWER_CFG_2_DR_SHU_LEVEL_SRAM_LATCH);
    mcDELAY_US(1);
    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_2, 0, LPIF_LOW_POWER_CFG_2_DR_SHU_LEVEL_SRAM_LATCH);


    pingpong_shu_level = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_10, LPIF_STATUS_10_DRAMC_DR_SHU_LEVEL);
    mcSHOW_DBG_MSG4(("Ping-pong CONF%d\n", (pingpong_shu_level & 0x1)));
    for (u1ChIdx = 0; u1ChIdx < u1ChNum_dpm; u1ChIdx++)
    {
        u2SramLevel |= (shu_level << (u1ChIdx*4));
        u1PingPong |= (!(pingpong_shu_level & 0x1)) << (u1ChIdx*2);
    }
    pingpong_shu_level = u1PingPong;

    if (p->u1PLLMode == PHYPLL_MODE)
    {
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_PHYPLL_SHU_EN);
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, pingpong_shu_level, LPIF_LOW_POWER_CFG_1_DR_SHU_LEVEL);
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnDPMCh, LPIF_LOW_POWER_CFG_0_PHYPLL2_SHU_EN);
        mcSHOW_DBG_MSG4(("Enable CLRPLL (0x%x 0x%x)\n", pingpong_shu_level, u2SramLevel));
    }
    else
    {
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_PHYPLL2_SHU_EN);
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, pingpong_shu_level, LPIF_LOW_POWER_CFG_1_DR_SHU_LEVEL);
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnDPMCh, LPIF_LOW_POWER_CFG_0_PHYPLL_SHU_EN);
        mcSHOW_DBG_MSG4(("Enable PHYPLL (0x%x 0x%x)\n", pingpong_shu_level, u2SramLevel));
    }
    mcDELAY_US(1);

#if ENABLE_DFS_RUNTIME_MRW
    DFSRuntimeMRWEn(p, p->u1PLLMode, shu_level);
#endif

#if 0
    vIO32WriteFldAlign(SPM_SPM_POWER_ON_VAL0, 0, SPM_POWER_ON_VAL0_SC_DR_SHU_LEVEL);
    vIO32WriteFldAlign(SPM_SPM_POWER_ON_VAL0, shu_level, SPM_POWER_ON_VAL0_SC_DR_SHU_LEVEL);
#else
    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, u2SramLevel, LPIF_LOW_POWER_CFG_1_DR_SHU_SRAM_LEVEL);
    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, u1EnDPMCh, LPIF_LOW_POWER_CFG_1_DR_SRAM_LOAD);

if (channel_num_auxadc <= 2) {
    while ((u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4, LPIF_STATUS_4_DR_SRAM_LOAD_ACK) & u1ShuAck) != u1ShuAck)
    {
        mcSHOW_DBG_MSG4(("\twait sram load ack.\n"));
    }
    }
else {
    while ((u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4, LPIF_STATUS_4_DR_SRAM_LOAD_ACK) & u1ShuAck) != u1ShuAck
//#if CHANNEL_NUM > 2
	    || (u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4+SHIFT_TO_CHB_ADDR, LPIF_STATUS_4_DR_SRAM_LOAD_ACK) & u1ShuAck) != u1ShuAck)
//#endif
    {
        mcSHOW_DBG_MSG4(("\twait sram load ack.\n"));
    }
    }
    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, 0, LPIF_LOW_POWER_CFG_1_DR_SRAM_LOAD);
#endif

    //vIO32WriteFldAlign(DDRPHY_REG_MISC_STBCAL2, 0x2, MISC_STBCAL2_STB_DBG_STATUS);

#if ENABLE_DFS_SSC_WA
    DVS_DMY_RD_ENTR(p);
#endif

    if (p->u1PLLMode == PHYPLL_MODE)
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnDPMCh, LPIF_LOW_POWER_CFG_0_PHYPLL2_MODE_SW);
    else
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnDPMCh, LPIF_LOW_POWER_CFG_0_PHYPLL_MODE_SW);

#if ENABLE_DFS_SSC_WA
    DramcSSCHoppingOnOff(p, shu_level, ENABLE);
#endif

#if ENABLE_DFS_RUNTIME_MRW
    DFSRTMRW_HwsetWA(p, p->u1PLLMode, !p->u1PLLMode, u1ChNum_dpm);
#else
    DFSHwSetWA(p, p->u1PLLMode, shu_level, u1ChNum_dpm);
#endif

    #if ENABLE_DFS_DEBUG_MODE
    EntryDFSDebugMode(p, CHG_CLK_MODE);
    #endif

    EnableDramcTrackingByShuffle(p, u1EnDPMCh, DISABLE);

#if ENABLE_DDR800_SOPEN_DSC_WA
    DDR800_SOPEN_DSC_WA(p, shu_level, DISABLE);
#endif

    mcDELAY_US(20);


    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_DDRPHY_FB_CK_EN);


    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnDPMCh, LPIF_LOW_POWER_CFG_0_DDRPHY_FB_CK_EN);

    //func_imp_tracking_value_backup();
    //func_imp_tracking_off();
    //func_force_mm_ultra();

#if ENABLE_DFS_SSC_WA
    DVS_DMY_RD_EXIT(p);
    //DramcSSCHoppingOnOff(p, cur_shu_level, ENABLE);
    //mcDELAY_US(10);
#endif

#if ENABLE_DDR800_OPEN_LOOP_MODE_OPTION
    DDR800semiPowerSavingOn(p, shu_level, DISABLE);
#endif

#if (ENABLE_TX_TRACKING && TX_RETRY_ENABLE)
    SPMTx_Track_Retry_OnOff(p, shu_level, ENABLE);
#endif

    mcSHOW_DBG_MSG4(("SHUFFLE Start\n"));
    //vIO32WriteFldAlign(DDRPHY_REG_MISC_STBCAL2, 0x3, MISC_STBCAL2_STB_DBG_STATUS);
    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnDPMCh, LPIF_LOW_POWER_CFG_0_DR_SHU_EN);

    #if ENABLE_DFS_DEBUG_MODE
    ChkDFSDebugMode(p, CHG_CLK_MODE);

    ExitDFSDebugMode(p, CHG_CLK_MODE);
    #endif

if (channel_num_auxadc <= 2) {
    while ((u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4, LPIF_STATUS_4_DR_SHU_EN_ACK) & u1ShuAck) != u1ShuAck)
	{
		mcSHOW_DBG_MSG4(("\twait shu_en ack.\n"));
	}
    }
else {
    while ((u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4, LPIF_STATUS_4_DR_SHU_EN_ACK) & u1ShuAck) != u1ShuAck
//#if CHANNEL_NUM > 2
	    || (u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4+SHIFT_TO_CHB_ADDR, LPIF_STATUS_4_DR_SHU_EN_ACK) & u1ShuAck) != u1ShuAck)
//#endif
    {
        mcSHOW_DBG_MSG4(("\twait shu_en ack.\n"));
    }
    }
#if DFS_NOQUEUE_FLUSH_LATENCY_CNT
    U8 MaxCnt = u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_RESERVED_6, LPIF_RESERVED_6_MAX_CNT_SHU_EN_HIGH_TO_ACK);

    mcSHOW_DBG_MSG2(("\tMAX CNT = %d\n", MaxCnt));
#endif

#if ENABLE_TX_REBASE_WDQS_DQS_PI_WA
    TxReBaseWDQSDqsPiWA(p, !p->u1PLLMode);
#endif

#if ENABLE_TX_REBASE_ODT_WA
    TxReadBaseODTWA(p, shu_level);
#endif
    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_DR_SHU_EN);
    //vIO32WriteFldAlign(DDRPHY_REG_MISC_STBCAL2, 0x4, MISC_STBCAL2_STB_DBG_STATUS);
    mcSHOW_DBG_MSG4(("SHUFFLE End\n"));

    if (p->u1PLLMode == PHYPLL_MODE)
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_PHYPLL_MODE_SW);
    else
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_PHYPLL2_MODE_SW);

#if ENABLE_DDR800_OPEN_LOOP_MODE_OPTION
    DDR800semiPowerSavingOn(p, shu_level, ENABLE);
#endif

#if (ENABLE_TX_TRACKING && TX_RETRY_ENABLE)
    SPMTx_Track_Retry_OnOff(p, shu_level, DISABLE);
#endif

#if ENABLE_DFS_SSC_WA
    DramcSSCHoppingOnOff(p, shu_level, DISABLE);
#endif

#if ENABLE_DDR800_SOPEN_DSC_WA
    DDR800_SOPEN_DSC_WA(p, shu_level, ENABLE);
#endif

    //func_imp_tracking_on();
#if 1
    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_2, u1EnDPMCh, LPIF_LOW_POWER_CFG_2_DR_SRAM_RESTORE);

if (channel_num_auxadc <= 2) {
    while ((u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4, LPIF_STATUS_4_DR_SRAM_RESTORE_ACK) & u1ShuAck) != u1ShuAck)
	{
        mcSHOW_DBG_MSG4(("\twait sram restore ack.\n"));
    }
	}
else {
    while ((u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4, LPIF_STATUS_4_DR_SRAM_RESTORE_ACK) & u1ShuAck) != u1ShuAck
//#if CHANNEL_NUM > 2
	    || (u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4+SHIFT_TO_CHB_ADDR, LPIF_STATUS_4_DR_SRAM_RESTORE_ACK) & u1ShuAck) != u1ShuAck)
//#endif
    {
        mcSHOW_DBG_MSG4(("\twait sram restore ack.\n"));
    }
	}
    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_2, 0, LPIF_LOW_POWER_CFG_2_DR_SRAM_RESTORE);
#endif


    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_DDRPHY_FB_CK_EN);
    EnableDramcTrackingByShuffle(p, u1EnDPMCh, ENABLE);

    //func_dram_dummy_read_on();
    //mcDELAY_US(2);
    //func_dram_dummy_read_off();

    p->u1PLLMode = !p->u1PLLMode;

    //vIO32WriteFldAlign(DDRPHY_REG_MISC_STBCAL2, 0x5, MISC_STBCAL2_STB_DBG_STATUS);
    mcSHOW_DBG_MSG4(("Shuffle flow complete\n"));

    return;
}

void DramcDFSDirectJump_SPMMode_forK(DRAMC_CTX_T *p, DRAM_DFS_REG_SHU_T shu_level)
{
    U8 u1ShuAck = 0, u1EnDPMCh = 0;
    U8 u1ChIdx = 0;
    U8 u1ChNum_dpm = (p->support_channel_num==CHANNEL_SINGLE)?0x1:0x2;
    U8 u1dpm_shu_level = 0; // for Conf0/1

    for (u1ChIdx = 0; u1ChIdx < u1ChNum_dpm; u1ChIdx++)
    {
        u1ShuAck |= (0x1 << u1ChIdx);
        u1EnDPMCh |= (0x1 << u1ChIdx);
    }

    if (p->u1PLLMode == PHYPLL_MODE)
    {
        mcSHOW_DBG_MSG4(("DramcDFSDirectJump_SPMMode_forK to CLRPLL, SHU_LEVEL=%d, ACK=%x\n", shu_level, u1ShuAck));
    }
    else
    {
        mcSHOW_DBG_MSG4(("DramcDFSDirectJump_SPMMode_forK to PHYPLL, SHU_LEVEL=%d, ACK=%x\n", shu_level, u1ShuAck));
    }


    mcSHOW_DBG_MSG4(("Direct jump to CONF%d\n", shu_level));
    for (u1ChIdx = 0; u1ChIdx < u1ChNum_dpm; u1ChIdx++)
    {
        u1dpm_shu_level |= (shu_level<< (u1ChIdx*2));
    }

    if (p->u1PLLMode == PHYPLL_MODE)
    {
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_PHYPLL_SHU_EN);
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, u1dpm_shu_level, LPIF_LOW_POWER_CFG_1_DR_SHU_LEVEL);
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnDPMCh, LPIF_LOW_POWER_CFG_0_PHYPLL2_SHU_EN);
        mcSHOW_DBG_MSG4(("Enable CLRPLL\n"));
    }
    else
    {
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_PHYPLL2_SHU_EN);
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_1, u1dpm_shu_level, LPIF_LOW_POWER_CFG_1_DR_SHU_LEVEL);
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnDPMCh, LPIF_LOW_POWER_CFG_0_PHYPLL_SHU_EN);
        mcSHOW_DBG_MSG4(("Enable PHYPLL\n"));
    }
    mcDELAY_US(1);

    if (p->u1PLLMode == PHYPLL_MODE)
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnDPMCh, LPIF_LOW_POWER_CFG_0_PHYPLL2_MODE_SW);
    else
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnDPMCh, LPIF_LOW_POWER_CFG_0_PHYPLL_MODE_SW);

    mcDELAY_US(20);


    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_DDRPHY_FB_CK_EN);


    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnDPMCh, LPIF_LOW_POWER_CFG_0_DDRPHY_FB_CK_EN);


    mcSHOW_DBG_MSG4(("SHUFFLE Start\n"));
    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, u1EnDPMCh, LPIF_LOW_POWER_CFG_0_DR_SHU_EN);

if (channel_num_auxadc <= 2) {
    while ((u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4, LPIF_STATUS_4_DR_SHU_EN_ACK) & u1ShuAck) != u1ShuAck)
    {
        mcSHOW_DBG_MSG4(("\twait shu_en ack.\n"));
    }
    }
else {
    while ((u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4, LPIF_STATUS_4_DR_SHU_EN_ACK) & u1ShuAck) != u1ShuAck
//#if CHANNEL_NUM > 2
	    || (u4IO32ReadFldAlign(DDRPHY_MD32_REG_LPIF_STATUS_4+SHIFT_TO_CHB_ADDR, LPIF_STATUS_4_DR_SHU_EN_ACK) & u1ShuAck) != u1ShuAck)
//#endif
    {
        mcSHOW_DBG_MSG4(("\twait shu_en ack.\n"));
    }
	}
    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_DR_SHU_EN);
    mcSHOW_DBG_MSG4(("SHUFFLE End\n"));

    if (p->u1PLLMode == PHYPLL_MODE)
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_PHYPLL_MODE_SW);
    else
        vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_PHYPLL2_MODE_SW);


    vIO32WriteFldAlign_All(DDRPHY_MD32_REG_LPIF_LOW_POWER_CFG_0, 0, LPIF_LOW_POWER_CFG_0_DDRPHY_FB_CK_EN);

    p->u1PLLMode = !p->u1PLLMode;

    mcSHOW_DBG_MSG4(("Shuffle flow complete\n"));

    return;
}

void DramcDFSDirectJump(DRAMC_CTX_T *p, U8 shu_level)
{
#if (DRAMC_DFS_MODE == 2)
    gDVFSCtrlSel = 2;
#elif (DRAMC_DFS_MODE == 1)
    gDVFSCtrlSel = 1;
#elif (DRAMC_DFS_MODE == 0)
    gDVFSCtrlSel = 0;
#endif

    if (gDVFSCtrlSel == 0)
    {
        if (shu_level == SRAM_SHU0)
            DramcDFSDirectJump_RGMode(p, 0);
        else
            DramcDFSDirectJump_RGMode(p, 1);
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
#if 0
static void No_Parking_On_CLRPLL(DRAMC_CTX_T *p)
{
    if (p->u1PLLMode == PHYPLL_MODE) return;

    DramcDFSDirectJump_RGMode(p, DRAM_DFS_REG_SHU0);
}
#endif
void ShuffleDfsToOriginalFSP(DRAMC_CTX_T *p)
{
    U8 operating_fsp = p->dram_fsp;
    U8 u1RankIdx, backup_rank= u1GetRank(p);


    if (operating_fsp == FSP_1)
    {
        cbt_dfs_mr13_global(p, CBT_HIGH_FREQ);
        cbt_switch_freq(p, CBT_HIGH_FREQ);
    }
    else
    {
        for(u1RankIdx =0; u1RankIdx < p->support_rank_num; u1RankIdx++)
        {
            vSetRank(p, u1RankIdx);
            DramcMRWriteFldAlign(p, 13, 0, MR13_FSP_OP, TO_MR);
        }
        vSetRank(p, backup_rank);
    }
}

