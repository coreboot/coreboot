/* $NoKeywords:$ */
/**
 * @file
 *
 * mrinit.c
 *
 * Initializer support functions for Recovery mode
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem)
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
#include "OptionMemory.h"
#include "Ids.h"
#include "mm.h"
#include "ma.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_RECOVERY_MEM_MRINIT_FILECODE
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
extern PSO_TABLE DefaultPlatformMemoryConfiguration[];
extern MEM_PLATFORM_CFG* memRecPlatformTypeInstalled[];

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function initializes the default parameter, function pointers, build options
 *   and SPD data for memory configuration
 *
 *     @param[in,out]   *MemPtr   - Pointer to the MEM_DATA_STRUCT
 *
 */

VOID
AmdMemInitDataStructDefRecovery (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  MEM_PARAMETER_STRUCT *RefPtr;
  UINT8 i;
  UINT8 p;

  RefPtr = MemPtr->ParameterListPtr;

  // Memory Map/Mgt.
  // Mask Bottom IO with 0xF8 to force hole size to have granularity of 128MB
  RefPtr->BottomIo = 0xF0;

  RefPtr->PlatformMemoryConfiguration = DefaultPlatformMemoryConfiguration;

  i = 0;
  for (p = 0; p < MAX_PLATFORM_TYPES; p++) {
    if (memRecPlatformTypeInstalled[i] != NULL) {
      MemPtr->GetPlatformCfg[p] = memRecPlatformTypeInstalled[i];
      i++;
    } else {
      MemPtr->GetPlatformCfg[p] = MemRecNGetPsCfgDef;
    }
  }
}
