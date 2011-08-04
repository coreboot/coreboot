/* $NoKeywords:$ */
/**
 * @file
 *
 * mrttwl3.c
 *
 * Technology Phy assisted write levelization for recovery DDR3
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem)
 * @e \$Revision: 48803 $ @e \$Date: 2011-03-10 20:18:28 -0700 (Thu, 10 Mar 2011) $
 *
 **/
/*
 *****************************************************************************
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
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mru.h"
#include "mt.h"
#include "mrt3.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_RECOVERY_MEM_TECH_DDR3_MRTTWL3_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define MAX_BYTELANES 8             /* Max Bytelanes per channel */

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

VOID
STATIC
MemRecTPrepareDIMMs3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       BOOLEAN Wl
  );

VOID
STATIC
MemRecTProcConfig3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

VOID
STATIC
MemRecTBeginWLTrain3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function executed hardware based write levelization for a specific die
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemRecTTrainDQSWriteHw3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;

  IDS_HDT_CONSOLE (MEM_STATUS, "\nStart write leveling\n");
  //  Disable auto refresh by configuring F2x[1, 0]8C[DisAutoRefresh] = 1.
  NBPtr->SetBitField (NBPtr, BFDisAutoRefresh, 1);
  //  Disable ZQ calibration short command by configuring F2x[1, 0]94[ZqcsInterval] = 00b.
  NBPtr->SetBitField (NBPtr, BFZqcsInterval, 0);

  // 1. Specify the target Dimm that is to be trained by programming
  //     F2x[1, 0]9C_x08[TrDimmSel].
  IDS_HDT_CONSOLE (MEM_STATUS, "\t\tCS %d\n", NBPtr->DimmToBeUsed << 1);
  NBPtr->SetBitField (NBPtr, BFTrDimmSel, NBPtr->DimmToBeUsed);

  // 2. Prepare the DIMMs for write levelization using DDR3-defined
  // MR commands.
  MemRecTPrepareDIMMs3 (TechPtr, TRUE);

  // 3. After the DIMMs are configured, BIOS waits 40 MEMCLKs to
  //     satisfy DDR3-defined internal DRAM timing.
  MemRecUWait10ns (10, NBPtr->MemPtr);

  // 4. Configure the processor's DDR phy for write levelization training:
  MemRecTProcConfig3 (TechPtr);

  // 5. Begin write levelization training
  MemRecTBeginWLTrain3 (TechPtr);

  // 6. Configure DRAM Phy Control Register so that the phy stops driving write levelization ODT.
  // Program WrLvOdtEn=0
  NBPtr->SetBitField (NBPtr, BFWrLvOdtEn, 0);

  // Wait 10 MEMCLKs to allow for ODT signal settling.
  MemRecUWait10ns (3, NBPtr->MemPtr);

  // 7. Program the target Dimm back to normal operation
  MemRecTPrepareDIMMs3 (TechPtr, FALSE);

  //  13.Program F2x[1, 0]8C[DisAutoRefresh] = 0.
  NBPtr->SetBitField (NBPtr, BFDisAutoRefresh, 0);
  //  14.Program F2x[1, 0]94[ZqcsInterval] to the proper interval for the current memory configuration.
  NBPtr->SetBitField (NBPtr, BFZqcsInterval, 2);

  IDS_HDT_CONSOLE (MEM_FLOW, "End write leveling\n\n");
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function prepares the DIMMS for Write Levelization
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in] Wl - Indicates if WL mode should be enabled
 *
 */

