/* $NoKeywords:$ */
/**
 * @file
 *
 * mnphyor.c
 *
 * Northbridge Phy support for Orochi
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/OR)
 * @e \$Revision: 58126 $ @e \$Date: 2011-08-21 23:38:29 -0600 (Sun, 21 Aug 2011) $
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2012 Advanced Micro Devices, Inc.
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
*
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
#include "mnor.h"
#include "cpuRegisters.h"
#include "PlatformMemoryConfiguration.h"
#include "F15PackageType.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)


#define FILECODE PROC_MEM_NB_OR_MNPHYOR_FILECODE
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

/// Type of an entry for processing phy init compensation for Orochi
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



/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
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
MemNInitPhyCompOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  //
  // Phy Predriver Calibration Codes for Data/DQS
  //
  CONST STATIC TXPREPN_STRUCT TxPrePNDataDqsV15Or[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.5V
    {DDR667 + DDR800, {0x924, 0x924, 0x924, 0x924}},
    {DDR1066 + DDR1333, {0xFF6, 0xFF6, 0xFF6, 0xFF6}},
    {DDR1600 + DDR1866, {0xFF6, 0xFF6, 0xFF6, 0xFF6}}
  };
  CONST STATIC TXPREPN_STRUCT TxPrePNDataDqsV135Or[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.35V
    {DDR667 + DDR800, {0xFF6, 0xB6D, 0xB6D, 0x924}},
    {DDR1066 + DDR1333, {0xFF6, 0xFF6, 0xFF6, 0xFF6}},
    {DDR1600 + DDR1866, {0xFF6, 0xFF6, 0xFF6, 0xFF6}}
  };
  CONST STATIC TXPREPN_STRUCT TxPrePNDataDqsV125Or[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.25V
    {DDR667 + DDR800, {0xFF6, 0xDAD, 0xDAD, 0x924}},
    {DDR1066 + DDR1333, {0xFF6, 0xFF6, 0xFF6, 0xFF6}},
    {DDR1600 + DDR1866, {0xFF6, 0xFF6, 0xFF6, 0xFF6}}
  };
  CONST STATIC TXPREPN_ENTRY TxPrePNDataDqsOr[] = {
    {GET_SIZE_OF (TxPrePNDataDqsV15Or), (TXPREPN_STRUCT *)&TxPrePNDataDqsV15Or},
    {GET_SIZE_OF (TxPrePNDataDqsV135Or), (TXPREPN_STRUCT *)&TxPrePNDataDqsV135Or},
    {GET_SIZE_OF (TxPrePNDataDqsV125Or), (TXPREPN_STRUCT *)&TxPrePNDataDqsV125Or}
  };

  //
  // Phy Predriver Calibration Codes for Data/DQS
  //
  CONST STATIC TXPREPN_STRUCT TxPrePNDataDqsV15OrB1[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.5V
    {DDR667 + DDR800, {0xB6D, 0x6DB, 0x492, 0x492}},
    {DDR1066 + DDR1333, {0xFFF, 0x924, 0x6DB, 0x6DB}},
    {DDR1600 + DDR1866, {0xFFF, 0xFFF, 0xFFF, 0xB6D}}
  };
  CONST STATIC TXPREPN_STRUCT TxPrePNDataDqsV135OrB1[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.35V
    {DDR667 + DDR800, {0xFFF, 0x924, 0x6DB, 0x492}},
    {DDR1066 + DDR1333, {0xFFF, 0xDB6, 0xB6D, 0x6DB}},
    {DDR1600 + DDR1866, {0xFFF, 0xFFF, 0xFFF, 0xDB6}}
  };
  CONST STATIC TXPREPN_STRUCT TxPrePNDataDqsV125OrB1[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.25V
    {DDR667 + DDR800, {0xFFF, 0xB6D, 0x924, 0x6DB}},
    {DDR1066 + DDR1333, {0xFFF, 0xFFF, 0xDB6, 0x924}},
    {DDR1600 + DDR1866, {0xFFF, 0xFFF, 0xFFF, 0xFFF}}
  };
  CONST STATIC TXPREPN_ENTRY TxPrePNDataDqsOrB1[] = {
    {GET_SIZE_OF (TxPrePNDataDqsV15OrB1), (TXPREPN_STRUCT *)&TxPrePNDataDqsV15OrB1},
    {GET_SIZE_OF (TxPrePNDataDqsV135OrB1), (TXPREPN_STRUCT *)&TxPrePNDataDqsV135OrB1},
    {GET_SIZE_OF (TxPrePNDataDqsV125OrB1), (TXPREPN_STRUCT *)&TxPrePNDataDqsV125OrB1}
  };

  //
  // Phy Predriver Calibration Codes for Cmd/Addr
  //
  CONST STATIC TXPREPN_STRUCT TxPrePNCmdAddrV15Or[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.5V
    {DDR667 + DDR800, {0x492, 0x492, 0x492, 0x492}},
    {DDR1066 + DDR1333, {0x6DB, 0x6DB, 0x6DB, 0x6DB}},
    {DDR1600 + DDR1866, {0xB6D, 0xB6D, 0xB6D, 0xB6D}}
  };
  CONST STATIC TXPREPN_STRUCT TxPrePNCmdAddrV135Or[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.35V
    {DDR667 + DDR800, {0x492, 0x492, 0x492, 0x492}},
    {DDR1066 + DDR1333, {0x924, 0x6DB, 0x6DB, 0x6DB}},
    {DDR1600 + DDR1866, {0xB6D, 0xB6D, 0x924, 0x924}}
  };
  CONST STATIC TXPREPN_STRUCT TxPrePNCmdAddrV125Or[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.25V
    {DDR667 + DDR800, {0x492, 0x492, 0x492, 0x492}},
    {DDR1066 + DDR1333, {0xDAD, 0x924, 0x6DB, 0x492}},
    {DDR1600 + DDR1866, {0xFF6, 0xDAD, 0xB64, 0xB64}}
  };
  CONST STATIC TXPREPN_ENTRY TxPrePNCmdAddrOr[] = {
    {GET_SIZE_OF (TxPrePNCmdAddrV15Or), (TXPREPN_STRUCT *)&TxPrePNCmdAddrV15Or},
    {GET_SIZE_OF (TxPrePNCmdAddrV135Or), (TXPREPN_STRUCT *)&TxPrePNCmdAddrV135Or},
    {GET_SIZE_OF (TxPrePNCmdAddrV125Or), (TXPREPN_STRUCT *)&TxPrePNCmdAddrV125Or}
  };

  //
  // Phy Predriver Calibration Codes for Clock
  //
  CONST STATIC TXPREPN_STRUCT TxPrePNClockV15Or[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.5V
    {DDR667 + DDR800, {0x924, 0x924, 0x924, 0x924}},
    {DDR1066 + DDR1333, {0xFF6, 0xFF6, 0xFF6, 0xB6D}},
    {DDR1600 + DDR1866, {0xFF6, 0xFF6, 0xFF6, 0xFF6}}
  };
  CONST STATIC TXPREPN_STRUCT TxPrePNClockV135Or[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.35V
    {DDR667 + DDR800, {0xDAD, 0xDAD, 0x924, 0x924}},
    {DDR1066 + DDR1333, {0xFF6, 0xFF6, 0xFF6, 0xDAD}},
    {DDR1600 + DDR1866, {0xFF6, 0xFF6, 0xFF6, 0xDAD}}
  };
  CONST STATIC TXPREPN_STRUCT TxPrePNClockV125Or[] = {
    //{TxPreP, TxPreN}[Speed][Drive Strength] at 1.25V
    {DDR667 + DDR800, {0xDAD, 0xDAD, 0x924, 0x924}},
    {DDR1066 + DDR1333, {0xFF6, 0xFF6, 0xFF6, 0xFF6}},
    {DDR1600 + DDR1866, {0xFF6, 0xFF6, 0xFF6, 0xFF6}}
  };
  CONST STATIC TXPREPN_ENTRY TxPrePNClockOr[] = {
    {GET_SIZE_OF (TxPrePNClockV15Or), (TXPREPN_STRUCT *)&TxPrePNClockV15Or},
    {GET_SIZE_OF (TxPrePNClockV135Or), (TXPREPN_STRUCT *)&TxPrePNClockV135Or},
    {GET_SIZE_OF (TxPrePNClockV125Or), (TXPREPN_STRUCT *)&TxPrePNClockV125Or}
  };

  //
  // Tables to describe the relationship between drive strength bit fields, PreDriver Calibration bit fields and also
  // the extra value that needs to be written to specific PreDriver bit fields
  //
  CONST PHY_COMP_INIT_NB PhyCompInitBitFieldOr[] = {
    // 3. Program TxPreP/TxPreN for Data and DQS according toTable 46 if VDDIO is 1.5V or Table 47 if 1.35V.
    //    A. Program D18F2x9C_x0D0F_0[F,8:0]0[A,6]_dct[1:0]={0000b, TxPreP, TxPreN}.
    //    B. Program D18F2x9C_x0D0F_0[F,8:0]0[A,6]_dct[1:0]={0000b, TxPreP, TxPreN}.
    {BFDqsDrvStren, BFDataByteTxPreDriverCal2Pad1, BFDataByteTxPreDriverCal2Pad1, 0, TxPrePNDataDqsOr},
    {BFDataDrvStren, BFDataByteTxPreDriverCal2Pad2, BFDataByteTxPreDriverCal2Pad2, 0, TxPrePNDataDqsOr},
    {BFDataDrvStren, BFDataByteTxPreDriverCal, BFDataByteTxPreDriverCal, 8, TxPrePNDataDqsOr},
    // 4. Program TxPreP/TxPreN for Cmd/Addr according to Table 49 if VDDIO is 1.5V or Table 50 if 1.35V.
    //    A. Program D18F2x9C_x0D0F_[C,8][1:0][12,0E,0A,06]_dct[1:0]={0000b, TxPreP, TxPreN}.
    //    B. Program D18F2x9C_x0D0F_[C,8][1:0]02_dct[1:0]={1000b, TxPreP, TxPreN}.
    {BFCsOdtDrvStren, BFCmdAddr0TxPreDriverCal2Pad1, BFCmdAddr0TxPreDriverCal2Pad2, 0, TxPrePNCmdAddrOr},
    {BFAddrCmdDrvStren, BFCmdAddr1TxPreDriverCal2Pad1, BFAddrTxPreDriverCal2Pad4, 0, TxPrePNCmdAddrOr},
    {BFCsOdtDrvStren, BFCmdAddr0TxPreDriverCalPad0, BFCmdAddr0TxPreDriverCalPad0, 8, TxPrePNCmdAddrOr},
    {BFCkeDrvStren, BFAddrTxPreDriverCalPad0, BFAddrTxPreDriverCalPad0, 8, TxPrePNCmdAddrOr},
    {BFAddrCmdDrvStren, BFCmdAddr1TxPreDriverCalPad0, BFCmdAddr1TxPreDriverCalPad0, 8, TxPrePNCmdAddrOr},
    // 5. Program TxPreP/TxPreN for Clock according to Table 52 if VDDIO is 1.5V or Table 53 if 1.35V.
    //    A. Program D18F2x9C_x0D0F_2[2:0]02_dct[1:0]={1000b, TxPreP, TxPreN}.
    {BFClkDrvStren, BFClock0TxPreDriverCalPad0, BFClock2TxPreDriverCalPad0, 8, TxPrePNClockOr}
  };

  BIT_FIELD_NAME CurrentBitField;
  UINT16 SpeedMask;
  UINT8 SizeOfTable;
  UINT8 Voltage;
  UINT8 i;
  UINT8 j;
  UINT8 k;
  UINT8 Dct;
  CONST TXPREPN_STRUCT *TblPtr;

  Dct = NBPtr->Dct;
  NBPtr->SwitchDCT (NBPtr, 0);
  // 1. Program D18F2x[1,0]9C_x0D0F_E003[DisAutoComp, DisablePreDriverCal] = {1b, 1b}.
  MemNSetBitFieldNb (NBPtr, BFDisablePredriverCal, 0x6000);
  NBPtr->SwitchDCT (NBPtr, Dct);

  SpeedMask = (UINT16) 1 << (NBPtr->DCTPtr->Timings.Speed / 66);
  Voltage = (UINT8) CONVERT_VDDIO_TO_ENCODED (NBPtr->RefPtr->DDR3Voltage);

  for (j = 0; j < GET_SIZE_OF (PhyCompInitBitFieldOr); j ++) {
    i = (UINT8) MemNGetBitFieldNb (NBPtr, PhyCompInitBitFieldOr[j].IndexBitField);
    TblPtr = (PhyCompInitBitFieldOr[j].TxPrePN[Voltage]).TxPrePNTblPtr;
    SizeOfTable = (PhyCompInitBitFieldOr[j].TxPrePN[Voltage]).TxPrePNTblSize;

    // Uses different TxPrePNDataDqsOr table for OR B1 and later
    if (((NBPtr->MCTPtr->LogicalCpuid.Revision & AMD_F15_OR_LT_B1) == 0) &&
        (PhyCompInitBitFieldOr[j].TxPrePN == TxPrePNDataDqsOr)) {
      ASSERT (Voltage < sizeof (TxPrePNDataDqsOrB1) / sizeof (TxPrePNDataDqsOrB1[0]));
      TblPtr = TxPrePNDataDqsOrB1[Voltage].TxPrePNTblPtr;
      SizeOfTable = TxPrePNDataDqsOrB1[Voltage].TxPrePNTblSize;
    }

    for (k = 0; k < SizeOfTable; k++, TblPtr++) {
      if ((TblPtr->Speed & SpeedMask) != 0) {
        for (CurrentBitField = PhyCompInitBitFieldOr[j].StartTargetBitField; CurrentBitField <= PhyCompInitBitFieldOr[j].EndTargetBitField; CurrentBitField ++) {
          MemNSetBitFieldNb (NBPtr, CurrentBitField, ((PhyCompInitBitFieldOr[j].ExtraValue << 12) | TblPtr->TxPrePNVal[i]));
        }
        break;
      }
    }
    // Asserting if no table is found corresponding to current memory speed.
    ASSERT (k < SizeOfTable);
  }
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
MemNBeforeDQSTrainingOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8   Dct;
  UINT32  PackageType;

  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_OR; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      //
      // 2.10.6.8.2 - BIOS should program D18F2x210_dct[1:0]_nbp[3:0][MaxRdLatency] to 55h.
      //
      MemNSetBitFieldNb (NBPtr, BFMaxLatency, 0x55);
    }
    MemNSetBitFieldNb (NBPtr, BFTraceModeEn, 0);
  }

  // DisDatMsk: Reset: 0. BIOS: IF (G34r1 || C32r1) THEN 1 ELSE 0 ENDIF.
  PackageType = LibAmdGetPackageType (&(NBPtr->MemPtr->StdHeader));
  if (PackageType != PACKAGE_TYPE_AM3r2) {
    MemNSetBitFieldNb (NBPtr, BFDisDatMsk, 1);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This is a function that executes after DRAM training for Orochi
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNAfterDQSTrainingOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  UINT8 Dimm;
  UINT8 Byte;
  UINT16 Dly;
  BOOLEAN DllShutDownEn;

  DllShutDownEn = TRUE;
  IDS_OPTION_HOOK (IDS_DLL_SHUT_DOWN, &DllShutDownEn, &(NBPtr->MemPtr->StdHeader));

  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_OR; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      //
      // 2.10.6.6 DCT Training Specific Configuration
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
      MemNSetBitFieldNb (NBPtr, BFDcqBypassMax, 0x0F);
      MemNPowerDownCtlOr (NBPtr);
      MemNSetBitFieldNb (NBPtr, BFDisSimulRdWr, 0);
      MemNSetBitFieldNb (NBPtr, BFZqcsInterval, 2);
      //
      // Post Training values for BFRxMaxDurDllNoLock, BFTxMaxDurDllNoLock,
      //  and BFEnRxPadStandby are handled by Power savings code
      //
      // BFBwCapEn and BFODTSEn are handled by OnDimmThermal Code
      //
      // BFDctSelIntLvEn is programmed by Interleaving feature
      //
      // BFL3Scrub, BFDramScrub, and DramScrubReDirEn are programmed by ECC Feature code
      //
      //
      MemNSetBitFieldNb (NBPtr, BFL3ScrbRedirDis, 0);
      // Doing DataTxFifoWrDly override for NB PState 0
      MemNDataTxFifoWrDlyOverrideOr (NBPtr, NBPtr);
    }
  }

  //
  // Synch RdDqs__Dly to RdDqsDly for S3 Save/Restore
  //
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_OR; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      if (!(NBPtr->DctCachePtr->Is__x4)) {
        // Only synch when 1D training has been performed or training with x8 DIMMs
        for (Dimm = 0; Dimm < 4; Dimm++) {
          for (Byte = 0; Byte < 9; Byte++) {
            Dly = (UINT16) MemNGetTrainDlyNb (NBPtr, AccessRdDqsDly, DIMM_BYTE_ACCESS (Dimm, Byte));
            MemNSetTrainDlyNb (NBPtr, AccessRdDqs__Dly, DIMM_NBBL_ACCESS (Dimm, Byte * 2), Dly);
            MemNSetTrainDlyNb (NBPtr, AccessRdDqs__Dly, DIMM_NBBL_ACCESS (Dimm, (Byte * 2) + 1), Dly);
            NBPtr->ChannelPtr->RdDqs__Dlys[(Dimm * MAX_NUMBER_LANES) + (Byte * 2)] = (UINT8) Dly;
            NBPtr->ChannelPtr->RdDqs__Dlys[(Dimm * MAX_NUMBER_LANES) + (Byte * 2) + 1] = (UINT8) Dly;
          }
        }
      }
    }
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function overrides the seed for hardware based RcvEn training of Orochi.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *SeedPtr - Pointer to the seed value.
 *
 *     @return    TRUE
 */

