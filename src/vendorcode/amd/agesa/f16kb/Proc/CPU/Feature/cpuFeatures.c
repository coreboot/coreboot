/* $NoKeywords:$ */
/**
 * @file
 *
 * Implement general feature dispatcher.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                            M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "GeneralServices.h"
#include "cpuFeatures.h"
#include "cpuFamilyTranslation.h"
#include "heapManager.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)
#define FILECODE PROC_CPU_FEATURE_CPUFEATURES_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *            E X P O R T E D    F U N C T I O N S - External General Services API
 *----------------------------------------------------------------------------------------
 */
extern CONST CPU_FEATURE_DESCRIPTOR* ROMDATA SupportedCpuFeatureList[];

/**
 * Update the status of a specific feature
 *
 * Update the status (is enabled or not) in heap
 *
 * @param[in]       Feature        Indicates the desired feature.
 * @param[in]       IsEnabled      Indicates the desired feature
 * @param[in]       StdHeader      Standard AMD configuration parameters.
 *
 */
VOID
UpdateFeatureStatusInHeap (
  IN       DISPATCHABLE_CPU_FEATURES Feature,
  IN       BOOLEAN IsEnabled,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8 *CpuFeatureIsEnabledHeap;
  LOCATE_HEAP_PTR LocateHeap;

  ASSERT (Feature < MaxCpuFeature);

  CpuFeatureIsEnabledHeap = NULL;
  LocateHeap.BufferHandle = AMD_IS_FEATURE_ENABLED;
  if (HeapLocateBuffer (&LocateHeap, StdHeader) == AGESA_SUCCESS) {
    CpuFeatureIsEnabledHeap = LocateHeap.BufferPtr;
  }
  if (CpuFeatureIsEnabledHeap != NULL) {
    CpuFeatureIsEnabledHeap[Feature] = (UINT8) IsEnabled;
  }
}

/**
 * Determines if a specific feature is or will be enabled.
 *
 * This code traverses the feature list until a match is
 * found, then invokes the 'IsEnabled' function of the
 * feature.
 *
 * @param[in]       Feature        Indicates the desired feature.
 * @param[in]       PlatformConfig Contains the runtime modifiable feature input data.
 * @param[in]       StdHeader      Standard AMD configuration parameters.
 *
 * @retval         TRUE      Feature is or will be enabled
 * @retval         FALSE     Feature is not enabled
 */
BOOLEAN
IsFeatureEnabled (
  IN       DISPATCHABLE_CPU_FEATURES Feature,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8 *CpuFeatureIsEnabledHeap;
  UINTN i;
  LOCATE_HEAP_PTR LocateHeap;

  ASSERT (Feature < MaxCpuFeature);

  CpuFeatureIsEnabledHeap = NULL;
  LocateHeap.BufferHandle = AMD_IS_FEATURE_ENABLED;
  if (HeapLocateBuffer (&LocateHeap, StdHeader) == AGESA_SUCCESS) {
    CpuFeatureIsEnabledHeap = LocateHeap.BufferPtr;
  }
  if (CpuFeatureIsEnabledHeap != NULL) {
    if (CpuFeatureIsEnabledHeap[Feature] != CPU_FEATURE_UNDECIDED) {
      return ((BOOLEAN) CpuFeatureIsEnabledHeap[Feature]);
    }
  }

  for (i = 0; SupportedCpuFeatureList[i] != NULL; i++) {
    if (SupportedCpuFeatureList[i]->Feature == Feature) {
      return (SupportedCpuFeatureList[i]->IsEnabled (PlatformConfig, StdHeader));
    }
  }
  return FALSE;
}

/**
 * Dispatches all features needing to perform some initialization at
 * this time point.
 *
 * This routine searches the feature table for features needing to
 * run at this time point, and invokes them.
 *
 * @param[in]      EntryPoint     Timepoint designator
 * @param[in]      PlatformConfig Contains the runtime modifiable feature input data.
 * @param[in]      StdHeader      Standard AMD configuration parameters.
 *
 * @return         The most severe status of any called service.
 */
