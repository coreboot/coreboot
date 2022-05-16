/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of family 14h support
 *
 * This file generates the default tables for family 14h processors.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
 * @e \$Revision: 37854 $   @e \$Date: 2010-09-14 06:35:39 +0800 (Tue, 14 Sep 2010) $
 */
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

#ifndef _OPTION_FAMILY_14H_INSTALL_H_
#define _OPTION_FAMILY_14H_INSTALL_H_


#include "OptionFamily14hEarlySample.h"

/*
 * Common Family 14h routines
 */
extern F_CPU_DISABLE_PSTATE F14DisablePstate;
extern F_CPU_TRANSITION_PSTATE F14TransitionPstate;
extern F_CPU_GET_TSC_RATE F14GetTscRate;
extern F_CPU_GET_NB_FREQ F14GetCurrentNbFrequency;
extern F_CPU_GET_NB_PSTATE_INFO F14GetNbPstateInfo;
extern F_CPU_IS_NBCOF_INIT_NEEDED F14GetNbCofVidUpdate;
extern F_CPU_AP_INITIAL_LAUNCH F14LaunchApCore;
extern F_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE F14GetApMailboxFromHardware;
extern F_CPU_GET_AP_CORE_NUMBER F14GetApCoreNumber;
extern F_CORE_ID_POSITION_IN_INITIAL_APIC_ID F14CpuAmdCoreIdPositionInInitialApicId;
extern F_CPU_SET_WARM_RESET_FLAG F14SetAgesaWarmResetFlag;
extern F_CPU_GET_WARM_RESET_FLAG F14GetAgesaWarmResetFlag;
extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF14BrandIdString1;
extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF14BrandIdString2;
extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF14CacheInfo;
extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF14SysPmTable;
extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF14WheaInitData;
//extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetEmptyArray;
extern F_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO F14GetPlatformTypeSpecificInfo;
extern F_CPU_GET_IDD_MAX F14GetProcIddMax;
extern CONST REGISTER_TABLE ROMDATA F14PciRegisterTable;
extern CONST REGISTER_TABLE ROMDATA F14PerCorePciRegisterTable;
extern CONST REGISTER_TABLE ROMDATA F14MsrRegisterTable;
extern F_CPU_NUMBER_OF_BRANDSTRING_CORES F14GetNumberOfCoresForBrandstring;
extern F_GET_EARLY_INIT_TABLE GetF14OnEarlyInitOnCoreTable;
extern F_IS_NB_PSTATE_ENABLED F14IsNbPstateEnabled;
#if OPTION_EARLY_SAMPLES == TRUE
  extern CONST REGISTER_TABLE ROMDATA F14EarlySampleMsrRegisterTable;
#endif


/*
 * Install family 14h model 0 support
 */
