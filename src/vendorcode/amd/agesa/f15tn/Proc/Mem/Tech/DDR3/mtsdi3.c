/* $NoKeywords:$ */
/**
 * @file
 *
 * mtsdi3.c
 *
 * Technology Software DRAM Init for DDR3
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech/DDR3)
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
#include "mu.h"
#include "mt.h"
#include "mt3.h"
#include "mtsdi3.h"
#include "mtrci3.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)
#define FILECODE PROC_MEM_TECH_DDR3_MTSDI3_FILECODE
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

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */



/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function initiates software DRAM init for  both DCTs
 *   at the same time.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

BOOLEAN
MemTDramInitSw3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 Dct;
  UINT8 ChipSel;

  MEM_DATA_STRUCT *MemPtr;
  DIE_STRUCT *MCTPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;
  MCTPtr = NBPtr->MCTPtr;

  IDS_HDT_CONSOLE (MEM_STATUS, "\nStart Dram Init\n");
  // 3.Program F2x[1,0]7C[EnDramInit]=1
  IDS_HDT_CONSOLE (MEM_FLOW, "\tEnDramInit = 1 for both DCTs\n");
  NBPtr->BrdcstSet (NBPtr, BFEnDramInit, 1);
  NBPtr->PollBitField (NBPtr, BFDctAccessDone, 1, PCI_ACCESS_TIMEOUT, TRUE);

  // 4.wait 200us
  MemUWait10ns (20000, MemPtr);

  // 5.Program F2x[1, 0]7C[DeassertMemRstX] = 1.
  NBPtr->BrdcstSet (NBPtr, BFDeassertMemRstX, 1);

  // 6.wait 500us
  MemUWait10ns (50000, MemPtr);

  // Do Phy Fence training before sending MRS commands
  if (!NBPtr->IsSupported[FenceTrnBeforeDramInit]) {
    AGESA_TESTPOINT (TpProcMemPhyFenceTraining, &(NBPtr->MemPtr->StdHeader));
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      NBPtr->SwitchDCT (NBPtr, Dct);
      if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
        IDS_HDT_CONSOLE (MEM_STATUS, "\tDct %d\n", Dct);
        NBPtr->PhyFenceTraining (NBPtr);
      }
    }
  }

  // 7.NOP or deselect & take CKE high
  NBPtr->BrdcstSet (NBPtr, BFAssertCke, 1);

  // 8.wait 360ns
  MemUWait10ns (36, MemPtr);

  // The following steps are performed once for each channel with unbuffered DIMMs
  // and once for each chip select on registered DIMMs:
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      IDS_HDT_CONSOLE (MEM_STATUS, "\tDct %d\n", Dct);

      // Enable Dram Parity if appropriate.
      NBPtr->FamilySpecificHook[EnableParityAfterMemRst] (NBPtr, NULL);

      // The following steps are performed with registered DIMMs only and
      // must be done for each chip select pair:
      if (MCTPtr->Status[SbRegistered]) {
        MemTDramControlRegInit3 (TechPtr);
      }

      // Initialize LRDIMM's register
      TechPtr->TechnologySpecificHook[LrdimmControlRegInit] (TechPtr, NULL);

      for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL; ChipSel++) {
        if ((NBPtr->DCTPtr->Timings.CsPresent & ((UINT16)1 << ChipSel)) != 0) {
          IDS_HDT_CONSOLE (MEM_STATUS, "\t\tCS %d\n", ChipSel);
          // if chip select present
          if (!(TechPtr->TechnologySpecificHook[LrdimmSendAllMRCmds] (TechPtr, &ChipSel))) {
            MemTSendAllMRCmds3 (TechPtr, ChipSel);
          }
          // NOTE: wait 512 clocks for DLL-relock
          MemUWait10ns (50000, NBPtr->MemPtr);  // wait 500us
          if (!(MCTPtr->Status[SbRegistered] || MCTPtr->Status[SbLrdimms])) {
            break;
          }
        }
      }

      // 17.Send two ZQCL commands (to even then odd chip select)
      NBPtr->sendZQCmd (NBPtr);
      NBPtr->sendZQCmd (NBPtr);
    }
  }

  // 18.Program F2x[1,0]7C[EnDramInit]=0
  NBPtr->BrdcstSet (NBPtr, BFEnDramInit, 0);
  NBPtr->PollBitField (NBPtr, BFDctAccessDone, 1, PCI_ACCESS_TIMEOUT, TRUE);
  //
  // For Unbuffered Dimms, Issue MRS for remaining CS without EnDramInit
  //
  NBPtr->FamilySpecificHook[SendMrsCmdsPerCs] (NBPtr, NBPtr);

  IDS_HDT_CONSOLE (MEM_FLOW, "End Dram Init\n\n");
  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function calculates the EMRS1 value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]        Wl - Indicates if WL mode should be enabled
 *     @param[in]        TargetDIMM - DIMM target for WL
 */

