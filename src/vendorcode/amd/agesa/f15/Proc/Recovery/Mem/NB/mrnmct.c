/* $NoKeywords:$ */
/**
 * @file
 *
 * mrnmct.c
 *
 * Northbridge Common MCT supporting functions Recovery
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem/NB)
 * @e \$Revision: 56555 $ @e \$Date: 2011-07-15 06:18:49 -0600 (Fri, 15 Jul 2011) $
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
#include "mrport.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "cpuFamilyTranslation.h"
#include "cpuCacheInit.h"
#include "Filecode.h"
#define FILECODE PROC_RECOVERY_MEM_NB_MRNMCT_FILECODE

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
 *      This function is the Recovery memory configuration function for Nb DDR3
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          AGESA_STATUS
 *                          - AGESA_ALERT
 *                          - AGESA_FATAL
 *                          - AGESA_SUCCESS
 *                          - AGESA_WARNING
 */

AGESA_STATUS
MemRecNMemInitNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  AGESA_STATUS Status;
  MEM_TECH_BLOCK *TechPtr;

  TechPtr = NBPtr->TechPtr;

  NBPtr->MemRecNInitializeMctNb (NBPtr);

  if (NBPtr->IsSupported[DramModeBeforeDimmPres]) {
    TechPtr->SetDramMode (TechPtr);
  }

  Status = AGESA_FATAL;
  if (TechPtr->DimmPresence (TechPtr)) {

    if (NBPtr->IsSupported[DramModeAfterDimmPres]) {
      TechPtr->SetDramMode (TechPtr);
    }

    if (MemRecNAutoConfigNb (NBPtr)) {

      AGESA_TESTPOINT (TpProcMemPlatformSpecificConfig, &(NBPtr->MemPtr->StdHeader));
      if (MemRecNPlatformSpecNb (NBPtr)) {
        AgesaHookBeforeDramInitRecovery (0, NBPtr->MemPtr);
        AGESA_TESTPOINT (TpProcMemStartDcts, &(NBPtr->MemPtr->StdHeader));
        MemRecNStartupDCTNb (NBPtr);

        AGESA_TESTPOINT (TpProcMemMtrrConfiguration, &(NBPtr->MemPtr->StdHeader));
        MemRecNCPUMemRecTypingNb (NBPtr);

        AGESA_TESTPOINT (TpProcMemDramTraining, &(NBPtr->MemPtr->StdHeader));
        NBPtr->TrainingFlow (NBPtr);

        Status = AGESA_SUCCESS;
      }
    }
  }

  NBPtr->MemRecNFinalizeMctNb (NBPtr);

  return Status;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function returns a physical address of a corresponding Chip select
 *
 *     @return      Addr -  System Address
 */

UINT32
MemRecNGetMCTSysAddrNb ( VOID )
{
  UINT32 CSBase;

  CSBase = (UINT32) 1 << 21;       // 1MB offset to avoid compat area from the base address.
  if ((CSBase >= (MCT_TRNG_KEEPOUT_START << 8)) && (CSBase <= (MCT_TRNG_KEEPOUT_END << 8))) {
    CSBase += (((MCT_TRNG_KEEPOUT_END << 8) - CSBase) + 0x0FFFFF) & 0xFFF00000;
  }

  return CSBase;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function runs on the BSP only, it sets the fixed MTRRs for common legacy ranges.
 *   It sets TOP_MEM and TOM2 and some variable MTRRs with WB Uncacheable type.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemRecNCPUMemRecTypingNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  S_UINT64 SMsr;

  MEM_DATA_STRUCT *MemPtr;
  MemPtr = NBPtr->MemPtr;

  //
  //======================================================================
  // Set default values for CPU registers
  //======================================================================
  //

  LibAmdMsrRead (SYS_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  SMsr.lo |= 0x1C0000;                // turn on modification enable bit and
                                      // mtrr enable bits
  LibAmdMsrWrite (SYS_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);

  SMsr.lo = SMsr.hi = 0x1E1E1E1E;
  LibAmdMsrWrite (0x250, (UINT64 *)&SMsr, &MemPtr->StdHeader);      // 0 - 512K = WB Mem
  LibAmdMsrWrite (0x258, (UINT64 *)&SMsr, &MemPtr->StdHeader);      // 512K - 640K = WB Mem

  LibAmdMsrRead (SYS_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  SMsr.lo &= 0xFFF7FFFF;                // turn off modification enable bit
  LibAmdMsrWrite (SYS_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  //
  //======================================================================
  // Set TOP_MEM and TOM2 CPU registers
  //======================================================================
  //
  SMsr.hi = 0;
  SMsr.lo = 0x08000000;
  LibAmdMsrWrite (TOP_MEM, (UINT64 *)&SMsr, &MemPtr->StdHeader);  // TOP_MEM

  // Set FS Base address for later memory accesses
  SMsr.lo = 0;
  LibAmdMsrWrite (FS_BASE, (UINT64 *)&SMsr, &MemPtr->StdHeader);

  //
  //======================================================================
  // Set variable MTRR values
  //======================================================================
  //
  SMsr.lo = 0x00000006;
  LibAmdMsrWrite (0x200, (UINT64 *)&SMsr, &MemPtr->StdHeader);      // MTRRPhysBase0

  SMsr.hi = NBPtr->VarMtrrHiMsk;
  SMsr.lo = 0xF8000800;
  LibAmdMsrWrite (0x201, (UINT64 *)&SMsr, &MemPtr->StdHeader);      // MTRRPhysMask0

}

/*-----------------------------------------------------------------------------*/
/**
 *
 *     This function returns the upper 32 bits mask for variable MTRR based on
 *     the CPU_LOGICAL_ID.
 *     @param[in]       *LogicalIdPtr - Pointer to the CPU_LOGICAL_ID
 *     @param[in]       StdHeader - Header for library and services
 *
 *     @return          UINT32 - MTRR mask for upper 32 bits
 *
 */
UINT32
MemRecGetVarMtrrHiMsk (
  IN       CPU_LOGICAL_ID *LogicalIdPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8 TempNotCare;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  CACHE_INFO *CacheInfoPtr;

  GetCpuServicesFromLogicalId (LogicalIdPtr, (CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  FamilySpecificServices->GetCacheInfo (FamilySpecificServices, (CONST VOID **) &CacheInfoPtr, &TempNotCare, StdHeader);
  return (UINT32) (CacheInfoPtr->VariableMtrrMask >> 32);
}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function re-enable phy compensation.
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemRecNReEnablePhyCompNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT8 Dct;

  Dct = NBPtr->Dct;

  NBPtr->SwitchDCT (NBPtr, 0);
  // Clear DisableCal and set DisablePredriverCal
  MemRecNSetBitFieldNb (NBPtr, BFDisablePredriverCal, 0x2000);
  NBPtr->SwitchDCT (NBPtr, Dct);

  return TRUE;
}
/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
