/* $NoKeywords:$ */
/**
 * @file
 *
 * mtspd2.c
 *
 * Technology SPD supporting functions for DDR2
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech/DDR2)
 * @e \$Revision: 56279 $ @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
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
#include "AdvancedApi.h"
#include "amdlib.h"
#include "Ids.h"
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mu.h"
#include "mt2.h"
#include "mtspd2.h"
#include "mftds.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_TECH_DDR2_MTSPD2_FILECODE

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

UINT8
STATIC
MemTSPDGetTCL2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
STATIC
MemTSysCapability2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 k,
  IN       UINT16 j
  );

BOOLEAN
STATIC
MemTDimmSupports2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 k,
  IN       UINT8 j,
  IN       UINT8 i
  );

UINT8
STATIC
MemTGetTk2 (
  IN       UINT8 k
  );

UINT8
STATIC
MemTGetBankAddr2 (
  IN       UINT8 k
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

extern BUILD_OPT_CFG UserOptions;

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sets the DRAM mode
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - indicates that the DRAM mode is set to DDR2
 */

BOOLEAN
MemTSetDramMode2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  TechPtr->NBPtr->SetBitField (TechPtr->NBPtr, BFLegacyBiosMode, 0);
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function determines if DIMMs are present. It checks checksum and interrogates the SPDs
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - indicates that a FATAL error has not occurred
 *     @return  FALSE - indicates that a FATAL error has occurred
 */

