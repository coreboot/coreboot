/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of family 10h support
 *
 * This file generates the defaults tables for family 10h processors.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
 * @e \$Revision: 37150 $   @e \$Date: 2010-08-31 23:53:37 +0800 (Tue, 31 Aug 2010) $
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

#ifndef _OPTION_FAMILY_10H_INSTALL_H_
#define _OPTION_FAMILY_10H_INSTALL_H_

/*
 * Common Family 10h routines
 */
extern F_CPU_DISABLE_PSTATE F10DisablePstate;
extern F_CPU_TRANSITION_PSTATE F10TransitionPstate;
extern F_CPU_GET_TSC_RATE F10GetTscRate;
extern F_CPU_GET_NB_FREQ F10GetCurrentNbFrequency;
extern F_CPU_AP_INITIAL_LAUNCH F10LaunchApCore;
extern F_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE F10GetApMailboxFromHardware;
extern F_CPU_SET_AP_CORE_NUMBER F10SetApCoreNumber;
extern F_CPU_GET_AP_CORE_NUMBER F10GetApCoreNumber;
extern F_CPU_TRANSFER_AP_CORE_NUMBER F10TransferApCoreNumber;
extern F_CORE_ID_POSITION_IN_INITIAL_APIC_ID F10CpuAmdCoreIdPositionInInitialApicId;
extern F_CPU_SAVE_FEATURES F10SaveFeatures;
extern F_CPU_WRITE_FEATURES F10WriteFeatures;
extern F_CPU_SET_WARM_RESET_FLAG F10SetAgesaWarmResetFlag;
extern F_CPU_GET_WARM_RESET_FLAG F10GetAgesaWarmResetFlag;
extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10BrandIdString1;
extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10BrandIdString2;
extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10CacheInfo;
extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10SysPmTable;
extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10WheaInitData;
extern F_CPU_SET_CFOH_REG SetF10CacheFlushOnHaltRegister;
extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetEmptyArray;
extern F_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO F10GetPlatformTypeSpecificInfo;
extern F_NEXT_LINK_HAS_HTFPY_FEATS F10NextLinkHasHtPhyFeats;
extern F_SET_HT_PHY_REGISTER F10SetHtPhyRegister;
extern F_GET_NEXT_HT_LINK_FEATURES F10GetNextHtLinkFeatures;
extern CONST REGISTER_TABLE ROMDATA F10PciRegisterTable;
extern CONST REGISTER_TABLE ROMDATA F10MsrRegisterTable;
extern CONST REGISTER_TABLE ROMDATA F10HtPhyRegisterTable;
extern CONST REGISTER_TABLE ROMDATA F10MultiLinkPciRegisterTable;
extern CONST REGISTER_TABLE ROMDATA F10SingleLinkPciRegisterTable;
extern CONST REGISTER_TABLE ROMDATA F10WorkaroundsTable;
extern F_GET_EARLY_INIT_TABLE GetF10EarlyInitOnCoreTable;


/*
 * Install family 10h model 5 support
 */
