/**
 * @file
 *
 * AMD Family_10 specific feature leveling functions.
 *
 * Provides feature leveling functions specific to family 10h.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x10
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
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuRegisters.h"
#include "cpuPostInit.h"
#include "cpuF10FeatureLeveling.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FAMILY_0X10_CPUF10FEATURELEVELING_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
BOOLEAN
STATIC
cpuFeatureListNeedUpdate (
  IN       CPU_FEATURES_LIST *globalCpuFeatureList,
  IN       CPU_FEATURES_LIST *thisCoreCpuFeatureList
  );

VOID
STATIC
updateCpuFeatureList (
  IN       CPU_FEATURES_LIST *globalCpuFeatureList,
  IN       CPU_FEATURES_LIST *thisCoreCpuFeatureList
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function get features which CPU supports.
 *
 *  @CpuServiceMethod{::F_CPU_SAVE_FEATURES}.
 *
 *  Read features from MSR_C0011004 and MSR_C0011005.
 *
 *    @param[in]      FamilySpecificServices   - Pointer to CPU_SPECIFIC_SERVICES struct.
 *    @param[in,out]  cpuFeatureList           - Pointer to CPU_FEATURES_LIST struct.
 *    @param[in]      StdHeader                - Pointer to AMD_CONFIG_PARAMS struct.
 *
 */