BOOLEAN
MemTDIMMPresence2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 *SpdBufferPtr;
  MEM_PARAMETER_STRUCT *RefPtr;
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  CH_DEF_STRUCT *ChannelPtr;
  MEM_NB_BLOCK *NBPtr;
  UINT16 Checksum;
  UINT16 Value16;
  UINT8 Dct;
  UINT8 Channel;
  UINT8 i;
  UINT8 ByteNum;
  UINT8 Devwidth;
  UINT8 Value8;
  UINT8 MaxDimms;
  UINT8 DimmSlots;
  UINT16 DimmMask;
  BOOLEAN SPDCtrl;

  NBPtr = TechPtr->NBPtr;
  RefPtr = NBPtr->RefPtr;
  MCTPtr = NBPtr->MCTPtr;

  SPDCtrl = UserOptions.CfgIgnoreSpdChecksum;

  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    DCTPtr = NBPtr->DCTPtr;
    for (Channel = 0; Channel < NBPtr->ChannelCount; Channel++) {
      NBPtr->SwitchChannel (NBPtr, Channel);
      ChannelPtr = NBPtr->ChannelPtr;
      ChannelPtr->DimmQrPresent = 0;

      //  Get the maximum number of DIMMs
      DimmSlots = GetMaxDimmsPerChannel (RefPtr->PlatformMemoryConfiguration,
                                         MCTPtr->SocketId,
                                         NBPtr->GetSocketRelativeChannel (NBPtr, Dct, Channel)
                                        );
      MaxDimms = MAX_DIMMS_PER_CHANNEL;
      for (i = 0; i < MaxDimms; i++) {
        //  Bitmask representing dimm #i.
        DimmMask = (UINT16)1 << i;

        if ((ChannelPtr->DimmQrPresent & DimmMask) || (i < DimmSlots)) {
          if (MemTGetDimmSpdBuffer2 (TechPtr, &SpdBufferPtr, i)) {
            MCTPtr->DimmPresent |= DimmMask;

            //  Start by computing checksum for this SPD
            Checksum = 0;
            for (ByteNum = 0; ByteNum < SPD_CHECKSUM; ByteNum++) {
              Checksum = Checksum + (UINT16) SpdBufferPtr[ByteNum];
            }
            //  Check for valid checksum value
            AGESA_TESTPOINT (TpProcMemSPDChecking, &(NBPtr->MemPtr->StdHeader));

            if (SpdBufferPtr[SPD_TYPE] == JED_DDR2_SDRAM) {
              ChannelPtr->ChDimmValid |= DimmMask;
              MCTPtr->DimmValid |= DimmMask;
            } else {
              // Current socket is set up to only support DDR2 dimms.
              IDS_ERROR_TRAP;
            }
            if ((SpdBufferPtr[SPD_CHECKSUM] != (UINT8)Checksum) && !SPDCtrl) {
              //
              // if NV_SPDCHK_RESTRT is set to 0,
              // cannot ignore faulty SPD checksum
              //
              //  Indicate checksum error
              ChannelPtr->DimmSpdCse |= DimmMask;
              PutEventLog (AGESA_ERROR, MEM_ERROR_CHECKSUM_NV_SPDCHK_RESTRT_ERROR, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
              SetMemError (AGESA_ERROR, MCTPtr);
            }

            //  Check module type information.
            if (SpdBufferPtr[SPD_DIMM_TYPE] & JED_REG_ADC_MSK) {
              ChannelPtr->RegDimmPresent |= DimmMask;
              MCTPtr->RegDimmPresent |= DimmMask;
            }

            if (SpdBufferPtr[SPD_DIMM_TYPE] & JED_SODIMM) {
              ChannelPtr->SODimmPresent |= DimmMask;
            }

            //  Check error correction type
            if (SpdBufferPtr[SPD_EDC_TYPE] & JED_ECC) {
              MCTPtr->DimmEccPresent |= DimmMask;  //  Dimm has ECC
            }
            if (SpdBufferPtr[SPD_EDC_TYPE] & JED_ADRC_PAR) {
              MCTPtr->DimmParPresent |= DimmMask;  //  Dimm has parity
            }

            //  Get the Dimm width data
            Devwidth = SpdBufferPtr[SPD_DEV_WIDTH] & 0xFE;
            if (Devwidth == 4) {
              ChannelPtr->Dimmx4Present |= DimmMask;   //  Dimm has parity
            } else if (Devwidth == 8) {
              ChannelPtr->Dimmx8Present |= DimmMask;   //  Dimm has parity
            } else if (Devwidth == 16) {
              ChannelPtr->Dimmx16Present |= DimmMask;  //  Dimm has parity
            }

            //  Determine the page size.
            //       page_size = 2^COLBITS * Devwidth/8
            //
            Value16 = (((UINT16)1 << SpdBufferPtr[SPD_COL_SZ]) * Devwidth) / 8;
            if (!(Value16 >> 11)) {
              DCTPtr->Timings.DIMM1KPage |= DimmMask;
            }

            //  Check for 'analysis probe installed'
            if (SpdBufferPtr[SPD_ATTRIB] & JED_PROBE_MSK) {
              MCTPtr->Status[SbDiagClks] = TRUE;
            }

            //  Determine the geometry of the DIMM module
            if (SpdBufferPtr[SPD_DM_BANKS] & SP_DPL_BIT) {
              ChannelPtr->DimmPlPresent |= DimmMask;   //  Dimm is planar
            }

            //  specify the number of ranks
            Value8 = (SpdBufferPtr[SPD_DM_BANKS] & 0x07) + 1;
            if (Value8 > 2) {
              if (ChannelPtr->DimmQrPresent == 0) {
                // if any DIMMs are QR,
                // we have to make two passes through DIMMs
                //
                MaxDimms = MaxDimms << 1;
              }

              if (i < DimmSlots) {
                ChannelPtr->DimmQrPresent |= DimmMask;
                ChannelPtr->DimmQrPresent |= (DimmMask << 2);
              }
              Value8 = 2;
            } else if (Value8 == 2) {
              ChannelPtr->DimmDrPresent |= DimmMask;   //  Dual rank dimms
            }

            //  Calculate bus loading per Channel
            if (Devwidth == 16) {
              Devwidth = 4;
            } else if (Devwidth == 4) {
              Devwidth = 16;
            }
            //  double Addr bus load value for dual rank DIMMs
            if (Value8 == 2) {
              Devwidth = Devwidth << 1;
            }

            ChannelPtr->Ranks = ChannelPtr->Ranks + Value8;
            ChannelPtr->Loads = ChannelPtr->Loads + Devwidth;
            ChannelPtr->Dimms++;

            //  Now examine the dimm packaging dates
            Value8 = SpdBufferPtr[SPD_MAN_DATE_YR];
            if (Value8 < M_YEAR_06) {
              ChannelPtr->DimmYr06 |= DimmMask;    //  Built before end of 2006
              ChannelPtr->DimmWk2406 |= DimmMask;  //  Built before end of week 24,2006
            } else if (Value8 == M_YEAR_06) {
              ChannelPtr->DimmYr06 |= DimmMask;    //  Built before end of 2006
              if (SpdBufferPtr[SPD_MAN_DATE_WK] <= M_WEEK_24) {
                ChannelPtr->DimmWk2406 |= DimmMask;  //  Built before end of week 24,2006
              }
            }
          } // if DIMM present
        } // Quadrank
      } // Dimm loop

      if (Channel == 0) {
        DCTPtr->Timings.DctDimmValid = ChannelPtr->ChDimmValid;
        DCTPtr->Timings.DimmSpdCse = ChannelPtr->DimmSpdCse;
        DCTPtr->Timings.DimmQrPresent = ChannelPtr->DimmQrPresent;
        DCTPtr->Timings.DimmDrPresent = ChannelPtr->DimmDrPresent;
        DCTPtr->Timings.Dimmx4Present = ChannelPtr->Dimmx4Present;
        DCTPtr->Timings.Dimmx8Present = ChannelPtr->Dimmx8Present;
        DCTPtr->Timings.Dimmx16Present = ChannelPtr->Dimmx16Present;
      }
      if ((Channel != 1) || (Dct != 1)) {
        MCTPtr->DimmPresent <<= 8;
        MCTPtr->DimmValid <<= 8;
        MCTPtr->RegDimmPresent <<= 8;
        MCTPtr->DimmEccPresent <<= 8;
        MCTPtr->DimmParPresent <<= 8;
      }
    } // Channel loop
  } // DCT loop


  //  If we have DIMMs, some further general characteristics checking
  if (MCTPtr->DimmValid) {
    //  If there are registered dimms, all the dimms must be registered
    if (MCTPtr->RegDimmPresent == MCTPtr->DimmValid) {
      //  All dimms registered
      MCTPtr->Status[SbRegistered] = TRUE;
    } else if (MCTPtr->RegDimmPresent) {
      //  We have an illegal DIMM mismatch
      PutEventLog (AGESA_FATAL, MEM_ERROR_MODULE_TYPE_MISMATCH_DIMM, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_FATAL, MCTPtr);
    }

    //  check the ECC capability of the DIMMs
    if (MCTPtr->DimmEccPresent == MCTPtr->DimmValid) {
      MCTPtr->Status[SbEccDimms] = TRUE;  //  All dimms ECC capable
    }

    //  check the parity capability of the DIMMs
    if (MCTPtr->DimmParPresent == MCTPtr->DimmValid) {
      MCTPtr->Status[SbParDimms] = TRUE;  //  All dimms parity capable
    }
  } else {
  }

  NBPtr->SwitchDCT (NBPtr, 0);
  NBPtr->SwitchChannel (NBPtr, 0);
  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function finds the best T and CL primary timing parameter pair, per Mfg.,for the given
 *   set of DIMMs, and store into DIE_STRUCT(.Speed and .Casl).
 *   See "Global relationship between index values and item values" for definition of
 *   CAS latency index (j) and Frequency index (k).
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - indicates that a FATAL error has not occurred
 *     @return  FALSE - indicates that a FATAL error has occurred
 */

