/* $NoKeywords:$ */
/**
 * @file
 *
 * mnphytn.c
 *
 * Northbridge Phy support for TN
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/TN)
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 **/
/*****************************************************************************
*
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* ***************************************************************************
*
*/


/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "mport.h"
#include "ma.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mu.h"
#include "OptionMemory.h"       // need def for MEM_FEAT_BLOCK_NB
#include "mntn.h"
#include "PlatformMemoryConfiguration.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)


#define FILECODE PROC_MEM_NB_TN_MNPHYTN_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define UNUSED_CLK 4


/// The structure of TxPrePN tables
typedef struct {
  UINT32 Speed;                          ///< Applied memory speed
  UINT16 TxPrePNVal[4];                  ///< Table values
} TXPREPN_STRUCT;

/// The entry of individual TxPrePN tables
typedef struct {
  UINT8 TxPrePNTblSize;                  ///< Total Table size
  CONST TXPREPN_STRUCT *TxPrePNTblPtr;   ///< Pointer to the table
} TXPREPN_ENTRY;

/// Type of an entry for processing phy init compensation for TN
typedef struct {
  BIT_FIELD_NAME IndexBitField;          ///< Bit field on which the value is decided
  BIT_FIELD_NAME StartTargetBitField;    ///< First bit field to be modified
  BIT_FIELD_NAME EndTargetBitField;      ///< Last bit field to be modified
  UINT16 ExtraValue;                     ///< Extra value needed to be written to bit field
  CONST TXPREPN_ENTRY *TxPrePN;          ///< Pointer to slew rate table
} PHY_COMP_INIT_NB;
/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

