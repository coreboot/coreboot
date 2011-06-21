/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of family 15h support
 *
 * This file generates the defaults tables for family 15h processors.
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

#ifndef _OPTION_FAMILY_15H_INSTALL_H_
#define _OPTION_FAMILY_15H_INSTALL_H_

/*
 * Pull in family specific services based on entry point
 */

/*
 * Common Family 15h routines
 */
extern F_GET_EARLY_INIT_TABLE GetF15EarlyInitOnCoreTable;
extern F_IS_NB_PSTATE_ENABLED F15IsNbPstateEnabled;

/*
 * Install family 15h model 0 support
 */
#ifdef OPTION_FAMILY15H_OR
  #if OPTION_FAMILY15H_OR == TRUE
    extern F_CPU_GET_IDD_MAX F15GetProcIddMax;
    extern F_CPU_GET_NB_PSTATE_INFO F15GetNbPstateInfo;
    extern F_CPU_IS_NBCOF_INIT_NEEDED F15CommonGetNbCofVidUpdate;
    extern F_CPU_DISABLE_PSTATE F15DisablePstate;
    extern F_CPU_TRANSITION_PSTATE F15TransitionPstate;
    extern F_CPU_GET_TSC_RATE F15GetTscRate;
    extern F_CPU_GET_NB_FREQ F15GetCurrentNbFrequency;
    extern F_CPU_AP_INITIAL_LAUNCH F15LaunchApCore;
    extern F_CPU_NUMBER_OF_BRANDSTRING_CORES F15CommonGetNumberOfCoresForBrandstring;
    extern F_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE F15GetApMailboxFromHardware;
    extern F_CPU_SET_AP_CORE_NUMBER F15SetApCoreNumber;
    extern F_CPU_GET_AP_CORE_NUMBER F15GetApCoreNumber;
    extern F_CPU_TRANSFER_AP_CORE_NUMBER F15TransferApCoreNumber;
    extern F_CORE_ID_POSITION_IN_INITIAL_APIC_ID F15CpuAmdCoreIdPositionInInitialApicId;
    extern F_CPU_SAVE_FEATURES F15SaveFeatures;
    extern F_CPU_WRITE_FEATURES F15WriteFeatures;
    extern F_CPU_SET_WARM_RESET_FLAG F15SetAgesaWarmResetFlag;
    extern F_CPU_GET_WARM_RESET_FLAG F15GetAgesaWarmResetFlag;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF15CacheInfo;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF15SysPmTable;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF15WheaInitData;
    extern F_CPU_SET_CFOH_REG SetF15CacheFlushOnHaltRegister;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetEmptyArray;
    extern F_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO F15GetPlatformTypeSpecificInfo;
    extern F_NEXT_LINK_HAS_HTFPY_FEATS F15NextLinkHasHtPhyFeats;
    extern F_SET_HT_PHY_REGISTER F15SetHtPhyRegister;
    extern F_GET_NEXT_HT_LINK_FEATURES F15GetNextHtLinkFeatures;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF15OrMicroCodePatchesStruct;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF15OrMicrocodeEquivalenceTable;
    extern CONST REGISTER_TABLE ROMDATA F15OrPciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15OrMsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15OrSharedMsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15OrHtPhyRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15PciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15MsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15HtPhyRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15MultiLinkPciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15SingleLinkPciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15WorkaroundsTable;
    extern CONST PACKAGE_HTLINK_MAP_ITEM ROMDATA HtFam15PackageLinkMap[];

    /**
     * Core Pair and core pair primary determination table.
     *
     * The two fields from the core pair hardware register can be used to determine whether
     * even number cores are primary or all cores are primary.  It can be extended if it is
     * decided to have other configs as well.  The other logically possible value sets are BitMapMapping,
     * but they are currently not supported by the processor.
     */
    CONST CORE_PAIR_MAP ROMDATA HtFam15CorePairMapping[] =
    {
      {1, 1, EvenCoresMapping},                                     ///< 1 Compute Unit with 2 cores
      {3, 3, EvenCoresMapping},                                     ///< 2 Compute Units both with 2 Cores
      {7, 7, EvenCoresMapping},                                     ///< 3 Compute Units all with 2 Cores
      {0xF, 0xF, EvenCoresMapping},                                 ///< 4 Compute Units all with 2 Cores
      {1, 0, AllCoresMapping},                                      ///< 1 Compute Unit with 1 core
      {3, 0, AllCoresMapping},                                      ///< 2 Compute Units both with 1 Core
      {7, 0, AllCoresMapping},                                      ///< 3 Compute Units all with 1 Core
      {0xF, 0, AllCoresMapping},                                    ///< 4 Compute Units all with 1 Core
      {HT_LIST_TERMINAL, HT_LIST_TERMINAL, MaxComputeUnitMapping}   ///< End
    };


    #if USES_REGISTER_TABLES == TRUE
      CONST REGISTER_TABLE ROMDATA *F15OrRegisterTables[] =
      {
        #if BASE_FAMILY_PCI == TRUE
          &F15PciRegisterTable,
        #endif
        #if MODEL_SPECIFIC_PCI == TRUE
          &F15MultiLinkPciRegisterTable,
        #endif
        #if MODEL_SPECIFIC_PCI == TRUE
          &F15OrPciRegisterTable,
        #endif
        #if BASE_FAMILY_MSR == TRUE
          &F15MsrRegisterTable,
        #endif
        #if MODEL_SPECIFIC_MSR == TRUE
          &F15OrMsrRegisterTable,
        #endif
        #if MODEL_SPECIFIC_MSR == TRUE
          &F15OrSharedMsrRegisterTable,
        #endif
        #if MODEL_SPECIFIC_HT_PCI == TRUE
          &F15HtPhyRegisterTable,
        #endif
        #if MODEL_SPECIFIC_HT_PCI == TRUE
          &F15OrHtPhyRegisterTable,
        #endif
        #if BASE_FAMILY_WORKAROUNDS == TRUE
          &F15WorkaroundsTable,
        #endif
        // the end.
        NULL
      };
    #endif

    #if USES_REGISTER_TABLES == TRUE
      CONST TABLE_ENTRY_TYPE_DESCRIPTOR ROMDATA F15OrTableEntryTypeDescriptors[] =
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
        {HtHostPerfPciRegister, SetRegisterForHtHostPerfEntry},
        {HtTokenPciRegister, SetRegisterForHtLinkTokenEntry},
        {CoreCountsPciRegister, SetRegisterForCoreCountsPerformanceEntry},
        {ProcCountsPciRegister, SetRegisterForProcessorCountsEntry},
        {CompUnitCountsPciRegister, SetRegisterForComputeUnitCountsEntry},
        {TokenPciRegister, SetRegisterForTokenPciEntry},
        {HtFeatPciRegister, SetRegisterForHtFeaturePciEntry},
        {HtLinkPciRegister, SetRegisterForHtLinkPciEntry},
        // End
        {TableEntryTypeMax, (PF_DO_TABLE_ENTRY)CommonVoid}
      };
    #endif

    CONST CPU_SPECIFIC_SERVICES ROMDATA cpuF15OrServices =
    {
      0,
      #if DISABLE_PSTATE == TRUE
        F15DisablePstate,
      #else
        (PF_CPU_DISABLE_PSTATE) CommonAssert,
      #endif
      #if TRANSITION_PSTATE == TRUE
        F15TransitionPstate,
      #else
        (PF_CPU_TRANSITION_PSTATE) CommonAssert,
      #endif
      #if PROC_IDD_MAX == TRUE
        F15GetProcIddMax,
      #else
        (PF_CPU_GET_IDD_MAX) CommonAssert,
      #endif
      #if GET_TSC_RATE == TRUE
        F15GetTscRate,
      #else
        (PF_CPU_GET_TSC_RATE) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F15GetCurrentNbFrequency,
      #else
        (PF_CPU_GET_NB_FREQ) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F15GetNbPstateInfo,
      #else
        (PF_CPU_GET_NB_PSTATE_INFO) CommonAssert,
      #endif
      #if IS_NBCOF_INIT_NEEDED == TRUE
        F15CommonGetNbCofVidUpdate,
      #else
        (PF_CPU_IS_NBCOF_INIT_NEEDED) CommonAssert,
      #endif
      #if AP_INITIAL_LAUNCH == TRUE
        F15LaunchApCore,
      #else
        (PF_CPU_AP_INITIAL_LAUNCH) CommonAssert,
      #endif
      #if (BRAND_STRING1 == TRUE) || (BRAND_STRING2 == TRUE)
        F15CommonGetNumberOfCoresForBrandstring,
      #else
        (PF_CPU_NUMBER_OF_BRANDSTRING_CORES) CommonAssert,
      #endif
      #if GET_AP_MAILBOX_FROM_HW == TRUE
        F15GetApMailboxFromHardware,
      #else
        (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE) CommonAssert,
      #endif
      #if SET_AP_CORE_NUMBER == TRUE
        F15SetApCoreNumber,
      #else
        (PF_CPU_SET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if GET_AP_CORE_NUMBER == TRUE
        F15GetApCoreNumber,
      #else
        (PF_CPU_GET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if TRANSFER_AP_CORE_NUMBER == TRUE
        F15TransferApCoreNumber,
      #else
        (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if ID_POSITION_INITIAL_APICID == TRUE
        F15CpuAmdCoreIdPositionInInitialApicId,
      #else
        (PF_CORE_ID_POSITION_IN_INITIAL_APIC_ID) CommonAssert,
      #endif
      #if SAVE_FEATURES == TRUE
        // F15SaveFeatures,
        (PF_CPU_SAVE_FEATURES) CommonVoid,
      #else
        (PF_CPU_SAVE_FEATURES) CommonAssert,
      #endif
      #if WRITE_FEATURES == TRUE
        // F15WriteFeatures,
        (PF_CPU_WRITE_FEATURES) CommonVoid,
      #else
        (PF_CPU_WRITE_FEATURES) CommonAssert,
      #endif
      #if SET_WARM_RESET_FLAG == TRUE
        F15SetAgesaWarmResetFlag,
      #else
        (PF_CPU_SET_WARM_RESET_FLAG) CommonAssert,
      #endif
      #if GET_WARM_RESET_FLAG == TRUE
        F15GetAgesaWarmResetFlag,
      #else
        (PF_CPU_GET_WARM_RESET_FLAG) CommonAssert,
      #endif
      #if BRAND_STRING1 == TRUE
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonVoid,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if BRAND_STRING2 == TRUE
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonVoid,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PATCHES == TRUE
        GetF15OrMicroCodePatchesStruct,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PATCHES_EQUIVALENCE_TABLE == TRUE
        GetF15OrMicrocodeEquivalenceTable,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_CACHE_INFO == TRUE
        GetF15CacheInfo,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_SYSTEM_PM_TABLE == TRUE
        GetF15SysPmTable,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_WHEA_INIT == TRUE
        GetF15WheaInitData,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PLATFORM_TYPE_SPECIFIC_INFO == TRUE
        F15GetPlatformTypeSpecificInfo,
      #else
        (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO) CommonAssert,
      #endif
      #if IS_NB_PSTATE_ENABLED == TRUE
        F15IsNbPstateEnabled,
      #else
        (PF_IS_NB_PSTATE_ENABLED) CommonAssert,
      #endif
      #if (BASE_FAMILY_HT_PCI == TRUE)
        F15NextLinkHasHtPhyFeats,
      #else
        (PF_NEXT_LINK_HAS_HTFPY_FEATS) CommonReturnFalse,
      #endif
      #if (BASE_FAMILY_HT_PCI == TRUE)
        F15SetHtPhyRegister,
      #else
        (PF_SET_HT_PHY_REGISTER) CommonAssert,
      #endif
      #if BASE_FAMILY_PCI == TRUE
        F15GetNextHtLinkFeatures,
      #else
        (PF_GET_NEXT_HT_LINK_FEATURES) CommonAssert,
      #endif
      #if USES_REGISTER_TABLES == TRUE
        (REGISTER_TABLE **) F15OrRegisterTables,
      #else
        NULL,
      #endif
      #if USES_REGISTER_TABLES == TRUE
        (TABLE_ENTRY_TYPE_DESCRIPTOR *) F15OrTableEntryTypeDescriptors,
      #else
        NULL,
      #endif
      #if MODEL_SPECIFIC_HT_PCI == TRUE
        (PACKAGE_HTLINK_MAP) &HtFam15PackageLinkMap,
      #else
        NULL,
      #endif
      (CORE_PAIR_MAP *) &HtFam15CorePairMapping,
      InitCacheEnabled,
      #if AGESA_ENTRY_INIT_EARLY == TRUE
        GetF15EarlyInitOnCoreTable
      #else
        (PF_GET_EARLY_INIT_TABLE) CommonVoid
      #endif
    };

    #define OR_SOCKETS 8
    #define OR_MODULES 2
    #define OR_RECOVERY_SOCKETS 1
    #define OR_RECOVERY_MODULES 1
    extern F_CPU_GET_SUBFAMILY_ID_ARRAY GetF15OrLogicalIdAndRev;
    #define OPT_F15_OR_ID (PF_CPU_GET_SUBFAMILY_ID_ARRAY) GetF15OrLogicalIdAndRev,
    #ifndef ADVCFG_PLATFORM_SOCKETS
      #define ADVCFG_PLATFORM_SOCKETS OR_SOCKETS
    #else
      #if ADVCFG_PLATFORM_SOCKETS < OR_SOCKETS
        #undef ADVCFG_PLATFORM_SOCKETS
        #define ADVCFG_PLATFORM_SOCKETS OR_SOCKETS
      #endif
    #endif
    #ifndef ADVCFG_PLATFORM_MODULES
      #define ADVCFG_PLATFORM_MODULES OR_MODULES
    #else
      #if ADVCFG_PLATFORM_MODULES < OR_MODULES
        #undef ADVCFG_PLATFORM_MODULES
        #define ADVCFG_PLATFORM_MODULES OR_MODULES
      #endif
    #endif

    #if GET_PATCHES == TRUE
      #define F15_OR_UCODE_09

      #if AGESA_ENTRY_INIT_EARLY == TRUE
        #if OPTION_EARLY_SAMPLES == TRUE
          extern  CONST MICROCODE_PATCHES_4K ROMDATA CpuF15OrMicrocodePatch06000009;
          #undef F15_OR_UCODE_09
          #define F15_OR_UCODE_09 &CpuF15OrMicrocodePatch06000009,
        #endif
      #endif

      CONST MICROCODE_PATCHES_4K ROMDATA *CpuF15OrMicroCodePatchArray[] =
      {
        F15_OR_UCODE_09
        NULL
      };

      CONST UINT8 ROMDATA CpuF15OrNumberOfMicrocodePatches = (UINT8) ((sizeof (CpuF15OrMicroCodePatchArray) / sizeof (CpuF15OrMicroCodePatchArray[0])) - 1);
    #endif

    #define OPT_F15_OR_CPU {AMD_FAMILY_15_OR, &cpuF15OrServices},

  #else  //  OPTION_FAMILY15H_OR == TRUE
    #define OPT_F15_OR_CPU
    #define OPT_F15_OR_ID
  #endif  //  OPTION_FAMILY15H_OR == TRUE
#else  //  defined (OPTION_FAMILY15H_OR)
  #define OPT_F15_OR_CPU
  #define OPT_F15_OR_ID
#endif  //  defined (OPTION_FAMILY15H_OR)

/*
 * Install unknown family 15h support
 */


#if USES_REGISTER_TABLES == TRUE
  CONST REGISTER_TABLE ROMDATA *F15UnknownRegisterTables[] =
  {
    #if BASE_FAMILY_PCI == TRUE
      &F15PciRegisterTable,
    #endif
    #if BASE_FAMILY_MSR == TRUE
      &F15MsrRegisterTable,
    #endif
    #if BASE_FAMILY_HT_PCI == TRUE
      &F15HtPhyRegisterTable,
    #endif
    #if OPTION_MULTISOCKET == TRUE
      #if MODEL_SPECIFIC_PCI == TRUE
        &F15MultiLinkPciRegisterTable,
      #endif
    #endif
    #if OPTION_MULTISOCKET == FALSE
      #if MODEL_SPECIFIC_PCI == TRUE
        &F15SingleLinkPciRegisterTable,
      #endif
    #endif
    #if BASE_FAMILY_WORKAROUNDS == TRUE
      &F15WorkaroundsTable,
    #endif
    // the end.
    NULL
  };
#endif

#if USES_REGISTER_TABLES == TRUE
  CONST TABLE_ENTRY_TYPE_DESCRIPTOR ROMDATA F15UnknownTableEntryTypeDescriptors[] =
  {
    {MsrRegister, SetRegisterForMsrEntry},
    {PciRegister, SetRegisterForPciEntry},
    {FamSpecificWorkaround, SetRegisterForFamSpecificWorkaroundEntry},
    {HtPhyRegister, SetRegisterForHtPhyEntry},
    {HtPhyRangeRegister, SetRegisterForHtPhyRangeEntry},
    {DeemphasisRegister, SetRegisterForDeemphasisEntry},
    {ProfileFixup, SetRegisterForPerformanceProfileEntry},
    {HtHostPciRegister, SetRegisterForHtHostEntry},
    {HtHostPerfPciRegister, SetRegisterForHtHostPerfEntry},
    {HtTokenPciRegister, (PF_DO_TABLE_ENTRY)CommonVoid},
    {CoreCountsPciRegister, SetRegisterForCoreCountsPerformanceEntry},
    {ProcCountsPciRegister, SetRegisterForProcessorCountsEntry},
    {CompUnitCountsPciRegister, SetRegisterForComputeUnitCountsEntry},
    {HtFeatPciRegister, SetRegisterForHtFeaturePciEntry},
    // End
    {TableEntryTypeMax, (PF_DO_TABLE_ENTRY)CommonVoid}
  };
#endif


CONST CPU_SPECIFIC_SERVICES ROMDATA cpuF15UnknownServices =
{
  0,
  #if DISABLE_PSTATE == TRUE
    F15DisablePstate,
  #else
    (PF_CPU_DISABLE_PSTATE) CommonAssert,
  #endif
  #if TRANSITION_PSTATE == TRUE
    F15TransitionPstate,
  #else
    (PF_CPU_TRANSITION_PSTATE) CommonAssert,
  #endif
  #if PROC_IDD_MAX == TRUE
    (PF_CPU_GET_IDD_MAX) CommonReturnFalse,
  #else
    (PF_CPU_GET_IDD_MAX) CommonAssert,
  #endif
  #if GET_TSC_RATE == TRUE
    F15GetTscRate,
  #else
    (PF_CPU_GET_TSC_RATE) CommonAssert,
  #endif
  #if GET_NB_FREQ == TRUE
    F15GetCurrentNbFrequency,
  #else
    (PF_CPU_GET_NB_FREQ) CommonAssert,
  #endif
  #if GET_NB_FREQ == TRUE
    F15GetNbPstateInfo,
  #else
    (PF_CPU_GET_NB_PSTATE_INFO) CommonAssert,
  #endif
  #if IS_NBCOF_INIT_NEEDED == TRUE
    (PF_CPU_IS_NBCOF_INIT_NEEDED) CommonReturnFalse,
  #else
    (PF_CPU_IS_NBCOF_INIT_NEEDED) CommonAssert,
  #endif
  #if AP_INITIAL_LAUNCH == TRUE
    F15LaunchApCore,
  #else
    (PF_CPU_AP_INITIAL_LAUNCH) CommonAssert,
  #endif
  #if (BRAND_STRING1 == TRUE) || (BRAND_STRING2 == TRUE)
    (PF_CPU_NUMBER_OF_BRANDSTRING_CORES) CommonVoid,
  #else
    (PF_CPU_NUMBER_OF_BRANDSTRING_CORES) CommonAssert,
  #endif
  #if GET_AP_MAILBOX_FROM_HW == TRUE
    F15GetApMailboxFromHardware,
  #else
    (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE) CommonAssert,
  #endif
  #if SET_AP_CORE_NUMBER == TRUE
    F15SetApCoreNumber,
  #else
    (PF_CPU_SET_AP_CORE_NUMBER) CommonAssert,
  #endif
  #if GET_AP_CORE_NUMBER == TRUE
    F15GetApCoreNumber,
  #else
    (PF_CPU_GET_AP_CORE_NUMBER) CommonAssert,
  #endif
  #if TRANSFER_AP_CORE_NUMBER == TRUE
    F15TransferApCoreNumber,
  #else
    (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonAssert,
  #endif
  #if ID_POSITION_INITIAL_APICID == TRUE
    F15CpuAmdCoreIdPositionInInitialApicId,
  #else
    (PF_CORE_ID_POSITION_IN_INITIAL_APIC_ID) CommonAssert,
  #endif
  #if SAVE_FEATURES == TRUE
    // F15SaveFeatures,
    (PF_CPU_SAVE_FEATURES) CommonVoid,
  #else
    (PF_CPU_SAVE_FEATURES) CommonAssert,
  #endif
  #if WRITE_FEATURES == TRUE
    // F15WriteFeatures,
    (PF_CPU_WRITE_FEATURES) CommonVoid,
  #else
    (PF_CPU_WRITE_FEATURES) CommonAssert,
  #endif
  #if SET_WARM_RESET_FLAG == TRUE
    F15SetAgesaWarmResetFlag,
  #else
    (PF_CPU_SET_WARM_RESET_FLAG) CommonAssert,
  #endif
  #if GET_WARM_RESET_FLAG == TRUE
    F15GetAgesaWarmResetFlag,
  #else
    (PF_CPU_GET_WARM_RESET_FLAG) CommonAssert,
  #endif
  #if BRAND_STRING1 == TRUE
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonVoid,
  #else
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
  #endif
  #if BRAND_STRING2 == TRUE
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonVoid,
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
    GetF15CacheInfo,
  #else
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
  #endif
  #if GET_SYSTEM_PM_TABLE == TRUE
    GetF15SysPmTable,
  #else
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
  #endif
  #if GET_WHEA_INIT == TRUE
    GetF15WheaInitData,
  #else
    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
  #endif
  #if GET_PLATFORM_TYPE_SPECIFIC_INFO == TRUE
    F15GetPlatformTypeSpecificInfo,
  #else
    (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO) CommonAssert,
  #endif
  #if IS_NB_PSTATE_ENABLED == TRUE
    F15IsNbPstateEnabled,
  #else
    (PF_IS_NB_PSTATE_ENABLED) CommonAssert,
  #endif
  #if (BASE_FAMILY_HT_PCI == TRUE)
    F15NextLinkHasHtPhyFeats,
  #else
    (PF_NEXT_LINK_HAS_HTFPY_FEATS) CommonReturnFalse,
  #endif
  #if (BASE_FAMILY_HT_PCI == TRUE)
    F15SetHtPhyRegister,
  #else
    (PF_SET_HT_PHY_REGISTER) CommonVoid,
  #endif
  #if BASE_FAMILY_PCI == TRUE
    F15GetNextHtLinkFeatures,
  #else
    (PF_GET_NEXT_HT_LINK_FEATURES) CommonAssert,
  #endif
  #if USES_REGISTER_TABLES == TRUE
    (REGISTER_TABLE **) F15UnknownRegisterTables,
  #else
    NULL,
  #endif
  #if USES_REGISTER_TABLES == TRUE
    (TABLE_ENTRY_TYPE_DESCRIPTOR *) F15UnknownTableEntryTypeDescriptors,
  #else
    NULL,
  #endif
  NULL,
  NULL,
  InitCacheEnabled,
  #if AGESA_ENTRY_INIT_EARLY == TRUE
    GetF15EarlyInitOnCoreTable
  #else
    (PF_GET_EARLY_INIT_TABLE) CommonVoid
  #endif
};

// Family 15h maximum base address is 48 bits. Limit BLDCFG to 48 bits, if appropriate.
#if (FAMILY_MMIO_BASE_MASK < 0xFFFF000000000000ull)
  #undef  FAMILY_MMIO_BASE_MASK
  #define FAMILY_MMIO_BASE_MASK (0xFFFF000000000000ull)
#endif


#undef OPT_F15_ID_TABLE
#define OPT_F15_ID_TABLE {0x15, {AMD_FAMILY_15, AMD_F15_UNKNOWN}, F15LogicalIdTable, (sizeof (F15LogicalIdTable) / sizeof (F15LogicalIdTable[0]))},
#define OPT_F15_UNKNOWN_CPU {AMD_FAMILY_15, &cpuF15UnknownServices},

#undef OPT_F15_TABLE
#define OPT_F15_TABLE   OPT_F15_OR_CPU  OPT_F15_UNKNOWN_CPU


#if OPTION_G34_SOCKET_SUPPORT == TRUE
  #define F15_G34_BRANDSTRING1 NULL,
  #define F15_G34_BRANDSTRING2 NULL,
#else
  #define F15_G34_BRANDSTRING1
  #define F15_G34_BRANDSTRING2
#endif
#if OPTION_C32_SOCKET_SUPPORT == TRUE
  #define F15_C32_BRANDSTRING1 NULL,
  #define F15_C32_BRANDSTRING2 NULL,
#else
  #define F15_C32_BRANDSTRING1
  #define F15_C32_BRANDSTRING2
#endif
#if OPTION_AM3_SOCKET_SUPPORT == TRUE
  #define F15_AM3_BRANDSTRING1 NULL,
  #define F15_AM3_BRANDSTRING2 NULL,
#else
  #define F15_AM3_BRANDSTRING1
  #define F15_AM3_BRANDSTRING2
#endif

#if BRAND_STRING1 == TRUE
  CONST CPU_BRAND_TABLE ROMDATA *F15BrandIdString1Tables[] =
  {
    F15_G34_BRANDSTRING1
    F15_C32_BRANDSTRING1
    F15_AM3_BRANDSTRING1
  };

  CONST UINT8 F15BrandIdString1TableCount = (sizeof (F15BrandIdString1Tables) / sizeof (F15BrandIdString1Tables[0]));
#endif

#if BRAND_STRING2 == TRUE
  CONST CPU_BRAND_TABLE ROMDATA *F15BrandIdString2Tables[] =
  {
    F15_G34_BRANDSTRING2
    F15_C32_BRANDSTRING2
    F15_AM3_BRANDSTRING2
  };

  CONST UINT8 F15BrandIdString2TableCount = (sizeof (F15BrandIdString2Tables) / sizeof (F15BrandIdString2Tables[0]));
#endif

CONST PF_CPU_GET_SUBFAMILY_ID_ARRAY ROMDATA F15LogicalIdTable[] =
{
  OPT_F15_OR_ID
};

#endif  // _OPTION_FAMILY_15H_INSTALL_H_
