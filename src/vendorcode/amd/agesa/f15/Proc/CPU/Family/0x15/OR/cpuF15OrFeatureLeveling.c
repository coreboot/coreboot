/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Orochi specific feature leveling functions.
 *
 * Provides feature leveling functions specific to family 15h models 00h-0Fh.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/OR
 * @e \$Revision: 45341 $   @e \$Date: 2011-01-14 15:49:18 -0700 (Fri, 14 Jan 2011) $
 *
 */
/*
 ******************************************************************************
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
#include "cpuF15OrFeatureLeveling.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_OR_CPUF15ORFEATURELEVELING_FILECODE

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
F15OrSaveFeatures (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN OUT   CPU_FEATURES_LIST *cpuFeatureList,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 CpuMsrData;
  BOOLEAN *FirstTime;
  BOOLEAN *NeedLeveling;
  CPU_F15_OR_FEATURES *CpuF15OrFeatures;
  CPU_F15_OR_EXT_FEATURES *CpuF15OrExtFeatures;
  CPU_FEATURES_LIST thisCoreCpuFeatureList;

  FirstTime = (BOOLEAN *) ((UINT8 *) cpuFeatureList + sizeof (CPU_FEATURES_LIST));
  NeedLeveling = (BOOLEAN *) ((UINT8 *) cpuFeatureList + sizeof (CPU_FEATURES_LIST) + sizeof (BOOLEAN));

  LibAmdMemFill (&thisCoreCpuFeatureList, 0x0, sizeof (CPU_FEATURES_LIST), StdHeader);
  LibAmdMsrRead (MSR_CPUID_FEATS, &CpuMsrData, StdHeader);
  CpuF15OrFeatures = (CPU_F15_OR_FEATURES *) &CpuMsrData;

  thisCoreCpuFeatureList.APIC = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.APIC;
  thisCoreCpuFeatureList.CLFSH = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.CLFSH;
  thisCoreCpuFeatureList.CMOV = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.CMOV;
  thisCoreCpuFeatureList.CMPXCHG8B = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.CMPXCHG8B;
  thisCoreCpuFeatureList.DE = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.DE;
  thisCoreCpuFeatureList.FPU = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.FPU;
  thisCoreCpuFeatureList.FXSR = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.FXSR;
  thisCoreCpuFeatureList.HTT = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.HTT;
  thisCoreCpuFeatureList.MCA = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.MCA;
  thisCoreCpuFeatureList.MCE = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.MCE;
  thisCoreCpuFeatureList.MMX = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.MMX;
  thisCoreCpuFeatureList.MSR = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.MSR;
  thisCoreCpuFeatureList.MTRR = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.MTRR;
  thisCoreCpuFeatureList.PAE = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.PAE;
  thisCoreCpuFeatureList.PAT = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.PAT;
  thisCoreCpuFeatureList.PGE = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.PGE;
  thisCoreCpuFeatureList.PSE = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.PSE;
  thisCoreCpuFeatureList.PSE36 = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.PSE36;
  thisCoreCpuFeatureList.SSE = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.SSE;
  thisCoreCpuFeatureList.SSE2 = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.SSE2;
  thisCoreCpuFeatureList.SysEnterSysExit = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.SysEnterSysExit;
  thisCoreCpuFeatureList.TimeStampCounter = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.TimeStampCounter;
  thisCoreCpuFeatureList.VME = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesLo.VME;

  thisCoreCpuFeatureList.AES = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesHi.AES;
  thisCoreCpuFeatureList.AVX = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesHi.AVX;
  thisCoreCpuFeatureList.CMPXCHG16B = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesHi.CMPXCHG16B;
  thisCoreCpuFeatureList.Monitor = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesHi.Monitor;
  thisCoreCpuFeatureList.OSXSAVE = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesHi.OSXSAVE;
  thisCoreCpuFeatureList.PCLMULQDQ = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesHi.PCLMULQDQ;
  thisCoreCpuFeatureList.POPCNT = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesHi.POPCNT;
  thisCoreCpuFeatureList.SSE3 = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesHi.SSE3;
  thisCoreCpuFeatureList.SSE41 = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesHi.SSE41;
  thisCoreCpuFeatureList.SSE42 = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesHi.SSE42;
  thisCoreCpuFeatureList.SSSE3 = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesHi.SSSE3;
  thisCoreCpuFeatureList.X2APIC = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesHi.X2APIC;
  thisCoreCpuFeatureList.XSAVE = (UINT8) CpuF15OrFeatures->CpuF15OrFeaturesHi.XSAVE;

  LibAmdMsrRead (MSR_CPUID_EXT_FEATS, &CpuMsrData, StdHeader);
  CpuF15OrExtFeatures = (CPU_F15_OR_EXT_FEATURES *) &CpuMsrData;

  thisCoreCpuFeatureList.ThreeDNow = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.ThreeDNow;
  thisCoreCpuFeatureList.ThreeDNowExt = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.ThreeDNowExt;
  thisCoreCpuFeatureList.APIC = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.APIC;
  thisCoreCpuFeatureList.CMOV = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.CMOV;
  thisCoreCpuFeatureList.CMPXCHG8B = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.CMPXCHG8B;
  thisCoreCpuFeatureList.DE = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.DE;
  thisCoreCpuFeatureList.FFXSR = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.FFXSR;
  thisCoreCpuFeatureList.FPU = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.FPU;
  thisCoreCpuFeatureList.FXSR = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.FXSR;
  thisCoreCpuFeatureList.LM = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.LM;
  thisCoreCpuFeatureList.MCA = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.MCA;
  thisCoreCpuFeatureList.MCE = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.MCE;
  thisCoreCpuFeatureList.MMX = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.MMX;
  thisCoreCpuFeatureList.MmxExt = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.MmxExt;
  thisCoreCpuFeatureList.MSR = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.MSR;
  thisCoreCpuFeatureList.MTRR = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.MTRR;
  thisCoreCpuFeatureList.NX = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.NX;
  thisCoreCpuFeatureList.PAE = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.PAE;
  thisCoreCpuFeatureList.Page1GB = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.Page1GB;
  thisCoreCpuFeatureList.PAT = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.PAT;
  thisCoreCpuFeatureList.PGE = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.PGE;
  thisCoreCpuFeatureList.PSE = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.PSE;
  thisCoreCpuFeatureList.PSE36 = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.PSE36;
  thisCoreCpuFeatureList.RDTSCP = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.RDTSCP;
  thisCoreCpuFeatureList.SysCallSysRet = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.SysCallSysRet;
  thisCoreCpuFeatureList.TimeStampCounter = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.TimeStampCounter;
  thisCoreCpuFeatureList.VME = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.VME;

  thisCoreCpuFeatureList.ThreeDNowPrefetch = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.ThreeDNowPrefetch;
  thisCoreCpuFeatureList.ABM = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.ABM;
  thisCoreCpuFeatureList.AltMovCr8 = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.AltMovCr8;
  thisCoreCpuFeatureList.CmpLegacy = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.CmpLegacy;
  thisCoreCpuFeatureList.ExtApicSpace = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.ExtApicSpace;
  thisCoreCpuFeatureList.IBS = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.IBS;
  thisCoreCpuFeatureList.LahfSahf = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.LahfSahf;
  thisCoreCpuFeatureList.MisAlignSse = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.MisAlignSse;
  thisCoreCpuFeatureList.OSVW = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.OSVM;
  thisCoreCpuFeatureList.SKINIT = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.SKINIT;
  thisCoreCpuFeatureList.SSE4A = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.SSE4A;
  thisCoreCpuFeatureList.SVM = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.SVM;
  thisCoreCpuFeatureList.WDT = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.WDT;
  thisCoreCpuFeatureList.NodeId = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.NodeId;
  thisCoreCpuFeatureList.XOP = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.XOP;
  thisCoreCpuFeatureList.TBM0 = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.TBM0;
  thisCoreCpuFeatureList.LWP = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.LWP;
  thisCoreCpuFeatureList.FMA4 = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.FMA4;
  thisCoreCpuFeatureList.TCE = (UINT8) CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.TCE;

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
F15OrWriteFeatures (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN OUT   CPU_FEATURES_LIST *cpuFeatureList,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 CpuMsrData;
  CPU_F15_OR_FEATURES *CpuF15OrFeatures;
  CPU_F15_OR_EXT_FEATURES *CpuF15OrExtFeatures;

  CpuMsrData = 0;
  CpuF15OrFeatures = (CPU_F15_OR_FEATURES *) &CpuMsrData;

  CpuF15OrFeatures->CpuF15OrFeaturesLo.APIC = cpuFeatureList->APIC;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.CLFSH = cpuFeatureList->CLFSH;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.CMOV = cpuFeatureList->CMOV;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.CMPXCHG8B = cpuFeatureList->CMPXCHG8B;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.DE = cpuFeatureList->DE;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.FPU = cpuFeatureList->FPU;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.FXSR = cpuFeatureList->FXSR;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.HTT = cpuFeatureList->HTT;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.MCA = cpuFeatureList->MCA;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.MCE = cpuFeatureList->MCE;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.MMX = cpuFeatureList->MMX;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.MSR = cpuFeatureList->MSR;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.MTRR = cpuFeatureList->MTRR;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.PAE = cpuFeatureList->PAE;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.PAT = cpuFeatureList->PAT;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.PGE = cpuFeatureList->PGE;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.PSE = cpuFeatureList->PSE;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.PSE36 = cpuFeatureList->PSE36;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.SSE = cpuFeatureList->SSE;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.SSE2 = cpuFeatureList->SSE2;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.SysEnterSysExit = cpuFeatureList->SysEnterSysExit;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.TimeStampCounter = cpuFeatureList->TimeStampCounter;
  CpuF15OrFeatures->CpuF15OrFeaturesLo.VME = cpuFeatureList->VME;

  CpuF15OrFeatures->CpuF15OrFeaturesHi.AES = cpuFeatureList->AES;
  CpuF15OrFeatures->CpuF15OrFeaturesHi.AVX = cpuFeatureList->AVX;
  CpuF15OrFeatures->CpuF15OrFeaturesHi.CMPXCHG16B = cpuFeatureList->CMPXCHG16B;
  CpuF15OrFeatures->CpuF15OrFeaturesHi.Monitor = cpuFeatureList->Monitor;
  CpuF15OrFeatures->CpuF15OrFeaturesHi.OSXSAVE = cpuFeatureList->OSXSAVE;
  CpuF15OrFeatures->CpuF15OrFeaturesHi.PCLMULQDQ = cpuFeatureList->PCLMULQDQ;
  CpuF15OrFeatures->CpuF15OrFeaturesHi.POPCNT = cpuFeatureList->POPCNT;
  CpuF15OrFeatures->CpuF15OrFeaturesHi.SSE3 = cpuFeatureList->SSE3;
  CpuF15OrFeatures->CpuF15OrFeaturesHi.SSE41 = cpuFeatureList->SSE41;
  CpuF15OrFeatures->CpuF15OrFeaturesHi.SSE42 = cpuFeatureList->SSE42;
  CpuF15OrFeatures->CpuF15OrFeaturesHi.SSSE3 = cpuFeatureList->SSSE3;
  CpuF15OrFeatures->CpuF15OrFeaturesHi.X2APIC = cpuFeatureList->X2APIC;
  CpuF15OrFeatures->CpuF15OrFeaturesHi.XSAVE = cpuFeatureList->XSAVE;

  LibAmdMsrWrite (MSR_CPUID_FEATS, &CpuMsrData, StdHeader);

  CpuMsrData = 0;
  CpuF15OrExtFeatures = (CPU_F15_OR_EXT_FEATURES *) &CpuMsrData;

  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.ThreeDNow = cpuFeatureList->ThreeDNow;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.ThreeDNowExt = cpuFeatureList->ThreeDNowExt;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.APIC = cpuFeatureList->APIC;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.CMOV = cpuFeatureList->CMOV;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.CMPXCHG8B = cpuFeatureList->CMPXCHG8B;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.DE = cpuFeatureList->DE;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.FFXSR = cpuFeatureList->FFXSR;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.FPU = cpuFeatureList->FPU;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.FXSR = cpuFeatureList->FXSR;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.LM = cpuFeatureList->LM;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.MCA = cpuFeatureList->MCA;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.MCE = cpuFeatureList->MCE;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.MMX = cpuFeatureList->MMX;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.MmxExt = cpuFeatureList->MmxExt;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.MSR = cpuFeatureList->MSR;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.MTRR = cpuFeatureList->MTRR;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.NX = cpuFeatureList->NX;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.PAE = cpuFeatureList->PAE;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.Page1GB = cpuFeatureList->Page1GB;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.PAT = cpuFeatureList->PAT;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.PGE = cpuFeatureList->PGE;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.PSE = cpuFeatureList->PSE;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.PSE36 = cpuFeatureList->PSE36;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.RDTSCP = cpuFeatureList->RDTSCP;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.SysCallSysRet = cpuFeatureList->SysCallSysRet;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.TimeStampCounter = cpuFeatureList->TimeStampCounter;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesLo.VME = cpuFeatureList->VME;

  CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.ThreeDNowPrefetch = cpuFeatureList->ThreeDNowPrefetch;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.ABM = cpuFeatureList->ABM;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.AltMovCr8 = cpuFeatureList->AltMovCr8;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.CmpLegacy = cpuFeatureList->CmpLegacy;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.ExtApicSpace = cpuFeatureList->ExtApicSpace;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.IBS = cpuFeatureList->IBS;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.LahfSahf = cpuFeatureList->LahfSahf;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.MisAlignSse = cpuFeatureList->MisAlignSse;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.OSVM = cpuFeatureList->OSVW;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.SKINIT = cpuFeatureList->SKINIT;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.SSE4A = cpuFeatureList->SSE4A;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.SVM = cpuFeatureList->SVM;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.WDT = cpuFeatureList->WDT;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.NodeId = cpuFeatureList->NodeId;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.XOP = cpuFeatureList->XOP;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.TBM0 = cpuFeatureList->TBM0;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.LWP = cpuFeatureList->LWP;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.FMA4 = cpuFeatureList->FMA4;
  CpuF15OrExtFeatures->CpuF15OrExtFeaturesHi.TCE = cpuFeatureList->TCE;

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

