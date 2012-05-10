/* $NoKeywords:$ */
/**
 * @file
 *
 * mrp.c
 *
 * Common platform specific configuration.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem/Ps)
 * @e \$Revision: 44324 $ @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
#include "AdvancedApi.h"
#include "amdlib.h"
#include "Ids.h"
#include "cpuFamRegisters.h"
#include "cpuRegisters.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "mru.h"
#include "ma.h"
#include "mp.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_RECOVERY_MEM_PS_MRP_FILECODE


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
extern MEM_PSC_FLOW_BLOCK* memRecPlatSpecFlowArray[];

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function gets platform specific configuration such as Max Freq., Slow Mode, Dram Term,
 *    and so on.
 *
 *     @param[in]       *NBPtr       Pointer to MEM_NB_BLOCK
 *     @return          TRUE - Successfully execute platform specific configuration flow.
 *     @return          FALSE - Fail to execute platform specific configuration flow.
 *
 */
BOOLEAN
MemPRecPSCFlow (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 i;

  i = 0;
  while (memRecPlatSpecFlowArray[i] != NULL) {
    if ((memRecPlatSpecFlowArray[i])->DramTerm (NBPtr, (memRecPlatSpecFlowArray[i])->EntryOfTables)) {
      if ((memRecPlatSpecFlowArray[i])->ODTPattern (NBPtr, (memRecPlatSpecFlowArray[i])->EntryOfTables)) {
        if ((memRecPlatSpecFlowArray[i])->SAO (NBPtr, (memRecPlatSpecFlowArray[i])->EntryOfTables)) {
          if ((memRecPlatSpecFlowArray[i])->MR0WrCL (NBPtr, (memRecPlatSpecFlowArray[i])->EntryOfTables)) {
            if ((memRecPlatSpecFlowArray[i])->RC2IBT (NBPtr, (memRecPlatSpecFlowArray[i])->EntryOfTables)) {
              if ((memRecPlatSpecFlowArray[i])->RC10OpSpeed (NBPtr, (memRecPlatSpecFlowArray[i])->EntryOfTables)) {
                if ((memRecPlatSpecFlowArray[i])->LRIBT (NBPtr, (memRecPlatSpecFlowArray[i])->EntryOfTables)) {
                  if ((memRecPlatSpecFlowArray[i])->LRNPR (NBPtr, (memRecPlatSpecFlowArray[i])->EntryOfTables)) {
                    if ((memRecPlatSpecFlowArray[i])->LRNLR (NBPtr, (memRecPlatSpecFlowArray[i])->EntryOfTables)) {
                      return TRUE;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    i++;
  }
  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function constructs the rank type map of Dimm0, Dimm1, Dimm2. Also it counts the number
 *     of dimm in the table.
 *
 *     @param[in]       Dimm0       Rank type of Dimm0
 *     @param[in]       Dimm1       Rank type of Dimm1
 *     @param[in]       Dimm2       Rank type of Dimm2
 *     @param[in, out]       *RankTypeInTable       Pointer to RankTypeInTable variable
 *
 *
 */
VOID
MemPRecConstructRankTypeMap (
  IN       UINT16 Dimm0,
  IN       UINT16 Dimm1,
  IN       UINT16 Dimm2,
  IN OUT   UINT16 *RankTypeInTable
  )
{
  UINT8 i;
  UINT16 RT;
  UINT8 BitShift;

  *RankTypeInTable = 0;
  RT = 0;
  BitShift = 0;

  for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i++) {
    switch (i) {
    case 0:
      RT = (Dimm0 == 0) ? NP : Dimm0;
      BitShift = 0;
      break;
    case 1:
      RT = (Dimm1 == 0) ? NP : Dimm1;
      BitShift = 4;
      break;
    case 2:
      RT = (Dimm2 == 0) ? NP : Dimm2;
      BitShift = 8;
      break;
    default:
      // dimm3 is not used, fills nibble3 with "NP"
      RT = NP;
      BitShift = 12;
    }
    *RankTypeInTable |= RT << BitShift;
  }
}

/*-----------------------------------------------------------------------------*/
/**
 *     MemPIsIdSupported
 *      This function matches the CPU_LOGICAL_ID and PackageType with certain criteria to
 *      determine if it is supported by this NB type.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       LogicalId - CPU_LOGICAL_ID
 *     @param[in]       PackageType - Package Type
 *
 *     @return          TRUE -  NB type is matched !
 *     @return          FALSE - NB type is not matched !
 *
 */
BOOLEAN
MemPRecIsIdSupported (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID LogicalId,
  IN       UINT8 PackageType
  )
{
  CPUID_DATA CpuId;
  UINT8 PkgType;

  LibAmdCpuidRead (AMD_CPUID_FMF, &CpuId, &(NBPtr->MemPtr->StdHeader));
  PkgType = (UINT8) (CpuId.EBX_Reg >> 28) & 0xF; // bit 31:28

  if (((NBPtr->MCTPtr->LogicalCpuid.Family & LogicalId.Family) != 0)
      && ((NBPtr->MCTPtr->LogicalCpuid.Revision & LogicalId.Revision) != 0)) {
    if ((PackageType == PT_DONT_CARE) || (PackageType == PkgType)) {
      return TRUE;
    }
  }
  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function returns the rank type map of a channel.
 *
 *     @param[in]       *CurrentChannel       Pointer to CH_DEF_STRUCT
 *
 *     @return          UINT16 - The map of rank type.
 *
 */
UINT16
MemPRecGetPsRankType (
  IN       CH_DEF_STRUCT *CurrentChannel
  )
{
  UINT8 i;
  UINT16 DIMMRankType;

  DIMMRankType = 0;
  for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i++) {
    if ((CurrentChannel->DimmQrPresent & (UINT8) 1 << i) != 0) {
      if (i < 2) {
        DIMMRankType |= (UINT16) 8 << (i << 2);
      }
    } else if ((CurrentChannel->DimmDrPresent & (UINT8) 1 << i) != 0) {
      DIMMRankType |= (UINT16) 4 << (i << 2);
    } else if ((CurrentChannel->DimmSRPresent & (UINT8) 1 << i) != 0) {
      DIMMRankType |= (UINT16) 2 << (i << 2);
    } else {
      DIMMRankType |= (UINT16) 1 << (i << 2);
    }
  }
  return DIMMRankType;
}