BOOLEAN
MemNRdPosTrnTN (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
extern MEM_FEAT_TRAIN_SEQ memTrainSequenceDDR3[];
/* -----------------------------------------------------------------------------*/


/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function initializes the DDR phy compensation logic
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNInitPhyCompTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  //
  // Phy Predriver Calibration Codes for Data/DQS
  //
  CONST STATIC TXPREPN_STRUCT TxPrePNDataDqsV15TN[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.5V
    {DDR667 + DDR800, {0x924, 0x924, 0x924, 0x924}},
    {DDR1066 + DDR1333, {0xFF6, 0xFF6, 0xFF6, 0xFF6}},
    {DDR1600 + DDR1866 + DDR2133, {0xFF6, 0xFF6, 0xFF6, 0xFF6}}
  };
  CONST STATIC TXPREPN_STRUCT TxPrePNDataDqsV135TN[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.35V
    {DDR667 + DDR800, {0xFF6, 0xB6D, 0xB6D, 0x924}},
    {DDR1066 + DDR1333, {0xFF6, 0xFF6, 0xFF6, 0xFF6}},
    {DDR1600 + DDR1866 + DDR2133, {0xFF6, 0xFF6, 0xFF6, 0xFF6}}
  };
  CONST STATIC TXPREPN_STRUCT TxPrePNDataDqsV125TN[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.25V
    {DDR667 + DDR800, {0xFF6, 0xDAD, 0xDAD, 0x924}},
    {DDR1066 + DDR1333, {0xFF6, 0xFF6, 0xFF6, 0xFF6}},
    {DDR1600 + DDR1866 + DDR2133, {0xFF6, 0xFF6, 0xFF6, 0xFF6}}
  };
  CONST STATIC TXPREPN_ENTRY TxPrePNDataDqsTN[] = {
    {GET_SIZE_OF (TxPrePNDataDqsV15TN), (TXPREPN_STRUCT *)&TxPrePNDataDqsV15TN},
    {GET_SIZE_OF (TxPrePNDataDqsV135TN), (TXPREPN_STRUCT *)&TxPrePNDataDqsV135TN},
    {GET_SIZE_OF (TxPrePNDataDqsV125TN), (TXPREPN_STRUCT *)&TxPrePNDataDqsV125TN}
  };

  //
  // Phy Predriver Calibration Codes for Cmd/Addr
  //
  CONST STATIC TXPREPN_STRUCT TxPrePNCmdAddrV15TN[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.5V
    {DDR667 + DDR800, {0x492, 0x492, 0x492, 0x492}},
    {DDR1066 + DDR1333, {0x6DB, 0x6DB, 0x6DB, 0x6DB}},
    {DDR1600 + DDR1866 + DDR2133, {0xB6D, 0xB6D, 0xB6D, 0xB6D}}
  };
  CONST STATIC TXPREPN_STRUCT TxPrePNCmdAddrV135TN[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.35V
    {DDR667 + DDR800, {0x492, 0x492, 0x492, 0x492}},
    {DDR1066 + DDR1333, {0x924, 0x6DB, 0x6DB, 0x6DB}},
    {DDR1600 + DDR1866 + DDR2133, {0xB6D, 0xB6D, 0x924, 0x924}}
  };
  CONST STATIC TXPREPN_STRUCT TxPrePNCmdAddrV125TN[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.25V
    {DDR667 + DDR800, {0x492, 0x492, 0x492, 0x492}},
    {DDR1066 + DDR1333, {0xDAD, 0x924, 0x6DB, 0x492}},
    {DDR1600 + DDR1866 + DDR2133, {0xFF6, 0xDAD, 0xB64, 0xB64}}
  };
  CONST STATIC TXPREPN_ENTRY TxPrePNCmdAddrTN[] = {
    {GET_SIZE_OF (TxPrePNCmdAddrV15TN), (TXPREPN_STRUCT *)&TxPrePNCmdAddrV15TN},
    {GET_SIZE_OF (TxPrePNCmdAddrV135TN), (TXPREPN_STRUCT *)&TxPrePNCmdAddrV135TN},
    {GET_SIZE_OF (TxPrePNCmdAddrV125TN), (TXPREPN_STRUCT *)&TxPrePNCmdAddrV125TN}
  };

  //
  // Phy Predriver Calibration Codes for Clock
  //
  CONST STATIC TXPREPN_STRUCT TxPrePNClockV15TN[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.5V
    {DDR667 + DDR800, {0x924, 0x924, 0x924, 0x924}},
    {DDR1066 + DDR1333, {0xFF6, 0xFF6, 0xFF6, 0xB6D}},
    {DDR1600 + DDR1866 + DDR2133, {0xFF6, 0xFF6, 0xFF6, 0xFF6}}
  };
  CONST STATIC TXPREPN_STRUCT TxPrePNClockV135TN[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.35V
    {DDR667 + DDR800, {0xDAD, 0xDAD, 0x924, 0x924}},
    {DDR1066 + DDR1333, {0xFF6, 0xFF6, 0xFF6, 0xDAD}},
    {DDR1600 + DDR1866 + DDR2133, {0xFF6, 0xFF6, 0xFF6, 0xDAD}}
  };
  CONST STATIC TXPREPN_STRUCT TxPrePNClockV125TN[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.25V
    {DDR667 + DDR800, {0xDAD, 0xDAD, 0x924, 0x924}},
    {DDR1066 + DDR1333, {0xFF6, 0xFF6, 0xFF6, 0xFF6}},
    {DDR1600 + DDR1866 + DDR2133, {0xFF6, 0xFF6, 0xFF6, 0xFF6}}
  };
  CONST STATIC TXPREPN_ENTRY TxPrePNClockTN[] = {
    {GET_SIZE_OF (TxPrePNClockV15TN), (TXPREPN_STRUCT *)&TxPrePNClockV15TN},
    {GET_SIZE_OF (TxPrePNClockV135TN), (TXPREPN_STRUCT *)&TxPrePNClockV135TN},
    {GET_SIZE_OF (TxPrePNClockV125TN), (TXPREPN_STRUCT *)&TxPrePNClockV125TN}
  };

  //
  // Tables to describe the relationship between drive strength bit fields, PreDriver Calibration bit fields and also
  // the extra value that needs to be written to specific PreDriver bit fields
  //
  CONST PHY_COMP_INIT_NB PhyCompInitBitFieldTN[] = {
    // 3. Program TxPreP/TxPreN for Data and DQS according toTable 25 if VDDIO is 1.5V or Table 26 if 1.35V.
    //    A. Program D18F2x9C_x0D0F_0[F,7:0]0[A,6]_dct[1:0]={0000b, TxPreP, TxPreN}.
    //    B. Program D18F2x9C_x0D0F_0[F,7:0]02_dct[1:0]={0000b, TxPreP, TxPreN}.
    {BFDqsDrvStren, BFDataByteTxPreDriverCal2Pad1, BFDataByteTxPreDriverCal2Pad1, 0, TxPrePNDataDqsTN},
    {BFDataDrvStren, BFDataByteTxPreDriverCal2Pad2, BFDataByteTxPreDriverCal2Pad2, 0, TxPrePNDataDqsTN},
    {BFDataDrvStren, BFDataByteTxPreDriverCal, BFDataByteTxPreDriverCal, 8, TxPrePNDataDqsTN},
    // 4. Program TxPreP/TxPreN for Cmd/Addr according to Table 28 if VDDIO is 1.5V or Table 29 if 1.35V.
    //    A. Program D18F2x9C_x0D0F_[C,8][1:0][12,0E,0A,06]_dct[1:0]={0000b, TxPreP, TxPreN}.
    //    B. Program D18F2x9C_x0D0F_[C,8][1:0]02_dct[1:0]={1000b, TxPreP, TxPreN}.
    {BFCsOdtDrvStren, BFCmdAddr0TxPreDriverCal2Pad1, BFCmdAddr0TxPreDriverCal2Pad2, 0, TxPrePNCmdAddrTN},
    {BFAddrCmdDrvStren, BFCmdAddr1TxPreDriverCal2Pad1, BFAddrTxPreDriverCal2Pad4, 0, TxPrePNCmdAddrTN},
    {BFCsOdtDrvStren, BFCmdAddr0TxPreDriverCalPad0, BFCmdAddr0TxPreDriverCalPad0, 8, TxPrePNCmdAddrTN},
    {BFCkeDrvStren, BFAddrTxPreDriverCalPad0, BFAddrTxPreDriverCalPad0, 8, TxPrePNCmdAddrTN},
    {BFAddrCmdDrvStren, BFCmdAddr1TxPreDriverCalPad0, BFCmdAddr1TxPreDriverCalPad0, 8, TxPrePNCmdAddrTN},
    // 5. Program TxPreP/TxPreN for Clock according to Table 31 if VDDIO is 1.5V or Table 32 if 1.35V.
    //    A. Program D18F2x9C_x0D0F_2[2:0]02_dct[1:0]={1000b, TxPreP, TxPreN}.
    {BFClkDrvStren, BFClock0TxPreDriverCalPad0, BFClock2TxPreDriverCalPad0, 8, TxPrePNClockTN}
  };

  BIT_FIELD_NAME CurrentBitField;
  UINT32 SpeedMask;
  UINT8 SizeOfTable;
  UINT8 Voltage;
  UINT8 i;
  UINT8 j;
  UINT8 k;
  UINT8 Dct;
  CONST TXPREPN_STRUCT *TblPtr;

  Dct = NBPtr->Dct;
  NBPtr->SwitchDCT (NBPtr, 0);
  // 1. Program D18F2x[1,0]9C_x0000_0008[DisAutoComp, DisablePreDriverCal] = {1b, 1b}.
  MemNSetBitFieldNb (NBPtr, BFDisablePredriverCal, 3);
  NBPtr->SwitchDCT (NBPtr, Dct);

  SpeedMask = (UINT32) 1 << (NBPtr->DCTPtr->Timings.Speed / 66);
  Voltage = (UINT8) CONVERT_VDDIO_TO_ENCODED (NBPtr->RefPtr->DDR3Voltage);

  for (j = 0; j < GET_SIZE_OF (PhyCompInitBitFieldTN); j ++) {
    i = (UINT8) MemNGetBitFieldNb (NBPtr, PhyCompInitBitFieldTN[j].IndexBitField);
    TblPtr = (PhyCompInitBitFieldTN[j].TxPrePN[Voltage]).TxPrePNTblPtr;
    SizeOfTable = (PhyCompInitBitFieldTN[j].TxPrePN[Voltage]).TxPrePNTblSize;
    for (k = 0; k < SizeOfTable; k++, TblPtr++) {
      if ((TblPtr->Speed & SpeedMask) != 0) {
        for (CurrentBitField = PhyCompInitBitFieldTN[j].StartTargetBitField; CurrentBitField <= PhyCompInitBitFieldTN[j].EndTargetBitField; CurrentBitField ++) {
          MemNSetBitFieldNb (NBPtr, CurrentBitField, ((PhyCompInitBitFieldTN[j].ExtraValue << 12) | TblPtr->TxPrePNVal[i]));
        }
        break;
      }
    }
    // Asserting if no table is found corresponding to current memory speed.
    ASSERT (k < SizeOfTable);
  }
  NBPtr->SwitchDCT (NBPtr, 0);
  // 6. Program D18F2x9C_x0000_0008_dct[1:0]_mp[1:0][DisAutoComp] = 0.
  MemNSetBitFieldNb (NBPtr, BFDisablePredriverCal, 1);
  NBPtr->SwitchDCT (NBPtr, Dct);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This is a general purpose function that executes before DRAM training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNBeforeDQSTrainingTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;

  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_TN; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      MemNSetBitFieldNb (NBPtr, BFTrNibbleSel, 0);
      //
      // 2.10.6.9.2 - BIOS should program D18F2x210_dct[1:0]_nbp[3:0][MaxRdLatency] to 55h.
      //
      MemNSetBitFieldNb (NBPtr, BFMaxLatency, 0x55);
      NBPtr->CsPerDelay = MemNCSPerDelayNb (NBPtr);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This is a function that executes after DRAM training for TN
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNAfterDQSTrainingTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  BOOLEAN DllShutDownEn;
  UINT8 Dimm;
  UINT8 Byte;
  UINT16 Dly;

  DllShutDownEn = TRUE;
  IDS_OPTION_HOOK (IDS_DLL_SHUT_DOWN, &DllShutDownEn, &(NBPtr->MemPtr->StdHeader));

  MemNBrdcstSetNb (NBPtr, BFMemPhyPllPdMode, 2);
  MemNBrdcstSetNb (NBPtr, BFPllLockTime, 0x190);

  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_TN; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      //
      // 2.10.6.7 DCT Training Specific Configuration
      //
      MemNSetBitFieldNb (NBPtr, BFAddrCmdTriEn, 1);
      MemNSetBitFieldNb (NBPtr, BFDisAutoRefresh, 0);
      if (DllShutDownEn && NBPtr->IsSupported[SetDllShutDown]) {
        MemNSetBitFieldNb (NBPtr, BFDisDllShutdownSR, 0);
      }
      MemNSetBitFieldNb (NBPtr , BFForceAutoPchg, 0);
      MemNSetBitFieldNb (NBPtr , BFDynPageCloseEn, 0);
      if (NBPtr->RefPtr->EnableBankSwizzle) {
        MemNSetBitFieldNb (NBPtr, BFBankSwizzleMode, 1);
      }
      MemNSetBitFieldNb (NBPtr, BFDcqBypassMax, 0x01F);
      MemNPowerDownCtlTN (NBPtr);
      MemNSetBitFieldNb (NBPtr, BFZqcsInterval, 2);
      MemNSetBitFieldNb (NBPtr, BFBankSwap, 1);
      //
      // Post Training values for BFRxMaxDurDllNoLock, BFTxMaxDurDllNoLock,
      //  and BFEnRxPadStandby are handled by Power savings code
      //
      // BFBwCapEn and BFODTSEn are handled by OnDimmThermal Code
      //
      // BFDctSelIntLvEn is programmed by Interleaving feature
      //
    }
  }
  //
  //
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_TN; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      if (!(NBPtr->DctCachePtr->excel846 )) {
        for (Dimm = 0; Dimm < 4; Dimm++) {
          for (Byte = 0; Byte < 9; Byte++) {
            Dly = (UINT16) MemNGetTrainDlyNb (NBPtr, AccessRdDqsDly, DIMM_BYTE_ACCESS (Dimm, Byte));
            MemNSetTrainDlyNb (NBPtr, excel845 , DIMM_NBBL_ACCESS (Dimm, Byte * 2), Dly);
            MemNSetTrainDlyNb (NBPtr, excel845 , DIMM_NBBL_ACCESS (Dimm, (Byte * 2) + 1), Dly);
          }
        }
      }
    }
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function overrides the seed for hardware based RcvEn training of TN.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *SeedPtr - Pointer to the seed value.
 *
 *     @return    TRUE
 */