VOID
STATIC
MemRecTPrepareDIMMs3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       BOOLEAN Wl
  )
{
  UINT8 ChipSel;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;

  AGESA_TESTPOINT (TpProcMemWlPrepDimms, &(NBPtr->MemPtr->StdHeader));

  for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL; ChipSel++) {
    // Set Dram ODT based on current mode.
    if ((NBPtr->DCTPtr->Timings.CsPresent & (UINT16) 1 << ChipSel) != 0) {
      if (Wl) {
        NBPtr->SetDramOdtRec (NBPtr, WRITE_LEVELING_MODE, ChipSel, (NBPtr->DimmToBeUsed << 1));
      } else {
        NBPtr->SetDramOdtRec (NBPtr, MISSION_MODE, 0, 0);
      }

      NBPtr->SetBitField (NBPtr, BFMrsChipSel, ChipSel);

      // Set MR1 to F2x7C[MrsAddress], F2x7C[MrsBank]=1
      MemRecTEMRS13 (TechPtr);
      // Program Level
      if (Wl) {
        if ((ChipSel >> 1) == NBPtr->DimmToBeUsed) {
          NBPtr->SetBitField (NBPtr, BFLevel, 1);
          if (ChipSel & 1) {
            NBPtr->SetBitField (NBPtr, BFMrsQoff, 1);
          }
        }
      }
      // Send command
      NBPtr->SendMrsCmd (NBPtr);

      // Set MR2 to F2x7C[MrsAddress], F2x7C[MrsBank]=1
      MemRecTEMRS23 (TechPtr);
      // Send command
      NBPtr->SendMrsCmd (NBPtr);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function configures the DIMMS for Write Levelization
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
STATIC
MemRecTProcConfig3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  MEM_NB_BLOCK *NBPtr;
  CH_DEF_STRUCT *ChannelPtr;
  UINT8 ByteLane;
  UINT8 *Seed;
  UINT8 DefaultSeed;
  UINT8 CurrentSeed;
  UINT8 Dimm;

  NBPtr = TechPtr->NBPtr;
  ChannelPtr = TechPtr->NBPtr->ChannelPtr;

  Dimm = NBPtr->DimmToBeUsed;

  // Program WrLvOdtEn=1
  NBPtr->SetBitField (NBPtr, BFWrLvOdtEn, 1);

  // Wait 10 MEMCLKs to allow for ODT signal settling.
  MemRecUWait10ns (3, NBPtr->MemPtr);

  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\t Byte: 00 01 02 03 04 05 06 07 ECC\n");
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tSeeds: ");
  //  Program an initialization Value to registers F2x[1, 0]9C_x[51:50] and F2x[1, 0]9C_x52 to set
  //  the gross and fine delay for all the byte lane fields. If the target frequency is different than 400MHz,
  //  BIOS must execute two training passes for each Dimm. For pass 1 at a 400MHz MEMCLK frequency,
  //  use an initial total delay Value of 01Fh. This represents a 1UI (UI=.5MEMCLK) delay and is determined
  //  by design.

  //  Get default seed
  if (ChannelPtr->RegDimmPresent != 0) {
    DefaultSeed = 0x41;
  } else if (ChannelPtr->SODimmPresent != 0) {
    DefaultSeed = 0x12;
  } else {
    DefaultSeed = 0x1A;
  }

  //  Get platform override seed
  Seed = (UINT8 *) MemRecFindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_WL_SEED, NBPtr->MCTPtr->SocketId, ChannelPtr->ChannelID);

  IDS_HDT_CONSOLE (MEM_FLOW, "Seeds: ");
  for (ByteLane = 0; ByteLane < 8; ByteLane++) {
    // This includes ECC as byte 8
    CurrentSeed = ((Seed != NULL) ? Seed[ByteLane] : DefaultSeed);
    NBPtr->SetTrainDly (NBPtr, AccessPhRecDly, DIMM_BYTE_ACCESS (Dimm, ByteLane), CurrentSeed);
    ChannelPtr->WrDqsDlys[Dimm * MAX_BYTELANES + ByteLane] = CurrentSeed;
    IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", CurrentSeed);
  }
  IDS_HDT_CONSOLE (MEM_FLOW, "\n");

  // Program F2x[1, 0]9C_x08[WrtLvTrMode]=0 for phy assisted training.

  // Program F2x[1, 0]9C_x08[TrNibbleSel]=0

  IDS_HDT_CONSOLE (MEM_FLOW, "\n");
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function begins WL training for a specific DIMM
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
STATIC
MemRecTBeginWLTrain3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  MEM_NB_BLOCK *NBPtr;
  UINT8 ByteLane;
  UINT8 Seed;
  UINT8 Delay;
  UINT8 Dimm;

  NBPtr = TechPtr->NBPtr;

  Dimm = NBPtr->DimmToBeUsed;
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tWrtLvTrEn = 1\n");
  // Program F2x[1, 0]9C_x08[WrtLlTrEn]=1.
  NBPtr->SetBitField (NBPtr, BFWrtLvTrEn, 1);

  //  Wait 200 MEMCLKs. If executing pass 2, wait 32 MEMCLKs.
  MemRecUWait10ns (50, NBPtr->MemPtr);

  //  Program F2x[1, 0]9C_x08[WrtLlTrEn]=0.
  NBPtr->SetBitField (NBPtr, BFWrtLvTrEn, 0);

  //  Read from registers F2x[1, 0]9C_x[51:50] and F2x[1, 0]9C_x52 to get the gross and fine Delay settings
  //   for the target Dimm and save these values.
  IDS_HDT_CONSOLE (MEM_FLOW, " PRE WrDqs\n");
  for (ByteLane = 0; ByteLane < 8; ByteLane++) {
    // This includes ECC as byte 8
    Seed = NBPtr->ChannelPtr->WrDqsDlys[(Dimm * MAX_BYTELANES) + ByteLane];
    Delay = (UINT8)NBPtr->GetTrainDly (NBPtr, AccessPhRecDly, DIMM_BYTE_ACCESS (Dimm, ByteLane));
    IDS_HDT_CONSOLE (MEM_FLOW, "  %02x ", Delay);
    if (((Seed >> 5) == 0) && ((Delay >> 5) == 3)) {
      // If seed has gross delay of 0 and PRE has gross delay of 3,
      // then round the total delay of TxDqs to 0.
      Delay = 0;
    }
    NBPtr->SetTrainDly (NBPtr, AccessWrDqsDly, DIMM_BYTE_ACCESS (Dimm, ByteLane), Delay);
    NBPtr->ChannelPtr->WrDqsDlys[(Dimm * MAX_BYTELANES) + ByteLane] = Delay;
    IDS_HDT_CONSOLE (MEM_FLOW, "  %02x\n", Delay);
  }

  IDS_HDT_CONSOLE_DEBUG_CODE (
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\tWrDqs: ");
    for (ByteLane = 0; ByteLane < 8; ByteLane++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", NBPtr->ChannelPtr->WrDqsDlys[ByteLane]);
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\n");
  );
}
