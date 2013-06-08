/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Reset API, and related functions and structures.
 *
 * Contains code that initialized the CPU after early reset.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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

#ifndef _CPU_POST_INIT_H_
#define _CPU_POST_INIT_H_


/*---------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *---------------------------------------------------------------------------------------
 */
//  Forward declaration needed for multi-structure mutual references
AGESA_FORWARD_DECLARATION (CPU_CFOH_FAMILY_SERVICES);

/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */
#define P_STATE_DATA_GATHER_TEMP_ADDR     0x200000ul ///< Fixed the row data at 2M memory address.
#define GLOBAL_CPU_FEATURE_LIST_TEMP_ADDR 0x200000ul ///< Fixed the row data at 2M memory address.
/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */
//----------------------------------------------------------------------------
//                         CPU FEATURE LEVELING TYPEDEFS, STRUCTURES, ENUMS
//
//----------------------------------------------------------------------------
/// CPU FEATURE LIST
typedef struct {
  UINT8  ABM:1;             ///< byte 0 bit 0
  UINT8  AES:1;             ///< byte 0 bit 1
  UINT8  AltMovCr8:1;       ///< byte 0 bit 2
  UINT8  APIC:1;            ///< byte 0 bit 3
  UINT8  AVX:1;             ///< byte 0 bit 4
  UINT8  CLFSH:1;           ///< byte 0 bit 5
  UINT8  CMOV:1;            ///< byte 0 bit 6
  UINT8  CmpLegacy:1;       ///< byte 0 bit 7
  UINT8  CMPXCHG8B:1;       ///< byte 1 bit 0
  UINT8  CMPXCHG16B:1;      ///< byte 1 bit 1
  UINT8  F16C :1;           ///< byte 1 bit 2
  UINT8  DE:1;              ///< byte 1 bit 3
  UINT8  ExtApicSpace:1;    ///< byte 1 bit 4
  UINT8  FFXSR:1;           ///< byte 1 bit 5
  UINT8  FMA:1;             ///< byte 1 bit 6
  UINT8  FMA4:1;            ///< byte 1 bit 7
  UINT8  FPU:1;             ///< byte 2 bit 0
  UINT8  FXSR:1;            ///< byte 2 bit 1
  UINT8  HTT:1;             ///< byte 2 bit 2
  UINT8  IBS:1;             ///< byte 2 bit 3
  UINT8  LahfSahf:1;        ///< byte 2 bit 4
  UINT8  LM:1;              ///< byte 2 bit 5
  UINT8  LWP:1;             ///< byte 2 bit 6
  UINT8  MCA:1;             ///< byte 2 bit 7
  UINT8  MCE:1;             ///< byte 3 bit 0
  UINT8  MisAlignSse:1;     ///< byte 3 bit 1
  UINT8  MMX:1;             ///< byte 3 bit 2
  UINT8  MmxExt:1;          ///< byte 3 bit 3
  UINT8  Monitor:1;         ///< byte 3 bit 4
  UINT8  MSR:1;             ///< byte 3 bit 5
  UINT8  MTRR:1;            ///< byte 3 bit 6
  UINT8  NodeId:1;          ///< byte 3 bit 7
  UINT8  NX:1;              ///< byte 4 bit 0
  UINT8  OSVW:1;            ///< byte 4 bit 1
  UINT8  OSXSAVE:1;         ///< byte 4 bit 2
  UINT8  PAE:1;             ///< byte 4 bit 3
  UINT8  Page1GB:1;         ///< byte 4 bit 4
  UINT8  PAT:1;             ///< byte 4 bit 5
  UINT8  PCLMULQDQ:1;       ///< byte 4 bit 6
  UINT8  PGE:1;             ///< byte 4 bit 7
  UINT8  POPCNT:1;          ///< byte 5 bit 0
  UINT8  PSE:1;             ///< byte 5 bit 1
  UINT8  PSE36:1;           ///< byte 5 bit 2
  UINT8  RDTSCP:1;          ///< byte 5 bit 3
  UINT8  SKINIT:1;          ///< byte 5 bit 4
  UINT8  SSE:1;             ///< byte 5 bit 5
  UINT8  SSE2:1;            ///< byte 5 bit 6
  UINT8  SSE3:1;            ///< byte 5 bit 7
  UINT8  SSE4A:1;           ///< byte 6 bit 0
  UINT8  SSE41:1;           ///< byte 6 bit 1
  UINT8  SSE42:1;           ///< byte 6 bit 2
  UINT8  SSE5:1;            ///< byte 6 bit 3
  UINT8  SSSE3:1;           ///< byte 6 bit 4
  UINT8  SVM:1;             ///< byte 6 bit 5
  UINT8  SysCallSysRet:1;   ///< byte 6 bit 6
  UINT8  SysEnterSysExit:1; ///< byte 6 bit 7
  UINT8  bit56 :1;            ///< byte 7 bit 0
  UINT8  TCE:1;             ///< byte 7 bit 1
  UINT8  ThreeDNow:1;       ///< byte 7 bit 2
  UINT8  ThreeDNowExt:1;    ///< byte 7 bit 3
  UINT8  ThreeDNowPrefetch:1; ///< byte 7 bit 4
  UINT8  TimeStampCounter:1;  ///< byte 7 bit 5
  UINT8  VME:1;             ///< byte 7 bit 6
  UINT8  WDT:1;             ///< byte 7 bit 7
  UINT8  X2APIC:1;          ///< byte 8 bit 0
  UINT8  XOP:1;             ///< byte 8 bit 1
  UINT8  XSAVE:1;           ///< byte 8 bit 2
  UINT8  Reserve:5;         ///< Reserved
} CPU_FEATURES_LIST;

