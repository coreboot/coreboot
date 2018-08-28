/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of family 12h support
 *
 * This file generates the defaults tables for family 12h processors.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
 * @e \$Revision: 49967 $   @e \$Date: 2011-03-31 11:15:12 +0800 (Thu, 31 Mar 2011) $
 */
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
 *
 ***************************************************************************/

#ifndef _OPTION_FAMILY_12H_INSTALL_H_
#define _OPTION_FAMILY_12H_INSTALL_H_


#include "OptionFamily12hEarlySample.h"

/*
 * Common Family 12h routines
 */
extern F_CPU_DISABLE_PSTATE F12DisablePstate;
extern F_CPU_TRANSITION_PSTATE F12TransitionPstate;
extern F_CPU_GET_TSC_RATE F12GetTscRate;
extern F_CPU_GET_NB_FREQ F12GetCurrentNbFrequency;
extern F_CPU_GET_NB_PSTATE_INFO F12GetNbPstateInfo;
extern F_CPU_IS_NBCOF_INIT_NEEDED F12GetNbCofVidUpdate;
extern F_CPU_AP_INITIAL_LAUNCH F12LaunchApCore;
extern F_CPU_GET_IDD_MAX F12GetProcIddMax;
extern F_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE F12GetApMailboxFromHardware;
extern F_CPU_GET_AP_CORE_NUMBER F12GetApCoreNumber;
extern F_CORE_ID_POSITION_IN_INITIAL_APIC_ID F12CpuAmdCoreIdPositionInInitialApicId;
extern F_CPU_SET_DOWN_CORE_REGISTER F12SetDownCoreRegister;
extern F_CPU_SET_WARM_RESET_FLAG F12SetAgesaWarmResetFlag;
extern F_CPU_GET_WARM_RESET_FLAG F12GetAgesaWarmResetFlag;
extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF12BrandIdString1;
extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF12BrandIdString2;
extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF12CacheInfo;
extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF12SysPmTable;
extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF12WheaInitData;
//extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetEmptyArray;
extern F_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO F12GetPlatformTypeSpecificInfo;
extern CONST REGISTER_TABLE ROMDATA F12PciRegisterTable;
extern CONST REGISTER_TABLE ROMDATA F12PerCorePciRegisterTable;
extern CONST REGISTER_TABLE ROMDATA F12MsrRegisterTable;
extern F_CPU_NUMBER_OF_PHYSICAL_CORES F12GetNumberOfPhysicalCores;
extern F_GET_EARLY_INIT_TABLE GetCommonEarlyInitOnCoreTable;
extern F_IS_NB_PSTATE_ENABLED F12IsNbPstateEnabled;
#if OPTION_EARLY_SAMPLES == TRUE
  extern CONST REGISTER_TABLE ROMDATA F12EarlySampleMsrRegisterTable;
#endif

/*
 * Install family 12h model 0 support
 */