BOOLEAN
MemTSPDGetTargetSpeed2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  CONST UINT16 SpeedCvt[] = {
    DDR400_FREQUENCY,
    DDR533_FREQUENCY,
    DDR667_FREQUENCY,
    DDR800_FREQUENCY,
    DDR1066_FREQUENCY
  };
  INT8 i;
  INT8 j;
  INT8 k;
  INT8 Dct;
  INT8 Channel;
  UINT8 T1min;
  UINT8 CL1min;
  BOOLEAN IsSupported;
  MEM_NB_BLOCK *NBPtr;
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  CH_DEF_STRUCT *ChannelPtr;

  NBPtr = TechPtr->NBPtr;
  MCTPtr = TechPtr->NBPtr->MCTPtr;

  CL1min = 0xFF;
  T1min = 0xFF;

  // For DDR2, run SyncTargetSpeed first to get frequency limit into DCTPtr->Timings.Speed
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    NBPtr->DCTPtr->Timings.TargetSpeed = 16;  // initialized with big number
  }
  NBPtr->SyncTargetSpeed (NBPtr);

  // Find target frequency and Tcl
  for (k = K_MAX; k >= K_MIN; k--) {
    for (j = J_MIN; j <= J_MAX; j++) {
      if (MemTSysCapability2 (TechPtr, k, j)) {
        IsSupported = TRUE;
        for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
          NBPtr->SwitchDCT (NBPtr, Dct);
          for (Channel = 0; Channel < NBPtr->ChannelCount; Channel++) {
            NBPtr->SwitchChannel (NBPtr, Channel);
            ChannelPtr = NBPtr->ChannelPtr;
            for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i++) {
              if (ChannelPtr->ChDimmValid & ((UINT8)1 << i)) {
                if (!MemTDimmSupports2 (TechPtr, k, j, i)) {
                  IsSupported = FALSE;
                  Dct = NBPtr->DctCount;
                  Channel = NBPtr->ChannelCount;
                  break;
                }
              }
            }
          }
        }

        if (IsSupported) {
          T1min = k;
          CL1min = j;
          //  Kill the loops...
          k = K_MIN - 1;
          j = J_MAX + 1;
        }
      }
    }
  }

  if (T1min == 0xFF) {
    //  Failsafe values, running in minimum mode
    PutEventLog (AGESA_FATAL, MEM_ERROR_MISMATCH_DIMM_CLOCKS, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
    PutEventLog (AGESA_FATAL, MEM_ERROR_MINIMUM_MODE, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
    SetMemError (AGESA_ERROR, MCTPtr);

    T1min = T_DEF;
    CL1min = CL_DEF;
  }

  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    DCTPtr = NBPtr->DCTPtr;
    DCTPtr->Timings.TargetSpeed = SpeedCvt[T1min - 1];
  }

  // Ensure the target speed can be applied to all channels of the current node
  NBPtr->SyncTargetSpeed (NBPtr);

  // Set the start-up frequency
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    DCTPtr = NBPtr->DCTPtr;
    DCTPtr->Timings.Speed = DCTPtr->Timings.TargetSpeed;
    DCTPtr->Timings.CasL = CL1min + 2;  // Convert to clocks
  }

  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function check the symmetry of DIMM pairs (DIMM on Channel A matching with
 *   DIMM on Channel B), the overall DIMM population, and determine the width mode:
 *   64-bit, 64-bit muxed, 128-bit.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - indicates that a FATAL error has not occurred
 *     @return  FALSE - indicates that a FATAL error has occurred
 */

