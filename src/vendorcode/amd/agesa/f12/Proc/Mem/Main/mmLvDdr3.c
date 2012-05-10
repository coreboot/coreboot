/* $NoKeywords:$ */
/**
 * @file
 *
 * mmLvDdr3.c
 *
 * Main Memory Feature implementation file for low voltage DDR3 support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
 * @e \$Revision: 47408 $ @e \$Date: 2011-02-19 00:56:31 +0800 (Sat, 19 Feb 2011) $
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
#include "amdlib.h"
#include "OptionMemory.h"
#include "mmlvddr3.h"
#include "mm.h"
#include "mn.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_MAIN_MMLVDDR3_FILECODE

extern MEM_FEAT_BLOCK_MAIN MemFeatMain;
/*-----------------------------------------------------------------------------
*                                EXPORTED FUNCTIONS
*
*-----------------------------------------------------------------------------
*/

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Find the common supported voltage on all nodes.
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemMLvDdr3 (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT8   Node;
  BOOLEAN RetVal;
  BOOLEAN SecondLoop;
  MEM_NB_BLOCK  *NBPtr;
  MEM_PARAMETER_STRUCT *ParameterPtr;
  MEM_SHARED_DATA *mmSharedPtr;

  NBPtr = MemMainPtr->NBPtr;
  mmSharedPtr = MemMainPtr->mmSharedPtr;
  ParameterPtr = MemMainPtr->MemPtr->ParameterListPtr;
  mmSharedPtr->VoltageMap = 0xFF;
  SecondLoop = FALSE;
  RetVal = TRUE;

  for (Node = 0; Node < MemMainPtr->DieCount; Node++) {
    NBPtr[Node].FeatPtr->LvDdr3 (&NBPtr[Node]);
    // Check if there is no common supported voltage
    if ((mmSharedPtr->VoltageMap == 0) && !SecondLoop) {
      // restart node loop by setting node to 0xFF
      Node = 0xFF;
      SecondLoop = TRUE;
    }
  }

  if (mmSharedPtr->VoltageMap == 0) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\nNo commonly supported VDDIO is found.\n");
    PutEventLog (AGESA_WARNING, MEM_WARNING_NO_COMMONLY_SUPPORTED_VDDIO, 0, 0, 0, 0, &(NBPtr[BSP_DIE].MemPtr->StdHeader));
    SetMemError (AGESA_WARNING, NBPtr[BSP_DIE].MCTPtr);
    // When there is no commonly supported VDDIO, use 1.35V as the temporal VDDIO
    ParameterPtr->DDR3Voltage = VOLT1_35;
  } else {
    IDS_HDT_CONSOLE (MEM_FLOW, "\nCommonly supported VDDIO is: %s%s%s.\n", ((mmSharedPtr->VoltageMap & 1) != 0) ? "1.5V, " : "", ((mmSharedPtr->VoltageMap & 2) != 0) ? "1.35V, " : "", ((mmSharedPtr->VoltageMap & 4) != 0) ? "1.25V" : "");
    ParameterPtr->DDR3Voltage = CONVERT_ENCODED_TO_VDDIO (LibAmdBitScanReverse (mmSharedPtr->VoltageMap));
  }

  for (Node = 0; Node < MemMainPtr->DieCount; Node ++) {
    // Check if the voltage needs force to 1.5V
    NBPtr[Node].FamilySpecificHook[ForceLvDimmVoltage] (&NBPtr[Node], MemMainPtr);

    RetVal &= (BOOLEAN) (NBPtr[Node].MCTPtr->ErrCode < AGESA_FATAL);
  }

  return RetVal;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Find the common supported voltage on all nodes, taken into account of the
 *  user option for performance and power saving.
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemMLvDdr3PerformanceEnhPre (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT8 Node;
  BOOLEAN RetVal;
  DIMM_VOLTAGE VDDIO;
  MEM_NB_BLOCK *NBPtr;
  MEM_PARAMETER_STRUCT *ParameterPtr;
  MEM_SHARED_DATA *mmSharedPtr;
  PLATFORM_POWER_POLICY PowerPolicy;

  NBPtr = MemMainPtr->NBPtr;
  mmSharedPtr = MemMainPtr->mmSharedPtr;
  ParameterPtr = MemMainPtr->MemPtr->ParameterListPtr;
  PowerPolicy = MemMainPtr->MemPtr->PlatFormConfig->PlatformProfile.PlatformPowerPolicy;

  IDS_OPTION_HOOK (IDS_SKIP_PERFORMANCE_OPT, &PowerPolicy, &NBPtr->MemPtr->StdHeader);
  IDS_HDT_CONSOLE (MEM_FLOW, (PowerPolicy == Performance) ? "\nMaximize Performance\n" : "\nMaximize Battery Life\n");

  if (ParameterPtr->DDR3Voltage != VOLT_INITIAL) {
    mmSharedPtr->VoltageMap = VDDIO_DETERMINED;
    PutEventLog (AGESA_WARNING, MEM_WARNING_INITIAL_DDR3VOLT_NONZERO, 0, 0, 0, 0, &(NBPtr[BSP_DIE].MemPtr->StdHeader));
    SetMemError (AGESA_WARNING, NBPtr[BSP_DIE].MCTPtr);
    IDS_HDT_CONSOLE (MEM_FLOW, "Warning: Initial Value for VDDIO has been changed.\n");
    RetVal = TRUE;
  } else {
    RetVal = MemMLvDdr3 (MemMainPtr);

    VDDIO = ParameterPtr->DDR3Voltage;
    if (NBPtr->IsSupported[PerformanceOnly] || ((PowerPolicy == Performance) && (mmSharedPtr->VoltageMap != 0))) {
      // When there is no commonly supported voltage, do not optimize performance
      // For cases where we can maximize performance, do the following
      // When VDDIO is enforced, DDR3Voltage will be overriden by specific VDDIO
      // So cases with DDR3Voltage left to be VOLT_UNSUPPORTED will be open to maximizing performance.
      ParameterPtr->DDR3Voltage = VOLT_UNSUPPORTED;
    }

    IDS_OPTION_HOOK (IDS_ENFORCE_VDDIO, &(ParameterPtr->DDR3Voltage), &NBPtr->MemPtr->StdHeader);

    if (ParameterPtr->DDR3Voltage != VOLT_UNSUPPORTED) {
      // When Voltage is already determined, do not have further process to choose maximum frequency to optimize performance
      mmSharedPtr->VoltageMap = VDDIO_DETERMINED;
      IDS_HDT_CONSOLE (MEM_FLOW, "VDDIO is determined. No further optimization will be done.\n");
    } else {
      for (Node = 0; Node < MemMainPtr->DieCount; Node++) {
        NBPtr[Node].MaxFreqVDDIO[VOLT1_5_ENCODED_VAL] = UNSUPPORTED_DDR_FREQUENCY;
        NBPtr[Node].MaxFreqVDDIO[VOLT1_35_ENCODED_VAL] = UNSUPPORTED_DDR_FREQUENCY;
        NBPtr[Node].MaxFreqVDDIO[VOLT1_25_ENCODED_VAL] = UNSUPPORTED_DDR_FREQUENCY;
      }
      // Reprogram the leveling result as temporal candidate
      ParameterPtr->DDR3Voltage = VDDIO;
    }
  }

  ASSERT (ParameterPtr->DDR3Voltage != VOLT_UNSUPPORTED);
  return RetVal;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Finalize the VDDIO for the board for performance enhancement.
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemMLvDdr3PerformanceEnhFinalize (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT8 Dct;
  UINT8 Node;
  UINT8 NodeCnt[VOLT1_25 + 1];
  UINT8 MaxCnt;
  MEM_NB_BLOCK *NBPtr;
  MEM_PARAMETER_STRUCT *ParameterPtr;
  MEM_SHARED_DATA *mmSharedPtr;
  UINT8 CurrentVoltage;
  DIMM_VOLTAGE Voltage;
  MEMORY_BUS_SPEED HighestFreq;

  ParameterPtr = MemMainPtr->MemPtr->ParameterListPtr;
  mmSharedPtr = MemMainPtr->mmSharedPtr;
  NBPtr = MemMainPtr->NBPtr;

  LibAmdMemFill (NodeCnt, 0, VOLT1_25_ENCODED_VAL + 1, &NBPtr->MemPtr->StdHeader);
  if (mmSharedPtr->VoltageMap != VDDIO_DETERMINED) {
    Voltage = ParameterPtr->DDR3Voltage;
    IDS_HDT_CONSOLE (MEM_FLOW, "\nSearching for VDDIO that can maximize frequency: \n");
    for (Node = 0; Node < MemMainPtr->DieCount; Node++) {
      HighestFreq = 0;
      // Find out what the highest frequency that can be reached is on this node across different voltage.
      for (CurrentVoltage = VOLT1_5_ENCODED_VAL; CurrentVoltage <= VOLT1_25_ENCODED_VAL; CurrentVoltage ++) {
        if (HighestFreq < NBPtr[Node].MaxFreqVDDIO[CurrentVoltage]) {
          HighestFreq = NBPtr[Node].MaxFreqVDDIO[CurrentVoltage];
        }
      }
      IDS_HDT_CONSOLE (MEM_FLOW, "Node%d: 1.5V -> %dMHz, 1.35V -> %dMHz, 1.25V -> %dMHz\n", Node, NBPtr[Node].MaxFreqVDDIO[VOLT1_5_ENCODED_VAL], NBPtr[Node].MaxFreqVDDIO[VOLT1_35_ENCODED_VAL], NBPtr[Node].MaxFreqVDDIO[VOLT1_25_ENCODED_VAL]);
      // Figure out what voltage we can have when attaining the highest frequency.
      for (CurrentVoltage = VOLT1_5_ENCODED_VAL; CurrentVoltage <= VOLT1_25_ENCODED_VAL; CurrentVoltage ++) {
        if (NBPtr[Node].MaxFreqVDDIO[CurrentVoltage] == HighestFreq) {
          NodeCnt[CurrentVoltage] ++;
        }
      }
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "Number of nodes that can run at maximize performance: 1.5V -> %d Nodes 1.35V -> %d Nodes 1.25V -> %d Nodes.\n", NodeCnt[VOLT1_5_ENCODED_VAL], NodeCnt[VOLT1_35_ENCODED_VAL], NodeCnt[VOLT1_25_ENCODED_VAL]);
    MaxCnt = 0;
    // Use the VDDIO at which most nodes can run at higher frequency
    for (CurrentVoltage = VOLT1_5_ENCODED_VAL; CurrentVoltage <= VOLT1_25_ENCODED_VAL; CurrentVoltage ++) {
      if (MaxCnt <= NodeCnt[CurrentVoltage]) {
        MaxCnt = NodeCnt[CurrentVoltage];
        ParameterPtr->DDR3Voltage = CONVERT_ENCODED_TO_VDDIO (CurrentVoltage);
      }
    }

    ASSERT (ParameterPtr->DDR3Voltage != VOLT_UNSUPPORTED);

    mmSharedPtr->VoltageMap = VDDIO_DETERMINED;
    if (Voltage != ParameterPtr->DDR3Voltage) {
      // Finalize frequency with updated finalized VDDIO
      for (Node = 0; Node < MemMainPtr->DieCount; Node++) {
        // Need to re-sync target speed and different VDDIO may cause different settings
        NBPtr[Node].TechPtr->SpdGetTargetSpeed (NBPtr[Node].TechPtr);
        for (Dct = 0; Dct < NBPtr[Node].DctCount; Dct++) {
          NBPtr[Node].SwitchDCT (&(NBPtr[Node]), Dct);
          if (NBPtr[Node].DCTPtr->Timings.CsEnabled != 0) {
            if (!NBPtr[Node].PlatformSpec (&(NBPtr[Node]))) {
              return FALSE;
            }
          }
        }
      }
    }
  }
  return TRUE;
}