#ifdef OPTION_FAMILY12H_LN
  #if OPTION_FAMILY12H_LN == TRUE
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF12LnMicroCodePatchesStruct;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF12LnMicrocodeEquivalenceTable;

    #if USES_REGISTER_TABLES == TRUE
      CONST REGISTER_TABLE ROMDATA *F12LnRegisterTables[] =
      {
        #if BASE_FAMILY_PCI == TRUE
          &F12PciRegisterTable,
        #endif
        #if BASE_FAMILY_PCI == TRUE
          &F12PerCorePciRegisterTable,
        #endif
        #if BASE_FAMILY_MSR == TRUE
          &F12MsrRegisterTable,
          #if OPTION_EARLY_SAMPLES == TRUE
            &F12EarlySampleMsrRegisterTable,
          #endif
        #endif
        // the end.
        NULL
      };
    #endif

    #if USES_REGISTER_TABLES == TRUE
      CONST TABLE_ENTRY_TYPE_DESCRIPTOR ROMDATA F12LnTableEntryTypeDescriptors[] =
      {
        {MsrRegister, SetRegisterForMsrEntry},
        {PciRegister, SetRegisterForPciEntry},
        {FamSpecificWorkaround, SetRegisterForFamSpecificWorkaroundEntry},
        {ProfileFixup, (PF_DO_TABLE_ENTRY)CommonVoid},
        // End
        {TableEntryTypeMax, (PF_DO_TABLE_ENTRY)CommonVoid}
      };
    #endif

    CONST CPU_SPECIFIC_SERVICES ROMDATA cpuF12LnServices =
    {
      0,
      #if DISABLE_PSTATE == TRUE
        F12DisablePstate,
      #else
        (PF_CPU_DISABLE_PSTATE) CommonAssert,
      #endif
      #if TRANSITION_PSTATE == TRUE
        F12TransitionPstate,
      #else
        (PF_CPU_TRANSITION_PSTATE) CommonAssert,
      #endif
      #if PROC_IDD_MAX == TRUE
        F12GetProcIddMax,
      #else
        (PF_CPU_GET_IDD_MAX) CommonAssert,
      #endif
      #if GET_TSC_RATE == TRUE
        F12GetTscRate,
      #else
        (PF_CPU_GET_TSC_RATE) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F12GetCurrentNbFrequency,
      #else
        (PF_CPU_GET_NB_FREQ) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        (PF_CPU_GET_MIN_MAX_NB_FREQ) CommonAssert,
      #else
        (PF_CPU_GET_MIN_MAX_NB_FREQ) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F12GetNbPstateInfo,
      #else
        (PF_CPU_GET_NB_PSTATE_INFO) CommonAssert,
      #endif
      #if IS_NBCOF_INIT_NEEDED == TRUE
        F12GetNbCofVidUpdate,
      #else
        (PF_CPU_IS_NBCOF_INIT_NEEDED) CommonAssert,
      #endif
      #if GET_NB_IDD_MAX == TRUE
        (PF_CPU_GET_NB_IDD_MAX) CommonAssert,
      #else
        (PF_CPU_GET_NB_IDD_MAX) CommonAssert,
      #endif
      #if AP_INITIAL_LAUNCH == TRUE
        F12LaunchApCore,
      #else
        (PF_CPU_AP_INITIAL_LAUNCH) CommonAssert,
      #endif
      #if (BRAND_STRING1 == TRUE) || (BRAND_STRING2 == TRUE)
        F12GetNumberOfPhysicalCores,
      #else
        (PF_CPU_NUMBER_OF_PHYSICAL_CORES) CommonAssert,
      #endif
      #if GET_AP_MAILBOX_FROM_HW == TRUE
        F12GetApMailboxFromHardware,
      #else
        (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE) CommonAssert,
      #endif
      #if SET_AP_CORE_NUMBER == TRUE
        (PF_CPU_SET_AP_CORE_NUMBER) CommonVoid,
      #else
        (PF_CPU_SET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if GET_AP_CORE_NUMBER == TRUE
        F12GetApCoreNumber,
      #else
        (PF_CPU_GET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if TRANSFER_AP_CORE_NUMBER == TRUE
        (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonVoid,
      #else
        (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if ID_POSITION_INITIAL_APICID == TRUE
        F12CpuAmdCoreIdPositionInInitialApicId,
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
        F12SetAgesaWarmResetFlag,
      #else
        (PF_CPU_SET_WARM_RESET_FLAG) CommonAssert,
      #endif
      #if GET_WARM_RESET_FLAG == TRUE
        F12GetAgesaWarmResetFlag,
      #else
        (PF_CPU_GET_WARM_RESET_FLAG) CommonAssert,
      #endif
      #if BRAND_STRING1 == TRUE
        GetF12BrandIdString1,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if BRAND_STRING2 == TRUE
        GetF12BrandIdString2,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PATCHES == TRUE
        GetF12LnMicroCodePatchesStruct,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PATCHES_EQUIVALENCE_TABLE == TRUE
        GetF12LnMicrocodeEquivalenceTable,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_CACHE_INFO == TRUE
        GetF12CacheInfo,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_SYSTEM_PM_TABLE == TRUE
        GetF12SysPmTable,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_WHEA_INIT == TRUE
        GetF12WheaInitData,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PLATFORM_TYPE_SPECIFIC_INFO == TRUE
        F12GetPlatformTypeSpecificInfo,
      #else
        (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO) CommonAssert,
      #endif
      #if IS_NB_PSTATE_ENABLED == TRUE
        F12IsNbPstateEnabled,
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
        (REGISTER_TABLE **) F12LnRegisterTables,
      #else
        NULL,
      #endif
      #if USES_REGISTER_TABLES == TRUE
        (TABLE_ENTRY_TYPE_DESCRIPTOR *) F12LnTableEntryTypeDescriptors,
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
        GetCommonEarlyInitOnCoreTable
      #else
        (PF_GET_EARLY_INIT_TABLE) CommonVoid
      #endif
    };

    #define LN_SOCKETS 1
    #define LN_MODULES 1
    #define LN_RECOVERY_SOCKETS 1
    #define LN_RECOVERY_MODULES 1
    extern F_CPU_GET_SUBFAMILY_ID_ARRAY GetF12LnLogicalIdAndRev;
    #define OPT_F12_LN_ID (PF_CPU_GET_SUBFAMILY_ID_ARRAY) GetF12LnLogicalIdAndRev,
    #ifndef ADVCFG_PLATFORM_SOCKETS
      #define ADVCFG_PLATFORM_SOCKETS LN_SOCKETS
    #else
      #if ADVCFG_PLATFORM_SOCKETS < LN_SOCKETS
        #undef ADVCFG_PLATFORM_SOCKETS
        #define ADVCFG_PLATFORM_SOCKETS LN_SOCKETS
      #endif
    #endif
    #ifndef ADVCFG_PLATFORM_MODULES
      #define ADVCFG_PLATFORM_MODULES LN_MODULES
    #else
      #if ADVCFG_PLATFORM_MODULES < LN_MODULES
        #undef ADVCFG_PLATFORM_MODULES
        #define ADVCFG_PLATFORM_MODULES LN_MODULES
      #endif
    #endif

    #if GET_PATCHES == TRUE
      #define F12_LN_UCODE_02
      #define F12_LN_UCODE_0E
      #define F12_LN_UCODE_0F

      #if AGESA_ENTRY_INIT_EARLY == TRUE
        extern  CONST MICROCODE_PATCHES ROMDATA CpuF12MicrocodePatch03000027;
        #undef F12_LN_UCODE_0F
        #define F12_LN_UCODE_0F &CpuF12MicrocodePatch03000027,
        #if OPTION_EARLY_SAMPLES == TRUE
          extern  CONST MICROCODE_PATCHES ROMDATA CpuF12MicrocodePatch03000002;
          extern  CONST MICROCODE_PATCHES ROMDATA CpuF12MicrocodePatch0300000e;
          #undef F12_LN_UCODE_02
          #define F12_LN_UCODE_02 &CpuF12MicrocodePatch03000002,
          #undef F12_LN_UCODE_0E
          #define F12_LN_UCODE_0E &CpuF12MicrocodePatch0300000e,
        #endif
      #endif

      CONST MICROCODE_PATCHES ROMDATA *CpuF12LnMicroCodePatchArray[] =
      {
        F12_LN_UCODE_0F
        F12_LN_UCODE_0E
        F12_LN_UCODE_02
        NULL
      };

      CONST UINT8 ROMDATA CpuF12LnNumberOfMicrocodePatches = (UINT8) ((sizeof (CpuF12LnMicroCodePatchArray) / sizeof (CpuF12LnMicroCodePatchArray[0])) - 1);
    #endif

    #if OPTION_EARLY_SAMPLES == TRUE
      extern F_F12_ES_NB_PSTATE_INIT F12NbPstateInitEarlySampleHook;
      extern F_F12_ES_POWER_PLANE_INIT F12PowerPlaneInitEarlySampleHook;

      CONST F12_ES_CORE_SUPPORT ROMDATA F12EarlySampleCoreSupport =
      {
        #if AGESA_ENTRY_INIT_EARLY == TRUE
          F12PowerPlaneInitEarlySampleHook,
        #else
          (PF_F12_ES_POWER_PLANE_INIT) CommonAssert,
        #endif
        #if (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_RESUME == TRUE)
          F12NbPstateInitEarlySampleHook
        #else
          (PF_F12_ES_NB_PSTATE_INIT) CommonAssert
        #endif
      };
    #else
      CONST F12_ES_CORE_SUPPORT ROMDATA F12EarlySampleCoreSupport =
      {
        #if AGESA_ENTRY_INIT_EARLY == TRUE
          (PF_F12_ES_POWER_PLANE_INIT) CommonVoid,
        #else
          (PF_F12_ES_POWER_PLANE_INIT) CommonAssert,
        #endif
        #if (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_RESUME == TRUE)
          (PF_F12_ES_NB_PSTATE_INIT) CommonVoid
        #else
          (PF_F12_ES_NB_PSTATE_INIT) CommonAssert
        #endif
      };
    #endif

    #define OPT_F12_LN_CPU {AMD_FAMILY_12_LN, &cpuF12LnServices},
  #else  //  OPTION_FAMILY12H_LN == TRUE
    #define OPT_F12_LN_CPU
    #define OPT_F12_LN_ID
  #endif  //  OPTION_FAMILY12H_LN == TRUE
#else  //  defined (OPTION_FAMILY12H_LN)
  #define OPT_F12_LN_CPU
  #define OPT_F12_LN_ID
#endif  //  defined (OPTION_FAMILY12H_LN)


/*
 * Install unknown family 12h support
 */

#if USES_REGISTER_TABLES == TRUE
  CONST REGISTER_TABLE ROMDATA *F12UnknownRegisterTables[] =
  {
    #if BASE_FAMILY_PCI == TRUE
      &F12PciRegisterTable,
    #endif
    #if BASE_FAMILY_PCI == TRUE
      &F12PerCorePciRegisterTable,
    #endif
    #if BASE_FAMILY_MSR == TRUE
      &F12MsrRegisterTable,
    #endif
    // the end.
    NULL
  };
#endif

#if USES_REGISTER_TABLES == TRUE
  CONST TABLE_ENTRY_TYPE_DESCRIPTOR ROMDATA F12UnknownTableEntryTypeDescriptors[] =
  {
    {MsrRegister, SetRegisterForMsrEntry},
    {PciRegister, SetRegisterForPciEntry},
    {FamSpecificWorkaround, SetRegisterForFamSpecificWorkaroundEntry},
    {ProfileFixup, (PF_DO_TABLE_ENTRY)CommonVoid},
    // End
    {TableEntryTypeMax, (PF_DO_TABLE_ENTRY)CommonVoid}
  };
#endif

CONST CPU_SPECIFIC_SERVICES ROMDATA cpuF12UnknownServices =
{
  0,
  #if DISABLE_PSTATE == TRUE
    F12DisablePstate,
  #else
    (PF_CPU_DISABLE_PSTATE) CommonAssert,
  #endif
  #if TRANSITION_PSTATE == TRUE
    F12TransitionPstate,
  #else
    (PF_CPU_TRANSITION_PSTATE) CommonAssert,
  #endif
  #if PROC_IDD_MAX == TRUE
    F12GetProcIddMax,
  #else
    (PF_CPU_GET_IDD_MAX) CommonAssert,
  #endif
  #if GET_TSC_RATE == TRUE
    F12GetTscRate,
  #else
    (PF_CPU_GET_TSC_RATE) CommonAssert,
  #endif
  #if GET_NB_FREQ == TRUE
    F12GetCurrentNbFrequency,
  #else
    (PF_CPU_GET_NB_FREQ) CommonAssert,
  #endif
  #if GET_NB_FREQ == TRUE
    (PF_CPU_GET_MIN_MAX_NB_FREQ) CommonAssert,
  #else
    (PF_CPU_GET_MIN_MAX_NB_FREQ) CommonAssert,
  #endif
  #if GET_NB_FREQ == TRUE
    F12GetNbPstateInfo,
  #else
    (PF_CPU_GET_NB_PSTATE_INFO) CommonAssert,
  #endif
  #if IS_NBCOF_INIT_NEEDED == TRUE
    F12GetNbCofVidUpdate,
  #else
    (PF_CPU_IS_NBCOF_INIT_NEEDED) CommonAssert,
  #endif
  #if GET_NB_IDD_MAX == TRUE
    (PF_CPU_GET_NB_IDD_MAX) CommonAssert,
  #else
    (PF_CPU_GET_NB_IDD_MAX) CommonAssert,
  #endif
  #if AP_INITIAL_LAUNCH == TRUE
    F12LaunchApCore,
  #else
    (PF_CPU_AP_INITIAL_LAUNCH) CommonAssert,
  #endif
  #if (BRAND_STRING1 == TRUE) || (BRAND_STRING2 == TRUE)
    F12GetNumberOfPhysicalCores,
  #else
    (PF_CPU_NUMBER_OF_PHYSICAL_CORES) CommonAssert,
  #endif
  #if GET_AP_MAILBOX_FROM_HW == TRUE
    F12GetApMailboxFromHardware,
  #else
    (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE) CommonAssert,
  #endif
  #if SET_AP_CORE_NUMBER == TRUE
    (PF_CPU_SET_AP_CORE_NUMBER) CommonVoid,
  #else
    (PF_CPU_SET_AP_CORE_NUMBER) CommonAssert,
  #endif
  #if GET_AP_CORE_NUMBER == TRUE
    F12GetApCoreNumber,
  #else
    (PF_CPU_GET_AP_CORE_NUMBER) CommonAssert,
  #endif
  #if TRANSFER_AP_CORE_NUMBER == TRUE
    (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonVoid,
  #else
    (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonAssert,
  #endif
  #if ID_POSITION_INITIAL_APICID == TRUE
    F12CpuAmdCoreIdPositionInInitialApicId,
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
    F12SetAgesaWarmResetFlag,
  #else
    (PF_CPU_SET_WARM_RESET_FLAG) CommonAssert,
  #endif
  #if GET_WARM_RESET_FLAG == TRUE
    F12GetAgesaWarmResetFlag,
  #else
    (PF_CPU_GET_WARM_RESET_FLAG) CommonAssert,
  #endif
  #if BRAND_STRING1 == TRUE
    GetF12BrandIdString1,
  #else
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
  #endif
  #if BRAND_STRING2 == TRUE
    GetF12BrandIdString2,
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
    GetF12CacheInfo,
  #else
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
  #endif
  #if GET_SYSTEM_PM_TABLE == TRUE
    GetF12SysPmTable,
  #else
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
  #endif
  #if GET_WHEA_INIT == TRUE
    GetF12WheaInitData,
  #else
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
  #endif
  #if GET_PLATFORM_TYPE_SPECIFIC_INFO == TRUE
    F12GetPlatformTypeSpecificInfo,
  #else
    (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO) CommonAssert,
  #endif
  #if IS_NB_PSTATE_ENABLED == TRUE
    F12IsNbPstateEnabled,
  #else
    (PF_IS_NB_PSTATE_ENABLED) CommonAssert,
  #endif
  #if (BASE_FAMILY_HT_PCI == TRUE)
    (PF_NEXT_LINK_HAS_HTFPY_FEATS) CommonReturnFalse,
  #else
    (PF_NEXT_LINK_HAS_HTFPY_FEATS) CommonAssert,
  #endif
  #if (BASE_FAMILY_HT_PCI == TRUE)
    (PF_SET_HT_PHY_REGISTER) CommonVoid,
  #else
    (PF_SET_HT_PHY_REGISTER) CommonAssert,
  #endif
  #if BASE_FAMILY_PCI == TRUE
    (PF_GET_NEXT_HT_LINK_FEATURES) CommonReturnFalse,
  #else
    (PF_GET_NEXT_HT_LINK_FEATURES) CommonAssert,
  #endif
  #if USES_REGISTER_TABLES == TRUE
    (REGISTER_TABLE **) F12UnknownRegisterTables,
  #else
    NULL,
  #endif
  #if USES_REGISTER_TABLES == TRUE
    (TABLE_ENTRY_TYPE_DESCRIPTOR *) F12UnknownTableEntryTypeDescriptors,
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
    GetCommonEarlyInitOnCoreTable
  #else
    (PF_GET_EARLY_INIT_TABLE) CommonVoid
  #endif
};

 // Family 12h maximum base address is 40 bits. Limit BLDCFG to 40 bits, if appropriate.
#if (FAMILY_MMIO_BASE_MASK < 0xFFFFFF0000000000ull)
  #undef  FAMILY_MMIO_BASE_MASK
  #define FAMILY_MMIO_BASE_MASK (0xFFFFFF0000000000ull)
#endif

#undef OPT_F12_ID_TABLE
#define OPT_F12_ID_TABLE {0x12ul, {AMD_FAMILY_12, AMD_F12_UNKNOWN}, F12LogicalIdTable, (sizeof (F12LogicalIdTable) / sizeof (F12LogicalIdTable[0]))},
#define OPT_F12_UNKNOWN_CPU {AMD_FAMILY_12, &cpuF12UnknownServices},

#undef OPT_F12_TABLE
#define OPT_F12_TABLE   OPT_F12_LN_CPU  OPT_F12_UNKNOWN_CPU

#if OPTION_FS1_SOCKET_SUPPORT == TRUE
  extern CONST CPU_BRAND_TABLE ROMDATA F12LnBrandIdString1ArrayFs1;
  extern CONST CPU_BRAND_TABLE ROMDATA F12LnBrandIdString2ArrayFs1;
  #define F12_FS1_BRANDSTRING1 &F12LnBrandIdString1ArrayFs1,
  #define F12_FS1_BRANDSTRING2 &F12LnBrandIdString2ArrayFs1,
#else
  #define F12_FS1_BRANDSTRING1
  #define F12_FS1_BRANDSTRING2
#endif
#if OPTION_FM1_SOCKET_SUPPORT == TRUE
  extern CONST CPU_BRAND_TABLE ROMDATA F12LnBrandIdString1ArrayFm1;
  extern CONST CPU_BRAND_TABLE ROMDATA F12LnBrandIdString2ArrayFm1;
  #define F12_FM1_BRANDSTRING1 &F12LnBrandIdString1ArrayFm1,
  #define F12_FM1_BRANDSTRING2 &F12LnBrandIdString2ArrayFm1,
#else
  #define F12_FM1_BRANDSTRING1
  #define F12_FM1_BRANDSTRING2
#endif
#if OPTION_FP1_SOCKET_SUPPORT == TRUE
  #define F12_FP1_BRANDSTRING1 NULL,
  #define F12_FP1_BRANDSTRING2 NULL,
#else
  #define F12_FP1_BRANDSTRING1
  #define F12_FP1_BRANDSTRING2
#endif

#if BRAND_STRING1 == TRUE
  CONST CPU_BRAND_TABLE ROMDATA *F12BrandIdString1Tables[] =
  {
    F12_FS1_BRANDSTRING1
    F12_FM1_BRANDSTRING1
    F12_FP1_BRANDSTRING1
  };

  CONST UINT8 F12BrandIdString1TableCount = (sizeof (F12BrandIdString1Tables) / sizeof (F12BrandIdString1Tables[0]));
#endif

#if BRAND_STRING2 == TRUE
  CONST CPU_BRAND_TABLE ROMDATA *F12BrandIdString2Tables[] =
  {
    F12_FS1_BRANDSTRING2
    F12_FM1_BRANDSTRING2
    F12_FP1_BRANDSTRING2
  };

  CONST UINT8 F12BrandIdString2TableCount = (sizeof (F12BrandIdString2Tables) / sizeof (F12BrandIdString2Tables[0]));
#endif

CONST PF_CPU_GET_SUBFAMILY_ID_ARRAY ROMDATA F12LogicalIdTable[] =
{
  OPT_F12_LN_ID
};

#endif  // _OPTION_FAMILY_12H_INSTALL_H_