AGESA_STATUS
DispatchCpuFeatures (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT8        *CpuFeatureIsEnabledHeap;
  UINTN        i;
  BOOLEAN      FeatureIsEnabled;
  BOOLEAN      ResultDecided;
  AGESA_STATUS AgesaStatus;
  AGESA_STATUS CalledStatus;
  AGESA_STATUS IgnoredStatus;
  LOCATE_HEAP_PTR LocateHeap;
  ALLOCATE_HEAP_PARAMS AllocateHeap;


  AgesaStatus = AGESA_SUCCESS;

  CpuFeatureIsEnabledHeap = NULL;
  LocateHeap.BufferHandle = AMD_IS_FEATURE_ENABLED;
  if (HeapLocateBuffer (&LocateHeap, StdHeader) == AGESA_SUCCESS) {
    CpuFeatureIsEnabledHeap = LocateHeap.BufferPtr;
  } else {
    AllocateHeap.BufferHandle = AMD_IS_FEATURE_ENABLED;
    AllocateHeap.RequestedBufferSize = MaxCpuFeature;
    AllocateHeap.Persist = HEAP_SYSTEM_MEM;
    if (HeapAllocateBuffer (&AllocateHeap, StdHeader) == AGESA_SUCCESS) {
      CpuFeatureIsEnabledHeap = AllocateHeap.BufferPtr;
      // Initialize all feature as NOT_KNOW_YET
      LibAmdMemFill ((VOID *) CpuFeatureIsEnabledHeap, CPU_FEATURE_UNDECIDED, MaxCpuFeature, StdHeader);
    }
  }

  if (IsBsp (StdHeader, &IgnoredStatus)) {
    for (i = 0; SupportedCpuFeatureList[i] != NULL; i++) {
      if ((SupportedCpuFeatureList[i]->EntryPoint & EntryPoint) != 0) {
        ResultDecided = FALSE;
        FeatureIsEnabled = FALSE;
        /// @todo IDS need updated
        IDS_SKIP_HOOK (IDS_CPU_FEAT, (CPU_FEATURE_DESCRIPTOR *) SupportedCpuFeatureList[i], StdHeader) {
          if (CpuFeatureIsEnabledHeap != NULL) {
            // Try to get isFeatureEnabled from heap
            if (CpuFeatureIsEnabledHeap[SupportedCpuFeatureList[i]->Feature] != CPU_FEATURE_UNDECIDED) {
              FeatureIsEnabled = (BOOLEAN) (CpuFeatureIsEnabledHeap[SupportedCpuFeatureList[i]->Feature]);
              ResultDecided = TRUE;
            }
          }
          if (!ResultDecided) {
            // Get isFeatureEnabled by running IsEnabled
            FeatureIsEnabled = SupportedCpuFeatureList[i]->IsEnabled (PlatformConfig, StdHeader);
            if (CpuFeatureIsEnabledHeap != NULL) {
              CpuFeatureIsEnabledHeap[SupportedCpuFeatureList[i]->Feature] = (UINT8) FeatureIsEnabled;
            }
          }
          if (FeatureIsEnabled) {
            CalledStatus = SupportedCpuFeatureList[i]->InitializeFeature (EntryPoint, PlatformConfig, StdHeader);
            if (CalledStatus > AgesaStatus) {
              AgesaStatus = CalledStatus;
            }
          }
        }
      }
    }
  }
  return AgesaStatus;
}

/**
 * This routine checks whether any non-coherent links in the system
 * runs in HT1 mode; used to determine whether certain features
 * should be disabled when this routine returns TRUE.
 *
 * @param[in]      StdHeader  Standard AMD configuration parameters.
 *
 * @retval         TRUE       One of the non-coherent links in the
 *                            system runs in HT1 mode
 * @retval         FALSE      None of the non-coherent links in the
 *                            system is running in HT1 mode
 */
BOOLEAN
IsNonCoherentHt1 (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINTN                 Link;
  UINT32                Socket;
  UINT32                Module;
  PCI_ADDR              PciAddress;
  AGESA_STATUS          AgesaStatus;
  HT_HOST_FEATS         HtHostFeats;
  CPU_SPECIFIC_SERVICES *CpuServices;

  for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
    if (IsProcessorPresent (Socket, StdHeader)) {
      GetCpuServicesOfSocket (Socket, (CONST CPU_SPECIFIC_SERVICES **)&CpuServices, StdHeader);
      for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
        if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &AgesaStatus)) {
          HtHostFeats.HtHostValue = 0;
          Link = 0;
          while (CpuServices->GetNextHtLinkFeatures (CpuServices, &Link, &PciAddress, &HtHostFeats, StdHeader)) {
            // Return TRUE and exit routine once we find a non-coherent link in HT1
            if ((HtHostFeats.HtHostFeatures.NonCoherent == 1) && (HtHostFeats.HtHostFeatures.Ht1 == 1)) {
              return TRUE;
            }
          }
        }
      }
    }
  }

  return FALSE;
}
