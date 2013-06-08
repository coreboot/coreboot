/* $NoKeywords:$ */
/**
 * @file
 *
 * mnottn.c
 *
 * Northbridge Non-SPD timings for TN
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
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "OptionMemory.h"
#include "mntn.h"
#include "mu.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_MEM_NB_TN_MNOTTN_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

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
#if 0
UINT32
STATIC
MemNGetODTDelaysTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );
#endif

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

extern BUILD_OPT_CFG UserOptions;

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets the non-SPD timings
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNOtherTimingTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  IDS_HDT_CONSOLE (MEM_STATUS, "\nStart Programming of Non-SPD Timings.\n");
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_TN; Dct++) {
    IDS_HDT_CONSOLE (MEM_STATUS, "\tDct %d\n", Dct);
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctDimmValid > 0) {
      MemNSetOtherTimingTN (NBPtr);
    }
  }
  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets the non-SPD timings  in PCI registers
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSetOtherTimingTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  INT8 ROD;
  INT8 WOD;
  INT8 LD;
  INT8 WrEarlyx2;
  INT8 CDDTrdrdSdDc;
  INT8 CDDTrdrdDd;
  INT8 CDDTwrwrDd;
  INT8 CDDTwrwrSdDc;
  INT8 CDDTrwtTO;
  INT8 CDDTwrrd;
  UINT8 TrdrdSdDc;
  UINT8 TrdrdDd;
  UINT8 TwrwrSdDc;
  UINT8 TwrwrDd;
  UINT8 TrdrdSdSc;
  UINT8 TwrwrSdSc;
  UINT8 Twrrd;
  UINT8 TrwtTO;
  BOOLEAN PerRankTimingEn;

  CH_DEF_STRUCT *ChannelPtr;
  ChannelPtr = NBPtr->ChannelPtr;

  PerRankTimingEn = (BOOLEAN) (MemNGetBitFieldNb (NBPtr, BFPerRankTimingEn));
  //
  // Latency Difference (LD) = Tcl - Tcwl
  //
  LD = (INT8) (MemNGetBitFieldNb (NBPtr, BFTcl)) - (INT8) (MemNGetBitFieldNb (NBPtr, BFTcwl));

  //
  // Read ODT Delay (ROD) = MAX ( 0, (RdOdtOnDuration - 6)) + MAX ( 0, (RdOdtTrnOnDly - LD))
  //
  ROD = MAX (0, (INT8) (MemNGetBitFieldNb (NBPtr, BFRdOdtOnDuration) - 6)) +
          MAX ( 0, (INT8) (MemNGetBitFieldNb (NBPtr, BFRdOdtTrnOnDly) - LD));
  //
  // Write ODT Delay (WOD) = MAX (0, (WrOdtOnDuration - 6))
  //
  WOD = MAX (0, (INT8) (MemNGetBitFieldNb (NBPtr, BFWrOdtOnDuration) - 6));
  //
  // WrEarly = ABS (WrDqDqsEarly) / 2
  //
  WrEarlyx2 = (INT8) MemNGetBitFieldNb (NBPtr, BFWrDqDqsEarly);

  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tLD: %d     ROD: %d     WOD: %d     WrEarlyx2: %d\n\n", LD, ROD, WOD, WrEarlyx2);
  //
  // Read to Read Timing (TrdrdSdSc, TrdrdScDc, TrdrdDd)
  //
  // TrdrdSdSc = 1.
  // TrdrdSdDc (in MEMCLKs) = MAX(TrdrdSdSc, 3 + (IF (D18F2xA8_dct[1:0][PerRankTimingEn])
  //             THEN CEIL(CDDTrdrdSdDc / 2 ) ELSE 0 ENDIF)).
  // TrdrdDd   = MAX(TrdrdSdDc, CEIL(MAX(ROD + 3, CDDTrdrdDd/2 + 3.5)))
  //
  TrdrdSdSc = 1;

  CDDTrdrdSdDc = (INT8) MemNCalcCDDNb (NBPtr, AccessRcvEnDly, AccessRcvEnDly, TRUE, FALSE);
  TrdrdSdDc = MAX (0, PerRankTimingEn ? (3 + (CDDTrdrdSdDc + 1) / 2) : 3);
  TrdrdSdDc = MAX (TrdrdSdSc, TrdrdSdDc);

  CDDTrdrdDd = (INT8) MemNCalcCDDNb (NBPtr, AccessRcvEnDly, AccessRcvEnDly, FALSE, TRUE);
  TrdrdDd = MAX (ROD + 3, (CDDTrdrdDd + 7 + 1) / 2);
  TrdrdDd = MAX (TrdrdSdDc, TrdrdDd);

  MemNSetBitFieldNb (NBPtr, BFTrdrdDd, (UINT32) TrdrdDd);
  MemNSetBitFieldNb (NBPtr, BFTrdrdSdDc, (UINT32) TrdrdSdDc);
  MemNSetBitFieldNb (NBPtr, BFTrdrdSdSc, (UINT32) TrdrdSdSc);
  //
  // Write to Write Timing (TwrwrSdSc, TwrwrScDc, TwrwrDd)
  //
  // TwrwrSdSc = 1.
  // TwrwrSdDc = CEIL(MAX(WOD + 3, CDDTwrwrSdDc / 2 +
  //            (IF (D18F2xA8_dct[1:0][PerRankTimingEn]) THEN 3.5 ELSE 3 ENDIF))).
  //
  // TwrwrDd = CEIL (MAX (WOD + 3, CDDTwrwrDd / 2 + 3.5))
  // TwrwrSdSc <= TwrwrSdDc <= TwrwrDd
  //
  TwrwrSdSc = 1;

  CDDTwrwrSdDc = (INT8) MemNCalcCDDNb (NBPtr, AccessWrDqsDly, AccessWrDqsDly, TRUE, FALSE);
  TwrwrSdDc = (UINT8) MAX (WOD + 3, (CDDTwrwrSdDc + (PerRankTimingEn ? 7 : 6) + 1 ) / 2);

  CDDTwrwrDd = (INT8) MemNCalcCDDNb (NBPtr, AccessWrDqsDly, AccessWrDqsDly, FALSE, TRUE);
  TwrwrDd = (UINT8) MAX ((UINT8) (WOD + 3), (CDDTwrwrDd + 7 + 1) / 2);

  TwrwrSdDc = (TwrwrSdSc <= TwrwrSdDc) ? TwrwrSdDc : TwrwrSdSc;
  TwrwrDd = (TwrwrSdDc <= TwrwrDd) ? TwrwrDd : TwrwrSdDc;

  MemNSetBitFieldNb (NBPtr, BFTwrwrDd, (UINT32) TwrwrDd);
  MemNSetBitFieldNb (NBPtr, BFTwrwrSdDc, (UINT32) TwrwrSdDc);
  MemNSetBitFieldNb (NBPtr, BFTwrwrSdSc, (UINT32) TwrwrSdSc);
  //
  // Write to Read DIMM Termination Turn-around
  //
  // Twrrd = MAX ( 1, CEIL (MAX (WOD, (CDDTwrrd / 2) + 0.5 - WrEarly) - LD + 3))
  //
  CDDTwrrd = (INT8) MemNCalcCDDNb (NBPtr, AccessWrDqsDly, AccessRcvEnDly, TRUE, TRUE);
  Twrrd =  MAX (1, MAX (WOD, (CDDTwrrd + 1 - WrEarlyx2 + 1) / 2) - LD + 3);

  MemNSetBitFieldNb (NBPtr, BFTwrrd, (UINT32) Twrrd);
  //
  // Read to Write Turnaround for Data, DQS Contention
  //
  // TrwtTO = CEIL (MAX (ROD, (CDDTrwtTO / 2) - 0.5 + WrEarly) + LD + 3)
  //
  CDDTrwtTO = (INT8) MemNCalcCDDNb (NBPtr, AccessRcvEnDly, AccessWrDqsDly, TRUE, TRUE);

  TrwtTO = MAX ((ChannelPtr->Dimms == 1 ? 0 : ROD) , (CDDTrwtTO - 1 + WrEarlyx2 + 1) / 2) + LD + 3;

  MemNSetBitFieldNb (NBPtr, BFTrwtTO, (UINT32) TrwtTO);
  //
  // Read to Write Turnaround for opportunistic Write Bursting
  //
  // TrwtWB = TrwtTO + 1
  //
  MemNSetBitFieldNb (NBPtr, BFTrwtWB, (UINT32) TrwtTO + 1);

  IDS_HDT_CONSOLE (MEM_FLOW, "\t\t                            TrdrdSdSc : %02x\n", TrdrdSdSc);
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tCDDTrdrdSdDc : %02x           TrdrdSdDc : %02x\n", CDDTrdrdSdDc, TrdrdSdDc);
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tCDDTrdrdDd   : %02x           TrdrdDd   : %02x\n\n", CDDTrdrdDd, TrdrdDd);

  IDS_HDT_CONSOLE (MEM_FLOW, "\t\t                            TwrwrSdSc  : %02x\n", TwrwrSdSc);
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tCDDTwrwrSdDc : %02x           TwrwrSdDc : %02x\n", CDDTwrwrSdDc, TwrwrSdDc );
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tCDDTwrwrDd   : %02x           TwrwrDd   : %02x\n\n", CDDTwrwrDd, TwrwrDd);

  IDS_HDT_CONSOLE (MEM_FLOW, "\t\t                            TrwtWB    : %02x\n", TrwtTO + 1);
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tCDDTwrrd     : %02x           Twrrd     : %02x\n", (UINT8) CDDTwrrd, (UINT8) Twrrd );
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tCDDTrwtTO    : %02x           TrwtTO    : %02x\n\n", (UINT8) CDDTrwtTO, (UINT8) TrwtTO );
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets the ODT delays
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
#if 0
UINT32
STATIC
MemNGetODTDelaysTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  INT8 Ld;
  UINT32 ODTDelays;
  //
  // The BIOS must additionally configure the ODT pattern
  // and the ODT switching delays.
  //
  // Program F2x[1, 0]9C_x83 DRAM Phy ODT Assertion Control Register based on Burst length.
  //     -Read the Burst Length from F2x[1, 0]84[BurstCtrl].
  //     -Value of 2, BL = 4 else assume BL=8.
  //     -Initialize ODTDelays based on BL value
  //     -WrOdtOnDuration [14:12] = BL / 2 + 1
  //     -WrOdtTrnOnDly [10:8] = 0
  //     -RdOdtOnDuration [6:4] = BL / 2 + 1
  //
  ODTDelays = (MemNGetBitFieldNb (NBPtr, BFBurstCtrl) == 2) ? 0x00003030 : 0x00005050;

  // RdOdtTrnOnDly [3:0] < (CL-CWL) or (CL-CWL - 1)
  // See BKDG F2x[1, 0]9C_x83 DRAM Phy ODT Assertion Control Register [3:0]
  Ld = ((INT8)MemNGetBitFieldNb (NBPtr, BFTcl) + 1) - ((INT8)MemNGetBitFieldNb (NBPtr, BFTcwl) + 5);
  if (Ld < 0) {
    Ld = 0;
  }
  if (Ld > 7) {
    Ld = 7;
  }
  ODTDelays += Ld;
  return ODTDelays;
}
#endif
