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
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
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