#ifdef OPTION_FAMILY10H_BL
  #if OPTION_FAMILY10H_BL == TRUE
    extern CONST REGISTER_TABLE ROMDATA F10RevCPciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10RevCMsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10RevCHtPhyRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10BlPciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10BlMsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10BlHtPhyRegisterTable;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10BlMicroCodePatchesStruct;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10BlMicrocodeEquivalenceTable;
    extern F_CPU_GET_IDD_MAX F10CommonRevCGetProcIddMax;
    extern F_CPU_GET_NB_PSTATE_INFO F10CommonRevCGetNbPstateInfo;
    extern F_CPU_IS_NBCOF_INIT_NEEDED F10CommonRevCGetNbCofVidUpdate;
    extern F_IS_NB_PSTATE_ENABLED F10CommonRevCIsNbPstateEnabled;
    extern F_CPU_NUMBER_OF_BRANDSTRING_CORES F10CommonRevCGetNumberOfCoresForBrandstring;

    #if USES_REGISTER_TABLES == TRUE
      CONST REGISTER_TABLE ROMDATA *F10BlRegisterTables[] =
      {
        #if BASE_FAMILY_PCI == TRUE
          &F10PciRegisterTable,
        #endif
        #if MODEL_SPECIFIC_PCI == TRUE
          &F10SingleLinkPciRegisterTable,
        #endif
        #if MODEL_SPECIFIC_PCI == TRUE
          &F10RevCPciRegisterTable,
        #endif
        #if MODEL_SPECIFIC_PCI == TRUE
          &F10BlPciRegisterTable,
        #endif
        #if BASE_FAMILY_MSR == TRUE
          &F10MsrRegisterTable,
        #endif
        #if MODEL_SPECIFIC_MSR == TRUE
          &F10RevCMsrRegisterTable,
        #endif
        #if MODEL_SPECIFIC_MSR == TRUE
          &F10BlMsrRegisterTable,
        #endif
        #if MODEL_SPECIFIC_HT_PCI == TRUE
          &F10HtPhyRegisterTable,
        #endif
        #if MODEL_SPECIFIC_HT_PCI == TRUE
          &F10RevCHtPhyRegisterTable,
        #endif
        #if MODEL_SPECIFIC_HT_PCI == TRUE
          &F10BlHtPhyRegisterTable,
        #endif
        #if BASE_FAMILY_WORKAROUNDS == TRUE
          &F10WorkaroundsTable,
        #endif
        // the end.
        NULL
      };
    #endif

    #if USES_REGISTER_TABLES == TRUE
      CONST TABLE_ENTRY_TYPE_DESCRIPTOR ROMDATA F10BlTableEntryTypeDescriptors[] =
      {
        {MsrRegister, SetRegisterForMsrEntry},
        {PciRegister, SetRegisterForPciEntry},
        {FamSpecificWorkaround, SetRegisterForFamSpecificWorkaroundEntry},
        {HtPhyRegister, SetRegisterForHtPhyEntry},
        {HtPhyRangeRegister, SetRegisterForHtPhyRangeEntry},
        {DeemphasisRegister, SetRegisterForDeemphasisEntry},
        {ProfileFixup, SetRegisterForPerformanceProfileEntry},
        {HtHostPciRegister, SetRegisterForHtHostEntry},
        {HtTokenPciRegister, (PF_DO_TABLE_ENTRY)CommonVoid},
        {CoreCountsPciRegister, SetRegisterForCoreCountsPerformanceEntry},
        {ProcCountsPciRegister, SetRegisterForProcessorCountsEntry},
        {HtFeatPciRegister, SetRegisterForHtFeaturePciEntry},
        {HtPhyProfileRegister, SetRegisterForHtPhyProfileEntry},
        // End
        {TableEntryTypeMax, (PF_DO_TABLE_ENTRY)CommonVoid}
      };
    #endif

    CONST CPU_SPECIFIC_SERVICES ROMDATA cpuF10BlServices =
    {
      0,
      #if DISABLE_PSTATE == TRUE
        F10DisablePstate,
      #else
        (PF_CPU_DISABLE_PSTATE) CommonAssert,
      #endif
      #if TRANSITION_PSTATE == TRUE
        F10TransitionPstate,
      #else
        (PF_CPU_TRANSITION_PSTATE) CommonAssert,
      #endif
      #if PROC_IDD_MAX == TRUE
        F10CommonRevCGetProcIddMax,
      #else
        (PF_CPU_GET_IDD_MAX) CommonAssert,
      #endif
      #if GET_TSC_RATE == TRUE
        F10GetTscRate,
      #else
        (PF_CPU_GET_TSC_RATE) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F10GetCurrentNbFrequency,
      #else
        (PF_CPU_GET_NB_FREQ) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F10CommonRevCGetNbPstateInfo,
      #else
        (PF_CPU_GET_NB_PSTATE_INFO) CommonAssert,
      #endif
      #if IS_NBCOF_INIT_NEEDED == TRUE
        F10CommonRevCGetNbCofVidUpdate,
      #else
        (PF_CPU_IS_NBCOF_INIT_NEEDED) CommonAssert,
      #endif
      #if AP_INITIAL_LAUNCH == TRUE
        F10LaunchApCore,
      #else
        (PF_CPU_AP_INITIAL_LAUNCH) CommonAssert,
      #endif
      #if (BRAND_STRING1 == TRUE) || (BRAND_STRING2 == TRUE)
        F10CommonRevCGetNumberOfCoresForBrandstring,
      #else
        (PF_CPU_NUMBER_OF_BRANDSTRING_CORES) CommonAssert,
      #endif
      #if GET_AP_MAILBOX_FROM_HW == TRUE
        F10GetApMailboxFromHardware,
      #else
        (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE) CommonAssert,
      #endif
      #if SET_AP_CORE_NUMBER == TRUE
        F10SetApCoreNumber,
      #else
        (PF_CPU_SET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if GET_AP_CORE_NUMBER == TRUE
        F10GetApCoreNumber,
      #else
        (PF_CPU_GET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if TRANSFER_AP_CORE_NUMBER == TRUE
        F10TransferApCoreNumber,
      #else
        (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if ID_POSITION_INITIAL_APICID == TRUE
        F10CpuAmdCoreIdPositionInInitialApicId,
      #else
        (PF_CORE_ID_POSITION_IN_INITIAL_APIC_ID) CommonAssert,
      #endif
      #if SAVE_FEATURES == TRUE
        F10SaveFeatures,
      #else
        (PF_CPU_SAVE_FEATURES) CommonAssert,
      #endif
      #if WRITE_FEATURES == TRUE
        F10WriteFeatures,
      #else
        (PF_CPU_WRITE_FEATURES) CommonAssert,
      #endif
      #if SET_WARM_RESET_FLAG == TRUE
        F10SetAgesaWarmResetFlag,
      #else
        (PF_CPU_SET_WARM_RESET_FLAG) CommonAssert,
      #endif
      #if GET_WARM_RESET_FLAG == TRUE
        F10GetAgesaWarmResetFlag,
      #else
        (PF_CPU_GET_WARM_RESET_FLAG) CommonAssert,
      #endif
      #if BRAND_STRING1 == TRUE
        GetF10BrandIdString1,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if BRAND_STRING2 == TRUE
        GetF10BrandIdString2,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PATCHES == TRUE
        GetF10BlMicroCodePatchesStruct,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PATCHES_EQUIVALENCE_TABLE == TRUE
        GetF10BlMicrocodeEquivalenceTable,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_CACHE_INFO == TRUE
        GetF10CacheInfo,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_SYSTEM_PM_TABLE == TRUE
        GetF10SysPmTable,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_WHEA_INIT == TRUE
        GetF10WheaInitData,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PLATFORM_TYPE_SPECIFIC_INFO == TRUE
        F10GetPlatformTypeSpecificInfo,
      #else
        (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO) CommonAssert,
      #endif
      #if IS_NB_PSTATE_ENABLED == TRUE
        F10CommonRevCIsNbPstateEnabled,
      #else
        (PF_IS_NB_PSTATE_ENABLED) CommonAssert,
      #endif
      #if (BASE_FAMILY_HT_PCI == TRUE)
        F10NextLinkHasHtPhyFeats,
      #else
        (PF_NEXT_LINK_HAS_HTFPY_FEATS) CommonReturnFalse,
      #endif
      #if (BASE_FAMILY_HT_PCI == TRUE)
        F10SetHtPhyRegister,
      #else
        (PF_SET_HT_PHY_REGISTER) CommonAssert,
      #endif
      #if BASE_FAMILY_PCI == TRUE
        F10GetNextHtLinkFeatures,
      #else
        (PF_GET_NEXT_HT_LINK_FEATURES) CommonReturnFalse,
      #endif
      #if USES_REGISTER_TABLES == TRUE
        (REGISTER_TABLE **) F10BlRegisterTables,
      #else
        NULL,
      #endif
      #if USES_REGISTER_TABLES == TRUE
        (TABLE_ENTRY_TYPE_DESCRIPTOR *) F10BlTableEntryTypeDescriptors,
      #else
        NULL,
      #endif
      NULL,
      NULL,
      InitCacheDisabled,
      #if AGESA_ENTRY_INIT_EARLY == TRUE
        GetF10EarlyInitOnCoreTable
      #else
        (PF_GET_EARLY_INIT_TABLE) CommonVoid
      #endif
    };

    #define BL_SOCKETS 1
    #define BL_MODULES 1
    #define BL_RECOVERY_SOCKETS 1
    #define BL_RECOVERY_MODULES 1
    extern F_CPU_GET_SUBFAMILY_ID_ARRAY GetF10BlLogicalIdAndRev;
    #define OPT_F10_BL_ID (PF_CPU_GET_SUBFAMILY_ID_ARRAY) GetF10BlLogicalIdAndRev,
    #ifndef ADVCFG_PLATFORM_SOCKETS
      #define ADVCFG_PLATFORM_SOCKETS BL_SOCKETS
    #else
      #if ADVCFG_PLATFORM_SOCKETS < BL_SOCKETS
        #undef ADVCFG_PLATFORM_SOCKETS
        #define ADVCFG_PLATFORM_SOCKETS BL_SOCKETS
      #endif
    #endif
    #ifndef ADVCFG_PLATFORM_MODULES
      #define ADVCFG_PLATFORM_MODULES BL_MODULES
    #else
      #if ADVCFG_PLATFORM_MODULES < BL_MODULES
        #undef ADVCFG_PLATFORM_MODULES
        #define ADVCFG_PLATFORM_MODULES BL_MODULES
      #endif
    #endif

    #if GET_PATCHES == TRUE
      #define F10_BL_UCODE_C6
      #define F10_BL_UCODE_C8

      // If a patch is required for recovery mode to function properly, add a
      // conditional for AGESA_ENTRY_INIT_RECOVERY, and pull it in.
      #if AGESA_ENTRY_INIT_EARLY == TRUE
        #if OPTION_AM3_SOCKET_SUPPORT == TRUE
          extern  CONST MICROCODE_PATCHES ROMDATA CpuF10MicrocodePatch010000c6;
          #undef F10_BL_UCODE_C6
          #define F10_BL_UCODE_C6 &CpuF10MicrocodePatch010000c6,
        #endif
        #if (OPTION_S1G4_SOCKET_SUPPORT == TRUE) || (OPTION_AM3_SOCKET_SUPPORT == TRUE) || (OPTION_ASB2_SOCKET_SUPPORT == TRUE)
          extern  CONST MICROCODE_PATCHES ROMDATA CpuF10MicrocodePatch010000c8;
          #undef F10_BL_UCODE_C8
          #define F10_BL_UCODE_C8 &CpuF10MicrocodePatch010000c8,
        #endif
      #endif

      CONST MICROCODE_PATCHES ROMDATA *CpuF10BlMicroCodePatchArray[] =
      {
        F10_BL_UCODE_C6
        F10_BL_UCODE_C8
        NULL
      };

      CONST UINT8 ROMDATA CpuF10BlNumberOfMicrocodePatches = (UINT8) ((sizeof (CpuF10BlMicroCodePatchArray) / sizeof (CpuF10BlMicroCodePatchArray[0])) - 1);
    #endif

    #define OPT_F10_BL_CPU {AMD_FAMILY_10_BL, &cpuF10BlServices},
  #else
    #define OPT_F10_BL_CPU
    #define OPT_F10_BL_ID
  #endif
#else
  #define OPT_F10_BL_CPU
  #define OPT_F10_BL_ID
#endif

/*
 * Install family 10h model 6 support
 */
#ifdef OPTION_FAMILY10H_DA
  #if OPTION_FAMILY10H_DA == TRUE
    extern CONST REGISTER_TABLE ROMDATA F10RevCPciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10RevCMsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10RevCHtPhyRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10DaPciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10DaMsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10DaHtPhyRegisterTable;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10DaMicroCodePatchesStruct;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10DaMicrocodeEquivalenceTable;
    extern F_CPU_GET_IDD_MAX F10CommonRevCGetProcIddMax;
    extern F_CPU_GET_NB_PSTATE_INFO F10CommonRevCGetNbPstateInfo;
    extern F_CPU_IS_NBCOF_INIT_NEEDED F10CommonRevCGetNbCofVidUpdate;
    extern F_CPU_SET_CFOH_REG SetF10DaCacheFlushOnHaltRegister;
    extern F_IS_NB_PSTATE_ENABLED F10CommonRevCIsNbPstateEnabled;
    extern F_CPU_NUMBER_OF_BRANDSTRING_CORES F10CommonRevCGetNumberOfCoresForBrandstring;

    #if USES_REGISTER_TABLES == TRUE
      CONST REGISTER_TABLE ROMDATA *F10DaRegisterTables[] =
      {
        #if BASE_FAMILY_PCI == TRUE
          &F10PciRegisterTable,
        #endif
        #if MODEL_SPECIFIC_PCI == TRUE
          &F10SingleLinkPciRegisterTable,
        #endif
        #if MODEL_SPECIFIC_PCI == TRUE
          &F10RevCPciRegisterTable,
        #endif
        #if MODEL_SPECIFIC_PCI == TRUE
          &F10DaPciRegisterTable,
        #endif
        #if BASE_FAMILY_MSR == TRUE
          &F10MsrRegisterTable,
        #endif
        #if MODEL_SPECIFIC_MSR == TRUE
          &F10RevCMsrRegisterTable,
        #endif
        #if MODEL_SPECIFIC_MSR == TRUE
          &F10DaMsrRegisterTable,
        #endif
        #if MODEL_SPECIFIC_HT_PCI == TRUE
          &F10HtPhyRegisterTable,
        #endif
        #if MODEL_SPECIFIC_HT_PCI == TRUE
          &F10RevCHtPhyRegisterTable,
        #endif
        #if MODEL_SPECIFIC_HT_PCI == TRUE
          &F10DaHtPhyRegisterTable,
        #endif
        #if BASE_FAMILY_WORKAROUNDS == TRUE
          &F10WorkaroundsTable,
        #endif
        // the end.
        NULL
      };
    #endif

    #if USES_REGISTER_TABLES == TRUE
      CONST TABLE_ENTRY_TYPE_DESCRIPTOR ROMDATA F10DaTableEntryTypeDescriptors[] =
      {
        {MsrRegister, SetRegisterForMsrEntry},
        {PciRegister, SetRegisterForPciEntry},
        {FamSpecificWorkaround, SetRegisterForFamSpecificWorkaroundEntry},
        {HtPhyRegister, SetRegisterForHtPhyEntry},
        {HtPhyRangeRegister, SetRegisterForHtPhyRangeEntry},
        {DeemphasisRegister, SetRegisterForDeemphasisEntry},
        {ProfileFixup, SetRegisterForPerformanceProfileEntry},
        {HtHostPciRegister, SetRegisterForHtHostEntry},
        {HtTokenPciRegister, (PF_DO_TABLE_ENTRY)CommonVoid},
        {CoreCountsPciRegister, SetRegisterForCoreCountsPerformanceEntry},
        {ProcCountsPciRegister, SetRegisterForProcessorCountsEntry},
        {HtFeatPciRegister, SetRegisterForHtFeaturePciEntry},
        {HtPhyProfileRegister, SetRegisterForHtPhyProfileEntry},
        // End
        {TableEntryTypeMax, (PF_DO_TABLE_ENTRY)CommonVoid}
      };
    #endif

    CONST CPU_SPECIFIC_SERVICES ROMDATA cpuF10DaServices =
    {
      0,
      #if DISABLE_PSTATE == TRUE
        F10DisablePstate,
      #else
        (PF_CPU_DISABLE_PSTATE) CommonAssert,
      #endif
      #if TRANSITION_PSTATE == TRUE
        F10TransitionPstate,
      #else
        (PF_CPU_TRANSITION_PSTATE) CommonAssert,
      #endif
      #if PROC_IDD_MAX == TRUE
        F10CommonRevCGetProcIddMax,
      #else
        (PF_CPU_GET_IDD_MAX) CommonAssert,
      #endif
      #if GET_TSC_RATE == TRUE
        F10GetTscRate,
      #else
        (PF_CPU_GET_TSC_RATE) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F10GetCurrentNbFrequency,
      #else
        (PF_CPU_GET_NB_FREQ) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F10CommonRevCGetNbPstateInfo,
      #else
        (PF_CPU_GET_NB_PSTATE_INFO) CommonAssert,
      #endif
      #if IS_NBCOF_INIT_NEEDED == TRUE
        F10CommonRevCGetNbCofVidUpdate,
      #else
        (PF_CPU_IS_NBCOF_INIT_NEEDED) CommonAssert,
      #endif
      #if AP_INITIAL_LAUNCH == TRUE
        F10LaunchApCore,
      #else
        (PF_CPU_AP_INITIAL_LAUNCH) CommonAssert,
      #endif
      #if (BRAND_STRING1 == TRUE) || (BRAND_STRING2 == TRUE)
        F10CommonRevCGetNumberOfCoresForBrandstring,
      #else
        (PF_CPU_NUMBER_OF_BRANDSTRING_CORES) CommonAssert,
      #endif
      #if GET_AP_MAILBOX_FROM_HW == TRUE
        F10GetApMailboxFromHardware,
      #else
        (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE) CommonAssert,
      #endif
      #if SET_AP_CORE_NUMBER == TRUE
        F10SetApCoreNumber,
      #else
        (PF_CPU_SET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if GET_AP_CORE_NUMBER == TRUE
        F10GetApCoreNumber,
      #else
        (PF_CPU_GET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if TRANSFER_AP_CORE_NUMBER == TRUE
        F10TransferApCoreNumber,
      #else
        (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if ID_POSITION_INITIAL_APICID == TRUE
        F10CpuAmdCoreIdPositionInInitialApicId,
      #else
        (PF_CORE_ID_POSITION_IN_INITIAL_APIC_ID) CommonAssert,
      #endif
      #if SAVE_FEATURES == TRUE
        F10SaveFeatures,
      #else
        (PF_CPU_SAVE_FEATURES) CommonAssert,
      #endif
      #if WRITE_FEATURES == TRUE
        F10WriteFeatures,
      #else
        (PF_CPU_WRITE_FEATURES) CommonAssert,
      #endif
      #if SET_WARM_RESET_FLAG == TRUE
        F10SetAgesaWarmResetFlag,
      #else
        (PF_CPU_SET_WARM_RESET_FLAG) CommonAssert,
      #endif
      #if GET_WARM_RESET_FLAG == TRUE
        F10GetAgesaWarmResetFlag,
      #else
        (PF_CPU_GET_WARM_RESET_FLAG) CommonAssert,
      #endif
      #if BRAND_STRING1 == TRUE
        GetF10BrandIdString1,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if BRAND_STRING2 == TRUE
        GetF10BrandIdString2,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PATCHES == TRUE
        GetF10DaMicroCodePatchesStruct,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PATCHES_EQUIVALENCE_TABLE == TRUE
        GetF10DaMicrocodeEquivalenceTable,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_CACHE_INFO == TRUE
        GetF10CacheInfo,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_SYSTEM_PM_TABLE == TRUE
        GetF10SysPmTable,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_WHEA_INIT == TRUE
        GetF10WheaInitData,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PLATFORM_TYPE_SPECIFIC_INFO == TRUE
        F10GetPlatformTypeSpecificInfo,
      #else
        (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO) CommonAssert,
      #endif
      #if IS_NB_PSTATE_ENABLED == TRUE
        F10CommonRevCIsNbPstateEnabled,
      #else
        (PF_IS_NB_PSTATE_ENABLED) CommonAssert,
      #endif
      #if (BASE_FAMILY_HT_PCI == TRUE)
        F10NextLinkHasHtPhyFeats,
      #else
        (PF_NEXT_LINK_HAS_HTFPY_FEATS) CommonReturnFalse,
      #endif
      #if (BASE_FAMILY_HT_PCI == TRUE)
        F10SetHtPhyRegister,
      #else
        (PF_SET_HT_PHY_REGISTER) CommonAssert,
      #endif
      #if BASE_FAMILY_PCI == TRUE
        F10GetNextHtLinkFeatures,
      #else
        (PF_GET_NEXT_HT_LINK_FEATURES) CommonReturnFalse,
      #endif
      #if USES_REGISTER_TABLES == TRUE
        (REGISTER_TABLE **) F10DaRegisterTables,
      #else
        NULL,
      #endif
      #if USES_REGISTER_TABLES == TRUE
        (TABLE_ENTRY_TYPE_DESCRIPTOR *) F10DaTableEntryTypeDescriptors,
      #else
        NULL,
      #endif
      NULL,
      NULL,
      InitCacheDisabled,
      #if AGESA_ENTRY_INIT_EARLY == TRUE
        GetF10EarlyInitOnCoreTable
      #else
        (PF_GET_EARLY_INIT_TABLE) CommonVoid
      #endif
    };

    #define DA_SOCKETS 1
    #define DA_MODULES 1
    #define DA_RECOVERY_SOCKETS 1
    #define DA_RECOVERY_MODULES 1
    extern F_CPU_GET_SUBFAMILY_ID_ARRAY GetF10DaLogicalIdAndRev;
    #define OPT_F10_DA_ID (PF_CPU_GET_SUBFAMILY_ID_ARRAY) GetF10DaLogicalIdAndRev,
    #ifndef ADVCFG_PLATFORM_SOCKETS
      #define ADVCFG_PLATFORM_SOCKETS DA_SOCKETS
    #else
      #if ADVCFG_PLATFORM_SOCKETS < DA_SOCKETS
        #undef ADVCFG_PLATFORM_SOCKETS
        #define ADVCFG_PLATFORM_SOCKETS DA_SOCKETS
      #endif
    #endif
    #ifndef ADVCFG_PLATFORM_MODULES
      #define ADVCFG_PLATFORM_MODULES DA_MODULES
    #else
      #if ADVCFG_PLATFORM_MODULES < DA_MODULES
        #undef ADVCFG_PLATFORM_MODULES
        #define ADVCFG_PLATFORM_MODULES DA_MODULES
      #endif
    #endif

    #if GET_PATCHES == TRUE
      #define F10_DA_UCODE_C7
      #define F10_DA_UCODE_C8

      // If a patch is required for recovery mode to function properly, add a
      // conditional for AGESA_ENTRY_INIT_RECOVERY, and pull it in.
      #if AGESA_ENTRY_INIT_EARLY == TRUE
        #if (OPTION_S1G3_SOCKET_SUPPORT == TRUE) || (OPTION_AM3_SOCKET_SUPPORT == TRUE)
          extern  CONST MICROCODE_PATCHES ROMDATA CpuF10MicrocodePatch010000c7;
          #undef F10_DA_UCODE_C7
          #define F10_DA_UCODE_C7 &CpuF10MicrocodePatch010000c7,
        #endif
        #if (OPTION_S1G4_SOCKET_SUPPORT == TRUE) || (OPTION_AM3_SOCKET_SUPPORT == TRUE) || (OPTION_ASB2_SOCKET_SUPPORT == TRUE)
          extern  CONST MICROCODE_PATCHES ROMDATA CpuF10MicrocodePatch010000c8;
          #undef F10_DA_UCODE_C8
          #define F10_DA_UCODE_C8 &CpuF10MicrocodePatch010000c8,
        #endif
      #endif

      CONST MICROCODE_PATCHES ROMDATA *CpuF10DaMicroCodePatchArray[] =
      {
        F10_DA_UCODE_C7
        F10_DA_UCODE_C8
        NULL
      };

      CONST UINT8 ROMDATA CpuF10DaNumberOfMicrocodePatches = (UINT8) ((sizeof (CpuF10DaMicroCodePatchArray) / sizeof (CpuF10DaMicroCodePatchArray[0])) - 1);
    #endif

    #define OPT_F10_DA_CPU {AMD_FAMILY_10_DA, &cpuF10DaServices},
  #else
    #define OPT_F10_DA_CPU
    #define OPT_F10_DA_ID
  #endif
#else
  #define OPT_F10_DA_CPU
  #define OPT_F10_DA_ID
#endif

/*
 * Install family 10h models 8 & 9 support
 */
#ifdef OPTION_FAMILY10H_HY
  #if OPTION_FAMILY10H_HY == TRUE
    extern CONST REGISTER_TABLE ROMDATA F10HyPciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10HyMsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10HyHtPhyRegisterTable;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10HyMicroCodePatchesStruct;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10HyMicrocodeEquivalenceTable;
    extern F_CPU_GET_IDD_MAX F10CommonRevDGetProcIddMax;
    extern F_CPU_GET_NB_PSTATE_INFO F10CommonRevDGetNbPstateInfo;
    extern F_CPU_IS_NBCOF_INIT_NEEDED F10CommonRevDGetNbCofVidUpdate;
    extern CONST PACKAGE_HTLINK_MAP_ITEM ROMDATA HtFam10RevDPackageLinkMap[];
    extern F_CPU_NUMBER_OF_BRANDSTRING_CORES F10CommonRevDGetNumberOfCoresForBrandstring;
    extern F_GET_EARLY_INIT_TABLE GetF10HyEarlyInitOnCoreTable;

    #if USES_REGISTER_TABLES == TRUE
      CONST REGISTER_TABLE ROMDATA *F10HyRegisterTables[] =
      {
        #if BASE_FAMILY_PCI == TRUE
          &F10PciRegisterTable,
        #endif
        #if MODEL_SPECIFIC_PCI == TRUE
          &F10MultiLinkPciRegisterTable,
        #endif
        #if MODEL_SPECIFIC_PCI == TRUE
          &F10HyPciRegisterTable,
        #endif
        #if BASE_FAMILY_MSR == TRUE
          &F10MsrRegisterTable,
        #endif
        #if MODEL_SPECIFIC_MSR == TRUE
          &F10HyMsrRegisterTable,
        #endif
        #if MODEL_SPECIFIC_HT_PCI == TRUE
          &F10HtPhyRegisterTable,
        #endif
        #if MODEL_SPECIFIC_HT_PCI == TRUE
          &F10HyHtPhyRegisterTable,
        #endif
        #if BASE_FAMILY_WORKAROUNDS == TRUE
          &F10WorkaroundsTable,
        #endif
        // the end.
        NULL
      };
    #endif

    #if USES_REGISTER_TABLES == TRUE
      CONST TABLE_ENTRY_TYPE_DESCRIPTOR ROMDATA F10HyTableEntryTypeDescriptors[] =
      {
        {MsrRegister, SetRegisterForMsrEntry},
        {PciRegister, SetRegisterForPciEntry},
        {FamSpecificWorkaround, SetRegisterForFamSpecificWorkaroundEntry},
        {HtPhyRegister, SetRegisterForHtPhyEntry},
        {HtPhyRangeRegister, SetRegisterForHtPhyRangeEntry},
        {DeemphasisRegister, SetRegisterForDeemphasisEntry},
        {HtPhyFreqRegister, SetRegisterForHtPhyFreqEntry},
        {ProfileFixup, SetRegisterForPerformanceProfileEntry},
        {HtHostPciRegister, SetRegisterForHtHostEntry},
        {HtTokenPciRegister, SetRegisterForHtLinkTokenEntry},
        {CoreCountsPciRegister, SetRegisterForCoreCountsPerformanceEntry},
        {ProcCountsPciRegister, SetRegisterForProcessorCountsEntry},
        {TokenPciRegister, SetRegisterForTokenPciEntry},
        {HtFeatPciRegister, SetRegisterForHtFeaturePciEntry},
        {HtLinkPciRegister, SetRegisterForHtLinkPciEntry},
        // End
        {TableEntryTypeMax, (PF_DO_TABLE_ENTRY)CommonVoid}
      };
    #endif

    CONST CPU_SPECIFIC_SERVICES ROMDATA cpuF10HyServices =
    {
      0,
      #if DISABLE_PSTATE == TRUE
        F10DisablePstate,
      #else
        (PF_CPU_DISABLE_PSTATE) CommonAssert,
      #endif
      #if TRANSITION_PSTATE == TRUE
        F10TransitionPstate,
      #else
        (PF_CPU_TRANSITION_PSTATE) CommonAssert,
      #endif
      #if PROC_IDD_MAX == TRUE
        F10CommonRevDGetProcIddMax,
      #else
        (PF_CPU_GET_IDD_MAX) CommonAssert,
      #endif
      #if GET_TSC_RATE == TRUE
        F10GetTscRate,
      #else
        (PF_CPU_GET_TSC_RATE) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F10GetCurrentNbFrequency,
      #else
        (PF_CPU_GET_NB_FREQ) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F10CommonRevDGetNbPstateInfo,
      #else
        (PF_CPU_GET_NB_PSTATE_INFO) CommonAssert,
      #endif
      #if IS_NBCOF_INIT_NEEDED == TRUE
        F10CommonRevDGetNbCofVidUpdate,
      #else
        (PF_CPU_IS_NBCOF_INIT_NEEDED) CommonAssert,
      #endif
      #if AP_INITIAL_LAUNCH == TRUE
        F10LaunchApCore,
      #else
        (PF_CPU_AP_INITIAL_LAUNCH) CommonAssert,
      #endif
      #if (BRAND_STRING1 == TRUE) || (BRAND_STRING2 == TRUE)
        F10CommonRevDGetNumberOfCoresForBrandstring,
      #else
        (PF_CPU_NUMBER_OF_BRANDSTRING_CORES) CommonAssert,
      #endif
      #if GET_AP_MAILBOX_FROM_HW == TRUE
        F10GetApMailboxFromHardware,
      #else
        (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE) CommonAssert,
      #endif
      #if SET_AP_CORE_NUMBER == TRUE
        F10SetApCoreNumber,
      #else
        (PF_CPU_SET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if GET_AP_CORE_NUMBER == TRUE
        F10GetApCoreNumber,
      #else
        (PF_CPU_GET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if TRANSFER_AP_CORE_NUMBER == TRUE
        F10TransferApCoreNumber,
      #else
        (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if ID_POSITION_INITIAL_APICID == TRUE
        F10CpuAmdCoreIdPositionInInitialApicId,
      #else
        (PF_CORE_ID_POSITION_IN_INITIAL_APIC_ID) CommonAssert,
      #endif
      #if SAVE_FEATURES == TRUE
        F10SaveFeatures,
      #else
        (PF_CPU_SAVE_FEATURES) CommonAssert,
      #endif
      #if WRITE_FEATURES == TRUE
        F10WriteFeatures,
      #else
        (PF_CPU_WRITE_FEATURES) CommonAssert,
      #endif
      #if SET_WARM_RESET_FLAG == TRUE
        F10SetAgesaWarmResetFlag,
      #else
        (PF_CPU_SET_WARM_RESET_FLAG) CommonAssert,
      #endif
      #if GET_WARM_RESET_FLAG == TRUE
        F10GetAgesaWarmResetFlag,
      #else
        (PF_CPU_GET_WARM_RESET_FLAG) CommonAssert,
      #endif
      #if BRAND_STRING1 == TRUE
        GetF10BrandIdString1,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if BRAND_STRING2 == TRUE
        GetF10BrandIdString2,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PATCHES == TRUE
        GetF10HyMicroCodePatchesStruct,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PATCHES_EQUIVALENCE_TABLE == TRUE
        GetF10HyMicrocodeEquivalenceTable,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_CACHE_INFO == TRUE
        GetF10CacheInfo,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_SYSTEM_PM_TABLE == TRUE
        GetF10SysPmTable,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_WHEA_INIT == TRUE
        GetF10WheaInitData,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PLATFORM_TYPE_SPECIFIC_INFO == TRUE
        F10GetPlatformTypeSpecificInfo,
      #else
        (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO) CommonAssert,
      #endif
      #if IS_NB_PSTATE_ENABLED == TRUE
        (PF_IS_NB_PSTATE_ENABLED) CommonReturnFalse,
      #else
        (PF_IS_NB_PSTATE_ENABLED) CommonAssert,
      #endif
      #if (BASE_FAMILY_HT_PCI == TRUE)
        F10NextLinkHasHtPhyFeats,
      #else
        (PF_NEXT_LINK_HAS_HTFPY_FEATS) CommonReturnFalse,
      #endif
      #if (BASE_FAMILY_HT_PCI == TRUE)
        F10SetHtPhyRegister,
      #else
        (PF_SET_HT_PHY_REGISTER) CommonAssert,
      #endif
      #if BASE_FAMILY_PCI == TRUE
        F10GetNextHtLinkFeatures,
      #else
        (PF_GET_NEXT_HT_LINK_FEATURES) CommonReturnFalse,
      #endif
      #if USES_REGISTER_TABLES == TRUE
        (REGISTER_TABLE **) F10HyRegisterTables,
      #else
        NULL,
      #endif
      #if USES_REGISTER_TABLES == TRUE
        (TABLE_ENTRY_TYPE_DESCRIPTOR *) F10HyTableEntryTypeDescriptors,
      #else
        NULL,
      #endif
      #if MODEL_SPECIFIC_HT_PCI == TRUE
        (PACKAGE_HTLINK_MAP) &HtFam10RevDPackageLinkMap,
      #else
        NULL,
      #endif
      NULL,
      InitCacheDisabled,
      #if AGESA_ENTRY_INIT_EARLY == TRUE
        #if OPTION_C32_SOCKET_SUPPORT == TRUE
          GetF10HyEarlyInitOnCoreTable
        #else
          GetF10EarlyInitOnCoreTable
        #endif
      #else
        (PF_GET_EARLY_INIT_TABLE) CommonVoid
      #endif
    };

    #define HY_SOCKETS 8
    #if (OPTION_G34_SOCKET_SUPPORT == TRUE)
      #define HY_MODULES 2
    #else
      #define HY_MODULES 1
    #endif
    #define HY_RECOVERY_SOCKETS 1
    #define HY_RECOVERY_MODULES 1
    extern F_CPU_GET_SUBFAMILY_ID_ARRAY GetF10HyLogicalIdAndRev;
    #define OPT_F10_HY_ID (PF_CPU_GET_SUBFAMILY_ID_ARRAY) GetF10HyLogicalIdAndRev,
    #ifndef ADVCFG_PLATFORM_SOCKETS
      #define ADVCFG_PLATFORM_SOCKETS HY_SOCKETS
    #else
      #if ADVCFG_PLATFORM_SOCKETS < HY_SOCKETS
        #undef ADVCFG_PLATFORM_SOCKETS
        #define ADVCFG_PLATFORM_SOCKETS HY_SOCKETS
      #endif
    #endif
    #ifndef ADVCFG_PLATFORM_MODULES
      #define ADVCFG_PLATFORM_MODULES HY_MODULES
    #else
      #if ADVCFG_PLATFORM_MODULES < HY_MODULES
        #undef ADVCFG_PLATFORM_MODULES
        #define ADVCFG_PLATFORM_MODULES HY_MODULES
      #endif
    #endif

    #if GET_PATCHES == TRUE
      #define F10_HY_UCODE_C4
      #define F10_HY_UCODE_C5

      // If a patch is required for recovery mode to function properly, add a
      // conditional for AGESA_ENTRY_INIT_RECOVERY, and pull it in.
      #if AGESA_ENTRY_INIT_EARLY == TRUE
        #if OPTION_C32_SOCKET_SUPPORT == TRUE
          extern  CONST MICROCODE_PATCHES ROMDATA CpuF10MicrocodePatch010000c5;
          #undef F10_HY_UCODE_C5
          #define F10_HY_UCODE_C5 &CpuF10MicrocodePatch010000c5,
        #endif
        #if (OPTION_C32_SOCKET_SUPPORT == TRUE) || (OPTION_G34_SOCKET_SUPPORT == TRUE)
          extern  CONST MICROCODE_PATCHES ROMDATA CpuF10MicrocodePatch010000c4;
          #undef F10_HY_UCODE_C4
          #define F10_HY_UCODE_C4 &CpuF10MicrocodePatch010000c4,
        #endif
      #endif

      CONST MICROCODE_PATCHES ROMDATA *CpuF10HyMicroCodePatchArray[] =
      {
        F10_HY_UCODE_C4
        F10_HY_UCODE_C5
        NULL
      };

      CONST UINT8 ROMDATA CpuF10HyNumberOfMicrocodePatches = (UINT8) ((sizeof (CpuF10HyMicroCodePatchArray) / sizeof (CpuF10HyMicroCodePatchArray[0])) - 1);
    #endif

    #define OPT_F10_HY_CPU {AMD_FAMILY_10_HY, &cpuF10HyServices},
  #else
    #define OPT_F10_HY_CPU
    #define OPT_F10_HY_ID
  #endif
#else
  #define OPT_F10_HY_CPU
  #define OPT_F10_HY_ID
#endif

/*
 * Install family 10h model 10 support
 */
#ifdef OPTION_FAMILY10H_PH
  #if OPTION_FAMILY10H_PH == TRUE
    extern CONST REGISTER_TABLE ROMDATA F10RevEPciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10RevEMsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10RevEHtPhyRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10PhPciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10PhMsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10PhHtPhyRegisterTable;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10PhMicroCodePatchesStruct;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10PhMicrocodeEquivalenceTable;
    extern F_CPU_GET_IDD_MAX F10CommonRevEGetProcIddMax;
    extern F_CPU_GET_NB_PSTATE_INFO F10CommonRevEGetNbPstateInfo;
    extern F_CPU_IS_NBCOF_INIT_NEEDED F10CommonRevEGetNbCofVidUpdate;
    extern F_CPU_NUMBER_OF_BRANDSTRING_CORES F10CommonRevEGetNumberOfCoresForBrandstring;

    #if USES_REGISTER_TABLES == TRUE
      CONST REGISTER_TABLE ROMDATA *F10PhRegisterTables[] =
      {
        #if BASE_FAMILY_PCI == TRUE
          &F10PciRegisterTable,
        #endif
        #if MODEL_SPECIFIC_PCI == TRUE
          &F10SingleLinkPciRegisterTable,
        #endif
        #if MODEL_SPECIFIC_PCI == TRUE
          &F10RevEPciRegisterTable,
        #endif
        #if BASE_FAMILY_MSR == TRUE
          &F10MsrRegisterTable,
        #endif
        #if MODEL_SPECIFIC_MSR == TRUE
          &F10RevEMsrRegisterTable,
        #endif
        #if MODEL_SPECIFIC_HT_PCI == TRUE
          &F10HtPhyRegisterTable,
        #endif
        #if MODEL_SPECIFIC_HT_PCI == TRUE
          &F10RevEHtPhyRegisterTable,
        #endif
        #if MODEL_SPECIFIC_HT_PCI == TRUE
          &F10PhHtPhyRegisterTable,
        #endif
        #if BASE_FAMILY_WORKAROUNDS == TRUE
          &F10WorkaroundsTable,
        #endif
        // the end.
        NULL
      };
    #endif

    #if USES_REGISTER_TABLES == TRUE
      CONST TABLE_ENTRY_TYPE_DESCRIPTOR ROMDATA F10PhTableEntryTypeDescriptors[] =
      {
        {MsrRegister, SetRegisterForMsrEntry},
        {PciRegister, SetRegisterForPciEntry},
        {FamSpecificWorkaround, SetRegisterForFamSpecificWorkaroundEntry},
        {HtPhyRegister, SetRegisterForHtPhyEntry},
        {HtPhyRangeRegister, SetRegisterForHtPhyRangeEntry},
        {DeemphasisRegister, SetRegisterForDeemphasisEntry},
        {ProfileFixup, SetRegisterForPerformanceProfileEntry},
        {HtHostPciRegister, SetRegisterForHtHostEntry},
        {HtTokenPciRegister, (PF_DO_TABLE_ENTRY)CommonVoid},
        {CoreCountsPciRegister, SetRegisterForCoreCountsPerformanceEntry},
        {ProcCountsPciRegister, SetRegisterForProcessorCountsEntry},
        {HtFeatPciRegister, SetRegisterForHtFeaturePciEntry},
        {HtPhyProfileRegister, SetRegisterForHtPhyProfileEntry},
        // End
        {TableEntryTypeMax, (PF_DO_TABLE_ENTRY)CommonVoid}
      };
    #endif

    CONST CPU_SPECIFIC_SERVICES ROMDATA cpuF10PhServices =
    {
      0,
      #if DISABLE_PSTATE == TRUE
        F10DisablePstate,
      #else
        (PF_CPU_DISABLE_PSTATE) CommonAssert,
      #endif
      #if TRANSITION_PSTATE == TRUE
        F10TransitionPstate,
      #else
        (PF_CPU_TRANSITION_PSTATE) CommonAssert,
      #endif
      #if PROC_IDD_MAX == TRUE
        F10CommonRevEGetProcIddMax,
      #else
        (PF_CPU_GET_IDD_MAX) CommonAssert,
      #endif
      #if GET_TSC_RATE == TRUE
        F10GetTscRate,
      #else
        (PF_CPU_GET_TSC_RATE) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F10GetCurrentNbFrequency,
      #else
        (PF_CPU_GET_NB_FREQ) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F10CommonRevEGetNbPstateInfo,
      #else
        (PF_CPU_GET_NB_PSTATE_INFO) CommonAssert,
      #endif
      #if IS_NBCOF_INIT_NEEDED == TRUE
        F10CommonRevEGetNbCofVidUpdate,
      #else
        (PF_CPU_IS_NBCOF_INIT_NEEDED) CommonAssert,
      #endif
      #if AP_INITIAL_LAUNCH == TRUE
        F10LaunchApCore,
      #else
        (PF_CPU_AP_INITIAL_LAUNCH) CommonAssert,
      #endif
      #if (BRAND_STRING1 == TRUE) || (BRAND_STRING2 == TRUE)
        F10CommonRevEGetNumberOfCoresForBrandstring,
      #else
        (PF_CPU_NUMBER_OF_BRANDSTRING_CORES) CommonAssert,
      #endif
      #if GET_AP_MAILBOX_FROM_HW == TRUE
        F10GetApMailboxFromHardware,
      #else
        (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE) CommonAssert,
      #endif
      #if SET_AP_CORE_NUMBER == TRUE
        F10SetApCoreNumber,
      #else
        (PF_CPU_SET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if GET_AP_CORE_NUMBER == TRUE
        F10GetApCoreNumber,
      #else
        (PF_CPU_GET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if TRANSFER_AP_CORE_NUMBER == TRUE
        F10TransferApCoreNumber,
      #else
        (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if ID_POSITION_INITIAL_APICID == TRUE
        F10CpuAmdCoreIdPositionInInitialApicId,
      #else
        (PF_CORE_ID_POSITION_IN_INITIAL_APIC_ID) CommonAssert,
      #endif
      #if SAVE_FEATURES == TRUE
        F10SaveFeatures,
      #else
        (PF_CPU_SAVE_FEATURES) CommonAssert,
      #endif
      #if WRITE_FEATURES == TRUE
        F10WriteFeatures,
      #else
        (PF_CPU_WRITE_FEATURES) CommonAssert,
      #endif
      #if SET_WARM_RESET_FLAG == TRUE
        F10SetAgesaWarmResetFlag,
      #else
        (PF_CPU_SET_WARM_RESET_FLAG) CommonAssert,
      #endif
      #if GET_WARM_RESET_FLAG == TRUE
        F10GetAgesaWarmResetFlag,
      #else
        (PF_CPU_GET_WARM_RESET_FLAG) CommonAssert,
      #endif
      #if BRAND_STRING1 == TRUE
        GetF10BrandIdString1,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if BRAND_STRING2 == TRUE
        GetF10BrandIdString2,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PATCHES == TRUE
        GetF10PhMicroCodePatchesStruct,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PATCHES_EQUIVALENCE_TABLE == TRUE
        GetF10PhMicrocodeEquivalenceTable,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_CACHE_INFO == TRUE
        GetF10CacheInfo,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_SYSTEM_PM_TABLE == TRUE
        GetF10SysPmTable,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_WHEA_INIT == TRUE
        GetF10WheaInitData,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PLATFORM_TYPE_SPECIFIC_INFO == TRUE
        F10GetPlatformTypeSpecificInfo,
      #else
        (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO) CommonAssert,
      #endif
      #if IS_NB_PSTATE_ENABLED == TRUE
        (PF_IS_NB_PSTATE_ENABLED) CommonReturnFalse,
      #else
        (PF_IS_NB_PSTATE_ENABLED) CommonAssert,
      #endif
      #if (BASE_FAMILY_HT_PCI == TRUE)
        F10NextLinkHasHtPhyFeats,
      #else
        (PF_NEXT_LINK_HAS_HTFPY_FEATS) CommonReturnFalse,
      #endif
      #if (BASE_FAMILY_HT_PCI == TRUE)
        F10SetHtPhyRegister,
      #else
        (PF_SET_HT_PHY_REGISTER) CommonAssert,
      #endif
      #if BASE_FAMILY_PCI == TRUE
        F10GetNextHtLinkFeatures,
      #else
        (PF_GET_NEXT_HT_LINK_FEATURES) CommonReturnFalse,
      #endif
      #if USES_REGISTER_TABLES == TRUE
        (REGISTER_TABLE **) F10PhRegisterTables,
      #else
        NULL,
      #endif
      #if USES_REGISTER_TABLES == TRUE
        (TABLE_ENTRY_TYPE_DESCRIPTOR *) F10PhTableEntryTypeDescriptors,
      #else
        NULL,
      #endif
      NULL,
      NULL,
      InitCacheDisabled,
      #if AGESA_ENTRY_INIT_EARLY == TRUE
        GetF10EarlyInitOnCoreTable
      #else
        (PF_GET_EARLY_INIT_TABLE) CommonVoid
      #endif
    };

    #define PH_SOCKETS 1
    #define PH_MODULES 1
    #define PH_RECOVERY_SOCKETS 1
    #define PH_RECOVERY_MODULES 1
    extern F_CPU_GET_SUBFAMILY_ID_ARRAY GetF10PhLogicalIdAndRev;
    #define OPT_F10_PH_ID (PF_CPU_GET_SUBFAMILY_ID_ARRAY) GetF10PhLogicalIdAndRev,
    #ifndef ADVCFG_PLATFORM_SOCKETS
      #define ADVCFG_PLATFORM_SOCKETS PH_SOCKETS
    #else
      #if ADVCFG_PLATFORM_SOCKETS < PH_SOCKETS
        #undef ADVCFG_PLATFORM_SOCKETS
        #define ADVCFG_PLATFORM_SOCKETS PH_SOCKETS
      #endif
    #endif
    #ifndef ADVCFG_PLATFORM_MODULES
      #define ADVCFG_PLATFORM_MODULES PH_MODULES
    #else
      #if ADVCFG_PLATFORM_MODULES < PH_MODULES
        #undef ADVCFG_PLATFORM_MODULES
        #define ADVCFG_PLATFORM_MODULES PH_MODULES
      #endif
    #endif

    #if GET_PATCHES == TRUE
      #define F10_PH_UCODE_BF

      // If a patch is required for recovery mode to function properly, add a
      // conditional for AGESA_ENTRY_INIT_RECOVERY, and pull it in.
      #if AGESA_ENTRY_INIT_EARLY == TRUE
        #if OPTION_AM3_SOCKET_SUPPORT == TRUE
          extern  CONST MICROCODE_PATCHES ROMDATA CpuF10MicrocodePatch010000bf;
          #undef F10_PH_UCODE_BF
          #define F10_PH_UCODE_BF &CpuF10MicrocodePatch010000bf,
        #endif
      #endif

      CONST MICROCODE_PATCHES ROMDATA *CpuF10PhMicroCodePatchArray[] =
      {
        F10_PH_UCODE_BF
        NULL
      };

      CONST UINT8 ROMDATA CpuF10PhNumberOfMicrocodePatches = (UINT8) ((sizeof (CpuF10PhMicroCodePatchArray) / sizeof (CpuF10PhMicroCodePatchArray[0])) - 1);
    #endif

    #define OPT_F10_PH_CPU {AMD_FAMILY_10_PH, &cpuF10PhServices},
  #else
    #define OPT_F10_PH_CPU
    #define OPT_F10_PH_ID
  #endif
#else
  #define OPT_F10_PH_CPU
  #define OPT_F10_PH_ID
#endif


/*
 * Install family 10h model 4 support
 */
#ifdef OPTION_FAMILY10H_RB
  #if OPTION_FAMILY10H_RB == TRUE
    extern CONST REGISTER_TABLE ROMDATA F10RevCPciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10RevCMsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10RevCHtPhyRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10RbPciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10RbMsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F10RbHtPhyRegisterTable;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10RbMicroCodePatchesStruct;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF10RbMicrocodeEquivalenceTable;
    extern F_CPU_GET_IDD_MAX F10CommonRevCGetProcIddMax;
    extern F_CPU_GET_NB_PSTATE_INFO F10CommonRevCGetNbPstateInfo;
    extern F_CPU_IS_NBCOF_INIT_NEEDED F10CommonRevCGetNbCofVidUpdate;
    extern F_IS_NB_PSTATE_ENABLED F10CommonRevCIsNbPstateEnabled;
    extern F_CPU_NUMBER_OF_BRANDSTRING_CORES F10CommonRevCGetNumberOfCoresForBrandstring;

    #if USES_REGISTER_TABLES == TRUE
      CONST REGISTER_TABLE ROMDATA *F10RbRegisterTables[] =
      {
        #if BASE_FAMILY_PCI == TRUE
          &F10PciRegisterTable,
        #endif
        #if MODEL_SPECIFIC_PCI == TRUE
          &F10MultiLinkPciRegisterTable,
          &F10SingleLinkPciRegisterTable,
        #endif
        #if MODEL_SPECIFIC_PCI == TRUE
          &F10RevCPciRegisterTable,
        #endif
        #if MODEL_SPECIFIC_PCI == TRUE
          &F10RbPciRegisterTable,
        #endif
        #if BASE_FAMILY_MSR == TRUE
          &F10MsrRegisterTable,
        #endif
        #if MODEL_SPECIFIC_MSR == TRUE
          &F10RevCMsrRegisterTable,
        #endif
        #if MODEL_SPECIFIC_MSR == TRUE
          &F10RbMsrRegisterTable,
        #endif
        #if MODEL_SPECIFIC_HT_PCI == TRUE
          &F10HtPhyRegisterTable,
        #endif
        #if MODEL_SPECIFIC_HT_PCI == TRUE
          &F10RevCHtPhyRegisterTable,
        #endif
        #if MODEL_SPECIFIC_HT_PCI == TRUE
          &F10RbHtPhyRegisterTable,
        #endif
        #if BASE_FAMILY_WORKAROUNDS == TRUE
          &F10WorkaroundsTable,
        #endif
        // the end.
        NULL
      };
    #endif

    #if USES_REGISTER_TABLES == TRUE
      CONST TABLE_ENTRY_TYPE_DESCRIPTOR ROMDATA F10RbTableEntryTypeDescriptors[] =
      {
        {MsrRegister, SetRegisterForMsrEntry},
        {PciRegister, SetRegisterForPciEntry},
        {FamSpecificWorkaround, SetRegisterForFamSpecificWorkaroundEntry},
        {HtPhyRegister, SetRegisterForHtPhyEntry},
        {HtPhyRangeRegister, SetRegisterForHtPhyRangeEntry},
        {DeemphasisRegister, SetRegisterForDeemphasisEntry},
        {ProfileFixup, SetRegisterForPerformanceProfileEntry},
        {HtHostPciRegister, SetRegisterForHtHostEntry},
        {HtTokenPciRegister, SetRegisterForHtLinkTokenEntry},
        {CoreCountsPciRegister, SetRegisterForCoreCountsPerformanceEntry},
        {ProcCountsPciRegister, SetRegisterForProcessorCountsEntry},
        {TokenPciRegister, SetRegisterForTokenPciEntry},
        {HtFeatPciRegister, SetRegisterForHtFeaturePciEntry},
        {HtPhyProfileRegister, SetRegisterForHtPhyProfileEntry},
        // End
        {TableEntryTypeMax, (PF_DO_TABLE_ENTRY)CommonVoid}
      };
    #endif

    CONST CPU_SPECIFIC_SERVICES ROMDATA cpuF10RbServices =
    {
      0,
      #if DISABLE_PSTATE == TRUE
        F10DisablePstate,
      #else
        (PF_CPU_DISABLE_PSTATE) CommonAssert,
      #endif
      #if TRANSITION_PSTATE == TRUE
        F10TransitionPstate,
      #else
        (PF_CPU_TRANSITION_PSTATE) CommonAssert,
      #endif
      #if PROC_IDD_MAX == TRUE
        F10CommonRevCGetProcIddMax,
      #else
        (PF_CPU_GET_IDD_MAX) CommonAssert,
      #endif
      #if GET_TSC_RATE == TRUE
        F10GetTscRate,
      #else
        (PF_CPU_GET_TSC_RATE) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F10GetCurrentNbFrequency,
      #else
        (PF_CPU_GET_NB_FREQ) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F10CommonRevCGetNbPstateInfo,
      #else
        (PF_CPU_GET_NB_PSTATE_INFO) CommonAssert,
      #endif
      #if IS_NBCOF_INIT_NEEDED == TRUE
        F10CommonRevCGetNbCofVidUpdate,
      #else
        (PF_CPU_IS_NBCOF_INIT_NEEDED) CommonAssert,
      #endif
      #if AP_INITIAL_LAUNCH == TRUE
        F10LaunchApCore,
      #else
        (PF_CPU_AP_INITIAL_LAUNCH) CommonAssert,
      #endif
      #if (BRAND_STRING1 == TRUE) || (BRAND_STRING2 == TRUE)
        F10CommonRevCGetNumberOfCoresForBrandstring,
      #else
        (PF_CPU_NUMBER_OF_BRANDSTRING_CORES) CommonAssert,
      #endif
      #if GET_AP_MAILBOX_FROM_HW == TRUE
        F10GetApMailboxFromHardware,
      #else
        (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE) CommonAssert,
      #endif
      #if SET_AP_CORE_NUMBER == TRUE
        F10SetApCoreNumber,
      #else
        (PF_CPU_SET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if GET_AP_CORE_NUMBER == TRUE
        F10GetApCoreNumber,
      #else
        (PF_CPU_GET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if TRANSFER_AP_CORE_NUMBER == TRUE
        F10TransferApCoreNumber,
      #else
        (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if ID_POSITION_INITIAL_APICID == TRUE
        F10CpuAmdCoreIdPositionInInitialApicId,
      #else
        (PF_CORE_ID_POSITION_IN_INITIAL_APIC_ID) CommonAssert,
      #endif
      #if SAVE_FEATURES == TRUE
        F10SaveFeatures,
      #else
        (PF_CPU_SAVE_FEATURES) CommonAssert,
      #endif
      #if WRITE_FEATURES == TRUE
        F10WriteFeatures,
      #else
        (PF_CPU_WRITE_FEATURES) CommonAssert,
      #endif
      #if SET_WARM_RESET_FLAG == TRUE
        F10SetAgesaWarmResetFlag,
      #else
        (PF_CPU_SET_WARM_RESET_FLAG) CommonAssert,
      #endif
      #if GET_WARM_RESET_FLAG == TRUE
        F10GetAgesaWarmResetFlag,
      #else
        (PF_CPU_GET_WARM_RESET_FLAG) CommonAssert,
      #endif
      #if BRAND_STRING1 == TRUE
        GetF10BrandIdString1,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if BRAND_STRING2 == TRUE
        GetF10BrandIdString2,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PATCHES == TRUE
        GetF10RbMicroCodePatchesStruct,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PATCHES_EQUIVALENCE_TABLE == TRUE
        GetF10RbMicrocodeEquivalenceTable,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_CACHE_INFO == TRUE
        GetF10CacheInfo,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_SYSTEM_PM_TABLE == TRUE
        GetF10SysPmTable,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_WHEA_INIT == TRUE
        GetF10WheaInitData,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PLATFORM_TYPE_SPECIFIC_INFO == TRUE
        F10GetPlatformTypeSpecificInfo,
      #else
        (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO) CommonAssert,
      #endif
      #if IS_NB_PSTATE_ENABLED == TRUE
        F10CommonRevCIsNbPstateEnabled,
      #else
        (PF_IS_NB_PSTATE_ENABLED) CommonAssert,
      #endif
      #if (BASE_FAMILY_HT_PCI == TRUE)
        F10NextLinkHasHtPhyFeats,
      #else
        (PF_NEXT_LINK_HAS_HTFPY_FEATS) CommonReturnFalse,
      #endif
      #if (BASE_FAMILY_HT_PCI == TRUE)
        F10SetHtPhyRegister,
      #else
        (PF_SET_HT_PHY_REGISTER) CommonAssert,
      #endif
      #if BASE_FAMILY_PCI == TRUE
        F10GetNextHtLinkFeatures,
      #else
        (PF_GET_NEXT_HT_LINK_FEATURES) CommonReturnFalse,
      #endif
      #if USES_REGISTER_TABLES == TRUE
        (REGISTER_TABLE **) F10RbRegisterTables,
      #else
        NULL,
      #endif
      #if USES_REGISTER_TABLES == TRUE
        (TABLE_ENTRY_TYPE_DESCRIPTOR *) F10RbTableEntryTypeDescriptors,
      #else
        NULL,
      #endif
      NULL,
      NULL,
      InitCacheDisabled,
      #if AGESA_ENTRY_INIT_EARLY == TRUE
        GetF10EarlyInitOnCoreTable
      #else
        (PF_GET_EARLY_INIT_TABLE) CommonVoid
      #endif
    };

    #define RB_SOCKETS 8
    #define RB_MODULES 1
    #define RB_RECOVERY_SOCKETS 1
    #define RB_RECOVERY_MODULES 1
    extern F_CPU_GET_SUBFAMILY_ID_ARRAY GetF10RbLogicalIdAndRev;
    #define OPT_F10_RB_ID (PF_CPU_GET_SUBFAMILY_ID_ARRAY) GetF10RbLogicalIdAndRev,
    #ifndef ADVCFG_PLATFORM_SOCKETS
      #define ADVCFG_PLATFORM_SOCKETS RB_SOCKETS
    #else
      #if ADVCFG_PLATFORM_SOCKETS < RB_SOCKETS
        #undef ADVCFG_PLATFORM_SOCKETS
        #define ADVCFG_PLATFORM_SOCKETS RB_SOCKETS
      #endif
    #endif
    #ifndef ADVCFG_PLATFORM_MODULES
      #define ADVCFG_PLATFORM_MODULES RB_MODULES
    #else
      #if ADVCFG_PLATFORM_MODULES < RB_MODULES
        #undef ADVCFG_PLATFORM_MODULES
        #define ADVCFG_PLATFORM_MODULES RB_MODULES
      #endif
    #endif

    #if GET_PATCHES == TRUE
      #define F10_RB_UCODE_85
      #define F10_RB_UCODE_C6
      #define F10_RB_UCODE_C8

      // If a patch is required for recovery mode to function properly, add a
      // conditional for AGESA_ENTRY_INIT_RECOVERY, and pull it in.
      #if AGESA_ENTRY_INIT_EARLY == TRUE
        #if OPTION_AM3_SOCKET_SUPPORT == TRUE
          extern  CONST MICROCODE_PATCHES ROMDATA CpuF10MicrocodePatch01000085;
          extern  CONST MICROCODE_PATCHES ROMDATA CpuF10MicrocodePatch010000c6;
          extern  CONST MICROCODE_PATCHES ROMDATA CpuF10MicrocodePatch010000c8;
          #undef F10_RB_UCODE_85
          #define F10_RB_UCODE_85 &CpuF10MicrocodePatch01000085,
          #undef F10_RB_UCODE_C6
          #define F10_RB_UCODE_C6 &CpuF10MicrocodePatch010000c6,
          #undef F10_RB_UCODE_C8
          #define F10_RB_UCODE_C8 &CpuF10MicrocodePatch010000c8,
        #endif
      #endif

      CONST MICROCODE_PATCHES ROMDATA *CpuF10RbMicroCodePatchArray[] =
      {
        F10_RB_UCODE_85
        F10_RB_UCODE_C6
        F10_RB_UCODE_C8
        NULL
      };

      CONST UINT8 ROMDATA CpuF10RbNumberOfMicrocodePatches = (UINT8) ((sizeof (CpuF10RbMicroCodePatchArray) / sizeof (CpuF10RbMicroCodePatchArray[0])) - 1);
    #endif

    #define OPT_F10_RB_CPU {AMD_FAMILY_10_RB, &cpuF10RbServices},
  #else
    #define OPT_F10_RB_CPU
    #define OPT_F10_RB_ID
  #endif
#else
  #define OPT_F10_RB_CPU
  #define OPT_F10_RB_ID
#endif


/*
 * Install unknown family 10h support
 */

#if USES_REGISTER_TABLES == TRUE
  CONST REGISTER_TABLE ROMDATA *F10UnknownRegisterTables[] =
  {
    #if BASE_FAMILY_PCI == TRUE
      &F10PciRegisterTable,
    #endif
    #if BASE_FAMILY_MSR == TRUE
      &F10MsrRegisterTable,
    #endif
    #if BASE_FAMILY_HT_PCI == TRUE
      &F10HtPhyRegisterTable,
    #endif
    #if OPTION_MULTISOCKET == TRUE
      #if MODEL_SPECIFIC_PCI == TRUE
        &F10MultiLinkPciRegisterTable,
      #endif
    #endif
    #if OPTION_MULTISOCKET == FALSE
      #if MODEL_SPECIFIC_PCI == TRUE
        &F10SingleLinkPciRegisterTable,
      #endif
    #endif
    #if BASE_FAMILY_WORKAROUNDS == TRUE
      &F10WorkaroundsTable,
    #endif
    // the end.
    NULL
  };
#endif

#if USES_REGISTER_TABLES == TRUE
  CONST TABLE_ENTRY_TYPE_DESCRIPTOR ROMDATA F10UnknownTableEntryTypeDescriptors[] =
  {
    {MsrRegister, SetRegisterForMsrEntry},
    {PciRegister, SetRegisterForPciEntry},
    {FamSpecificWorkaround, SetRegisterForFamSpecificWorkaroundEntry},
    {HtPhyRegister, SetRegisterForHtPhyEntry},
    {HtPhyRangeRegister, SetRegisterForHtPhyRangeEntry},
    {DeemphasisRegister, SetRegisterForDeemphasisEntry},
    {ProfileFixup, SetRegisterForPerformanceProfileEntry},
    {HtHostPciRegister, SetRegisterForHtHostEntry},
    {HtTokenPciRegister, (PF_DO_TABLE_ENTRY)CommonVoid},
    {CoreCountsPciRegister, SetRegisterForCoreCountsPerformanceEntry},
    {ProcCountsPciRegister, SetRegisterForProcessorCountsEntry},
    {HtFeatPciRegister, SetRegisterForHtFeaturePciEntry},
    // End
    {TableEntryTypeMax, (PF_DO_TABLE_ENTRY)CommonVoid}
  };
#endif


CONST CPU_SPECIFIC_SERVICES ROMDATA cpuF10UnknownServices =
{
  0,
  #if DISABLE_PSTATE == TRUE
    F10DisablePstate,
  #else
    (PF_CPU_DISABLE_PSTATE) CommonAssert,
  #endif
  #if TRANSITION_PSTATE == TRUE
    F10TransitionPstate,
  #else
    (PF_CPU_TRANSITION_PSTATE) CommonAssert,
  #endif
  #if PROC_IDD_MAX == TRUE
    (PF_CPU_GET_IDD_MAX) CommonReturnFalse,
  #else
    (PF_CPU_GET_IDD_MAX) CommonAssert,
  #endif
  #if GET_TSC_RATE == TRUE
    F10GetTscRate,
  #else
    (PF_CPU_GET_TSC_RATE) CommonAssert,
  #endif
  #if GET_NB_FREQ == TRUE
    F10GetCurrentNbFrequency,
  #else
    (PF_CPU_GET_NB_FREQ) CommonAssert,
  #endif
  #if GET_NB_FREQ == TRUE
    (PF_CPU_GET_NB_PSTATE_INFO) CommonReturnFalse,
  #else
    (PF_CPU_GET_NB_PSTATE_INFO) CommonAssert,
  #endif
  #if IS_NBCOF_INIT_NEEDED == TRUE
    (PF_CPU_IS_NBCOF_INIT_NEEDED) CommonReturnFalse,
  #else
    (PF_CPU_IS_NBCOF_INIT_NEEDED) CommonAssert,
  #endif
  #if AP_INITIAL_LAUNCH == TRUE
    F10LaunchApCore,
  #else
    (PF_CPU_AP_INITIAL_LAUNCH) CommonAssert,
  #endif
  #if (BRAND_STRING1 == TRUE) || (BRAND_STRING2 == TRUE)
    (PF_CPU_NUMBER_OF_BRANDSTRING_CORES) CommonReturnZero8,
  #else
    (PF_CPU_NUMBER_OF_BRANDSTRING_CORES) CommonAssert,
  #endif
  #if GET_AP_MAILBOX_FROM_HW == TRUE
    F10GetApMailboxFromHardware,
  #else
    (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE) CommonAssert,
  #endif
  #if SET_AP_CORE_NUMBER == TRUE
    F10SetApCoreNumber,
  #else
    (PF_CPU_SET_AP_CORE_NUMBER) CommonAssert,
  #endif
  #if GET_AP_CORE_NUMBER == TRUE
    F10GetApCoreNumber,
  #else
    (PF_CPU_GET_AP_CORE_NUMBER) CommonAssert,
  #endif
  #if TRANSFER_AP_CORE_NUMBER == TRUE
    F10TransferApCoreNumber,
  #else
    (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonAssert,
  #endif
  #if ID_POSITION_INITIAL_APICID == TRUE
    F10CpuAmdCoreIdPositionInInitialApicId,
  #else
    (PF_CORE_ID_POSITION_IN_INITIAL_APIC_ID) CommonAssert,
  #endif
  #if SAVE_FEATURES == TRUE
    F10SaveFeatures,
  #else
    (PF_CPU_SAVE_FEATURES) CommonAssert,
  #endif
  #if WRITE_FEATURES == TRUE
    F10WriteFeatures,
  #else
    (PF_CPU_WRITE_FEATURES) CommonAssert,
  #endif
  #if SET_WARM_RESET_FLAG == TRUE
    F10SetAgesaWarmResetFlag,
  #else
    (PF_CPU_SET_WARM_RESET_FLAG) CommonAssert,
  #endif
  #if GET_WARM_RESET_FLAG == TRUE
    F10GetAgesaWarmResetFlag,
  #else
    (PF_CPU_GET_WARM_RESET_FLAG) CommonAssert,
  #endif
  #if BRAND_STRING1 == TRUE
    GetF10BrandIdString1,
  #else
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
  #endif
  #if BRAND_STRING2 == TRUE
    GetF10BrandIdString2,
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
    GetF10CacheInfo,
  #else
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
  #endif
  #if GET_SYSTEM_PM_TABLE == TRUE
    GetF10SysPmTable,
  #else
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
  #endif
  #if GET_WHEA_INIT == TRUE
    GetF10WheaInitData,
  #else
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
  #endif
  #if GET_PLATFORM_TYPE_SPECIFIC_INFO == TRUE
    F10GetPlatformTypeSpecificInfo,
  #else
    (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO) CommonAssert,
  #endif
  #if IS_NB_PSTATE_ENABLED == TRUE
    (PF_IS_NB_PSTATE_ENABLED) CommonReturnFalse,
  #else
    (PF_IS_NB_PSTATE_ENABLED) CommonAssert,
  #endif
  #if (BASE_FAMILY_HT_PCI == TRUE)
    F10NextLinkHasHtPhyFeats,
  #else
    (PF_NEXT_LINK_HAS_HTFPY_FEATS) CommonReturnFalse,
  #endif
  #if (BASE_FAMILY_HT_PCI == TRUE)
    F10SetHtPhyRegister,
  #else
    (PF_SET_HT_PHY_REGISTER) CommonVoid,
  #endif
  #if BASE_FAMILY_PCI == TRUE
    F10GetNextHtLinkFeatures,
  #else
    (PF_GET_NEXT_HT_LINK_FEATURES) CommonReturnFalse,
  #endif
  #if USES_REGISTER_TABLES == TRUE
    (REGISTER_TABLE **) F10UnknownRegisterTables,
  #else
    NULL,
  #endif
  #if USES_REGISTER_TABLES == TRUE
    (TABLE_ENTRY_TYPE_DESCRIPTOR *) F10UnknownTableEntryTypeDescriptors,
  #else
    NULL,
  #endif
  NULL,
  NULL,
  InitCacheDisabled,
  #if AGESA_ENTRY_INIT_EARLY == TRUE
    GetF10EarlyInitOnCoreTable
  #else
    (PF_GET_EARLY_INIT_TABLE) CommonVoid
  #endif
};

// Family 10h maximum base address is 48 bits. Limit BLDCFG to 48 bits, if appropriate.
#if (FAMILY_MMIO_BASE_MASK < 0xFFFF000000000000ull)
  #undef  FAMILY_MMIO_BASE_MASK
  #define FAMILY_MMIO_BASE_MASK (0xFFFF000000000000ull)
#endif

#undef OPT_F10_ID_TABLE
#define OPT_F10_ID_TABLE {0x10, {AMD_FAMILY_10, AMD_F10_UNKNOWN}, F10LogicalIdTable, (sizeof (F10LogicalIdTable) / sizeof (F10LogicalIdTable[0]))},
#define OPT_F10_UNKNOWN_CPU {AMD_FAMILY_10, &cpuF10UnknownServices},

#undef OPT_F10_TABLE
#define OPT_F10_TABLE   OPT_F10_BL_CPU  OPT_F10_DA_CPU  OPT_F10_HY_CPU  OPT_F10_PH_CPU  OPT_F10_RB_CPU  OPT_F10_UNKNOWN_CPU

#if OPTION_G34_SOCKET_SUPPORT == TRUE
  extern CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString1ArrayG34;
  extern CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString2ArrayG34;
  #define F10_G34_BRANDSTRING1 &F10BrandIdString1ArrayG34,
  #define F10_G34_BRANDSTRING2 &F10BrandIdString2ArrayG34,
#else
  #define F10_G34_BRANDSTRING1
  #define F10_G34_BRANDSTRING2
#endif
#if OPTION_C32_SOCKET_SUPPORT == TRUE
  extern CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString1ArrayC32;
  extern CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString2ArrayC32;
  #define F10_C32_BRANDSTRING1 &F10BrandIdString1ArrayC32,
  #define F10_C32_BRANDSTRING2 &F10BrandIdString2ArrayC32,
#else
  #define F10_C32_BRANDSTRING1
  #define F10_C32_BRANDSTRING2
#endif
#if OPTION_S1G3_SOCKET_SUPPORT == TRUE
  extern CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString1ArrayS1g3;
  extern CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString2ArrayS1g3;
  #define F10_S1G3_BRANDSTRING1 &F10BrandIdString1ArrayS1g3,
  #define F10_S1G3_BRANDSTRING2 &F10BrandIdString2ArrayS1g3,
#else
  #define F10_S1G3_BRANDSTRING1
  #define F10_S1G3_BRANDSTRING2
#endif
#if OPTION_S1G4_SOCKET_SUPPORT == TRUE
  extern CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString1ArrayS1g4;
  extern CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString2ArrayS1g4;
  #define F10_S1G4_BRANDSTRING1 &F10BrandIdString1ArrayS1g4,
  #define F10_S1G4_BRANDSTRING2 &F10BrandIdString2ArrayS1g4,
#else
  #define F10_S1G4_BRANDSTRING1
  #define F10_S1G4_BRANDSTRING2
#endif
#if OPTION_ASB2_SOCKET_SUPPORT == TRUE
  extern CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString1ArrayAsb2;
  extern CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString2ArrayAsb2;
  #define F10_ASB2_BRANDSTRING1 &F10BrandIdString1ArrayAsb2,
  #define F10_ASB2_BRANDSTRING2 &F10BrandIdString2ArrayAsb2,
#else
  #define F10_ASB2_BRANDSTRING1
  #define F10_ASB2_BRANDSTRING2
#endif
#if OPTION_AM3_SOCKET_SUPPORT == TRUE
  extern CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString1ArrayAm3;
  extern CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString2ArrayAm3;
  #define F10_AM3_BRANDSTRING1 &F10BrandIdString1ArrayAm3,
  #define F10_AM3_BRANDSTRING2 &F10BrandIdString2ArrayAm3,
#else
  #define F10_AM3_BRANDSTRING1
  #define F10_AM3_BRANDSTRING2
#endif

#if BRAND_STRING1 == TRUE
  CONST CPU_BRAND_TABLE ROMDATA *F10BrandIdString1Tables[] =
  {
    F10_G34_BRANDSTRING1
    F10_C32_BRANDSTRING1
    F10_S1G3_BRANDSTRING1
    F10_S1G4_BRANDSTRING1
    F10_ASB2_BRANDSTRING1
    F10_AM3_BRANDSTRING1
  };

  CONST UINT8 F10BrandIdString1TableCount = (sizeof (F10BrandIdString1Tables) / sizeof (F10BrandIdString1Tables[0]));
#endif

#if BRAND_STRING2 == TRUE
  CONST CPU_BRAND_TABLE ROMDATA *F10BrandIdString2Tables[] =
  {
    F10_G34_BRANDSTRING2
    F10_C32_BRANDSTRING2
    F10_S1G3_BRANDSTRING2
    F10_S1G4_BRANDSTRING2
    F10_ASB2_BRANDSTRING2
    F10_AM3_BRANDSTRING2
  };

  CONST UINT8 F10BrandIdString2TableCount = (sizeof (F10BrandIdString2Tables) / sizeof (F10BrandIdString2Tables[0]));
#endif

CONST PF_CPU_GET_SUBFAMILY_ID_ARRAY ROMDATA F10LogicalIdTable[] =
{
  OPT_F10_BL_ID
  OPT_F10_DA_ID
  OPT_F10_HY_ID
  OPT_F10_PH_ID
  OPT_F10_RB_ID
};

#endif  // _OPTION_FAMILY_10H_INSTALL_H_
