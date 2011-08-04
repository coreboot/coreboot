/* $NoKeywords:$ */
/**
 * @file
 *
 * minit.c
 *
 * Initializer support function
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
 * @e \$Revision: 35136 $ @e \$Date: 2010-07-16 11:29:48 +0800 (Fri, 16 Jul 2010) $
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
#include "amdlib.h"
#include "AdvancedApi.h"
#include "mu.h"
#include "OptionMemory.h"
#include "Ids.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_MAIN_MINIT_FILECODE
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

extern MEM_NB_SUPPORT memNBInstalled[];
extern MEM_PLATFORM_CFG* memPlatformTypeInstalled[];


/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *  This function initializes the default parameter, function pointers, build options
 *   and SPD data for memory configuration
 *
 *     @param[in,out]   *MemPtr   - Pointer to the MEM_DATA_STRUCT
 *     @param[in,out]   *PlatFormConfig -  Platform profile/build option config structure
 *
 */

VOID
AmdMemInitDataStructDef (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   PLATFORM_CONFIGURATION   *PlatFormConfig
  )
{
  UINT8 p;
  UINT8 i;
  // We need a way of specifying default values for each particular northbridge
  // family.  We also need to make sure that the IBV knows which parameter struct
  // is for which northbridge.
  //----------------------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemBeforeMemDataInit, &MemPtr->StdHeader);

  MemPtr->PlatFormConfig = PlatFormConfig;

  memNBInstalled[0].MemNInitDefaults (MemPtr);

  //----------------------------------------------------------------------------
  // INITIALIZE PLATFORM SPECIFIC CONFIGURATION STRUCT
  //----------------------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemPlatformSpecificConfig, &MemPtr->StdHeader);
  i = 0;
  for (p = 0; p < MAX_PLATFORM_TYPES; p++) {
    if (memPlatformTypeInstalled[i] != NULL) {
      MemPtr->GetPlatformCfg[p] = memPlatformTypeInstalled[i];
      i++;
    } else {
      MemPtr->GetPlatformCfg[p] = MemAGetPsCfgDef;
    }
  }
  AGESA_TESTPOINT (TpProcMemAfterMemDataInit, &MemPtr->StdHeader);
  MemPtr->ErrorHandling = MemErrHandle;
}
