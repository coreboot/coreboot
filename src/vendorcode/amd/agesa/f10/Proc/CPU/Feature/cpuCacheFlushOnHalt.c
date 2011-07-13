/**
 * @file
 *
 * AMD CPU Cache Flush On Halt Function.
 *
 * Contains code to Level the Feature in a multi-socket system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 6626 $   @e \$Date: 2008-07-04 02:01:02 +0800 (Fri, 04 Jul 2008) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 *----------------------------------------------------------------------------
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
#include "cpuRegisters.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuFeatures.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FEATURE_CPUCACHEFLUSHONHALT_FILECODE
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

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern CPU_FAMILY_SUPPORT_TABLE CacheFlushOnHaltFamilyServiceTable;

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          P U B L I C     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 *  Should cache flush on halt be enabled
 *
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               core leveling is supported.
 * @retval       FALSE              core leveling cannot be enabled.
 *
 */
BOOLEAN
STATIC
IsCFOHEnabled (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  return (TRUE);
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *  InitializeCacheFlushOnHaltFeature
 *
 *    CPU feature leveling. Enable Cpu Cache Flush On Halt Function
 *
 *    @param[in]       EntryPoint       Timepoint designator.
 *    @param[in]       PlatformConfig   Contains the runtime modifiable feature input data.
 *    @param[in,out]   StdHeader        Pointer to AMD_CONFIG_PARAMS struct.
 *
 *    @return          The most severe status of any family specific service.
 */
STATIC AGESA_STATUS
InitializeCacheFlushOnHaltFeature (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32 Socket;
  UINT32 Module;
  UINT32 AndMask;
  UINT32 OrMask;
  UINT32 PciRegister;
  PCI_ADDR PciAddress;
  PCI_ADDR CfohPciAddress;
  AGESA_STATUS AgesaStatus;
  CPU_CFOH_FAMILY_SERVICES *FamilySpecificServices;

  ASSERT (IsBsp (StdHeader, &AgesaStatus));

  FamilySpecificServices = NULL;
  AndMask = 0xFFFFFFFF;
  OrMask = 0x00000000;
  PciRegister = 0;
  AgesaStatus = AGESA_SUCCESS;

  for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
    if (IsProcessorPresent (Socket, StdHeader)) {

      // Get services for the socket
      GetFeatureServicesOfSocket (&CacheFlushOnHaltFamilyServiceTable, Socket, (CONST VOID **)&FamilySpecificServices, StdHeader);
      if (FamilySpecificServices != NULL) {
        FamilySpecificServices->GetCacheFlushOnHaltRegister (FamilySpecificServices, &CfohPciAddress, &AndMask, &OrMask, StdHeader);

        // Get the Or Mask value from IDS
        IDS_OPTION_HOOK (IDS_CACHE_FLUSH_HLT, &OrMask, StdHeader);

        // Set Cache Flush On Halt register
        for (Module = 0; Module < (UINT8)GetPlatformNumberOfModules (); Module++) {
          if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &AgesaStatus)) {
            PciAddress.Address.Function = CfohPciAddress.Address.Function;
            PciAddress.Address.Register = CfohPciAddress.Address.Register;
            LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
            PciRegister &= AndMask;
            PciRegister |= OrMask;
            LibAmdPciWrite (AccessWidth32, PciAddress, &PciRegister, StdHeader);
          }
        }
      }
    }
  }
  return AgesaStatus;
}

CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureCacheFlushOnHalt =
{
  CacheFlushOnHalt,
  (CPU_FEAT_AFTER_POST_MTRR_SYNC | CPU_FEAT_AFTER_RESUME_MTRR_SYNC),
  IsCFOHEnabled,
  InitializeCacheFlushOnHaltFeature
};