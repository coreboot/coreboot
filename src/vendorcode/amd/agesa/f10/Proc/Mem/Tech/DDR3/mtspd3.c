/**
 * @file
 *
 * mtspd3.c
 *
 * Technology SPD supporting functions for DDR3
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech/DDR3)
 * @e \$Revision: 44323 $ @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 *
 **/
/*****************************************************************************
*
* Copyright (c) 2011, Advanced Micro Devices, Inc.
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
#include "Ids.h"
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mt3.h"
#include "mu.h"
#include "mtspd3.h"
#include "mftds.h"
#include "GeneralServices.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_TECH_DDR3_MTSPD3_FILECODE

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
BOOLEAN
STATIC
MemTCRCCheck3 (
  IN OUT   UINT8 *SPDPtr
  );

UINT8
STATIC
MemTSPDGetTCL3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
STATIC
MemTCheckBankAddr3 (
  IN       UINT8 Encode,
     OUT   UINT8 *Index
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
 *     @return  TRUE - indicates that the DRAM mode is set to DDR3
 */

BOOLEAN
MemTSetDramMode3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  TechPtr->NBPtr->SetBitField (TechPtr->NBPtr, BFLegacyBiosMode, 0);
  TechPtr->NBPtr->SetBitField (TechPtr->NBPtr, BFDdr3Mode, 1);
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function determines if DIMMs are present. It checks checksum and interrogates the SPDs
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE  - indicates that a FATAL error has not occurred
 *     @return  FALSE - indicates that a FATAL error has occurred
 */

