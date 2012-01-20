/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Orochi specific feature leveling functions.
 *
 * Provides feature leveling functions specific to family 15h.
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

#ifndef _CPU_F15_OR_FEATURE_LEVELING_H_
#define _CPU_F15_OR_FEATURE_LEVELING_H_

#include "cpuFamilyTranslation.h"
#include "cpuPostInit.h"
/*---------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */
/// F15 Orochi CPU Feature Low
typedef struct {
  UINT32 FPU:1;               ///< Bit0
  UINT32 VME:1;               ///< Bit1
  UINT32 DE:1;                ///< Bit2
  UINT32 PSE:1;               ///< Bit3
  UINT32 TimeStampCounter:1;  ///< Bit4
  UINT32 MSR:1;               ///< Bit5
  UINT32 PAE:1;               ///< Bit6
  UINT32 MCE:1;               ///< Bit7
  UINT32 CMPXCHG8B:1;         ///< Bit8
  UINT32 APIC:1;              ///< Bit9
  UINT32 :1;                  ///< Bit10
  UINT32 SysEnterSysExit:1;   ///< Bit11
  UINT32 MTRR:1;              ///< Bit12
  UINT32 PGE:1;               ///< Bit13
  UINT32 MCA:1;               ///< Bit14
  UINT32 CMOV:1;              ///< Bit15
  UINT32 PAT:1;               ///< Bit16
  UINT32 PSE36:1;             ///< Bit17
  UINT32 :1;                  ///< Bit18
  UINT32 CLFSH:1;             ///< Bit19
  UINT32 :3;                  ///< Bit20~22
  UINT32 MMX:1;               ///< Bit23
  UINT32 FXSR:1;              ///< Bit24
  UINT32 SSE:1;               ///< Bit25
  UINT32 SSE2:1;              ///< Bit26
  UINT32 :1;                  ///< Bit27
  UINT32 HTT:1;               ///< Bit28
  UINT32 :3;                  ///< Bit29~31
} CPU_F15_OR_FEATURES_LO;

/// F15 Orochi CPU Feature High
typedef struct {
  UINT32 SSE3:1;            ///< Bit0
  UINT32 PCLMULQDQ:1;       ///< Bit1
  UINT32 :1;                ///< Bit2
  UINT32 Monitor:1;         ///< Bit3
  UINT32 :5;                ///< Bit4~8
  UINT32 SSSE3:1;           ///< Bit9
  UINT32 :3;                ///< Bit10~12
  UINT32 CMPXCHG16B:1;      ///< Bit13
  UINT32 :5;                ///< Bit14~18
  UINT32 SSE41:1;           ///< Bit19
  UINT32 SSE42:1;           ///< Bit20
  UINT32 X2APIC:1;          ///< Bit21
  UINT32 :1;                ///< Bit22
  UINT32 POPCNT:1;          ///< Bit23
  UINT32 :1;                ///< Bit24
  UINT32 AES:1;             ///< Bit25
  UINT32 XSAVE:1;           ///< Bit26
  UINT32 OSXSAVE:1;         ///< Bit27
  UINT32 AVX:1;             ///< Bit28
  UINT32 :3;                ///< Bit29~32
} CPU_F15_OR_FEATURES_HI;

/// F15 Orochi CPU Feature
typedef struct {
  CPU_F15_OR_FEATURES_LO CpuF15OrFeaturesLo; ///< Low
  CPU_F15_OR_FEATURES_HI CpuF15OrFeaturesHi; ///< High
} CPU_F15_OR_FEATURES;

/// F15 Orochi CPU Extended Feature Low
typedef struct {
  UINT32 FPU:1;               ///< Bit0
  UINT32 VME:1;               ///< Bit1
  UINT32 DE:1;                ///< Bit2
  UINT32 PSE:1;               ///< Bit3
  UINT32 TimeStampCounter:1;  ///< Bit4
  UINT32 MSR:1;               ///< Bit5
  UINT32 PAE:1;               ///< Bit6
  UINT32 MCE:1;               ///< Bit7
  UINT32 CMPXCHG8B:1;         ///< Bit8
  UINT32 APIC:1;              ///< Bit9
  UINT32 :1;                  ///< Bit10
  UINT32 SysCallSysRet:1;     ///< Bit11
  UINT32 MTRR:1;              ///< Bit12
  UINT32 PGE:1;               ///< Bit13
  UINT32 MCA:1;               ///< Bit14
  UINT32 CMOV:1;              ///< Bit15
  UINT32 PAT:1;               ///< Bit16
  UINT32 PSE36:1;             ///< Bit17
  UINT32 :2;                  ///< Bit18~19
  UINT32 NX:1;                ///< Bit20
  UINT32 :1;                  ///< Bit21
  UINT32 MmxExt:1;            ///< Bit22
  UINT32 MMX:1;               ///< Bit23
  UINT32 FXSR:1;              ///< Bit24
  UINT32 FFXSR:1;             ///< Bit25
  UINT32 Page1GB:1;           ///< Bit26
  UINT32 RDTSCP:1;            ///< Bit27
  UINT32 :1;                  ///< Bit28
  UINT32 LM:1;                ///< Bit29
  UINT32 ThreeDNowExt:1;      ///< Bit30
  UINT32 ThreeDNow:1;         ///< Bit31
} CPU_F15_OR_EXT_FEATURES_LO;

/// F15 Orochi CPU Extended Feature High
typedef struct {
  UINT32 LahfSahf:1;        ///< Bit0
  UINT32 CmpLegacy:1;       ///< Bit1
  UINT32 SVM:1;             ///< Bit2
  UINT32 ExtApicSpace:1;    ///< Bit3
  UINT32 AltMovCr8:1;       ///< Bit4
  UINT32 ABM:1;             ///< Bit5
  UINT32 SSE4A:1;           ///< Bit6
  UINT32 MisAlignSse:1;     ///< Bit7
  UINT32 ThreeDNowPrefetch:1; ///< Bit8
  UINT32 OSVM:1;            ///< Bit9
  UINT32 IBS:1;             ///< Bit10
  UINT32 XOP:1;             ///< Bit11
  UINT32 SKINIT:1;          ///< Bit12
  UINT32 WDT:1;             ///< Bit13
  UINT32 TBM0:1;            ///< Bit14
  UINT32 LWP:1;             ///< Bit15
  UINT32 FMA4:1;            ///< Bit16
  UINT32 TCE:1;             ///< Bit17
  UINT32 :1;                ///< Bit18
  UINT32 NodeId:1;          ///< Bit19
  UINT32 :12;               ///< Bit20~31
} CPU_F15_OR_EXT_FEATURES_HI;

/// F15 Orochi CPU Extended Feature
typedef struct {
  CPU_F15_OR_EXT_FEATURES_LO CpuF15OrExtFeaturesLo;  ///< Low
  CPU_F15_OR_EXT_FEATURES_HI CpuF15OrExtFeaturesHi;  ///< High
} CPU_F15_OR_EXT_FEATURES;
/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */
VOID
F15OrSaveFeatures (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN OUT   CPU_FEATURES_LIST *cpuFeatureList,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
F15OrWriteFeatures (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN OUT   CPU_FEATURES_LIST *cpuFeatureList,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );
#endif  // _CPU_F15_OR_FEATURE_LEVELING_H_