BOOLEAN
MemTSPDCalcWidth2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 *SpdBufferAPtr;
  UINT8 *SpdBufferBPtr;
  MEM_NB_BLOCK *NBPtr;
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  UINT8 i;
  UINT16 DimmMask;
  UINT8 UngangMode;

  NBPtr = TechPtr->NBPtr;
  MCTPtr = NBPtr->MCTPtr;
  DCTPtr = NBPtr->DCTPtr;

  UngangMode = UserOptions.CfgMemoryModeUnganged;
  IDS_OPTION_HOOK (IDS_GANGING_MODE, &UngangMode, &(NBPtr->MemPtr->StdHeader));

  //  Check symmetry of channel A and channel B dimms for 128-bit mode
  //  capability.
  //
  AGESA_TESTPOINT (TpProcMemModeChecking, &(NBPtr->MemPtr->StdHeader));
  i = 0;
  if (MCTPtr->DctData[0].Timings.DctDimmValid == MCTPtr->DctData[1].Timings.DctDimmValid) {
    for (; i < MAX_DIMMS_PER_CHANNEL; i++) {
      DimmMask = (UINT16)1 << i;
      if (DCTPtr->Timings.DctDimmValid & DimmMask) {
        NBPtr->SwitchDCT (NBPtr, 0);
        MemTGetDimmSpdBuffer2 (TechPtr, &SpdBufferAPtr, i);
        NBPtr->SwitchDCT (NBPtr, 1);
        MemTGetDimmSpdBuffer2 (TechPtr, &SpdBufferBPtr, i);

        if ((SpdBufferAPtr[SPD_ROW_SZ]&0x1F) != (SpdBufferBPtr[SPD_ROW_SZ]&0x1F)) {
          break;
        }

        if ((SpdBufferAPtr[SPD_COL_SZ]&0x1F) != (SpdBufferBPtr[SPD_COL_SZ]&0x1F)) {
          break;
        }

        if (SpdBufferAPtr[SPD_BANK_SZ] != SpdBufferBPtr[SPD_BANK_SZ]) {
          break;
        }

        if ((SpdBufferAPtr[SPD_DEV_WIDTH]&0x7F) != (SpdBufferBPtr[SPD_DEV_WIDTH]&0x7F)) {
          break;
        }

        if ((SpdBufferAPtr[SPD_DM_BANKS]&0x07) != (SpdBufferBPtr[SPD_DM_BANKS]&0x07)) {
          break;
        }
      }
    }
  }
  if (i < MAX_DIMMS_PER_CHANNEL) {
    PutEventLog (AGESA_ALERT, MEM_ALERT_ORG_MISMATCH_DIMM, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
    SetMemError (AGESA_ALERT, MCTPtr);
  } else if (!UngangMode) {
    NBPtr->Ganged = TRUE;
    MCTPtr->GangedMode = TRUE;
    MCTPtr->Status[Sb128bitmode] = TRUE;
    NBPtr->SetBitField (NBPtr, BFDctGangEn, 1);
  }

  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *       Initialize DCT Timing registers as per DIMM SPD.
 *       For primary timing (T, CL) use best case T value.
 *       For secondary timing params., use most aggressive settings
 *           of slowest DIMM.
 *
 *   Note:
 *   There are three components to determining "maximum frequency": SPD component,
 *   Bus load component, and "Preset" max frequency component.
 *   The SPD component is a function of the min cycle time specified by each DIMM,
 *   and the interaction of cycle times from all DIMMs in conjunction with CAS
 *   latency.  The SPD component only applies when user timing mode is 'Auto'.
 *
 *   The Bus load component is a limiting factor determined by electrical
 *   characteristics on the bus as a result of varying number of device loads.  The
 *   Bus load component is specific to each platform but may also be a function of
 *   other factors.  The bus load component only applies when user timing mode is
 * ' Auto'.
 *
 *   The Preset component is subdivided into three items and is the minimum of
 *   the set: Silicon revision, user limit setting when user timing mode is 'Auto' and
 *   memclock mode is 'Limit', OEM build specification of the maximum frequency.
 *   The Preset component only applies when user timing mode is 'Auto'.

 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - indicates that a FATAL error has not occurred
 *     @return  FALSE - indicates that a FATAL error has occurred
 */

BOOLEAN
MemTAutoCycTiming2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  CONST UINT8 SpdIndexes[] = {
    SPD_TRCD,
    SPD_TRP,
    SPD_TRTP,
    SPD_TRAS,
    SPD_TRC,
    SPD_TWR,
    SPD_TRRD,
    SPD_TWTR
  };
  CONST UINT8 Multiples[] = {10, 10, 10, 40, 40, 10, 10, 10};

  CONST UINT8 Tab1KTfawTK[] = {8, 10, 13, 14, 0, 20};
  CONST UINT8 Tab2KTfawTK[] = {10, 14, 17, 18, 0, 24};
  CONST UINT8 TabDefTrcK[]  = {0x41, 0x3C, 0x3C, 0x3A, 0, 0x3A};

  UINT8 MiniMaxTmg[GET_SIZE_OF (SpdIndexes)];
  UINT8 MiniMaxTrfc[4];

  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  MEM_NB_BLOCK *NBPtr;
  UINT16 DimmMask;
  UINT16 Value16;
  UINT16 Tk40;
  UINT8 i;
  UINT8 j;
  UINT8 Value8;
  UINT8 Temp8;
  UINT8  *StatTmgPtr;
  UINT16 *StatDimmTmgPtr;
  BOOLEAN   Is1066;
  UINT8 *SpdBufferPtr;

  NBPtr = TechPtr->NBPtr;
  MCTPtr = NBPtr->MCTPtr;
  DCTPtr = NBPtr->DCTPtr;

  // initialize mini-max arrays
  for (j = 0; j < GET_SIZE_OF (MiniMaxTmg); j++) {
    MiniMaxTmg[j] = 0;
  }
  for (j = 0; j < GET_SIZE_OF (MiniMaxTrfc); j++) {
    MiniMaxTrfc[j] = 0;
  }

  // ======================================================================
  //  Get primary timing (CAS Latency and Cycle Time)
  // ======================================================================
  //  Get OEM specific load variant max
  //

  //======================================================================
  // Gather all DIMM mini-max values for cycle timing data
  //======================================================================
  //
  DimmMask = 1;
  for (i = 0; i < (MAX_CS_PER_CHANNEL / 2); i++) {
    if (DCTPtr->Timings.DctDimmValid & DimmMask) {
      MemTGetDimmSpdBuffer2 (TechPtr, &SpdBufferPtr, i);
      for (j = 0; j < GET_SIZE_OF (SpdIndexes); j++) {
        Value8 = SpdBufferPtr[SpdIndexes[j]];
        if (SpdIndexes[j] == SPD_TRC) {
          if (Value8 == 0 || Value8 == 0xFF) {
            PutEventLog (AGESA_WARNING, MEM_WARNING_NO_SPDTRC_FOUND, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, i, &NBPtr->MemPtr->StdHeader);
            SetMemError (AGESA_WARNING, MCTPtr);
            Value8 = TabDefTrcK[(DCTPtr->Timings.Speed / 66) - 3];
          }
        }
        if (MiniMaxTmg[j] < Value8) {
          MiniMaxTmg[j] = Value8;
        }
      }

      //  get Trfc0 - Trfc3 values
      Value8 = SpdBufferPtr[SPD_BANK_SZ];
      Temp8 = (Value8 << 3) | (Value8 >> 5);
      Value8 = SpdBufferPtr[SPD_DEV_WIDTH];
      ASSERT (LibAmdBitScanReverse ((UINT32)Value8) <= 4);
      Temp8 >>= 4 - LibAmdBitScanReverse ((UINT32)Value8);
      Value8 = LibAmdBitScanReverse ((UINT32)Temp8);
      if (MiniMaxTrfc[i] < Value8) {
        MiniMaxTrfc[i] = Value8;
      }
    }
    DimmMask <<= 1;
  }

  // ======================================================================
  //  Convert  DRAM CycleTiming values and store into DCT structure
  // ======================================================================
  //
  Tk40 = 40000 / DCTPtr->Timings.Speed;
  if (DCTPtr->Timings.Speed == DDR1066_FREQUENCY) {
    Is1066 = TRUE;
  } else {
    Is1066 = FALSE;
  }
  //   Notes:
  //   1. All secondary time values given in SPDs are in binary with UINTs of ns.
  //   2. Some time values are scaled by four, in order to have least count of 0.25 ns
  //      (more accuracy).  JEDEC SPD spec. shows which ones are x1 and x4.
  //   3. Internally to this SW, cycle time, Tk, is scaled by 10 to affect a
  //      least count of 0.1 ns (more accuracy).
  //   4. SPD values not scaled are multiplied by 10 and then divided by 10T to find
  //      equivalent minimum number of bus clocks (a remainder causes round-up of clocks).
  //   5. SPD values that are prescaled by 4 are multiplied by 10 and then divided by 40T to find
  //      equivalent minimum number of bus clocks (a remainder causes round-up of clocks).
  //
  StatDimmTmgPtr = &DCTPtr->Timings.DIMMTrcd;
  StatTmgPtr = &DCTPtr->Timings.Trcd;
  for (j = 0; j < GET_SIZE_OF (SpdIndexes); j++) {
    Value16 = (UINT16)MiniMaxTmg[j] * Multiples[j];
    StatDimmTmgPtr[j] = Value16;

    MiniMaxTmg[j] = (UINT8) ((Value16 + Tk40 - 1) / Tk40);
    if (SpdIndexes[j] == SPD_TRTP) {
      MiniMaxTmg[j] = (DCTPtr->Timings.Speed <= DDR533_FREQUENCY) ? 2 : 3;   // based on BL of 32 bytes
    }

    StatTmgPtr[j] = MiniMaxTmg[j];
  }
  DCTPtr->Timings.Trfc0 = MiniMaxTrfc[0];
  DCTPtr->Timings.Trfc1 = MiniMaxTrfc[1];
  DCTPtr->Timings.Trfc2 = MiniMaxTrfc[2];
  DCTPtr->Timings.Trfc3 = MiniMaxTrfc[3];

  DCTPtr->Timings.CasL = MemTSPDGetTCL2 (TechPtr);

  if (DCTPtr->Timings.DIMM1KPage) {
    DCTPtr->Timings.Tfaw = Tab1KTfawTK[(DCTPtr->Timings.Speed / 66) - 3];
  } else {
    DCTPtr->Timings.Tfaw = Tab2KTfawTK[(DCTPtr->Timings.Speed / 66) - 3];
  }
  if (Is1066) {
    DCTPtr->Timings.Tfaw >>= 1;
  }

  //======================================================================
  // Program DRAM Timing values
  //======================================================================
  //
  NBPtr->ProgramCycTimings (NBPtr);

  MemFInitTableDrive (NBPtr, MTAfterAutoCycTiming);

  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sets the bank addressing, program Mask values and build a chip-select population map.
 *   This routine programs PCI 0:24N:2x80 config register.
 *   This routine programs PCI 0:24N:2x60,64,68,6C config registers (CS Mask 0-3)
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - indicates that a FATAL error has not occurred
 *     @return  FALSE - indicates that a FATAL error has occurred
 */

BOOLEAN
MemTSPDSetBanks2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 *SpdBufferPtr;
  UINT8 i;
  UINT8 ChipSel;
  UINT8 DimmID;
  UINT8 Value8;
  UINT8 Rows;
  UINT8 Cols;
  UINT8 Ranks;
  UINT8 Banks;
  UINT32 BankAddrReg;
  UINT32 CsMask;
  UINT16 CSSpdCSE;
  UINT16 CSExclude;
  UINT16 DimmQRDR;
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  MEM_NB_BLOCK *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MCTPtr = NBPtr->MCTPtr;
  DCTPtr = NBPtr->DCTPtr;

  BankAddrReg = 0;
  CSSpdCSE = 0;
  CSExclude = 0;
  for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL; ChipSel += 2) {
    DimmID = ChipSel >> 1;

    DimmQRDR = (DCTPtr->Timings.DimmQrPresent) | (DCTPtr->Timings.DimmDrPresent);
    if (DCTPtr->Timings.DimmSpdCse & (UINT16) 1 << DimmID) {
      CSSpdCSE |= (UINT16) ((DimmQRDR & (UINT16) 1 << DimmID) ? 3 : 1) << ChipSel;
    }
    if ((DCTPtr->Timings.DimmExclude & ((UINT16) 1 << DimmID)) != 0) {
      CSExclude |= (UINT16) ((DimmQRDR & (UINT16) 1 << DimmID) ? 3: 1) << ChipSel;
    }

    if (DCTPtr->Timings.DctDimmValid & ((UINT16)1 << DimmID)) {
      MemTGetDimmSpdBuffer2 (TechPtr, &SpdBufferPtr, DimmID);

      //  Get the basic data
      Rows = SpdBufferPtr[SPD_ROW_SZ] & 0x1F;
      Cols = SpdBufferPtr[SPD_COL_SZ] & 0x1F;
      Banks = SpdBufferPtr[SPD_L_BANKS];
      Ranks = (SpdBufferPtr[SPD_DM_BANKS] & 0x07) + 1;

      //  Configure the bank encoding
      Value8 = (Cols - 9) << 3;
      Value8 |= (Banks == 8) ? 4 : 0;
      Value8 |= (Rows - 13);

      for (i = 0; i < 12; i++) {
        if (Value8 == MemTGetBankAddr2 (i)) {
          break;
        }
      }

      if (i < 12) {
        BankAddrReg |= ((UINT32)i << (ChipSel << 1));

        // Mask value=(2pow(rows+cols+banks+3)-1)>>8,
        // or 2pow(rows+cols+banks-5)-1
        //
        Value8 = Rows + Cols;
        Value8 -= (Banks == 8) ? 2:3;
        if (MCTPtr->Status[Sb128bitmode]) {
          Value8++;
        }
        CsMask = ((UINT32)1 << Value8) - 1;
        DCTPtr->Timings.CsPresent |= (UINT16)1 << ChipSel;

        if (Ranks >= 2) {
          DCTPtr->Timings.CsPresent |= (UINT16)1 << (ChipSel + 1);
        }

        //  Update the DRAM CS Mask for this chipselect
        NBPtr->SetBitField (NBPtr, BFCSMask0Reg + (ChipSel >> 1), (CsMask & NBPtr->CsRegMsk));
      }
    }
  }
  // For ranks that need to be excluded, the loading of this rank should be considered
  // in timing, so need to set CsPresent before setting CsTestFail
  if ((CSSpdCSE != 0) || (CSExclude != 0)) {
    if (!NBPtr->MemPtr->ErrorHandling (MCTPtr, NBPtr->Dct, (CSSpdCSE | CSExclude), &NBPtr->MemPtr->StdHeader)) {
      ASSERT (FALSE);
    }
  }

  //  If there are no chip selects, we have an error situation.
  if (DCTPtr->Timings.CsPresent == 0) {
    PutEventLog (AGESA_ERROR, MEM_ERROR_NO_CHIPSELECT, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
    SetMemError (AGESA_ERROR, MCTPtr);
  }

  NBPtr->SetBitField (NBPtr, BFDramBankAddrReg, BankAddrReg);

  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function returns the low bit that will be swapped to enable CS interleaving
 *
 *     @param[in]   BankEnc - AddrMap Bank encoding from F2x80
 *     @param[in]   *LowBit - pointer to low bit
 *     @param[in]   *HiBit  - pointer hight bit
 *
 */

VOID
MemTGetCSIntLvAddr2 (
  IN       UINT8 BankEnc,
     OUT   UINT8 *LowBit,
     OUT   UINT8 *HiBit
  )
{
  CONST UINT8 ArrCodesLo[] = {6, 7, 7, 8, 8, 8, 8, 8, 9, 9, 8, 9};
  CONST UINT8 ArrCodesHi[] = {19, 20, 21, 21, 21, 22, 22, 23, 23, 24, 24, 25};
  ASSERT (BankEnc < GET_SIZE_OF (ArrCodesLo));
  ASSERT (BankEnc < GET_SIZE_OF (ArrCodesHi));
  //  return ArrCodes[BankEnc];
  *LowBit = ArrCodesLo[BankEnc];
  *HiBit = ArrCodesHi[BankEnc];
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function returns the CAS latency of the current frequency.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  CAS Latency
 */
UINT8
STATIC
MemTSPDGetTCL2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TechPtr->NBPtr->DCTPtr->Timings.CasL;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *               Get max frequency from OEM platform definition, from
 *               any user override (limiting) of max frequency, and
 *               from any Si Revision Specific information.  Return
 *               the least of these three in DIE_STRUCT.PresetmaxFreq.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   k -  Frequency index
 *     @param[in]   j - CAS Latency index
 *
 *     @return  TRUE  - (k << 8) | j
 *     @return  FALSE - 0
 */

BOOLEAN
STATIC
MemTSysCapability2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 k,
  IN       UINT16 j
  )
{
  if ((k > TechPtr->NBPtr->DCTPtr->Timings.TargetSpeed) || (j > J_MAX)) {
    return FALSE;
  }

  return TRUE;    //(k << 8) | j;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *      Determine whether dimm(b,i) supports CL(j) and F(k)
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   k -  Frequency index
 *     @param[in]   j - CAS Latency index
 *     @param[in]   i - DIMM number
 *
 *     @return  TRUE  - DIMM supports
 *     @return  FALSE - DIMM does not support
 */

BOOLEAN
STATIC
MemTDimmSupports2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 k,
  IN       UINT8 j,
  IN       UINT8 i
  )
{
  CONST UINT8 SpdBytesForCL[3] = { 9, 23, 25};   // SPD bytes for CL X, CL X-.5, and CL X-1
  UINT8 CLj;
  UINT8 CLi;
  UINT8 T1;
  UINT8 T2;
  UINT8 Tk;
  UINT8 *SpdBufferPtr;
  MEM_NB_BLOCK *NBPtr;

  NBPtr = TechPtr->NBPtr;

  MemTGetDimmSpdBuffer2 (TechPtr, &SpdBufferPtr, i);
  CLj = (UINT8) 1 << (j + 2);
  CLi = SpdBufferPtr[SPD_CAS_LAT];

  if (CLj & CLi) {
    //  If this dimm supports the desired CAS latency...
    //  Determine the SPD location of the dimm speed UINT8 appropriate
    //  to the CAS latency indicated by Table_CL2_j.
    //
    T1 = LibAmdBitScanReverse ((UINT32)CLj);
    T2 = LibAmdBitScanReverse ((UINT32)CLi);
    ASSERT ((T2 - T1) < 3);
    CLi = SpdBufferPtr[SpdBytesForCL[(T2 - T1)]];
    Tk = MemTGetTk2 (k);
    if (CLi == 0) {
      PutEventLog (AGESA_FATAL, MEM_ERROR_NO_CYC_TIME, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_WARNING, NBPtr->MCTPtr);
    } else if (Tk >= CLi) {
      return TRUE;
    }
  }
  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function returns the cycle time
 *
 *     @param[in]   k - CAS Latency index
 *
 *     @return      Tk as specified by JEDEC SPD byte 9.
 */

UINT8
STATIC
MemTGetTk2 (
  IN       UINT8 k
  )
{
  CONST UINT8 TableTK[] = {0x00, 0x50, 0x3D, 0x30, 0x25, 0x18};
  ASSERT (k < GET_SIZE_OF (TableTK));
  return TableTK[k];
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function returns the encoded value of bank address.
 *
 *     @param[in]  k  value
 *
 *     @return      RRRBCC, where CC is the number of Columns minus 9,
 *                  RRR is the number of Rows minus 12, and B is the number of banks
 *                  minus 3.
 */

UINT8
STATIC
MemTGetBankAddr2 (
  IN       UINT8 k
  )
{
  CONST UINT8 TabBankAddr[] = {
    0x00, 0x08, 0x09, 0x10, 0x0C, 0x0D,
    0x11, 0x0E, 0x15, 0x16, 0x0F, 0x17
  };
  ASSERT (k < GET_SIZE_OF (TabBankAddr));
  return TabBankAddr[k];
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function returns a pointer to the SPD Buffer of a specific dimm on
 *    the current channel.
 *
 *     @param[in,out] *TechPtr - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out] **SpdBuffer - Pointer to a pointer to a UINT8 Buffer
 *     @param[in] Dimm - Dimm number
 *
 *
 *     @return BOOLEAN - Value of DimmPresent
 *                       TRUE = Dimm is present, pointer is valid
 *                       FALSE = Dimm is not present, pointer has not been modified.
 */

BOOLEAN
MemTGetDimmSpdBuffer2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   UINT8 **SpdBuffer,
  IN       UINT8 Dimm
  )
{
  CH_DEF_STRUCT *ChannelPtr;
  SPD_DEF_STRUCT *SPDPtr;
  BOOLEAN DimmPresent;

  DimmPresent = FALSE;
  ChannelPtr = TechPtr->NBPtr->ChannelPtr;
  ASSERT (Dimm < (sizeof (ChannelPtr->DimmSpdPtr) / sizeof (ChannelPtr->DimmSpdPtr[0])))
  SPDPtr = ChannelPtr->DimmSpdPtr[Dimm];


  if (SPDPtr != NULL) {
    DimmPresent = SPDPtr->DimmPresent;
    if (DimmPresent) {
      *SpdBuffer = SPDPtr->Data;
    }
  }
  return DimmPresent;
}