BOOLEAN
MemTDIMMPresence3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 Dct;
  UINT8 Channel;
  UINT8 i;
  MEM_PARAMETER_STRUCT *RefPtr;
  UINT8 *SpdBufferPtr;
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  CH_DEF_STRUCT *ChannelPtr;
  MEM_NB_BLOCK *NBPtr;
  BOOLEAN SPDCtrl;
  UINT8 Devwidth;
  UINT8 MaxDimms;
  UINT8 Value8;
  UINT16  DimmMask;

  NBPtr = TechPtr->NBPtr;
  RefPtr = NBPtr->RefPtr;
  MCTPtr = NBPtr->MCTPtr;

  SPDCtrl = UserOptions.CfgIgnoreSpdChecksum;
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    printk(BIOS_DEBUG, " Dct %x ", Dct);
    NBPtr->SwitchDCT (NBPtr, Dct);
    DCTPtr = NBPtr->DCTPtr;
    for (Channel = 0; Channel < NBPtr->ChannelCount; Channel++) {
      printk(BIOS_DEBUG, "Channel %x\n", Channel);
      NBPtr->SwitchChannel (NBPtr, Channel);
      ChannelPtr = NBPtr->ChannelPtr;
      ChannelPtr->DimmQrPresent = 0;
      //
      // Get the maximum number of DIMMs
      //
      MaxDimms = MAX_DIMMS_PER_CHANNEL;
      for (i = 0; i < MaxDimms; i++) {
        //  Bitmask representing dimm #i.
        DimmMask = (UINT16)1 << i;
        //
        if (MemTGetDimmSpdBuffer3 (TechPtr, &SpdBufferPtr, i)) {
          MCTPtr->DimmPresent |= DimmMask;
          //
          //  Check for valid checksum value
          //
          AGESA_TESTPOINT (TpProcMemSPDChecking, &(NBPtr->MemPtr->StdHeader));
          if (SpdBufferPtr[SPD_TYPE] == JED_DDR3SDRAM) {
            ChannelPtr->ChDimmValid |= DimmMask;
            MCTPtr->DimmValid |= DimmMask;
          } else {
            // Current socket is set up to only support DDR3 dimms.
            IDS_ERROR_TRAP;
          }
          if (!MemTCRCCheck3 (SpdBufferPtr) && !SPDCtrl) {
            //
            // NV_SPDCHK_RESTRT is set to 0,
            // cannot ignore faulty SPD checksum
            //
            //  Indicate checksum error
            ChannelPtr->DimmSpdCse |= DimmMask;
            PutEventLog (AGESA_ERROR, MEM_ERROR_CHECKSUM_NV_SPDCHK_RESTRT_ERROR, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
            SetMemError (AGESA_ERROR, MCTPtr);
          }
          //
          //  Check module type information.
          //
          if (SpdBufferPtr[SPD_DIMM_TYPE] == JED_RDIMM || SpdBufferPtr[SPD_DIMM_TYPE] == JED_MINIRDIMM) {
            ChannelPtr->RegDimmPresent |= DimmMask;
            MCTPtr->RegDimmPresent |= DimmMask;
            if (!UserOptions.CfgMemoryRDimmCapable) {
              PutEventLog (AGESA_WARNING, MEM_WARNING_UNSUPPORTED_RDIMM, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
              IDS_ERROR_TRAP;
            }
          }
          if ((SpdBufferPtr[SPD_DIMM_TYPE] == JED_UDIMM) && !UserOptions.CfgMemoryUDimmCapable) {
            PutEventLog (AGESA_WARNING, MEM_WARNING_UNSUPPORTED_UDIMM, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
            IDS_ERROR_TRAP;
          }
          if (SpdBufferPtr[SPD_DIMM_TYPE] == JED_SODIMM) {
            ChannelPtr->SODimmPresent |= DimmMask;
            if (!UserOptions.CfgMemorySODimmCapable) {
              PutEventLog (AGESA_WARNING, MEM_WARNING_UNSUPPORTED_SODIMM, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
              IDS_ERROR_TRAP;
            }
          }
          //
          //  Check error correction type
          //
          if ((SpdBufferPtr[SPD_ECCBITS] & JED_ECC) != 0) {
            MCTPtr->DimmEccPresent |= DimmMask;  //  Dimm has ECC
          }
          //
          //  Get the Dimm width data
          //
          Devwidth = SpdBufferPtr[SPD_DEV_WIDTH] & 0x7;
          switch (Devwidth) {
          case 0:
            ChannelPtr->Dimmx4Present |= DimmMask;
            Devwidth = 4;
            break;
          case 1:
            ChannelPtr->Dimmx8Present |= DimmMask;
            Devwidth = 8;
            break;
          case 2:
            ChannelPtr->Dimmx16Present |= DimmMask;
            Devwidth = 16;
            break;
          default:
            IDS_ERROR_TRAP;
          }
          //
          //  Check for 'analysis probe installed'
          // if (SpdBufferPtr[SPD_ATTRIB] & JED_PROBE_MSK)
          //
          //  Determine the geometry of the DIMM module
          // if (SpdBufferPtr[SPD_DM_BANKS] & SP_DPL_BIT)
          //
          //  specify the number of ranks
          //
          Value8 = ((SpdBufferPtr[SPD_RANKS] >> 3) & 0x07) + 1;
          if (Value8 > 2) {
            if (!UserOptions.CfgMemoryQuadRankCapable) {
              PutEventLog (AGESA_WARNING, MEM_WARNING_UNSUPPORTED_QRDIMM, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
            }
            //
            // Mark this Dimm as Quad Rank
            //
            ChannelPtr->DimmQrPresent |= DimmMask;
            Value8 = 2;
          } else if (Value8 == 2) {
            ChannelPtr->DimmDrPresent |= DimmMask;   //  Dual rank dimms
          } else {
            ChannelPtr->DimmSRPresent |= DimmMask;   //  Single rank dimms
          }
          //
          //  Calculate bus loading per Channel
          if (Devwidth == 16) {
            Devwidth = 4;
          } else if (Devwidth == 4) {
            Devwidth = 16;
          }
          //
          //  double Addr bus load value for dual rank DIMMs
          //
          if (Value8 == 2) {
            Devwidth = Devwidth << 1;
          }
          //
          ChannelPtr->Ranks = ChannelPtr->Ranks + Value8;
          ChannelPtr->Loads = ChannelPtr->Loads + Devwidth;
          if ((i < 2) || ((ChannelPtr->DimmQrPresent & DimmMask) == 0)) {
            ChannelPtr->Dimms++;
          }
          //
          // Check address mirror support for Unbuffered Dimms only
          //
          if ((ChannelPtr->RegDimmPresent & DimmMask) == 0) {
            if ((SpdBufferPtr[SPD_ADDRMAP] & 1) != 0) {
              ChannelPtr->DimmMirrorPresent |= DimmMask;
            }
          }
          //
          // Get byte62: Reference Raw Card information
          //
          ChannelPtr->RefRawCard[i] = SpdBufferPtr[SPD_RAWCARD] & 0x1F;
          //
          // Get control word values for RC3, RC4 and RC5
          //
          ChannelPtr->CtrlWrd03[i] = SpdBufferPtr[SPD_CTLWRD03] >> 4;
          ChannelPtr->CtrlWrd04[i] = SpdBufferPtr[SPD_CTLWRD04] & 0x0F;
          ChannelPtr->CtrlWrd05[i] = SpdBufferPtr[SPD_CTLWRD05] >> 4;
          //
          // Temporarily store info. of SPD byte 63 into CtrlWrd02(s),
          // and they will be used late to calculate real RC2 and RC8 value
          //
          ChannelPtr->CtrlWrd02[i] = SpdBufferPtr[SPD_ADDRMAP] & 0x03;
          //
        } // if DIMM present
      } // Dimm loop

      if (Channel == 0) {
        DCTPtr->Timings.DctDimmValid = ChannelPtr->ChDimmValid;
        DCTPtr->Timings.DimmMirrorPresent = ChannelPtr->DimmMirrorPresent;
        DCTPtr->Timings.DimmSpdCse = ChannelPtr->DimmSpdCse;
        DCTPtr->Timings.DimmQrPresent = ChannelPtr->DimmQrPresent;
        DCTPtr->Timings.DimmDrPresent = ChannelPtr->DimmDrPresent;
        DCTPtr->Timings.DimmSRPresent = ChannelPtr->DimmSRPresent;
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
      printk(BIOS_DEBUG, "  RegDimmPresent:    %x\n", ChannelPtr->RegDimmPresent);
      printk(BIOS_DEBUG, "  SODimmPresent:     %x\n", ChannelPtr->SODimmPresent);
      printk(BIOS_DEBUG, "  ChDimmValid:       %x\n", ChannelPtr->ChDimmValid);
      printk(BIOS_DEBUG, "  DimmPlPresent:     %x\n", ChannelPtr->DimmPlPresent);
      printk(BIOS_DEBUG, "  DimmQrPresent:     %x\n", ChannelPtr->DimmQrPresent);
      printk(BIOS_DEBUG, "  DimmDrPresent:     %x\n", ChannelPtr->DimmDrPresent);
      printk(BIOS_DEBUG, "  DimmSRPresent:     %x\n", ChannelPtr->DimmSRPresent);
      printk(BIOS_DEBUG, "  Dimmx4Present:     %x\n", ChannelPtr->Dimmx4Present);
      printk(BIOS_DEBUG, "  DimmX8Present:     %x\n", ChannelPtr->Dimmx8Present);
      printk(BIOS_DEBUG, "  DimmX16Present:    %x\n", ChannelPtr->Dimmx16Present);
      printk(BIOS_DEBUG, "  DimmMirrorPresent: %x\n", ChannelPtr->DimmMirrorPresent);
    } // Channel loop
  } // DCT loop

  //  If we have DIMMs, some further general characteristics checking
  if (MCTPtr->DimmValid != 0) {
    //  If there are registered dimms, all the dimms must be registered
    if (MCTPtr->RegDimmPresent == MCTPtr->DimmValid) {
      //  All dimms registered
      MCTPtr->Status[SbRegistered] = TRUE;
      MCTPtr->Status[SbParDimms] = TRUE;  //  All DDR3 RDIMMs are parity capable
      TechPtr->SetDqsEccTmgs = MemTSetDQSEccTmgsRDdr3; // Change the function pointer for DQS ECC timing
    } else if (MCTPtr->RegDimmPresent != 0) {
      //  We have an illegal DIMM mismatch
      PutEventLog (AGESA_FATAL, MEM_ERROR_MODULE_TYPE_MISMATCH_DIMM, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_FATAL, MCTPtr);
    }

    //  check the ECC capability of the DIMMs
    if (MCTPtr->DimmEccPresent == MCTPtr->DimmValid) {
      MCTPtr->Status[SbEccDimms] = TRUE;  //  All dimms ECC capable
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
 *   This function finds the maximum frequency that each channel is capable to run at.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE  - indicates that a FATAL error has not occurred
 *     @return  FALSE - indicates that a FATAL error has occurred
 */

BOOLEAN
MemTSPDGetTargetSpeed3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 *SpdBufferPtr;
  UINT8 Dimm;
  UINT8 Dct;
  UINT8 Channel;
  UINT16 MTB16x;
  UINT16 TCKmin16x;
  UINT16 Value16;
  MEM_NB_BLOCK *NBPtr;
  DCT_STRUCT *DCTPtr;
  CH_DEF_STRUCT *ChannelPtr;

  NBPtr = TechPtr->NBPtr;
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    DCTPtr = NBPtr->DCTPtr;
    TCKmin16x = 0;
    for (Channel = 0; Channel < NBPtr->ChannelCount; Channel++) {
      NBPtr->SwitchChannel (NBPtr, Channel);
      ChannelPtr = NBPtr->ChannelPtr;
      for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm++) {
        if ((ChannelPtr->ChDimmValid & ((UINT8)1 << Dimm)) != 0) {
          MemTGetDimmSpdBuffer3 (TechPtr, &SpdBufferPtr, Dimm);

          // Determine tCKmin(all) which is the largest tCKmin
          // value for all modules on the memory Channel (SPD byte 12).
          //
          MTB16x = ((UINT16)SpdBufferPtr[SPD_DIVIDENT] << 4) / SpdBufferPtr[SPD_DIVISOR];
          Value16 = SpdBufferPtr[SPD_TCK] * MTB16x;
          if (TCKmin16x < Value16) {
            TCKmin16x = Value16;
          }
        }
      }
    }
    if (TCKmin16x <= 17) {
      DCTPtr->Timings.TargetSpeed = DDR1866_FREQUENCY;
    } else if (TCKmin16x <= 20) {
      DCTPtr->Timings.TargetSpeed = DDR1600_FREQUENCY;
    } else if (TCKmin16x <= 24) {
      DCTPtr->Timings.TargetSpeed = DDR1333_FREQUENCY;
    } else if (TCKmin16x <= 30) {
      DCTPtr->Timings.TargetSpeed = DDR1066_FREQUENCY;
    } else if (TCKmin16x <= 40) {
      DCTPtr->Timings.TargetSpeed = DDR800_FREQUENCY;
    } else {
      DCTPtr->Timings.TargetSpeed = DDR667_FREQUENCY;
    }
  }

  // Ensure the target speed can be applied to all channels of the current node
  NBPtr->SyncTargetSpeed (NBPtr);

  // Set the start-up frequency
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    NBPtr->DCTPtr->Timings.Speed = TechPtr->NBPtr->StartupSpeed;
  }
  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
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
 *     @return  TRUE  - indicates that a FATAL error has not occurred
 *     @return  FALSE - indicates that a FATAL error has occurred
 */

BOOLEAN
MemTSPDCalcWidth3 (
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
  // Does not support ganged mode for DDR3 dimms
  ASSERT (UngangMode);
  IDS_OPTION_HOOK (IDS_GANGING_MODE, &UngangMode, &(NBPtr->MemPtr->StdHeader));

  //  Check symmetry of channel A and channel B dimms for 128-bit mode
  //  capability.
  //
  AGESA_TESTPOINT (TpProcMemModeChecking, &(NBPtr->MemPtr->StdHeader));
  i = 0;
  if (!UngangMode) {
    if (MCTPtr->DctData[0].Timings.DctDimmValid == MCTPtr->DctData[1].Timings.DctDimmValid) {
      for (; i < MAX_DIMMS_PER_CHANNEL; i++) {
        DimmMask = (UINT16)1 << i;
        if ((DCTPtr->Timings.DctDimmValid & DimmMask) != 0) {
          NBPtr->SwitchDCT (NBPtr, 0);
          MemTGetDimmSpdBuffer3 (TechPtr, &SpdBufferAPtr, i);
          NBPtr->SwitchDCT (NBPtr, 1);
          MemTGetDimmSpdBuffer3 (TechPtr, &SpdBufferBPtr, i);
          // compare rows and columns
          if ((SpdBufferAPtr[SPD_ROW_SZ]&0x3F) != (SpdBufferBPtr[SPD_ROW_SZ]&0x3F)) {
            break;
          }
          if ((SpdBufferAPtr[SPD_DENSITY]&0x0F) != (SpdBufferBPtr[SPD_DENSITY]&0x0F)) {
            break;
          }
          // compare ranks and devwidth
          if ((SpdBufferAPtr[SPD_DEV_WIDTH]&0x7F) != (SpdBufferBPtr[SPD_DEV_WIDTH]&0x7F)) {
            break;
          }
        }
      }
    }
    if (i < MAX_DIMMS_PER_CHANNEL) {
      PutEventLog (AGESA_ALERT, MEM_ALERT_ORG_MISMATCH_DIMM, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_ALERT, MCTPtr);
    } else {
      NBPtr->Ganged = TRUE;
      MCTPtr->GangedMode = TRUE;
      MCTPtr->Status[Sb128bitmode] = TRUE;
      NBPtr->SetBitField (NBPtr, BFDctGangEn, 1);
    }
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
 *     @return  TRUE  - indicates that a FATAL error has not occurred
 *     @return  FALSE - indicates that a FATAL error has occurred
 */

BOOLEAN
MemTAutoCycTiming3 (
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
    SPD_TWTR,
    SPD_TFAW
  };

  UINT8  *SpdBufferPtr;
  UINT16 MiniMaxTmg[GET_SIZE_OF (SpdIndexes)];
  UINT8  MiniMaxTrfc[4];

  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  MEM_NB_BLOCK *NBPtr;
  UINT16 DimmMask;
  UINT16 Value16;
  UINT16 MTB16x;
  UINT16 TCK16x;
  UINT8 i;
  UINT8 j;
  UINT8 Value8;
  UINT8  *StatTmgPtr;
  UINT16 *StatDimmTmgPtr;

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
    if ((DCTPtr->Timings.DctDimmValid & DimmMask) != 0) {
      MemTGetDimmSpdBuffer3 (TechPtr, &SpdBufferPtr, i);
      MTB16x = ((UINT16)SpdBufferPtr[SPD_DIVIDENT] << 4) / SpdBufferPtr[SPD_DIVISOR];

      for (j = 0; j < GET_SIZE_OF (SpdIndexes); j++) {
        Value16 = (UINT16)SpdBufferPtr[SpdIndexes[j]];
        if (SpdIndexes[j] == SPD_TRC) {
          Value16 |= ((UINT16)SpdBufferPtr[SPD_UPPER_TRC] & 0xF0) << 4;
        } else if (SpdIndexes[j] == SPD_TRAS) {
          Value16 |= ((UINT16)SpdBufferPtr[SPD_UPPER_TRAS] & 0x0F) << 8;
        } else if (SpdIndexes[j] == SPD_TFAW) {
          Value16 |= ((UINT16)SpdBufferPtr[SPD_UPPER_TFAW] & 0x0F) << 8;
        }
        Value16 = Value16 * MTB16x;
        if (MiniMaxTmg[j] < Value16) {
          MiniMaxTmg[j] = Value16;
        }
      }

      //  get Trfc0 - Trfc3 values
      Value8 = SpdBufferPtr[SPD_DENSITY] & 0x0F;
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
  TCK16x = (16000 + 16) / DCTPtr->Timings.Speed;   // Offset of 16 is used to round to the nearest integer

  //   Notes:
  //      1. All secondary time values given in SPDs are in binary with UINTs of ns.
  //      2. All time values are scaled by 16, in order to have least count of 0.125 ns
  //         (more accuracy).
  //      3. Internally to this SW, cycle time, TCK16x, is scaled by 16 to match time values
  //
  StatDimmTmgPtr = &DCTPtr->Timings.DIMMTrcd;
  StatTmgPtr = &DCTPtr->Timings.Trcd;
  for (j = 0; j < GET_SIZE_OF (SpdIndexes); j++) {
    Value16 = MiniMaxTmg[j];

    MiniMaxTmg[j] = (MiniMaxTmg[j] + TCK16x - 1) / TCK16x;

    StatDimmTmgPtr[j] = Value16;
    StatTmgPtr[j] = (UINT8)MiniMaxTmg[j];
  }
  DCTPtr->Timings.Trfc0 = MiniMaxTrfc[0];
  DCTPtr->Timings.Trfc1 = MiniMaxTrfc[1];
  DCTPtr->Timings.Trfc2 = MiniMaxTrfc[2];
  DCTPtr->Timings.Trfc3 = MiniMaxTrfc[3];

  DCTPtr->Timings.CasL = MemTSPDGetTCL3 (TechPtr);

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
 *     @return  TRUE  - indicates that a FATAL error has not occurred
 *     @return  FALSE - indicates that a FATAL error has occurred
 */

BOOLEAN
MemTSPDSetBanks3 (
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
    if ((DCTPtr->Timings.DimmSpdCse & ((UINT16) 1 << DimmID)) != 0) {
      CSSpdCSE |= (UINT16) ((DimmQRDR & (UINT16) 1 << DimmID) ? 3 : 1) << ChipSel;
    }
    if ((DCTPtr->Timings.DimmExclude & ((UINT16) 1 << DimmID)) != 0) {
      CSExclude |= (UINT16) ((DimmQRDR & (UINT16) 1 << DimmID) ? 3: 1) << ChipSel;
    }

    if ((DCTPtr->Timings.DctDimmValid & ((UINT16)1 << DimmID)) != 0) {
      MemTGetDimmSpdBuffer3 (TechPtr, &SpdBufferPtr, DimmID);

      //  Get the basic data
      Rows = (SpdBufferPtr[SPD_ROW_SZ] >> 3) & 0x7;
      Cols = SpdBufferPtr[SPD_COL_SZ] & 0x7;
      Banks = (SpdBufferPtr[SPD_L_BANKS] >> 4) & 0x7;
      Ranks = ((SpdBufferPtr[SPD_RANKS] >> 3) & 0x07) + 1;

      //
      // Configure the bank encoding
      // Use a 6-bit key into a lookup table.
      // Key (index) = RRRBCC, where CC is the number of Columns minus 9,
      // RRR is the number of Rows minus 12, and B is the number of banks
      // minus 3.
      //
      Value8 = Cols;
      Value8 |= (Banks == 1) ? 4 : 0;
      Value8 |= Rows << 3;

      if (MemTCheckBankAddr3 (Value8, &i)) {
        BankAddrReg |= ((UINT32)i << (ChipSel << 1));

        // Mask value=(2pow(rows+cols+banks+3)-1)>>8,
        // or 2pow(rows+cols+banks-5)-1
        //
        Value8 = (Rows + 12) + (Cols + 9) + (Banks + 3) + 3 - 8;
        if (MCTPtr->Status[Sb128bitmode]) {
          Value8++;
        }
        CsMask = ((UINT32)1 << Value8) - 1;
        DCTPtr->Timings.CsPresent |= (UINT16)1 << ChipSel;

        if (Ranks >= 2) {
          DCTPtr->Timings.CsPresent |= (UINT16)1 << (ChipSel + 1);
        }

        //  Update the DRAM CS Mask for this chipselect
        NBPtr->SetBitField (NBPtr, BFCSMask0Reg + (ChipSel >> 1), (CsMask & 0x1FF83FE0));
      } else {
        // Dimm is not supported, as no address mapping is found.
        DCTPtr->Timings.CsPresent |= (UINT16)1 << ChipSel;
        DCTPtr->Timings.CsTestFail |= (UINT16)1 << ChipSel;
        if (Ranks >= 2) {
          DCTPtr->Timings.CsPresent |= (UINT16)1 << (ChipSel + 1);
          DCTPtr->Timings.CsTestFail |= (UINT16)1 << (ChipSel + 1);
        }
        PutEventLog (AGESA_ERROR, MEM_ERROR_NO_ADDRESS_MAPPING, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, DimmID, &NBPtr->MemPtr->StdHeader);
        SetMemError (AGESA_ERROR, MCTPtr);
      }
    }
  }
  // For ranks that need to be excluded, the loading of this rank should be considered
  // in timing, so need to set CsPresent before setting CsTestFail
  if ((CSSpdCSE != 0) || (CSExclude != 0)) {
    NBPtr->MemPtr->ErrorHandling (MCTPtr, NBPtr->Dct, (CSSpdCSE | CSExclude), &NBPtr->MemPtr->StdHeader);
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
MemTGetCSIntLvAddr3 (
  IN       UINT8 BankEnc,
     OUT   UINT8 *LowBit,
     OUT   UINT8 *HiBit
  )
{
  CONST UINT8 ArrCodesLo[] = {0, 8, 8, 0, 0, 8, 9, 8, 9, 9, 8, 9};
  CONST UINT8 ArrCodesHi[] = {0, 20, 21, 0, 0, 22, 22, 23, 23, 24, 24, 25};
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
 *       This function determines if the checksum is correct
 *
 *     @param[in]   *SPDPtr - Pointer to SPD data
 *
 *     @return    TRUE  - CRC check passes
 *     @return    FALSE - CRC check fails
 */

BOOLEAN
STATIC
MemTCRCCheck3 (
  IN OUT   UINT8 *SPDPtr
  )
{
  UINT16 Crc;
  INT16 i;
  INT16 j;
  INT16 Count;

  if (SPDPtr[SPD_TYPE] == JED_DDR3SDRAM) {
    Count = (SPDPtr[SPD_BYTE_USED] & 0x80) ? 117 : 126;
    Crc = 0;
    for (j = 0; j < Count; j++) {
      Crc = Crc ^ ((UINT16)SPDPtr[j] << 8);
      for (i = 0; i < 8; i++) {
        if (Crc & 0x8000) {
          Crc = (Crc << 1) ^ 0x1021;
        } else {
          Crc = (Crc << 1);
        }
      }
    }
    if (*(UINT16 *) (SPDPtr + 126) == Crc) {
      return TRUE;
    }
  }

  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function returns the CAS latency of the current frequency (DCTPtr->Timings.Speed).
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  CAS Latency
 */

UINT8
STATIC
MemTSPDGetTCL3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8  *SpdBufferPtr;
  UINT8 CLdesired;
  UINT8 CLactual;
  UINT8 Dimm;
  UINT8 Channel;
  UINT16 CASLat;
  UINT16 MTB16x;
  UINT16 TAAmin16x;
  UINT16 TCKproposed16x;
  UINT16 Value16;
  UINT16 Mask16;
  BOOLEAN CltFail;
  MEM_NB_BLOCK *NBPtr;
  DCT_STRUCT *DCTPtr;
  CH_DEF_STRUCT *ChannelPtr;

  NBPtr = TechPtr->NBPtr;
  DCTPtr = NBPtr->DCTPtr;

  CASLat = 0xFFFF;
  TAAmin16x = 0;
  CltFail = FALSE;

  for (Channel = 0; Channel < NBPtr->ChannelCount; Channel++) {
    NBPtr->SwitchChannel (NBPtr, Channel);
    ChannelPtr = NBPtr->ChannelPtr;
    for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm++) {
      if ((ChannelPtr->ChDimmValid & ((UINT8)1 << Dimm)) != 0) {
        MemTGetDimmSpdBuffer3 (TechPtr, &SpdBufferPtr, Dimm);

        // Step 1: Determine the common set of supported CAS Latency
        // values for all modules on the memory Channel using the CAS
        // Latencies Supported in SPD bytes 14 and 15.
        //
        CASLat &= ((UINT16)SpdBufferPtr[SPD_CASHI] << 8) | SpdBufferPtr[SPD_CASLO];

        // Step 2: Determine tAAmin(all) which is the largest tAAmin
        // value for all modules on the memory Channel (SPD byte 16).
        //
        MTB16x = ((UINT16)SpdBufferPtr[SPD_DIVIDENT] << 4) / SpdBufferPtr[SPD_DIVISOR];
        Value16 = SpdBufferPtr[SPD_TAA] * MTB16x;
        if (TAAmin16x < Value16) {
          TAAmin16x = Value16;
        }

        // Step 3: Determine tCKmin(all) which is the largest tCKmin
        // value for all modules on the memory Channel (SPD byte 12).
        // * This step has been done in SPDGetTargetSpeed
      }
    }
  }

  TCKproposed16x = (16000 + 16) / DCTPtr->Timings.Speed;   // Offset of 16 is used to round to the nearest integer

  // Step 4: For a proposed tCK value (tCKproposed) between tCKmin(all) and tCKmax,
  // determine the desired CAS Latency. If tCKproposed is not a standard JEDEC
  // value (2.5, 1.875, 1.5, or 1.25 ns) then tCKproposed must be adjusted to the
  // next lower standard tCK value for calculating CLdesired.
  // CLdesired = ceiling ( tAAmin(all) / tCKproposed )
  // where tAAmin is defined in Byte 16. The ceiling function requires that the
  // quotient be rounded up always.
  //
  CLdesired = (UINT8) ((TAAmin16x + TCKproposed16x - 1) / TCKproposed16x);

  // Step 5: Choose an actual CAS Latency (CLactual) that is greater than or equal
  // to CLdesired and is supported by all modules on the memory Channel as
  // determined in step 1. If no such value exists, choose a higher tCKproposed
  // value and repeat steps 4 and 5 until a solution is found.
  //
  CLactual = 4;
  for (Mask16 = 1; Mask16 < 0x8000; Mask16 <<= 1) {
    if (CASLat & Mask16) {
      if (CLdesired <= CLactual) {
        break;
      }
    }
    CLactual++;
  }
  if (Mask16 == 0x8000) {
    CltFail = TRUE;
  }

  // Step 6: Once the calculation of CLactual is completed, the BIOS must also
  // verify that this CAS Latency value does not exceed tAAmax, which is 20 ns
  // for all DDR3 speed grades, by multiplying CLactual times tCKproposed. If
  // not, choose a lower CL value and repeat steps 5 and 6 until a solution is found.
  //
  if ((TCKproposed16x * CLactual) > 320) {
    CltFail = TRUE;
  }

  if (!CltFail) {
    DCTPtr->Timings.CasL = CLactual;
  } else {
    //  Fail to find supported Tcl, use 6 clocks since it is required for all DDR3 speed bin.
    DCTPtr->Timings.CasL = 6;
  }

  return DCTPtr->Timings.CasL;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function returns the encoded value of bank address.
 *
 *     @param[in]   Encode - RRRBCC, where CC is the number of Columns minus 9,
 *                           RRR is the number of Rows minus 12, and B is the number of banks
 *                           minus 3.
 *     @param[out]  *Index - index in bank address table
 *     @return      TRUE   - encoded value is found.
 *                  FALSE  - encoded value is not found.
 */

BOOLEAN
STATIC
MemTCheckBankAddr3 (
  IN       UINT8 Encode,
     OUT   UINT8 *Index
  )
{
  UINT8 i;
  CONST UINT8 TabBankAddr[]  = {
    0x3F, 0x01, 0x09, 0x3F, 0x3F, 0x11,
    0x0A, 0x19, 0x12, 0x1A, 0x21, 0x22
  };

  for (i = 0; i < GET_SIZE_OF (TabBankAddr); i++) {
    if (Encode == TabBankAddr[i]) {
      *Index = i;
      return TRUE;
    }
  }
  return FALSE;
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
MemTGetDimmSpdBuffer3 (
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