#ifdef OPTION_FAMILY14H_ON
  #if OPTION_FAMILY14H_ON == TRUE
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF14OnMicroCodePatchesStruct;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF14OnMicrocodeEquivalenceTable;
    extern CONST REGISTER_TABLE ROMDATA F14OnPciRegisterTable;

    #if USES_REGISTER_TABLES == TRUE
      CONST REGISTER_TABLE ROMDATA * CONST F14OnRegisterTables[] =
      {
        #if BASE_FAMILY_PCI == TRUE
          &F14PciRegisterTable,
        #endif
        #if BASE_FAMILY_PCI == TRUE
          &F14PerCorePciRegisterTable,
        #endif
        #if BASE_FAMILY_MSR == TRUE
          &F14MsrRegisterTable,
          #if OPTION_EARLY_SAMPLES == TRUE
            &F14EarlySampleMsrRegisterTable,
          #endif
        #endif
        #if MODEL_SPECIFIC_PCI == TRUE
          &F14OnPciRegisterTable,
        #endif
        // the end.
        NULL
      };
    #endif

    #if USES_REGISTER_TABLES == TRUE
      CONST TABLE_ENTRY_TYPE_DESCRIPTOR ROMDATA F14OnTableEntryTypeDescriptors[] =
      {
        {MsrRegister, SetRegisterForMsrEntry},
        {PciRegister, SetRegisterForPciEntry},
        {FamSpecificWorkaround, SetRegisterForFamSpecificWorkaroundEntry},
        // End
        {TableEntryTypeMax, (PF_DO_TABLE_ENTRY)CommonVoid}
      };
    #endif

    CONST CPU_SPECIFIC_SERVICES ROMDATA cpuF14OnServices =
    {
      0,
      #if DISABLE_PSTATE == TRUE
        F14DisablePstate,
      #else
        (PF_CPU_DISABLE_PSTATE) CommonAssert,
      #endif
      #if TRANSITION_PSTATE == TRUE
        F14TransitionPstate,
      #else
        (PF_CPU_TRANSITION_PSTATE) CommonAssert,
      #endif
      #if PROC_IDD_MAX == TRUE
        F14GetProcIddMax,
      #else
        (PF_CPU_GET_IDD_MAX) CommonAssert,
      #endif
      #if GET_TSC_RATE == TRUE
        F14GetTscRate,
      #else
        (PF_CPU_GET_TSC_RATE) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F14GetCurrentNbFrequency,
      #else
        (PF_CPU_GET_NB_FREQ) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F14GetNbPstateInfo,
      #else
        (PF_CPU_GET_NB_PSTATE_INFO) CommonAssert,
      #endif
      #if IS_NBCOF_INIT_NEEDED == TRUE
        F14GetNbCofVidUpdate,
      #else
        (PF_CPU_IS_NBCOF_INIT_NEEDED) CommonAssert,
      #endif
      #if AP_INITIAL_LAUNCH == TRUE
        F14LaunchApCore,
      #else
        (PF_CPU_AP_INITIAL_LAUNCH) CommonAssert,
      #endif
      #if (BRAND_STRING1 == TRUE) || (BRAND_STRING2 == TRUE)
        F14GetNumberOfCoresForBrandstring,
      #else
        (PF_CPU_NUMBER_OF_BRANDSTRING_CORES) CommonAssert,
      #endif
      #if GET_AP_MAILBOX_FROM_HW == TRUE
        F14GetApMailboxFromHardware,
      #else
        (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE) CommonAssert,
      #endif
      #if SET_AP_CORE_NUMBER == TRUE
        (PF_CPU_SET_AP_CORE_NUMBER) CommonVoid,
      #else
        (PF_CPU_SET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if GET_AP_CORE_NUMBER == TRUE
        F14GetApCoreNumber,
      #else
        (PF_CPU_GET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if TRANSFER_AP_CORE_NUMBER == TRUE
        (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonVoid,
      #else
        (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if ID_POSITION_INITIAL_APICID == TRUE
        F14CpuAmdCoreIdPositionInInitialApicId,
      #else
        (PF_CORE_ID_POSITION_IN_INITIAL_APIC_ID) CommonAssert,
      #endif
      #if SAVE_FEATURES == TRUE
        (PF_CPU_SAVE_FEATURES) CommonVoid,
      #else
        (PF_CPU_SAVE_FEATURES) CommonAssert,
      #endif
      #if WRITE_FEATURES == TRUE
        (PF_CPU_WRITE_FEATURES) CommonVoid,
      #else
        (PF_CPU_WRITE_FEATURES) CommonAssert,
      #endif
      #if SET_WARM_RESET_FLAG == TRUE
        F14SetAgesaWarmResetFlag,
      #else
        (PF_CPU_SET_WARM_RESET_FLAG) CommonAssert,
      #endif
      #if GET_WARM_RESET_FLAG == TRUE
        F14GetAgesaWarmResetFlag,
      #else
        (PF_CPU_GET_WARM_RESET_FLAG) CommonAssert,
      #endif
      #if BRAND_STRING1 == TRUE
        GetF14BrandIdString1,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if BRAND_STRING2 == TRUE
        GetF14BrandIdString2,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PATCHES == TRUE
        GetF14OnMicroCodePatchesStruct,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PATCHES_EQUIVALENCE_TABLE == TRUE
        GetF14OnMicrocodeEquivalenceTable,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_CACHE_INFO == TRUE
        GetF14CacheInfo,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_SYSTEM_PM_TABLE == TRUE
        GetF14SysPmTable,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_WHEA_INIT == TRUE
        GetF14WheaInitData,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PLATFORM_TYPE_SPECIFIC_INFO == TRUE
        F14GetPlatformTypeSpecificInfo,
      #else
        (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO) CommonAssert,
      #endif
      #if IS_NB_PSTATE_ENABLED == TRUE
        F14IsNbPstateEnabled,
      #else
        (PF_IS_NB_PSTATE_ENABLED) CommonAssert,
      #endif
      #if (BASE_FAMILY_HT_PCI == TRUE)
        (PF_NEXT_LINK_HAS_HTFPY_FEATS) CommonReturnFalse,
      #else
        (PF_NEXT_LINK_HAS_HTFPY_FEATS) CommonReturnFalse,
      #endif
      #if (BASE_FAMILY_HT_PCI == TRUE)
        (PF_SET_HT_PHY_REGISTER) CommonVoid,
      #else
        (PF_SET_HT_PHY_REGISTER) CommonAssert,
      #endif
      #if BASE_FAMILY_PCI == TRUE
        (PF_GET_NEXT_HT_LINK_FEATURES) CommonReturnFalse,
      #else
        (PF_GET_NEXT_HT_LINK_FEATURES) CommonReturnFalse,
      #endif
      #if USES_REGISTER_TABLES == TRUE
        (REGISTER_TABLE **) F14OnRegisterTables,
      #else
        NULL,
      #endif
      #if USES_REGISTER_TABLES == TRUE
        (TABLE_ENTRY_TYPE_DESCRIPTOR *) F14OnTableEntryTypeDescriptors,
      #else
        NULL,
      #endif
      #if MODEL_SPECIFIC_HT_PCI == TRUE
        NULL,
      #else
        NULL,
      #endif
      NULL,
      InitCacheDisabled,
      #if AGESA_ENTRY_INIT_EARLY == TRUE
        GetF14OnEarlyInitOnCoreTable
      #else
        (PF_GET_EARLY_INIT_TABLE) CommonVoid
      #endif
    };

    #define ON_SOCKETS 1
    #define ON_MODULES 1
    #define ON_RECOVERY_SOCKETS 1
    #define ON_RECOVERY_MODULES 1
    extern F_CPU_GET_SUBFAMILY_ID_ARRAY GetF14OnLogicalIdAndRev;
    #define OPT_F14_ON_ID (PF_CPU_GET_SUBFAMILY_ID_ARRAY) GetF14OnLogicalIdAndRev,
    #ifndef ADVCFG_PLATFORM_SOCKETS
      #define ADVCFG_PLATFORM_SOCKETS ON_SOCKETS
    #else
      #if ADVCFG_PLATFORM_SOCKETS < ON_SOCKETS
        #undef ADVCFG_PLATFORM_SOCKETS
        #define ADVCFG_PLATFORM_SOCKETS ON_SOCKETS
      #endif
    #endif
    #ifndef ADVCFG_PLATFORM_MODULES
      #define ADVCFG_PLATFORM_MODULES ON_MODULES
    #else
      #if ADVCFG_PLATFORM_MODULES < ON_MODULES
        #undef ADVCFG_PLATFORM_MODULES
        #define ADVCFG_PLATFORM_MODULES ON_MODULES
      #endif
    #endif

    #if GET_PATCHES == TRUE
      #define F14_ON_UCODE_0B
      #define F14_ON_UCODE_1A
      #define F14_ON_UCODE_29
      #define F14_ON_UCODE_119

      #if AGESA_ENTRY_INIT_EARLY == TRUE
        #if OPTION_EARLY_SAMPLES == TRUE
          extern  CONST MICROCODE_PATCHES ROMDATA CpuF14MicrocodePatch0500000B;
          #undef F14_ON_UCODE_0B
          #define F14_ON_UCODE_0B &CpuF14MicrocodePatch0500000B,

          extern  CONST MICROCODE_PATCHES ROMDATA CpuF14MicrocodePatch0500001A;
          #undef F14_ON_UCODE_1A
          #define F14_ON_UCODE_1A &CpuF14MicrocodePatch0500001A,
        #endif
        extern  CONST MICROCODE_PATCHES ROMDATA CpuF14MicrocodePatch05000029;
        #undef F14_ON_UCODE_29
        #define F14_ON_UCODE_29 &CpuF14MicrocodePatch05000029,

	extern  CONST MICROCODE_PATCHES ROMDATA CpuF14MicrocodePatch05000119;
        #undef F14_ON_UCODE_119
        #define F14_ON_UCODE_119 &CpuF14MicrocodePatch05000119,
      #endif

      CONST MICROCODE_PATCHES ROMDATA * CONST CpuF14OnMicroCodePatchArray[] =
      {
        F14_ON_UCODE_119
        F14_ON_UCODE_29
        F14_ON_UCODE_0B
        F14_ON_UCODE_1A
        NULL
      };

      CONST UINT8 ROMDATA CpuF14OnNumberOfMicrocodePatches = (UINT8) ((sizeof (CpuF14OnMicroCodePatchArray) / sizeof (CpuF14OnMicroCodePatchArray[0])) - 1);
    #endif

    #if OPTION_EARLY_SAMPLES == TRUE
      extern F_F14_ES_GET_EARLY_INIT_TABLE GetF14OnEarlySampleEarlyInitTable;
      extern F_F14_ES_NB_PSTATE_INIT F14NbPstateInitEarlySampleHook;
      extern F_F14_ES_POWER_PLANE_INIT F14PowerPlaneInitEarlySampleHook;

      CONST F14_ES_CORE_SUPPORT ROMDATA F14EarlySampleCoreSupport =
      {
        #if AGESA_ENTRY_INIT_EARLY == TRUE
          GetF14OnEarlySampleEarlyInitTable,
          F14PowerPlaneInitEarlySampleHook,
        #else
          (PF_F14_ES_GET_EARLY_INIT_TABLE) CommonAssert,
          (PF_F14_ES_POWER_PLANE_INIT) CommonAssert,
        #endif
        #if (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_RESUME == TRUE)
          F14NbPstateInitEarlySampleHook
        #else
          (PF_F14_ES_NB_PSTATE_INIT) CommonAssert
        #endif
      };
    #else
      CONST F14_ES_CORE_SUPPORT ROMDATA F14EarlySampleCoreSupport =
      {
        #if AGESA_ENTRY_INIT_EARLY == TRUE
          (PF_F14_ES_GET_EARLY_INIT_TABLE) CommonVoid,
          (PF_F14_ES_POWER_PLANE_INIT) CommonVoid,
        #else
          (PF_F14_ES_GET_EARLY_INIT_TABLE) CommonAssert,
          (PF_F14_ES_POWER_PLANE_INIT) CommonAssert,
        #endif
        #if (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_RESUME == TRUE)
          (PF_F14_ES_NB_PSTATE_INIT) CommonVoid
        #else
          (PF_F14_ES_NB_PSTATE_INIT) CommonAssert
        #endif
      };
    #endif

    #define OPT_F14_ON_CPU {AMD_FAMILY_14_ON, &cpuF14OnServices},
  #else  //  OPTION_FAMILY14H_ON == TRUE
    #define OPT_F14_ON_CPU
    #define OPT_F14_ON_ID
  #endif  //  OPTION_FAMILY14H_ON == TRUE
#else  //  defined (OPTION_FAMILY14H_ON)
  #define OPT_F14_ON_CPU
  #define OPT_F14_ON_ID
#endif  //  defined (OPTION_FAMILY14H_ON)

/*
 * Install unknown family 14h support
 */

#if USES_REGISTER_TABLES == TRUE
  CONST REGISTER_TABLE ROMDATA * CONST F14UnknownRegisterTables[] =
  {
    #if BASE_FAMILY_PCI == TRUE
      &F14PciRegisterTable,
    #endif
    #if BASE_FAMILY_PCI == TRUE
      &F14PerCorePciRegisterTable,
    #endif
    #if BASE_FAMILY_MSR == TRUE
      &F14MsrRegisterTable,
    #endif
    // the end.
    NULL
  };
#endif

#if USES_REGISTER_TABLES == TRUE
  CONST TABLE_ENTRY_TYPE_DESCRIPTOR ROMDATA F14UnknownTableEntryTypeDescriptors[] =
  {
    {MsrRegister, SetRegisterForMsrEntry},
    {PciRegister, SetRegisterForPciEntry},
    {FamSpecificWorkaround, SetRegisterForFamSpecificWorkaroundEntry},
    // End
    {TableEntryTypeMax, (PF_DO_TABLE_ENTRY)CommonVoid}
  };
#endif

CONST CPU_SPECIFIC_SERVICES ROMDATA cpuF14UnknownServices =
{
  0,
  #if DISABLE_PSTATE == TRUE
    F14DisablePstate,
  #else
    (PF_CPU_DISABLE_PSTATE) CommonAssert,
  #endif
  #if TRANSITION_PSTATE == TRUE
    F14TransitionPstate,
  #else
    (PF_CPU_TRANSITION_PSTATE) CommonAssert,
  #endif
  #if PROC_IDD_MAX == TRUE
    (PF_CPU_GET_IDD_MAX) F14GetProcIddMax,
  #else
    (PF_CPU_GET_IDD_MAX) CommonAssert,
  #endif
  #if GET_TSC_RATE == TRUE
    F14GetTscRate,
  #else
    (PF_CPU_GET_TSC_RATE) CommonAssert,
  #endif
  #if GET_NB_FREQ == TRUE
    F14GetCurrentNbFrequency,
  #else
    (PF_CPU_GET_NB_FREQ) CommonAssert,
  #endif
  #if GET_NB_FREQ == TRUE
    F14GetNbPstateInfo,
  #else
    (PF_CPU_GET_NB_PSTATE_INFO) CommonAssert,
  #endif
  #if IS_NBCOF_INIT_NEEDED == TRUE
    F14GetNbCofVidUpdate,
  #else
    (PF_CPU_IS_NBCOF_INIT_NEEDED) CommonAssert,
  #endif
  #if AP_INITIAL_LAUNCH == TRUE
    F14LaunchApCore,
  #else
    (PF_CPU_AP_INITIAL_LAUNCH) CommonAssert,
  #endif
  #if (BRAND_STRING1 == TRUE) || (BRAND_STRING2 == TRUE)
    F14GetNumberOfCoresForBrandstring,
  #else
    (PF_CPU_NUMBER_OF_BRANDSTRING_CORES) CommonAssert,
  #endif
  #if GET_AP_MAILBOX_FROM_HW == TRUE
    F14GetApMailboxFromHardware,
  #else
    (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE) CommonAssert,
  #endif
  #if SET_AP_CORE_NUMBER == TRUE
    (PF_CPU_SET_AP_CORE_NUMBER) CommonVoid,
  #else
    (PF_CPU_SET_AP_CORE_NUMBER) CommonAssert,
  #endif
  #if GET_AP_CORE_NUMBER == TRUE
    F14GetApCoreNumber,
  #else
    (PF_CPU_GET_AP_CORE_NUMBER) CommonAssert,
  #endif
  #if TRANSFER_AP_CORE_NUMBER == TRUE
    (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonVoid,
  #else
    (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonAssert,
  #endif
  #if ID_POSITION_INITIAL_APICID == TRUE
    F14CpuAmdCoreIdPositionInInitialApicId,
  #else
    (PF_CORE_ID_POSITION_IN_INITIAL_APIC_ID) CommonAssert,
  #endif
  #if SAVE_FEATURES == TRUE
    (PF_CPU_SAVE_FEATURES) CommonVoid,
  #else
    (PF_CPU_SAVE_FEATURES) CommonAssert,
  #endif
  #if WRITE_FEATURES == TRUE
    (PF_CPU_WRITE_FEATURES) CommonVoid,
  #else
    (PF_CPU_WRITE_FEATURES) CommonAssert,
  #endif
  #if SET_WARM_RESET_FLAG == TRUE
    F14SetAgesaWarmResetFlag,
  #else
    (PF_CPU_SET_WARM_RESET_FLAG) CommonAssert,
  #endif
  #if GET_WARM_RESET_FLAG == TRUE
    F14GetAgesaWarmResetFlag,
  #else
    (PF_CPU_GET_WARM_RESET_FLAG) CommonAssert,
  #endif
  #if BRAND_STRING1 == TRUE
    GetF14BrandIdString1,
  #else
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
  #endif
  #if BRAND_STRING2 == TRUE
    GetF14BrandIdString2,
  #else
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
  #endif
  #if GET_PATCHES == TRUE
    GetEmptyArray,
  #else
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
  #endif
  #if GET_PATCHES_EQUIVALENCE_TABLE == TRUE
    GetEmptyArray,
  #else
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
  #endif
  #if GET_CACHE_INFO == TRUE
    GetF14CacheInfo,
  #else
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
  #endif
  #if GET_SYSTEM_PM_TABLE == TRUE
    GetF14SysPmTable,
  #else
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
  #endif
  #if GET_WHEA_INIT == TRUE
    GetF14WheaInitData,
  #else
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
  #endif
  #if GET_PLATFORM_TYPE_SPECIFIC_INFO == TRUE
    F14GetPlatformTypeSpecificInfo,
  #else
    (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO) CommonAssert,
  #endif
  #if IS_NB_PSTATE_ENABLED == TRUE
    F14IsNbPstateEnabled,
  #else
    (PF_IS_NB_PSTATE_ENABLED) CommonAssert,
  #endif
  #if (BASE_FAMILY_HT_PCI == TRUE)
    (PF_NEXT_LINK_HAS_HTFPY_FEATS) CommonReturnFalse,
  #else
    (PF_NEXT_LINK_HAS_HTFPY_FEATS) CommonVoid,
  #endif
  #if (BASE_FAMILY_HT_PCI == TRUE)
    (PF_SET_HT_PHY_REGISTER) CommonVoid,
  #else
    (PF_SET_HT_PHY_REGISTER) CommonVoid,
  #endif
  #if BASE_FAMILY_PCI == TRUE
    (PF_GET_NEXT_HT_LINK_FEATURES) CommonReturnFalse,
  #else
    (PF_GET_NEXT_HT_LINK_FEATURES) CommonReturnFalse,
  #endif
  #if USES_REGISTER_TABLES == TRUE
    (REGISTER_TABLE **) F14UnknownRegisterTables,
  #else
    NULL,
  #endif
  #if USES_REGISTER_TABLES == TRUE
    (TABLE_ENTRY_TYPE_DESCRIPTOR *) F14UnknownTableEntryTypeDescriptors,
  #else
    NULL,
  #endif
  #if MODEL_SPECIFIC_HT_PCI == TRUE
    NULL,
  #else
    NULL,
  #endif
  NULL,
  InitCacheDisabled,
  #if AGESA_ENTRY_INIT_EARLY == TRUE
    GetF14OnEarlyInitOnCoreTable
  #else
    (PF_GET_EARLY_INIT_TABLE) CommonVoid
  #endif
};

 // Family 14h maximum base address is 40 bits. Limit BLDCFG to 40 bits, if appropriate.
#if (FAMILY_MMIO_BASE_MASK < 0xFFFFFF0000000000ull)
  #undef  FAMILY_MMIO_BASE_MASK
  #define FAMILY_MMIO_BASE_MASK (0xFFFFFF0000000000ull)
#endif

#undef OPT_F14_ID_TABLE
#define OPT_F14_ID_TABLE {0x14, {AMD_FAMILY_14, AMD_F14_UNKNOWN}, F14LogicalIdTable, (sizeof (F14LogicalIdTable) / sizeof (F14LogicalIdTable[0]))},
#define OPT_F14_UNKNOWN_CPU {AMD_FAMILY_14, &cpuF14UnknownServices},

#undef OPT_F14_TABLE
#define OPT_F14_TABLE   OPT_F14_ON_CPU  OPT_F14_UNKNOWN_CPU

#if OPTION_FT1_SOCKET_SUPPORT == TRUE
  extern CONST CPU_BRAND_TABLE ROMDATA F14OnBrandIdString1ArrayFt1;
  extern CONST CPU_BRAND_TABLE ROMDATA F14OnBrandIdString2ArrayFt1;
  #define F14_FT1_BRANDSTRING1 &F14OnBrandIdString1ArrayFt1,
  #define F14_FT1_BRANDSTRING2 &F14OnBrandIdString2ArrayFt1,
#else
  #define F14_FT1_BRANDSTRING1
  #define F14_FT1_BRANDSTRING2
#endif

#if BRAND_STRING1 == TRUE
  CONST CPU_BRAND_TABLE ROMDATA * CONST F14BrandIdString1Tables[] =
  {
    F14_FT1_BRANDSTRING1
  };

  CONST UINT8 F14BrandIdString1TableCount = (sizeof (F14BrandIdString1Tables) / sizeof (F14BrandIdString1Tables[0]));
#endif

#if BRAND_STRING2 == TRUE
  CONST CPU_BRAND_TABLE ROMDATA *CONST F14BrandIdString2Tables[] =
  {
    F14_FT1_BRANDSTRING2
  };

  CONST UINT8 F14BrandIdString2TableCount = (sizeof (F14BrandIdString2Tables) / sizeof (F14BrandIdString2Tables[0]));
#endif

CONST PF_CPU_GET_SUBFAMILY_ID_ARRAY ROMDATA F14LogicalIdTable[] =
{
  OPT_F14_ON_ID
};

#endif  // _OPTION_FAMILY_14H_INSTALL_H_