VOID
MemTEMRS13 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       BOOLEAN Wl,
  IN       UINT8 TargetDIMM
  )
{
  UINT16 MrsAddress;
  UINT8 MaxDimmPerCH;
  UINT8 ChipSel;
  UINT8  Value8;

  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MaxDimmPerCH = GetMaxDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration,
                                        NBPtr->MCTPtr->SocketId,
                                        NBPtr->ChannelPtr->ChannelID);
  ChipSel = (UINT8) (0x0FF & NBPtr->GetBitField (NBPtr, BFMrsChipSel));

  // BA2=0,BA1=0,BA0=1
  NBPtr->SetBitField (NBPtr, BFMrsBank, 1);

  MrsAddress = 0;

  // program MrsAddress[5,1]=output driver impedance control (DIC):
  // based on F2x[1,0]84[DrvImpCtrl]
  if (!(NBPtr->IsSupported[CheckDrvImpCtrl])) {
    Value8 = (UINT8)NBPtr->GetBitField (NBPtr, BFDrvImpCtrl);
    if ((Value8 & ((UINT8) 1 << 1)) != 0) {
      MrsAddress |= ((UINT16) 1 << 5);
    }
    if ((Value8 & ((UINT8) 1 << 0)) != 0) {
      MrsAddress |= ((UINT16) 1 << 1);
    }
  } else {
    MrsAddress |= ((UINT16) 1 << 1);
  }
  // program MrsAddress[9,6,2]=nominal termination resistance of ODT (RTT):
  // Different CS may have different RTT.
  //
  Value8 = NBPtr->MemNGetDramTerm (NBPtr, ChipSel);

  //
  // If Write Leveling this DIMM
  //
  if (Wl) {
    if ((ChipSel / NBPtr->CsPerDelay) == TargetDIMM) {
      // Program MrsAddress[7] = 1 for Write leveling enable
      MrsAddress |= ((UINT16) 1 << 7);
      if (ChipSel & 1) {
        // Output buffer disabled, MrsAddress[7] (Qoff = 1)
        MrsAddress |= ((UINT16) 1 << 12);
      }
      //  Set Rtt_Nom = Rtt_Wr if there are 2 or more dimms
      if ((NBPtr->ChannelPtr->DimmQrPresent != 0) || (NBPtr->ChannelPtr->Dimms >= 2)) {
        Value8 = NBPtr->MemNGetDynDramTerm (NBPtr, ChipSel);
      } else if (NBPtr->IsSupported[WlRttNomFor1of3Cfg] && (MaxDimmPerCH == 3)) {
        // For some family, set Rtt_Nom = Rtt_Wr in one of three DIMMs per channel configurations
        Value8 = NBPtr->MemNGetDynDramTerm (NBPtr, ChipSel);
      }
    }
    NBPtr->FamilySpecificHook[WLMR1] (NBPtr, &MrsAddress);
  }
  //
  // Turn off Rtt_Nom (DramTerm=0) for certain CS in certain configs.
  //
  // All odd CS for 4 Dimm Systems
  if (MaxDimmPerCH == 4) {
    if (ChipSel & 0x01) {
      Value8 = 0;
    }
  // CS 1 and 5 for 3 Dimm configs
  } else if (MaxDimmPerCH == 3) {
    if ((ChipSel == 1) || (ChipSel == 5)) {
      Value8 = 0;
    }
  }
  // All odd CS of any QR Dimms
  if ((NBPtr->ChannelPtr->DimmQrPresent & ((UINT8) (1 << (ChipSel >> 1)))) != 0) {
    if (ChipSel & 0x01) {
      Value8 = 0;
    }
  }
  if ((Value8 & ((UINT8) 1 << 2)) != 0) {
    MrsAddress |= ((UINT16) 1 << 9);
  }
  if ((Value8 & ((UINT8) 1 << 1)) != 0) {
    MrsAddress |= ((UINT16) 1 << 6);
  }
  if ((Value8 & ((UINT8) 1 << 0)) != 0) {
    MrsAddress |= ((UINT16) 1 << 2);
  }

  // program MrsAddress[12]=output disable (QOFF):
  // based on F2x[1,0]84[Qoff]

  if (!NBPtr->IsSupported[CheckQoff]) {
    if (NBPtr->GetBitField (NBPtr, BFQoff) != 0) {
      MrsAddress |= ((UINT16) 1 << 12);
    }
  }

  // program MrsAddress[11]=TDQS:
  // based on F2x[1,0]94[RDqsEn]

  if ((NBPtr->DCTPtr->Timings.Dimmx4Present != 0) && (NBPtr->DCTPtr->Timings.Dimmx8Present != 0)) {
    if (!(NBPtr->IsSupported[SetTDqsForx8DimmOnly]) || ((NBPtr->DCTPtr->Timings.Dimmx8Present & ((UINT8) 1 << (ChipSel >> 1))) != 0)) {
      MrsAddress |= ((UINT16) 1 << 11);
    }
  }

  NBPtr->SetBitField (NBPtr, BFMrsAddress, MrsAddress);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function calculates the EMRS2 value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTEMRS23 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT32 MrsAddress;
  UINT8 DramTermDyn;
  UINT8 MaxDimmPerCH;
  UINT8 ChipSel;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;

  MaxDimmPerCH = GetMaxDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr->MCTPtr->SocketId, NBPtr->ChannelPtr->ChannelID );
  ChipSel = (UINT8) (0x0FF & NBPtr->GetBitField (NBPtr, BFMrsChipSel));

  // BA2=0,BA1=1,BA0=0
  NBPtr->SetBitField (NBPtr, BFMrsBank, 2);

  // program MrsAddress[5:3]=CAS write latency (CWL):
  MrsAddress = NBPtr->MemNGetMR2CWL (NBPtr);

  // program MrsAddress[6]=auto self refresh method (ASR):
  // program MrsAddress[7]=self refresh temperature range (SRT):
  MrsAddress |= 1 << 6;
  MrsAddress &= ( ~ (1 << 7));

  // program MrsAddress[10:9]=dynamic termination during writes (RTT_WR):
  DramTermDyn = NBPtr->MemNGetDynDramTerm (NBPtr, ChipSel);
  // Special Case for 1 DR Unbuffered Dimm in 3 Dimm/Ch
  if (!(NBPtr->MCTPtr->Status[SbRegistered])) {
    if (MaxDimmPerCH == 3) {
      if (NBPtr->ChannelPtr->Dimms == 1) {
        if ((NBPtr->ChannelPtr->DimmDrPresent & ((UINT8) (1 << (ChipSel >> 1)))) != 0) {
          DramTermDyn = 1;
        }
      }
    }
  }
  MrsAddress |= (UINT16) DramTermDyn << 9;

  NBPtr->SetBitField (NBPtr, BFMrsAddress, MrsAddress);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function calculates the EMRS3 value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTEMRS33 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;

  // BA2=0,BA1=1,BA0=1
  NBPtr->SetBitField (NBPtr, BFMrsBank, 3);

  // program MrsAddress[1:0]=multi purpose register address location
  // (MPR Location):based on F2x[1,0]84[MprLoc]
  // program MrsAddress[2]=multi purpose register
  // (MPR):based on F2x[1,0]84[MprEn]
  NBPtr->SetBitField (NBPtr, BFMrsAddress, (NBPtr->GetBitField (NBPtr, BFDramMRSReg) >> 24) & 0x0007);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This sets MRS value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTMRS3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT32 MrsAddress;
  MEM_NB_BLOCK  *NBPtr;
  UINT32 Ppd;

  NBPtr = TechPtr->NBPtr;

  // BA2=0,BA1=0,BA0=0
  NBPtr->SetBitField (NBPtr, BFMrsBank, 0);

  // program MrsAddress[1:0]=burst length and control method
  // (BL):based on F2x[1,0]84[BurstCtrl]
  MrsAddress = NBPtr->GetBitField (NBPtr, BFBurstCtrl);

  // program MrsAddress[3]=1 (BT):interleaved
  MrsAddress |= (UINT16) 1 << 3;

  // program MrsAddress[6:4,2]=read CAS latency
  MrsAddress |= NBPtr->MemNGetMR0CL (NBPtr);

  // program MrsAddress[11:9]=write recovery for auto-precharge
  MrsAddress |= NBPtr->MemNGetMR0WR (NBPtr);

  // program MrsAddress[12] (PPD):based on F2x[1,0]84[PChgPDModeSel]
  Ppd = NBPtr->GetBitField (NBPtr, BFPchgPDModeSel);
  NBPtr->FamilySpecificHook[MR0_PPD] (NBPtr, &Ppd);
  IDS_OPTION_HOOK (IDS_MEM_MR0, &Ppd, &TechPtr->NBPtr->MemPtr->StdHeader);
  MrsAddress |= Ppd << 12;

  // program MrsAddress[8]=1 (DLL):DLL reset
  MrsAddress |= (UINT32) 1 << 8;

  // During memory initialization, the value sent to MR0 is saved for S3 resume
  NBPtr->MemNSaveMR0 (NBPtr, MrsAddress);

  NBPtr->SetBitField (NBPtr, BFMrsAddress, MrsAddress);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This send all MR commands to a rank in sequence 2-3-1-0
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       ChipSel    - Target Chip Select
 */