BOOLEAN
MemNOverrideRcvEnSeedOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *SeedPtr
  )
{
  UINT16 *SeedPointer;
  SeedPointer = (UINT16*) SeedPtr;

  //
  // Get seed value saved in PS block
  //
  *SeedPointer = NBPtr->PsPtr->HWRxENSeedVal;
  *SeedPointer -= (0x20 * (UINT16) MemNGetBitFieldNb (NBPtr, BFWrDqDqsEarly));

  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function overrides the seed for Pass N hardware based RcvEn training of Orochi.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *SeedTotal - Pointer to the SeedTotal
 *
 *     @return    TRUE
 */

BOOLEAN
MemNOverrideRcvEnSeedPassNOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *SeedTotal
  )
{
  UINT16 RegisterDelay;
  UINT16 SeedTotalPreScaling;
  UINT8 *SpdBufferPtr;
  if (NBPtr->MCTPtr->Status[SbLrdimms]) {
    // LRDIMMs
    NBPtr->TechPtr->GetDimmSpdBuffer (NBPtr->TechPtr, &SpdBufferPtr, (NBPtr->TechPtr->ChipSel >> 1));
    RegisterDelay = 0x10 + (((SpdBufferPtr[67] & 1) == 0) ? (0x30 - (SpdBufferPtr[70] & 7)): 0x30);
  } else if (NBPtr->MCTPtr->Status[SbRegistered]) {
    // Registered
    RegisterDelay = ((NBPtr->ChannelPtr->CtrlWrd02[(NBPtr->TechPtr->ChipSel >> 1)] & BIT0) == 0) ? 0x20: 0x30;
  } else {
    // UDIMMs
    RegisterDelay = 0;
  }
  if (NBPtr->TechPtr->PrevPassRcvEnDly[NBPtr->TechPtr->Bytelane] < (0x20 + RegisterDelay)) {
    SeedTotalPreScaling = 0x20 + RegisterDelay;
  } else {
    SeedTotalPreScaling = NBPtr->TechPtr->PrevPassRcvEnDly[NBPtr->TechPtr->Bytelane] - 0x20 - RegisterDelay;
  }
  *(UINT16*) SeedTotal = ((UINT16) (((UINT32) SeedTotalPreScaling * NBPtr->DCTPtr->Timings.Speed) / NBPtr->TechPtr->PrevSpeed)) + RegisterDelay;
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function overrides the seed for write leveing training of Orochi.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *SeedPtr - Pointer to the seed value.
 *
 *     @return    TRUE
 */

BOOLEAN
MemNOverrideWLSeedOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *SeedPtr
  )
{
  DIE_STRUCT *MCTPtr;
  CH_DEF_STRUCT *ChannelPtr;
  UINT8 RCW2;

  MCTPtr = NBPtr->MCTPtr;
  ChannelPtr = NBPtr->ChannelPtr;
  RCW2 = ChannelPtr->CtrlWrd02[NBPtr->TechPtr->TargetDIMM];

  //
  // Get the default value of seed
  //
  if (ChannelPtr->SODimmPresent != 0) {
    //
    // SODIMMM
    //
    *(UINT8*) SeedPtr = 0x12;
  } else {
    //
    // Get seed value saved in PS block
    //
    *(UINT8*) SeedPtr = NBPtr->PsPtr->WLSeedVal;

    if (MCTPtr->Status[SbRegistered]) {
      *(UINT8*) SeedPtr += ((RCW2 & BIT0) == 0) ? 0 : 0x10;
    }
  }

  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function enables nibble based training for Write Levelization for Orochi.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *Dimm - Pointer to DIMM to be trained
 *
 *     @return    TRUE
 */

BOOLEAN
MemNTrainWlPerNibbleOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Dimm
  )
{
  UINT8 ByteLane;
  if ((NBPtr->ChannelPtr->Dimmx4Present & (1 << *(UINT16*) Dimm)) != 0) {
    if (NBPtr->TechPtr->TrnNibble <= NIBBLE_1) {
      //For x4 DIMMs, BIOS trains both nibbles of a byte lane by programming
      //D18F2x9C_x0000_0008_dct[1:0][TrNibbleSel] to specify the nibble. BIOS repeats steps 3 through
      //5 and uses the average of the trained values for the delay setting.
      if (NBPtr->TechPtr->TrnNibble == NIBBLE_1) {
        NBPtr->SetBitField (NBPtr, BFTrNibbleSel, NBPtr->TechPtr->TrnNibble);
      }
      return FALSE;
    } else {
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tWrDqs: ");
      for (ByteLane = 0; ByteLane < (NBPtr->MCTPtr->Status[SbEccDimms] ? 9 : 8) ; ByteLane++) {
        IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", NBPtr->TechPtr->WlNibbleDly[ByteLane]);
      }
      IDS_HDT_CONSOLE (MEM_FLOW, "   <<< Nibble AVG\n\n");
      return FALSE;
    }
  } else  {
    return TRUE;
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function adjusts the WL DQS Delay based on nibble traning results for Orochi.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *Delay - Pointer to Wr Dqs Delay
 *
 *     @return    FALSE - Supported
 *     @return    TRUE - Not supported
 */
BOOLEAN
MemNTrainWlPerNibbleAdjustWLDlyOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Delay
  )
{
  MEM_TECH_BLOCK *TechPtr;
  UINT8 Bytelane;
  TechPtr = NBPtr->TechPtr;
  Bytelane = TechPtr->Bytelane;
  if ((NBPtr->ChannelPtr->DimmNibbleAccess & (1 << TechPtr->TargetDIMM)) != 0) {
    if (TechPtr->TrnNibble == NIBBLE_1) {
      *(UINT8*) Delay = (TechPtr->WlNibbleDly[Bytelane] + *(UINT8*) Delay + 1) / 2;
      if (Bytelane == (NBPtr->MCTPtr->Status[SbEccDimms] ? 8 : 7)) {
        IDS_HDT_CONSOLE (MEM_FLOW, "   <<< Nibble 1");
      }
    } else {
      if (Bytelane == (NBPtr->MCTPtr->Status[SbEccDimms] ? 8 : 7)) {
        IDS_HDT_CONSOLE (MEM_FLOW, "   <<< Nibble 0");
      }
    }
    TechPtr->WlNibbleDly[Bytelane] = *(UINT8*) Delay;
    return FALSE;
  } else {
    return TRUE;
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function sets the correct seed for Nibble based Write Levelization.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *WrDqsDly   - Pointer to WrDqs value
 *
 *     @return    FALSE - Supported
 *     @return    TRUE - Not supported
 */

BOOLEAN
MemNTrainWlPerNibbleSeedOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *WrDqsDly
  )
{
  if (NBPtr->TechPtr->TrnNibble == NIBBLE_0) {
    NBPtr->TechPtr->WlNibble0Seed[NBPtr->TechPtr->Bytelane] = *(UINT16*) WrDqsDly;
  } else {
    *(UINT16*) WrDqsDly = NBPtr->TechPtr->WlNibble0Seed[NBPtr->TechPtr->Bytelane];
  }
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function initializes nibble based Receiver Enable Training for Orochi.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *OptParam - Optional paramater
 *
 *     @return    FALSE - Supported
 *     @return    TRUE - Not supported
 */
BOOLEAN
MemNInitPerNibbleTrnOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  // Program D18F2x9C_x0000_0008_dct[1:0][TrNibbleSel]=0
  NBPtr->TechPtr->TrnNibble = NIBBLE_0;
  NBPtr->SetBitField (NBPtr, BFTrNibbleSel, NIBBLE_0);
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function enables nibble based Receiver Enable Training for Orochi.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *ChipSel - Pointer to ChipSel to be trained
 *
 *     @return    FALSE - Supported
 *     @return    TRUE - Not supported
 */

BOOLEAN
MemNTrainRxEnPerNibbleOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *ChipSel
  )
{
  if ((NBPtr->ChannelPtr->DimmNibbleAccess & (1 << (*(UINT16*) ChipSel >> 1))) != 0) {
    if (NBPtr->TechPtr->TrnNibble == NIBBLE_1) {
      // For x4 DIMMs, BIOS trains both nibbles of a byte lane by programming
      // D18F2x9C_x0000_0008_dct[1:0][TrNibbleSel] to specify the nibble. BIOS repeats steps 2 through
      // 7 and uses the average of the trained values for the delay setting.
      NBPtr->SetBitField (NBPtr, BFTrNibbleSel, NBPtr->TechPtr->TrnNibble);
    }
    return FALSE;
  } else {
    return TRUE;
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function adjusts the RxEn Delay based on nibble traning results for Orochi.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *RcvEnDly - Pointer to RcvEn Dqs Delay
 *
 *     @return    FALSE - Supported
 *     @return    TRUE - Not supported
 */
BOOLEAN
MemNTrainRxEnAdjustDlyPerNibbleOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *RcvEnDly
  )
{
  MEM_TECH_BLOCK *TechPtr;
  UINT8 Bytelane;
  TechPtr = NBPtr->TechPtr;
  Bytelane = TechPtr->Bytelane;
  if ((NBPtr->ChannelPtr->DimmNibbleAccess & (1 << (TechPtr->ChipSel >> 1))) != 0) {
    if (TechPtr->TrnNibble == NIBBLE_1) {
      *(UINT16*) RcvEnDly = (TechPtr->RxEnNibbleDly[Bytelane] + *(UINT16*) RcvEnDly + 1) / 2;
      if (Bytelane == (NBPtr->MCTPtr->Status[SbEccDimms] ? 8 : 7)) {
        IDS_HDT_CONSOLE (MEM_FLOW, "   <<< Nibble 1");
      }
      TechPtr->RxEnNibbleDly[Bytelane] = *(UINT16*) RcvEnDly;
      return TRUE;
    } else {
      if (Bytelane == (NBPtr->MCTPtr->Status[SbEccDimms] ? 8 : 7)) {
        IDS_HDT_CONSOLE (MEM_FLOW, "   <<< Nibble 0");
      }
      TechPtr->RxEnNibbleDly[Bytelane] = *(UINT16*) RcvEnDly;
      return FALSE;
    }
  } else {
    return TRUE;
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function calculates the average nibble based Receiver Enable Training for Orochi.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *OptParam   - Optional parameter
 *
 *     @return    FALSE - Supported
 *     @return    TRUE - Not supported
 */

BOOLEAN
MemNTrainRxEnGetAvgDlyPerNibbleOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT8 ByteLane;
  if ((NBPtr->ChannelPtr->DimmNibbleAccess & (1 << (NBPtr->TechPtr->ChipSel >> 1))) != 0) {
    if (NBPtr->TechPtr->TrnNibble == NIBBLE_1) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\t     RxEn: ");
      for (ByteLane = 0; ByteLane < (NBPtr->MCTPtr->Status[SbEccDimms] ? 9 : 8) ; ByteLane++) {
        IDS_HDT_CONSOLE (MEM_FLOW, "%03x ", NBPtr->TechPtr->RxEnNibbleDly[ByteLane]);
      }
      IDS_HDT_CONSOLE (MEM_FLOW, "   <<< Nibble AVG\n\n");
      return TRUE;
    } else {
      return FALSE;
    }
  } else {
    return TRUE;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function returns false if nibble training is being used and nibble 1
 *     is being trained.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *ChipSel - Pointer to ChipSel to be trained
 *
 *     @return    FALSE - Supported
 *     @return    TRUE - Not supported
 */

BOOLEAN
MemNTrainingNibbleZeroOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *ChipSel
  )
{
  if (((NBPtr->ChannelPtr->DimmNibbleAccess & (1 << (NBPtr->TechPtr->ChipSel >> 1))) != 0) &&
    (NBPtr->TechPtr->TrnNibble == NIBBLE_1)) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function adjusts Avg PRE value of Phy fence training for OR.
 *
 *     @param[in,out]   *NBPtr  - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *Value16 - Pointer to the value that we want to adjust
 *
 */
VOID
MemNPFenceAdjustOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   INT16 *Value16
  )
{
  *Value16 += 2; //The Avg PRE value is subtracted by 6 only.
  if (*Value16 < 0) {
    *Value16 = 0;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function adjusts WrDqsBias before seed scaling
 *
 *     @param[in,out]   *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *WrDqsBias - Pointer to WrDqsBias
 *
 *     @return    FALSE - Supported
 *     @return    TRUE - Not supported
 */

BOOLEAN
MemNAdjustWrDqsBeforeSeedScalingOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *WrDqsBias
  )
{
  // Subtract (0x20 * WrDqDqsEarly) since it is a non-scalable component
  * (INT16 *) WrDqsBias = (INT16) (0x20 * MemNGetBitFieldNb (NBPtr, BFWrDqDqsEarly));
  return TRUE;
}

