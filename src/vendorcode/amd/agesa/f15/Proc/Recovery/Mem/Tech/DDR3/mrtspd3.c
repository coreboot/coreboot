/* $NoKeywords:$ */
/**
 * @file
 *
 * mrtspd3.c
 *
 * Technology SPD supporting functions for DDR3 Recovery
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem)
 * @e \$Revision: 44324 $ @e \$Date: 2010-12-22 02:16:51 -0700 (Wed, 22 Dec 2010) $
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
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mrtspd3.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_RECOVERY_MEM_TECH_DDR3_MRTSPD3_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define _UNDEF_ 0xFF
#define MAX_DIES_PER_SOCKET     2   ///< Set to largest of any CPU

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
/**
 *
 *   This function sets the DRAM mode
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - indicates that the DRAM mode is set to DDR2
 */

BOOLEAN
MemRecTSetDramMode3 (
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
 *     @return  TRUE - indicates that a FATAL error has not occurred
 *     @return  FALL - indicates that a FATAL error has not occurred
 */

BOOLEAN
MemRecTDIMMPresence3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 Node;
  UINT8 Dct;
  UINT8 Channel;
  UINT8 i;
  SPD_DEF_STRUCT *SPDPtr;
  UINT8 *SpdBufferPtr;
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  CH_DEF_STRUCT *ChannelPtr;
  MEM_NB_BLOCK *NBPtr;
  UINT16 MaxDimms;
  UINT16 Value16;
  UINT8 Devwidth;
  UINT8 Value8;
  UINT16 DimmMask;
  UINT8 VoltageMap;
  UINT8 VDDByte;
  UINT32 V1_2XDimmMap;
  UINT32 V1_35DimmMap;
  UINT32 V1_5DimmMap;

  NBPtr = TechPtr->NBPtr;
  MCTPtr = NBPtr->MCTPtr;
  VoltageMap = 0xFF;
  V1_2XDimmMap = 0;
  V1_35DimmMap = 0;
  V1_5DimmMap = 0;

  NBPtr->DimmToBeUsed = _UNDEF_;
  for (Node = 0; Node < NBPtr->MemPtr->DieCount; Node++) {
    NBPtr->SwitchNodeRec (NBPtr, Node);
    for (Dct = 0; Dct < NBPtr->MCTPtr->DctCount; Dct++) {
      NBPtr->SwitchDCT (NBPtr, Dct);
      DCTPtr = NBPtr->DCTPtr;
      for (Channel = 0; Channel < DCTPtr->ChannelCount; Channel++) {
        NBPtr->SwitchChannel (NBPtr, Channel);
        ChannelPtr = NBPtr->ChannelPtr;
        SPDPtr = NBPtr->SPDPtr;

        // Get the maximum number of DIMMs
        MaxDimms = MAX_DIMMS_PER_CHANNEL;
        for (i = 0; i < MaxDimms; i++) {
          //  Bitmask representing dimm #i.
          DimmMask = (UINT16) 1 << i;

          if (SPDPtr[i].DimmPresent) {
            SpdBufferPtr = (UINT8 *)&(SPDPtr[i].Data);

            MCTPtr->DimmPresent |= DimmMask;
            if (SpdBufferPtr[SPD_TYPE] == JED_DDR3SDRAM) {
              ChannelPtr->ChDimmValid |= DimmMask;
            }

            //  Check module type information.
            if (SpdBufferPtr[SPD_DIMM_TYPE] == JED_RDIMM || SpdBufferPtr[SPD_DIMM_TYPE] == JED_MINIRDIMM) {
              ChannelPtr->RegDimmPresent |= DimmMask;
            }

            if (SpdBufferPtr[SPD_DIMM_TYPE] == JED_SODIMM) {
              ChannelPtr->SODimmPresent |= DimmMask;
            }

            //  Get the Dimm width data
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

            //  Determine the page size.
            //       page_size = 2^COLBITS * Devwidth/8
            //
            Value16 = (((UINT16) 1 << (SpdBufferPtr[SPD_COL_SZ]&7)) * Devwidth) / 8;
            if ((Value16 >> 11) == 0) {
              DCTPtr->Timings.DIMM1KPage |= DimmMask;
            }

            //  Calculate bus loading per Channel
            if (Devwidth == 16) {
              Devwidth = 4;
            } else if (Devwidth == 4) {
              Devwidth = 16;
            }

            //  specify the number of ranks
            Value8 = ((SpdBufferPtr[SPD_RANKS] >> 3) & 0x07) + 1;
            if (Value8 > 2) {
              ChannelPtr->DimmQrPresent |= DimmMask;
              Devwidth = Devwidth << 2;
            } else if (Value8 == 2) {
              ChannelPtr->DimmDrPresent |= DimmMask;   //  Dual rank dimms
              Devwidth = Devwidth << 1;
            } else {
              ChannelPtr->DimmSRPresent |= DimmMask;
            }

            ChannelPtr->Ranks = ChannelPtr->Ranks + Value8;
            ChannelPtr->Loads = ChannelPtr->Loads + Devwidth;
            ChannelPtr->Dimms++;

            // Check address mirror support for Unbuffered Dimms only
            if ((ChannelPtr->RegDimmPresent & DimmMask) == 0) {
              if ((SpdBufferPtr[SPD_ADDRMAP] & 1) != 0) {
                ChannelPtr->DimmMirrorPresent |= DimmMask;
              }
            }

            // Get control word values for RC3, RC4 and RC5
            ChannelPtr->CtrlWrd03[i] = SpdBufferPtr[SPD_CTLWRD03] >> 4;
            ChannelPtr->CtrlWrd04[i] = SpdBufferPtr[SPD_CTLWRD04] & 0x0F;
            ChannelPtr->CtrlWrd05[i] = SpdBufferPtr[SPD_CTLWRD05] >> 4;
            //
            // Temporarily store info. of SPD byte 63 into CtrlWrd02(s),
            // and they will be used late to calculate real RC2 and RC8 value
            //
            ChannelPtr->CtrlWrd02[i] = SpdBufferPtr[SPD_ADDRMAP] & 0x03;

            // Get the common voltage if possible and create the individual Dimm maps per voltage
            VDDByte = SpdBufferPtr[SPD_MNVVDD];
            VDDByte ^= 1;
            VoltageMap &= VDDByte;
            //
            // Create the Dimms map
            //
            // Node:            1         0
            // Dct:          1    0    1    0
            // Dimm:       3210 3210 3210 3210
            // Dimmbitmap: xxxx xxxx xxxx xxxx
            // Ex.         0000 0001 0010 0000 (V1_2XDimmMap)
            //             This indicates Node0/Dct1/Dimm1 and Node1/Dct0/Dimm0 are 1.2XV supported.
            if ((VDDByte & (UINT8) (1 << VOLT1_25)) != 0) {
              V1_2XDimmMap |= (UINT32) DimmMask << ((Node * NBPtr->MCTPtr->DctCount + Dct) * MAX_DIMMS_PER_CHANNEL);
            } else if ((VDDByte & (UINT8) (1 << VOLT1_35)) != 0) {
              V1_35DimmMap |= (UINT32) DimmMask << ((Node * NBPtr->MCTPtr->DctCount + Dct) * MAX_DIMMS_PER_CHANNEL);
            } else {
              V1_5DimmMap |= (UINT32) DimmMask << ((Node * NBPtr->MCTPtr->DctCount + Dct) * MAX_DIMMS_PER_CHANNEL);
            }
          } // if DIMM present
        } // Dimm loop
      } // Channel loop
    } // DCT loop
  }

  if (VoltageMap != 0xFF) {
    if (VoltageMap == 0) {
      NBPtr->RefPtr->DDR3Voltage = VOLT1_35;
      if (V1_35DimmMap != 0) {
        i = (UINT8) LibAmdBitScanForward (V1_35DimmMap);
      } else {
        i = (UINT8) LibAmdBitScanForward (V1_2XDimmMap);
      }
    } else {
      NBPtr->RefPtr->DDR3Voltage = CONVERT_ENCODED_TO_VDDIO (LibAmdBitScanReverse (VoltageMap));
      i = (UINT8) LibAmdBitScanForward (V1_2XDimmMap | V1_35DimmMap | V1_5DimmMap);
      // In case of 1.35V Dimms and 1.5V Dimms mixture, we initialize the 1.35V Dimm.
      if ((V1_35DimmMap != 0) && (V1_5DimmMap != 0)) {
        NBPtr->RefPtr->DDR3Voltage = VOLT1_35;
        i = (UINT8) LibAmdBitScanForward (V1_35DimmMap);
      }
    }
    // Find out which Dimm we are going to initialize and which Node/Dct it belongs to
    NBPtr->DimmToBeUsed = i % MAX_DIMMS_PER_CHANNEL;
    Node = i / (NBPtr->MCTPtr->DctCount * MAX_DIMMS_PER_CHANNEL);
    Dct = (i / MAX_DIMMS_PER_CHANNEL) & (NBPtr->MCTPtr->DctCount - 1);
    NBPtr->SwitchNodeRec (NBPtr, Node);
    NBPtr->SwitchDCT (NBPtr, Dct);
  }

  //  If we have DIMMs, some further general characteristics checking
  if (NBPtr->DimmToBeUsed == _UNDEF_) {
    //  Leave with an error - no dimms on this DCT
    // LibAmdEventLog (AGESA_FATAL, MEM_ERROR_NO_DIMM_FOUND, 0, NBPtr->Dct, NBPtr->Channel, 0);   //@attention commented out since it is not defined in recovery code
    SetMemRecError (AGESA_FATAL, MCTPtr);
  }

  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