BOOLEAN
MemNOverrideRcvEnSeedTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *SeedPtr
  )
{
  *(UINT16 *)SeedPtr = 0x32 - (0x20 * (UINT16) MemNGetBitFieldNb (NBPtr, BFWrDqDqsEarly));

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function choose the correct PllLockTime for TN
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *PllLockTime -  Bit offset of the field to be programmed
 *
 *     @return    TRUE
 */
BOOLEAN
MemNAdjustPllLockTimeTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *PllLockTime
  )
{
  if (MemNGetBitFieldNb (NBPtr, BFMemPhyPllPdMode) == 2) {
    *(UINT16*) PllLockTime = 0x190;
  }

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function overrides the seed for hardware based WL for TN.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *SeedPtr - Pointer to the seed value.
 *
 *     @return    TRUE
 */

BOOLEAN
MemNOverrideWLSeedTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *SeedPtr
  )
{
  if (NBPtr->ChannelPtr->SODimmPresent != 0) {
    *(UINT8*) SeedPtr = 0xE;
  } else {
    // Unbuffered dimm
    *(UINT8*) SeedPtr = 0x15;
  }

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function adjusts Avg PRE value of Phy fence training for TN.
 *
 *     @param[in,out]   *NBPtr  - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *Value16 - Pointer to the value that we want to adjust
 *
 */
VOID
MemNPFenceAdjustTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   INT16 *Value16
  )
{
  if (*Value16 < 0) {
    *Value16 = 0;
  }

  // This makes sure the phy fence value will be written to M1 context as well.
  MULTI_MPSTATE_COPY_TSEFO (NBPtr->NBRegTable, BFPhyFence);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function programs Fence2RxDll for TN.
 *
 *     @param[in,out]   *NBPtr  - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *Fence2Data - Pointer to the value of fence2 data
 *
 */
BOOLEAN
MemNProgramFence2RxDllTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Fence2Data
  )
{
  UINT8 Dct;
  UINT16 Fence2RxDllTxPad;
  UINT16 Fence2Value;
  UINT16 Fence1;
  BIT_FIELD_NAME BitField;

  Fence2Value = (UINT16) MemNGetBitFieldNb (NBPtr, BFFence2);
  Fence2RxDllTxPad = (*(UINT16*) Fence2Data & 0x1F) | (((*(UINT16*) Fence2Data >> 5) & 0x1F) << 10);

  Fence2Value &= ~(UINT16) ((0x1F << 10) | 0x1F);
  Fence2Value |= Fence2RxDllTxPad;
  MemNSetBitFieldNb (NBPtr, BFFence2, Fence2Value);

  if (NBPtr->MemPstateStage == MEMORY_PSTATE_1ST_STAGE) {
    MAKE_TSEFO (NBPtr->NBRegTable, DCT_PHY_ACCESS, 0x0C, 30, 16, BFPhyFence);
    BitField = (NBPtr->Dct == 0) ? BFChAM1FenceSave : BFChBM1FenceSave;

    Fence1 = (UINT16) MemNGetBitFieldNb (NBPtr, BFPhyFence);
    Dct = NBPtr->Dct;
    MemNSwitchDCTNb (NBPtr, 1);
    MemNSetBitFieldNb (NBPtr, BitField, Fence1);
    MemNSwitchDCTNb (NBPtr, Dct);
  }

  return TRUE;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function checks if RdDqsDly needs to be restarted for Trinity
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *Center - Center of the data eye
 *
 *     @return    TRUE
 */

BOOLEAN
MemNRdDqsDlyRestartChkTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Center
  )
{
  INT8 EyeCenter;
  UINT8 ByteLane;
  BOOLEAN RetVal;
  MEM_TECH_BLOCK *TechPtr;
  CH_DEF_STRUCT *ChanPtr;

  TechPtr = NBPtr->TechPtr;
  ChanPtr = NBPtr->ChannelPtr;
  ByteLane = NBPtr->TechPtr->Bytelane;
  RetVal = TRUE;

  // If the average value of passing read DQS delay for the lane is negative, then adjust the input receiver
  // DQ delay in D18F2x9C_x0D0F_0[F,7:0][5F,1F]_dct[1:0] for the lane as follows:

  EyeCenter = ((INT8) ChanPtr->RdDqsMinDlys[ByteLane] + (INT8) ChanPtr->RdDqsMaxDlys[ByteLane] + 1) / 2;

  if ((EyeCenter < 0) && (NBPtr->RdDqsDlyRetrnStat != RDDQSDLY_RTN_SUSPEND)) {
    IDS_HDT_CONSOLE (MEM_FLOW, "        Negative data eye center.\n");

    if (MemNGetBitFieldNb (NBPtr, BFRxBypass3rd4thStg) == 4) {
      // IF (RxBypass3rd4thStg == 1) program RxBypass3rd4thStg=0 and repeat step 3 above for all
      // ranks and lanes
      IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tRxByPass3rd4thStg is 1, clear it and restart RdDqsDly training on current Dct.\n");

      MemNSetBitFieldNb (NBPtr, BFRxBypass3rd4thStg, 0);
      NBPtr->RdDqsDlyRetrnStat = RDDQSDLY_RTN_ONGOING;

      // When Retrain condition is first detected, record the current chipsel at which the retrain starts
      // so we don't need to retrain RcvEnDly and WrDatDly on the chipsels that are already done with these steps.
      TechPtr->RestartChipSel = (INT8) TechPtr->ChipSel;

      RetVal = FALSE;
    } else if (MemNGetBitFieldNb (NBPtr, BFRx4thStgEn) == 0) {
      // ELSEIF (Rx4thStgEn == 0) program Rx4thStgEn=1 and repeat step 3 above for all ranks and lanes
      IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tRx4thStg is 0, set it and restart RdDqsDly training on current Dct.\n");

      MemNSetBitFieldNb (NBPtr, BFRx4thStgEn, 0x100);
      NBPtr->RdDqsDlyRetrnStat = RDDQSDLY_RTN_ONGOING;

      // If the second retrain starts beyond the chip selects that are previously trained, update the record so
      // we don't need to retrain RcvEnDly and WrDatDly
      if (TechPtr->RestartChipSel < ((INT8) TechPtr->ChipSel)) {
        TechPtr->RestartChipSel = (INT8) TechPtr->ChipSel;
      }

      RetVal = FALSE;
    } else {
      // ELSE program the read DQS delay for the lane with a value of zero
      IDS_HDT_CONSOLE (MEM_FLOW, "                                                   ");
      IDS_HDT_CONSOLE (MEM_FLOW, "Center of data eye is still negative after 2 retires. Do not restart training, just use 0\n");
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\t  ");
      *(UINT8 *) Center = 0;
    }
  }

  return RetVal;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function executes RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - All bytelanes pass
 *     @return  FALSE - Some bytelanes fail
*/
BOOLEAN
MemNRdPosTrnTN (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  BOOLEAN RetVal;

  if (((INT8) TechPtr->ChipSel) > TechPtr->RestartChipSel) {
    RetVal = MemTRdPosWithRxEnDlySeeds3 (TechPtr);
  } else {
    // Skip RcvEnDly cycle training when current chip select has already gone through that step.
    // Because a retrain condition can only be detected on a chip select after RcvEnDly cycle training
    // So when current chip select is equal to RestartChipSel, we don't need to redo RcvEnDly cycle training.
    // Only redo DQS position training.
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\tSkip RcvEnDly Cycle Training on Current Chip Select.\n\n");
    RetVal = MemTTrainDQSEdgeDetect (TechPtr);
  }
  return RetVal;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function skips WrDatDly training when a retrain condition is just detected
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *ChipSel - Pointer to ChipSel
 *
 *     @return    TRUE
 */

BOOLEAN
MemNHookBfWrDatTrnTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *ChipSel
  )
{
  BOOLEAN RetVal;

  RetVal = TRUE;
  if (NBPtr->RdDqsDlyRetrnStat == RDDQSDLY_RTN_ONGOING) {
    NBPtr->RdDqsDlyRetrnStat = RDDQSDLY_RTN_NEEDED;
    // Clear chipsel value to force a restart of Rd Dqs Training
    if (NBPtr->CsPerDelay == 1) {
      *(UINT8 *) ChipSel = 0xFF;
    } else {
      *(UINT8 *) ChipSel = 0xFE;
    }

    RetVal = FALSE;
  } else if (((INT8) NBPtr->TechPtr->ChipSel) < NBPtr->TechPtr->RestartChipSel) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tSkip WrDatDly Training on Current Chip Select.\n\n");
    // Skip WrDatDly training when current chip select has gone through WrDatDly procedure
    // A retrain is detected during RdDqsDly training, so if RestartChipSel is equal to current
    // chip select, then WrDatDly has not been started for current chip select in the previous cycle.
    // However, RcvEnDly cycle training has been done for current chip select.
    // So we don't need to do RcvEnDly cycle training when current chip select is equal to RestartChipSel
    // but we need to do WrDatDly training for current chip select.
    RetVal = FALSE;
  }

  // when return is FALSE, WrDatDly training will be skipped

  return RetVal;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function sets up output driver and write leveling mode in MR1 during WL
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *Value   - MR1 value
 *
 *     @return    TRUE
 */

BOOLEAN
MemNWLMR1TN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Value
  )
{
  BOOLEAN Target;

  // For the target rank of the target DIMM, enable write leveling mode and enable the output driver.
  // For all other ranks of the target DIMM, enable write leveling mode and disable the output driver.
  Target = (BOOLEAN) (*(UINT16 *) Value >> 7) & 1;

  if (NBPtr->CsPerDelay == 1) {
    // Clear Qoff and reset it based on TN requirement
    *(UINT16 *) Value &= ~((UINT16) 1 << 12);

    if (!Target) {
      *(UINT16 *) Value |= (((UINT16) 1 << 7) | ((UINT16) 1 << 12));
    }
  }

  return TRUE;
}