//----------------------------------------------------------------------------
//                         POST INIT TYPEDEFS, STRUCTURES, ENUMS
//
//----------------------------------------------------------------------------
/// BSC to AP MSR sync up
typedef struct {
  UINT32 RegisterAddress;   ///< MSR Address
  UINT64 RegisterValue;     ///< BSC's MSR Value
} BSC_AP_MSR_SYNC;

/**
 *  Set Cache Flush On Halt Register.
 *
 *  @CpuServiceInstances
 *
 * @param[in]          FamilySpecificServices   The current Family Specific Services.
 * @param[in]          EntryPoint               Timepoint designator.
 * @param[in]          PlatformConfig           Contains the runtime modifiable feature input data.
 * @param[in]          StdHeader                Handle of Header for calling lib functions and services.
 *
 */
typedef VOID (F_CPU_SET_CFOH_REG) (
  IN       CPU_CFOH_FAMILY_SERVICES     *FamilySpecificServices,
  IN       UINT64                       EntryPoint,
  IN       PLATFORM_CONFIGURATION       *PlatformConfig,
  IN       AMD_CONFIG_PARAMS            *StdHeader
  );

 /// Reference to a Method.
typedef F_CPU_SET_CFOH_REG *PF_CPU_SET_CFOH_REG;

/**
 * Provide the interface to the Cache Flush On Halt Family Specific Services.
 *
 * Use the methods or data in this struct to adapt the feature code to a specific cpu family or model (or stepping!).
 * Each supported Family must provide an implementation for all methods in this interface, even if the
 * implementation is a CommonReturn().
 */
struct _CPU_CFOH_FAMILY_SERVICES {                     // See forward reference above
  UINT16          Revision;                             ///< Interface version
  // Public Methods.
  PF_CPU_SET_CFOH_REG SetCacheFlushOnHaltRegister;    ///< Method: Set Cache Flush On Halt register.
};

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */

//                   These are   P U B L I C   functions, used by IBVs
AGESA_STATUS
AmdCpuPost (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig
  );

//                   These are   P U B L I C   functions, used by AGESA

VOID
FeatureLeveling (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
CopyHeapToTempRamAtPost (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
SyncApMsrsToBsc (
  IN OUT   BSC_AP_MSR_SYNC    *ApMsrSync,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

AGESA_STATUS
FinalizeAtPost (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
SetCoresTscFreqSel (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

AGESA_STATUS
GetPstateGatherDataAddressAtPost (
     OUT   UINT64            **Ptr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
SyncAllApMtrrToBsc (
  IN       VOID *MtrrTable,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );
#endif  // _CPU_POST_INIT_H_

