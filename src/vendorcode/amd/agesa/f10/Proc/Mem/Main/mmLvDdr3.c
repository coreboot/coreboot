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
 * @e \$Revision: 6474 $ @e \$Date: 2008-06-20 03:07:59 -0500 (Fri, 20 Jun 2008) $
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
#include "Ids.h"
#include "amdlib.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_MAIN_MMLVDDR3_FILECODE

extern MEM_FEAT_BLOCK_MAIN MemFeatMain;
/*-----------------------------------------------------------------------------
*                                EXPORTED FUNCTIONS
*
*-----------------------------------------------------------------------------
*/
BOOLEAN
MemMLvDdr3 (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

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
    ParameterPtr->DDR3Voltage = VOLT_UNSUPPORTED;
  } else {
    ParameterPtr->DDR3Voltage = (DIMM_VOLTAGE) LibAmdBitScanReverse (mmSharedPtr->VoltageMap);
  }

  for (Node = 0; Node < MemMainPtr->DieCount; Node ++) {
    // Check if the voltage needs force to 1.5V
    NBPtr[Node].FamilySpecificHook[ForceLvDimmVoltage] (&NBPtr[Node], MemMainPtr);

    RetVal &= (BOOLEAN) (NBPtr[Node].MCTPtr->ErrCode < AGESA_FATAL);
  }

  return RetVal;
}