VOID
F10SaveFeatures (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN OUT   CPU_FEATURES_LIST *cpuFeatureList,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 CpuMsrData;
  BOOLEAN *FirstTime;
  BOOLEAN *NeedLeveling;
  CPU_F10_FEATURES *CpuF10Features;
  CPU_F10_EXT_FEATURES *CpuF10ExtFeatures;
  CPU_FEATURES_LIST thisCoreCpuFeatureList;

  FirstTime = (BOOLEAN *) ((UINT8 *) cpuFeatureList + sizeof (CPU_FEATURES_LIST));
  NeedLeveling = (BOOLEAN *) ((UINT8 *) cpuFeatureList + sizeof (CPU_FEATURES_LIST) + sizeof (BOOLEAN));

  LibAmdMemFill (&thisCoreCpuFeatureList, 0x0, sizeof (CPU_FEATURES_LIST), StdHeader);
  LibAmdMsrRead (MSR_CPUID_FEATS, &CpuMsrData, StdHeader);
  CpuF10Features = (CPU_F10_FEATURES *) &CpuMsrData;

  thisCoreCpuFeatureList.APIC = (UINT8) CpuF10Features->CpuF10FeaturesLo.APIC;
  thisCoreCpuFeatureList.CLFSH = (UINT8) CpuF10Features->CpuF10FeaturesLo.CLFSH;
  thisCoreCpuFeatureList.CMOV = (UINT8) CpuF10Features->CpuF10FeaturesLo.CMOV;
  thisCoreCpuFeatureList.CMPXCHG8B = (UINT8) CpuF10Features->CpuF10FeaturesLo.CMPXCHG8B;
  thisCoreCpuFeatureList.DE = (UINT8) CpuF10Features->CpuF10FeaturesLo.DE;
  thisCoreCpuFeatureList.FPU = (UINT8) CpuF10Features->CpuF10FeaturesLo.FPU;
  thisCoreCpuFeatureList.FXSR = (UINT8) CpuF10Features->CpuF10FeaturesLo.FXSR;
  thisCoreCpuFeatureList.HTT = (UINT8) CpuF10Features->CpuF10FeaturesLo.HTT;
  thisCoreCpuFeatureList.MCA = (UINT8) CpuF10Features->CpuF10FeaturesLo.MCA;
  thisCoreCpuFeatureList.MCE = (UINT8) CpuF10Features->CpuF10FeaturesLo.MCE;
  thisCoreCpuFeatureList.MMX = (UINT8) CpuF10Features->CpuF10FeaturesLo.MMX;
  thisCoreCpuFeatureList.MSR = (UINT8) CpuF10Features->CpuF10FeaturesLo.MSR;
  thisCoreCpuFeatureList.MTRR = (UINT8) CpuF10Features->CpuF10FeaturesLo.MTRR;
  thisCoreCpuFeatureList.PAE = (UINT8) CpuF10Features->CpuF10FeaturesLo.PAE;
  thisCoreCpuFeatureList.PAT = (UINT8) CpuF10Features->CpuF10FeaturesLo.PAT;
  thisCoreCpuFeatureList.PGE = (UINT8) CpuF10Features->CpuF10FeaturesLo.PGE;
  thisCoreCpuFeatureList.PSE = (UINT8) CpuF10Features->CpuF10FeaturesLo.PSE;
  thisCoreCpuFeatureList.PSE36 = (UINT8) CpuF10Features->CpuF10FeaturesLo.PSE36;
  thisCoreCpuFeatureList.SSE = (UINT8) CpuF10Features->CpuF10FeaturesLo.SSE;
  thisCoreCpuFeatureList.SSE2 = (UINT8) CpuF10Features->CpuF10FeaturesLo.SSE2;
  thisCoreCpuFeatureList.SysEnterSysExit = (UINT8) CpuF10Features->CpuF10FeaturesLo.SysEnterSysExit;
  thisCoreCpuFeatureList.TimeStampCounter = (UINT8) CpuF10Features->CpuF10FeaturesLo.TimeStampCounter;
  thisCoreCpuFeatureList.VME = (UINT8) CpuF10Features->CpuF10FeaturesLo.VME;

  thisCoreCpuFeatureList.CMPXCHG16B = (UINT8) CpuF10Features->CpuF10FeaturesHi.CMPXCHG16B;
  thisCoreCpuFeatureList.Monitor = (UINT8) CpuF10Features->CpuF10FeaturesHi.Monitor;
  thisCoreCpuFeatureList.POPCNT = (UINT8) CpuF10Features->CpuF10FeaturesHi.POPCNT;
  thisCoreCpuFeatureList.SSE3 = (UINT8) CpuF10Features->CpuF10FeaturesHi.SSE3;

  LibAmdMsrRead (MSR_CPUID_EXT_FEATS, &CpuMsrData, StdHeader);
  CpuF10ExtFeatures = (CPU_F10_EXT_FEATURES *) &CpuMsrData;

  thisCoreCpuFeatureList.ThreeDNow = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.ThreeDNow;
  thisCoreCpuFeatureList.ThreeDNowExt = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.ThreeDNowExt;
  thisCoreCpuFeatureList.APIC = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.APIC;
  thisCoreCpuFeatureList.CMOV = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.CMOV;
  thisCoreCpuFeatureList.CMPXCHG8B = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.CMPXCHG8B;
  thisCoreCpuFeatureList.DE = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.DE;
  thisCoreCpuFeatureList.FFXSR = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.FFXSR;
  thisCoreCpuFeatureList.FPU = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.FPU;
  thisCoreCpuFeatureList.FXSR = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.FXSR;
  thisCoreCpuFeatureList.LM = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.LM;
  thisCoreCpuFeatureList.MCA = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.MCA;
  thisCoreCpuFeatureList.MCE = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.MCE;
  thisCoreCpuFeatureList.MMX = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.MMX;
  thisCoreCpuFeatureList.MmxExt = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.MmxExt;
  thisCoreCpuFeatureList.MSR = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.MSR;
  thisCoreCpuFeatureList.MTRR = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.MTRR;
  thisCoreCpuFeatureList.NX = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.NX;
  thisCoreCpuFeatureList.PAE = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.PAE;
  thisCoreCpuFeatureList.Page1GB = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.Page1GB;
  thisCoreCpuFeatureList.PAT = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.PAT;
  thisCoreCpuFeatureList.PGE = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.PGE;
  thisCoreCpuFeatureList.PSE = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.PSE;
  thisCoreCpuFeatureList.PSE36 = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.PSE36;
  thisCoreCpuFeatureList.RDTSCP = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.RDTSCP;
  thisCoreCpuFeatureList.SysCallSysRet = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.SysCallSysRet;
  thisCoreCpuFeatureList.TimeStampCounter = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.TimeStampCounter;
  thisCoreCpuFeatureList.VME = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesLo.VME;

  thisCoreCpuFeatureList.ThreeDNowPrefetch = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesHi.ThreeDNowPrefetch;
  thisCoreCpuFeatureList.ABM = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesHi.ABM;
  thisCoreCpuFeatureList.AltMovCr8 = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesHi.AltMovCr8;
  thisCoreCpuFeatureList.CmpLegacy = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesHi.CmpLegacy;
  thisCoreCpuFeatureList.ExtApicSpace = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesHi.ExtApicSpace;
  thisCoreCpuFeatureList.IBS = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesHi.IBS;
  thisCoreCpuFeatureList.LahfSahf = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesHi.LahfSahf;
  thisCoreCpuFeatureList.MisAlignSse = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesHi.MisAlignSse;
  thisCoreCpuFeatureList.OSVW = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesHi.OSVM;
  thisCoreCpuFeatureList.SKINIT = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesHi.SKINIT;
  thisCoreCpuFeatureList.SSE4A = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesHi.SSE4A;
  thisCoreCpuFeatureList.SVM = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesHi.SVM;
  thisCoreCpuFeatureList.WDT = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesHi.WDT;
  thisCoreCpuFeatureList.NodeId = (UINT8) CpuF10ExtFeatures->CpuF10ExtFeaturesHi.NodeId;

  if (*FirstTime) {
    updateCpuFeatureList (cpuFeatureList, &thisCoreCpuFeatureList);
    *FirstTime = FALSE;
  } else if (cpuFeatureListNeedUpdate (cpuFeatureList, &thisCoreCpuFeatureList)) {
    updateCpuFeatureList (cpuFeatureList, &thisCoreCpuFeatureList);
    *NeedLeveling = TRUE;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function set features which All CPUs support.
 *
 *  @CpuServiceMethod{::F_CPU_WRITE_FEATURES}.
 *
 *  Write least common features to MSR_C0011004 and MSR_C0011005.
 *
 *    @param[in]      FamilySpecificServices   - Pointer to CPU_SPECIFIC_SERVICES struct.
 *    @param[in,out]  cpuFeatureList           - Pointer to CPU_FEATURES_LIST struct.
 *    @param[in]      StdHeader                - Pointer to AMD_CONFIG_PARAMS struct.
 *
 */
VOID
F10WriteFeatures (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN OUT   CPU_FEATURES_LIST *cpuFeatureList,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 CpuMsrData;
  CPU_F10_FEATURES *CpuF10Features;
  CPU_F10_EXT_FEATURES *CpuF10ExtFeatures;

  CpuMsrData = 0;
  CpuF10Features = (CPU_F10_FEATURES *) &CpuMsrData;

  CpuF10Features->CpuF10FeaturesLo.APIC = cpuFeatureList->APIC;
  CpuF10Features->CpuF10FeaturesLo.CLFSH = cpuFeatureList->CLFSH;
  CpuF10Features->CpuF10FeaturesLo.CMOV = cpuFeatureList->CMOV;
  CpuF10Features->CpuF10FeaturesLo.CMPXCHG8B = cpuFeatureList->CMPXCHG8B;
  CpuF10Features->CpuF10FeaturesLo.DE = cpuFeatureList->DE;
  CpuF10Features->CpuF10FeaturesLo.FPU = cpuFeatureList->FPU;
  CpuF10Features->CpuF10FeaturesLo.FXSR = cpuFeatureList->FXSR;
  CpuF10Features->CpuF10FeaturesLo.HTT = cpuFeatureList->HTT;
  CpuF10Features->CpuF10FeaturesLo.MCA = cpuFeatureList->MCA;
  CpuF10Features->CpuF10FeaturesLo.MCE = cpuFeatureList->MCE;
  CpuF10Features->CpuF10FeaturesLo.MMX = cpuFeatureList->MMX;
  CpuF10Features->CpuF10FeaturesLo.MSR = cpuFeatureList->MSR;
  CpuF10Features->CpuF10FeaturesLo.MTRR = cpuFeatureList->MTRR;
  CpuF10Features->CpuF10FeaturesLo.PAE = cpuFeatureList->PAE;
  CpuF10Features->CpuF10FeaturesLo.PAT = cpuFeatureList->PAT;
  CpuF10Features->CpuF10FeaturesLo.PGE = cpuFeatureList->PGE;
  CpuF10Features->CpuF10FeaturesLo.PSE = cpuFeatureList->PSE;
  CpuF10Features->CpuF10FeaturesLo.PSE36 = cpuFeatureList->PSE36;
  CpuF10Features->CpuF10FeaturesLo.SSE = cpuFeatureList->SSE;
  CpuF10Features->CpuF10FeaturesLo.SSE2 = cpuFeatureList->SSE2;
  CpuF10Features->CpuF10FeaturesLo.SysEnterSysExit = cpuFeatureList->SysEnterSysExit;
  CpuF10Features->CpuF10FeaturesLo.TimeStampCounter = cpuFeatureList->TimeStampCounter;
  CpuF10Features->CpuF10FeaturesLo.VME = cpuFeatureList->VME;

  CpuF10Features->CpuF10FeaturesHi.CMPXCHG16B = cpuFeatureList->CMPXCHG16B;
  CpuF10Features->CpuF10FeaturesHi.Monitor = cpuFeatureList->Monitor;
  CpuF10Features->CpuF10FeaturesHi.POPCNT = cpuFeatureList->POPCNT;
  CpuF10Features->CpuF10FeaturesHi.SSE3 = cpuFeatureList->SSE3;

  LibAmdMsrWrite (MSR_CPUID_FEATS, &CpuMsrData, StdHeader);

  CpuMsrData = 0;
  CpuF10ExtFeatures = (CPU_F10_EXT_FEATURES *) &CpuMsrData;

  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.ThreeDNow = cpuFeatureList->ThreeDNow;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.ThreeDNowExt = cpuFeatureList->ThreeDNowExt;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.APIC = cpuFeatureList->APIC;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.CMOV = cpuFeatureList->CMOV;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.CMPXCHG8B = cpuFeatureList->CMPXCHG8B;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.DE = cpuFeatureList->DE;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.FFXSR = cpuFeatureList->FFXSR;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.FPU = cpuFeatureList->FPU;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.FXSR = cpuFeatureList->FXSR;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.LM = cpuFeatureList->LM;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.MCA = cpuFeatureList->MCA;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.MCE = cpuFeatureList->MCE;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.MMX = cpuFeatureList->MMX;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.MmxExt = cpuFeatureList->MmxExt;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.MSR = cpuFeatureList->MSR;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.MTRR = cpuFeatureList->MTRR;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.NX = cpuFeatureList->NX;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.PAE = cpuFeatureList->PAE;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.Page1GB = cpuFeatureList->Page1GB;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.PAT = cpuFeatureList->PAT;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.PGE = cpuFeatureList->PGE;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.PSE = cpuFeatureList->PSE;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.PSE36 = cpuFeatureList->PSE36;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.RDTSCP = cpuFeatureList->RDTSCP;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.SysCallSysRet = cpuFeatureList->SysCallSysRet;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.TimeStampCounter = cpuFeatureList->TimeStampCounter;
  CpuF10ExtFeatures->CpuF10ExtFeaturesLo.VME = cpuFeatureList->VME;

  CpuF10ExtFeatures->CpuF10ExtFeaturesHi.ThreeDNowPrefetch = cpuFeatureList->ThreeDNowPrefetch;
  CpuF10ExtFeatures->CpuF10ExtFeaturesHi.ABM = cpuFeatureList->ABM;
  CpuF10ExtFeatures->CpuF10ExtFeaturesHi.AltMovCr8 = cpuFeatureList->AltMovCr8;
  CpuF10ExtFeatures->CpuF10ExtFeaturesHi.CmpLegacy = cpuFeatureList->CmpLegacy;
  CpuF10ExtFeatures->CpuF10ExtFeaturesHi.ExtApicSpace = cpuFeatureList->ExtApicSpace;
  CpuF10ExtFeatures->CpuF10ExtFeaturesHi.IBS = cpuFeatureList->IBS;
  CpuF10ExtFeatures->CpuF10ExtFeaturesHi.LahfSahf = cpuFeatureList->LahfSahf;
  CpuF10ExtFeatures->CpuF10ExtFeaturesHi.MisAlignSse = cpuFeatureList->MisAlignSse;
  CpuF10ExtFeatures->CpuF10ExtFeaturesHi.OSVM = cpuFeatureList->OSVW;
  CpuF10ExtFeatures->CpuF10ExtFeaturesHi.SKINIT = cpuFeatureList->SKINIT;
  CpuF10ExtFeatures->CpuF10ExtFeaturesHi.SSE4A = cpuFeatureList->SSE4A;
  CpuF10ExtFeatures->CpuF10ExtFeaturesHi.SVM = cpuFeatureList->SVM;
  CpuF10ExtFeatures->CpuF10ExtFeaturesHi.WDT = cpuFeatureList->WDT;
  CpuF10ExtFeatures->CpuF10ExtFeaturesHi.NodeId = cpuFeatureList->NodeId;
  LibAmdMsrWrite (MSR_CPUID_EXT_FEATS, &CpuMsrData, StdHeader);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  cpuFeatureListNeedUpdate
 *
 *    Compare global CPU feature list with this core feature list to see if global CPU feature list
 *    needs updated.
 *
 *    @param[in]  globalCpuFeatureList  - Pointer to global CPU Feature List.
 *    @param[in]  thisCoreCpuFeatureList  - Pointer to this core CPU Feature List.
 *
 *    @retval FALSE  globalCpuFeatureList is equal to thisCoreCpuFeatureList
 *    @retval True  globalCpuFeatureList is NOT equal to thisCoreCpuFeatureList
 */
BOOLEAN
STATIC
cpuFeatureListNeedUpdate (
  IN       CPU_FEATURES_LIST *globalCpuFeatureList,
  IN       CPU_FEATURES_LIST *thisCoreCpuFeatureList
  )
{
  BOOLEAN flag;
  UINT8 *global;
  UINT8 *thisCore;
  UINT8 i;

  flag = FALSE;
  global = (UINT8 *) globalCpuFeatureList;
  thisCore = (UINT8 *) thisCoreCpuFeatureList;

  for (i = 0; i < sizeof (CPU_FEATURES_LIST); i++) {
    if ((*global) != (*thisCore)) {
      flag = TRUE;
      break;
    }
    global++;
    thisCore++;
  }
  return flag;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  updateCpuFeatureList
 *
 *    Update global CPU feature list
 *
 *    @param[in]  globalCpuFeatureList  - Pointer to global CPU Feature List.
 *    @param[in]  thisCoreCpuFeatureList  - Pointer to this core CPU Feature List.
 *
 */
VOID
STATIC
updateCpuFeatureList (
  IN       CPU_FEATURES_LIST *globalCpuFeatureList,
  IN       CPU_FEATURES_LIST *thisCoreCpuFeatureList
  )
{
  UINT8 *globalFeatureList;
  UINT8 *thisCoreFeatureList;
  UINT32 sizeInByte;

  globalFeatureList = (UINT8 *) globalCpuFeatureList;
  thisCoreFeatureList = (UINT8 *) thisCoreCpuFeatureList;

  for (sizeInByte = 0; sizeInByte < sizeof (CPU_FEATURES_LIST); sizeInByte++) {
    *globalFeatureList &= *thisCoreFeatureList;
    globalFeatureList++;
    thisCoreFeatureList++;
  }
}