VOID
MemTSendAllMRCmds3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 ChipSel
  )
{
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;

  NBPtr->SetBitField (NBPtr, BFMrsChipSel, ChipSel);

  // 13.Send EMRS(2)
  MemTEMRS23 (TechPtr);
  AGESA_TESTPOINT (TpProcMemSendMRS2, &(NBPtr->MemPtr->StdHeader));
  NBPtr->SendMrsCmd (NBPtr);

  // 14.Send EMRS(3). Ordinarily at this time, MrsAddress[2:0]=000b
  MemTEMRS33 (TechPtr);
  AGESA_TESTPOINT (TpProcMemSendMRS3, &(NBPtr->MemPtr->StdHeader));
  NBPtr->SendMrsCmd (NBPtr);

  // 15.Send EMRS(1).
  MemTEMRS13 (TechPtr, FALSE, (ChipSel >> 1));
  AGESA_TESTPOINT (TpProcMemSendMRS1, &(NBPtr->MemPtr->StdHeader));
  NBPtr->SendMrsCmd (NBPtr);

  // 16.Send MRS with MrsAddress[8]=1(reset the DLL)
  MemTMRS3 (TechPtr);
  AGESA_TESTPOINT (TpProcMemSendMRS0, &(NBPtr->MemPtr->StdHeader));
  NBPtr->SendMrsCmd (NBPtr);
}
