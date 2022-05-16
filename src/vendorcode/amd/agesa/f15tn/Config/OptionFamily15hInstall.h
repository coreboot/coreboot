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
 * @e \$Revision: 64491 $   @e \$Date: 2012-01-23 12:37:30 -0600 (Mon, 23 Jan 2012) $
 */
/*****************************************************************************
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
 *
 ***************************************************************************/

#ifndef _OPTION_FAMILY_15H_INSTALL_H_
#define _OPTION_FAMILY_15H_INSTALL_H_

#include <Proc/CPU/cpuFamilyTranslation.h>

/*
 * Pull in family specific services based on entry point
 */

/*
 * Common Family 15h routines
 */
extern F_IS_NB_PSTATE_ENABLED F15IsNbPstateEnabled;

/*
 * Install family 15h model 0 support
 */
#ifdef OPTION_FAMILY15H_OR
  #if OPTION_FAMILY15H_OR == TRUE
    extern F_CPU_GET_IDD_MAX F15OrGetProcIddMax;
    extern F_CPU_GET_NB_PSTATE_INFO F15OrGetNbPstateInfo;
    extern F_CPU_IS_NBCOF_INIT_NEEDED F15CommonGetNbCofVidUpdate;
    extern F_CPU_DISABLE_PSTATE F15DisablePstate;
    extern F_CPU_TRANSITION_PSTATE F15TransitionPstate;
    extern F_CPU_GET_TSC_RATE F15GetTscRate;
    extern F_CPU_GET_NB_FREQ F15OrGetCurrentNbFrequency;
    extern F_CPU_GET_MIN_MAX_NB_FREQ F15OrGetMinMaxNbFrequency;
    extern F_CPU_AP_INITIAL_LAUNCH F15LaunchApCore;
    extern F_CPU_NUMBER_OF_PHYSICAL_CORES F15OrGetNumberOfPhysicalCores;
    extern F_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE F15OrGetApMailboxFromHardware;
    extern F_CPU_SET_AP_CORE_NUMBER F15OrSetApCoreNumber;
    extern F_CPU_GET_AP_CORE_NUMBER F15OrGetApCoreNumber;
    extern F_CPU_TRANSFER_AP_CORE_NUMBER F15OrTransferApCoreNumber;
    extern F_CORE_ID_POSITION_IN_INITIAL_APIC_ID F15CpuAmdCoreIdPositionInInitialApicId;
    extern F_CPU_SET_WARM_RESET_FLAG F15SetAgesaWarmResetFlag;
    extern F_CPU_GET_WARM_RESET_FLAG F15GetAgesaWarmResetFlag;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF15CacheInfo;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF15OrSysPmTable;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF15WheaInitData;
    extern F_CPU_SET_CFOH_REG SetF15OrCacheFlushOnHaltRegister;
    extern F_NEXT_LINK_HAS_HTFPY_FEATS F15NextLinkHasHtPhyFeats;
    extern F_SET_HT_PHY_REGISTER F15SetHtPhyRegister;
    extern F_GET_NEXT_HT_LINK_FEATURES F15GetNextHtLinkFeatures;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF15OrMicroCodePatchesStruct;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF15OrMicrocodeEquivalenceTable;
    extern F_GET_EARLY_INIT_TABLE GetF15OrEarlyInitOnCoreTable;
    extern CONST REGISTER_TABLE ROMDATA F15OrPciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15OrMsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15OrSharedMsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15OrSharedMsrCuRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15OrSharedMsrWorkaroundTable;
    extern CONST REGISTER_TABLE ROMDATA F15OrHtPhyRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15PciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15MsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15OrMultiLinkPciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15OrSingleLinkPciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15OrWorkaroundsTable;
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
          &F15OrMultiLinkPciRegisterTable,
          &F15OrSingleLinkPciRegisterTable,
        #endif
        #if MODEL_SPECIFIC_PCI == TRUE
          &F15OrPciRegisterTable,
          #if OPTION_EARLY_SAMPLES == TRUE
            &F15OrEarlySamplePciRegisterTable,
          #endif
        #endif
        #if BASE_FAMILY_MSR == TRUE
          &F15MsrRegisterTable,
        #endif
        #if MODEL_SPECIFIC_MSR == TRUE
          &F15OrMsrRegisterTable,
          #if OPTION_EARLY_SAMPLES == TRUE
            &F15OrEarlySampleMsrRegisterTable,
          #endif
        #endif
        #if MODEL_SPECIFIC_MSR == TRUE
          &F15OrSharedMsrRegisterTable,
          &F15OrSharedMsrCuRegisterTable,
          &F15OrSharedMsrWorkaroundTable,
          #if OPTION_EARLY_SAMPLES == TRUE
            &F15OrEarlySampleSharedMsrRegisterTable,
            &F15OrEarlySampleSharedMsrWorkaroundTable,
          #endif
        #endif
        #if MODEL_SPECIFIC_HT_PCI == TRUE
          &F15OrHtPhyRegisterTable,
        #endif
        #if BASE_FAMILY_WORKAROUNDS == TRUE
          &F15OrWorkaroundsTable,
          #if OPTION_EARLY_SAMPLES == TRUE
            &F15OrEarlySampleWorkaroundsTable,
          #endif
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
        {CompUnitCountsMsr, SetMsrForComputeUnitCountsEntry},
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
        F15OrGetProcIddMax,
      #else
        (PF_CPU_GET_IDD_MAX) CommonAssert,
      #endif
      #if GET_TSC_RATE == TRUE
        F15GetTscRate,
      #else
        (PF_CPU_GET_TSC_RATE) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F15OrGetCurrentNbFrequency,
      #else
        (PF_CPU_GET_NB_FREQ) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F15OrGetMinMaxNbFrequency,
      #else
        (PF_CPU_GET_MIN_MAX_NB_FREQ) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F15OrGetNbPstateInfo,
      #else
        (PF_CPU_GET_NB_PSTATE_INFO) CommonAssert,
      #endif
      #if IS_NBCOF_INIT_NEEDED == TRUE
        F15CommonGetNbCofVidUpdate,
      #else
        (PF_CPU_IS_NBCOF_INIT_NEEDED) CommonAssert,
      #endif
      #if GET_NB_IDD_MAX == TRUE
        (PF_CPU_GET_NB_IDD_MAX) CommonAssert,
      #else
        (PF_CPU_GET_NB_IDD_MAX) CommonAssert,
      #endif
      #if AP_INITIAL_LAUNCH == TRUE
        F15LaunchApCore,
      #else
        (PF_CPU_AP_INITIAL_LAUNCH) CommonAssert,
      #endif
      #if (BRAND_STRING1 == TRUE) || (BRAND_STRING2 == TRUE)
        F15OrGetNumberOfPhysicalCores,
      #else
        (PF_CPU_NUMBER_OF_PHYSICAL_CORES) CommonAssert,
      #endif
      #if GET_AP_MAILBOX_FROM_HW == TRUE
        F15OrGetApMailboxFromHardware,
      #else
        (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE) CommonAssert,
      #endif
      #if SET_AP_CORE_NUMBER == TRUE
        F15OrSetApCoreNumber,
      #else
        (PF_CPU_SET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if GET_AP_CORE_NUMBER == TRUE
        F15OrGetApCoreNumber,
      #else
        (PF_CPU_GET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if TRANSFER_AP_CORE_NUMBER == TRUE
        F15OrTransferApCoreNumber,
      #else
        (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if ID_POSITION_INITIAL_APICID == TRUE
        F15CpuAmdCoreIdPositionInInitialApicId,
      #else
        (PF_CORE_ID_POSITION_IN_INITIAL_APIC_ID) CommonAssert,
      #endif
      #if SAVE_FEATURES == TRUE
        // F15OrSaveFeatures,
        (PF_CPU_SAVE_FEATURES) CommonVoid,
      #else
        (PF_CPU_SAVE_FEATURES) CommonAssert,
      #endif
      #if WRITE_FEATURES == TRUE
        // F15OrWriteFeatures,
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
        GetF15OrSysPmTable,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_WHEA_INIT == TRUE
        GetF15WheaInitData,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PLATFORM_TYPE_SPECIFIC_INFO == TRUE
        (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO) CommonReturnAgesaSuccess,
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
        GetF15OrEarlyInitOnCoreTable
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
      #define F15_OR_UCODE_17_UNENC
      #define F15_OR_UCODE_11F_UNENC
      #define F15_OR_UCODE_425
      #define F15_OR_UCODE_509
      #define F15_OR_UCODE_602

      #if AGESA_ENTRY_INIT_EARLY == TRUE
        extern  CONST UINT8 ROMDATA CpuF15OrMicrocodePatch06000602 [];
        #undef F15_OR_UCODE_602
        #define F15_OR_UCODE_602 CpuF15OrMicrocodePatch06000602,
      #endif

      CONST UINT8 ROMDATA *CpuF15OrMicroCodePatchArray[] =
      {
        F15_OR_UCODE_602
        F15_OR_UCODE_509
        F15_OR_UCODE_425
        F15_OR_UCODE_11F_UNENC
        F15_OR_UCODE_17_UNENC
        NULL
      };

      CONST UINT8 ROMDATA CpuF15OrNumberOfMicrocodePatches = (UINT8) ((sizeof (CpuF15OrMicroCodePatchArray) / sizeof (CpuF15OrMicroCodePatchArray[0])) - 1);
    #endif

    #if OPTION_EARLY_SAMPLES == TRUE
      extern F_F15_OR_ES_AVOID_NB_CYCLES_START F15OrEarlySamplesAvoidNbCyclesStart;
      extern F_F15_OR_ES_AVOID_NB_CYCLES_END F15OrEarlySamplesAvoidNbCyclesEnd;
      extern F_F15_OR_ES_LOAD_MCU_PATCH F15OrEarlySamplesLoadMicrocodePatch;
      extern F_F15_OR_ES_AFTER_PATCH_LOADED F15OrEarlySamplesAfterPatchLoaded;

      CONST F15_OR_ES_MCU_PATCH ROMDATA F15OrEarlySampleLoadMcuPatch =
      {
        F15OrEarlySamplesAvoidNbCyclesStart,
        F15OrEarlySamplesAvoidNbCyclesEnd,
        F15OrEarlySamplesLoadMicrocodePatch,
        F15OrEarlySamplesAfterPatchLoaded
      };
    #else
      CONST F15_OR_ES_MCU_PATCH ROMDATA F15OrEarlySampleLoadMcuPatch =
      {
        (PF_F15_OR_ES_AVOID_NB_CYCLES_START)  CommonVoid,
        (PF_F15_OR_ES_AVOID_NB_CYCLES_END)    CommonVoid,
        (PF_F15_OR_ES_LOAD_MCU_PATCH)         LoadMicrocodePatch,
        (PF_F15_OR_ES_AFTER_PATCH_LOADED)     CommonVoid
      };
    #endif

    #if OPTION_EARLY_SAMPLES == TRUE
      extern F_F15_OR_ES_HTC_INIT_HOOK F15OrHtcInitEarlySampleHook;

      CONST F15_OR_ES_CORE_SUPPORT ROMDATA F15OrEarlySampleCoreSupport =
      {
        #if AGESA_ENTRY_INIT_EARLY == TRUE
          F15OrHtcInitEarlySampleHook,
        #else
          (PF_F15_OR_ES_HTC_INIT_HOOK) CommonAssert,
        #endif
      };
    #else
      CONST F15_OR_ES_CORE_SUPPORT ROMDATA F15OrEarlySampleCoreSupport =
      {
        #if AGESA_ENTRY_INIT_EARLY == TRUE
          (PF_F15_OR_ES_HTC_INIT_HOOK) CommonVoid,
        #else
          (PF_F15_OR_ES_HTC_INIT_HOOK) CommonAssert,
        #endif
      };
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
 * Install family 15h model 10h - 1Fh support
 */
#ifdef OPTION_FAMILY15H_TN
  #if OPTION_FAMILY15H_TN == TRUE
    extern F_CPU_GET_IDD_MAX F15TnGetProcIddMax;
    extern F_CPU_GET_NB_PSTATE_INFO F15TnGetNbPstateInfo;
    extern F_CPU_IS_NBCOF_INIT_NEEDED F15CommonGetNbCofVidUpdate;
    extern F_CPU_GET_NB_IDD_MAX F15TnGetNbIddMax;
    extern F_CPU_DISABLE_PSTATE F15DisablePstate;
    extern F_CPU_TRANSITION_PSTATE F15TransitionPstate;
    extern F_CPU_GET_TSC_RATE F15GetTscRate;
    extern F_CPU_GET_NB_FREQ F15TnGetCurrentNbFrequency;
    extern F_CPU_GET_MIN_MAX_NB_FREQ F15TnGetMinMaxNbFrequency;
    extern F_CPU_AP_INITIAL_LAUNCH F15LaunchApCore;
    extern F_CPU_NUMBER_OF_PHYSICAL_CORES F15TnGetNumberOfPhysicalCores;
    extern F_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE F15TnGetApMailboxFromHardware;
    extern F_CPU_GET_AP_CORE_NUMBER F15TnGetApCoreNumber;
    extern F_CORE_ID_POSITION_IN_INITIAL_APIC_ID F15CpuAmdCoreIdPositionInInitialApicId;
    extern F_CPU_SET_WARM_RESET_FLAG F15SetAgesaWarmResetFlag;
    extern F_CPU_GET_WARM_RESET_FLAG F15GetAgesaWarmResetFlag;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF15CacheInfo;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF15TnSysPmTable;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF15WheaInitData;
    extern F_IS_NB_PSTATE_ENABLED F15TnIsNbPstateEnabled;
    extern F_CPU_SET_CFOH_REG SetF15TnCacheFlushOnHaltRegister;
    extern F_NEXT_LINK_HAS_HTFPY_FEATS F15NextLinkHasHtPhyFeats;
    extern F_SET_HT_PHY_REGISTER F15SetHtPhyRegister;
    extern F_GET_NEXT_HT_LINK_FEATURES F15GetNextHtLinkFeatures;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF15TnMicroCodePatchesStruct;
    extern F_CPU_GET_FAMILY_SPECIFIC_ARRAY GetF15TnMicrocodeEquivalenceTable;
    extern F_GET_EARLY_INIT_TABLE GetF15TnEarlyInitOnCoreTable;
    extern CONST REGISTER_TABLE ROMDATA F15TnPciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15TnPciWorkaroundTable;
    extern CONST REGISTER_TABLE ROMDATA F15TnMsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15TnMsrWorkaroundTable;
    extern CONST REGISTER_TABLE ROMDATA F15TnSharedMsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15TnSharedMsrCuRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15TnSharedMsrWorkaroundTable;
    extern CONST REGISTER_TABLE ROMDATA F15TnPerNodeMsrWorkaroundTable;
    extern CONST REGISTER_TABLE ROMDATA F15PciRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F15MsrRegisterTable;

    /**
     * Core Pair and core pair primary determination table.
     *
     * The two fields from the core pair hardware register can be used to determine whether
     * even number cores are primary or all cores are primary.  It can be extended if it is
     * decided to have other configs as well.  The other logically possible value sets are BitMapMapping,
     * but they are currently not supported by the processor.
     */
    CONST CORE_PAIR_MAP ROMDATA HtFam15TnCorePairMapping[] =
    {
      {1, 1, EvenCoresMapping},                                     ///< 1 Compute Unit with 2 cores
      {3, 3, EvenCoresMapping},                                     ///< 2 Compute Units both with 2 Cores
      {1, 0, AllCoresMapping},                                      ///< 1 Compute Unit with 1 core
      {3, 0, AllCoresMapping},                                      ///< 2 Compute Units both with 1 Core
      {HT_LIST_TERMINAL, HT_LIST_TERMINAL, MaxComputeUnitMapping}   ///< End
    };


    #if USES_REGISTER_TABLES == TRUE
      CONST REGISTER_TABLE ROMDATA * CONST F15TnRegisterTables[] =
      {
        #if BASE_FAMILY_PCI == TRUE
          &F15PciRegisterTable,
        #endif
        #if MODEL_SPECIFIC_PCI == TRUE
          &F15TnPciRegisterTable,
          &F15TnPciWorkaroundTable,
        #endif
        #if BASE_FAMILY_MSR == TRUE
          &F15MsrRegisterTable,
        #endif
        #if MODEL_SPECIFIC_MSR == TRUE
          &F15TnMsrRegisterTable,
          &F15TnMsrWorkaroundTable,
          &F15TnSharedMsrRegisterTable,
          &F15TnSharedMsrCuRegisterTable,
          &F15TnSharedMsrWorkaroundTable,
          &F15TnPerNodeMsrWorkaroundTable,
        #endif
        // the end.
        NULL
      };
    #endif

    #if USES_REGISTER_TABLES == TRUE
      CONST TABLE_ENTRY_TYPE_DESCRIPTOR ROMDATA F15TnTableEntryTypeDescriptors[] =
      {
        {MsrRegister, SetRegisterForMsrEntry},
        {PciRegister, SetRegisterForPciEntry},
        {FamSpecificWorkaround, SetRegisterForFamSpecificWorkaroundEntry},
        {ProfileFixup, SetRegisterForPerformanceProfileEntry},
        {CompUnitCountsMsr, SetMsrForComputeUnitCountsEntry},
        {CoreCountsPciRegister, SetRegisterForCoreCountsPerformanceEntry},
        {ProcCountsPciRegister, SetRegisterForProcessorCountsEntry},
        {CompUnitCountsPciRegister, SetRegisterForComputeUnitCountsEntry},
        // End
        {TableEntryTypeMax, (PF_DO_TABLE_ENTRY)CommonVoid}
      };
    #endif

    CONST CPU_SPECIFIC_SERVICES ROMDATA cpuF15TnServices =
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
        F15TnGetProcIddMax,
      #else
        (PF_CPU_GET_IDD_MAX) CommonAssert,
      #endif
      #if GET_TSC_RATE == TRUE
        F15GetTscRate,
      #else
        (PF_CPU_GET_TSC_RATE) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F15TnGetCurrentNbFrequency,
      #else
        (PF_CPU_GET_NB_FREQ) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F15TnGetMinMaxNbFrequency,
      #else
        (PF_CPU_GET_MIN_MAX_NB_FREQ) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        F15TnGetNbPstateInfo,
      #else
        (PF_CPU_GET_NB_PSTATE_INFO) CommonAssert,
      #endif
      #if IS_NBCOF_INIT_NEEDED == TRUE
        F15CommonGetNbCofVidUpdate,
      #else
        (PF_CPU_IS_NBCOF_INIT_NEEDED) CommonAssert,
      #endif
      #if GET_NB_IDD_MAX == TRUE
        (PF_CPU_GET_NB_IDD_MAX) F15TnGetNbIddMax,
      #else
        (PF_CPU_GET_NB_IDD_MAX) CommonAssert,
      #endif
      #if AP_INITIAL_LAUNCH == TRUE
        F15LaunchApCore,
      #else
        (PF_CPU_AP_INITIAL_LAUNCH) CommonAssert,
      #endif
      #if (BRAND_STRING1 == TRUE) || (BRAND_STRING2 == TRUE)
        F15TnGetNumberOfPhysicalCores,
      #else
        (PF_CPU_NUMBER_OF_PHYSICAL_CORES) CommonAssert,
      #endif
      #if GET_AP_MAILBOX_FROM_HW == TRUE
        F15TnGetApMailboxFromHardware,
      #else
        (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE) CommonAssert,
      #endif
      #if SET_AP_CORE_NUMBER == TRUE
        (PF_CPU_SET_AP_CORE_NUMBER) CommonVoid,
      #else
        (PF_CPU_SET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if GET_AP_CORE_NUMBER == TRUE
        F15TnGetApCoreNumber,
      #else
        (PF_CPU_GET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if TRANSFER_AP_CORE_NUMBER == TRUE
        (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonVoid,
      #else
        (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if ID_POSITION_INITIAL_APICID == TRUE
        F15CpuAmdCoreIdPositionInInitialApicId,
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
        GetF15TnMicroCodePatchesStruct,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PATCHES_EQUIVALENCE_TABLE == TRUE
        GetF15TnMicrocodeEquivalenceTable,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_CACHE_INFO == TRUE
        GetF15CacheInfo,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_SYSTEM_PM_TABLE == TRUE
        GetF15TnSysPmTable,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_WHEA_INIT == TRUE
        GetF15WheaInitData,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PLATFORM_TYPE_SPECIFIC_INFO == TRUE
        (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO) CommonReturnAgesaSuccess,
      #else
        (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO) CommonAssert,
      #endif
      #if IS_NB_PSTATE_ENABLED == TRUE
        F15TnIsNbPstateEnabled,
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
        (CONST REGISTER_TABLE **) F15TnRegisterTables,
      #else
        NULL,
      #endif
      #if USES_REGISTER_TABLES == TRUE
        (TABLE_ENTRY_TYPE_DESCRIPTOR *) F15TnTableEntryTypeDescriptors,
      #else
        NULL,
      #endif
      #if MODEL_SPECIFIC_HT_PCI == TRUE
        NULL,
      #else
        NULL,
      #endif
      (CORE_PAIR_MAP *) &HtFam15TnCorePairMapping,
      InitCacheEnabled,
      #if AGESA_ENTRY_INIT_EARLY == TRUE
        GetF15TnEarlyInitOnCoreTable
      #else
        (PF_GET_EARLY_INIT_TABLE) CommonVoid
      #endif
    };

    #define TN_SOCKETS 1
    #define TN_MODULES 1
    #define TN_RECOVERY_SOCKETS 1
    #define TN_RECOVERY_MODULES 1
    extern F_CPU_GET_SUBFAMILY_ID_ARRAY GetF15TnLogicalIdAndRev;
    #define OPT_F15_TN_ID (PF_CPU_GET_SUBFAMILY_ID_ARRAY) GetF15TnLogicalIdAndRev,
    #ifndef ADVCFG_PLATFORM_SOCKETS
      #define ADVCFG_PLATFORM_SOCKETS TN_SOCKETS
    #else
      #if ADVCFG_PLATFORM_SOCKETS < TN_SOCKETS
        #undef ADVCFG_PLATFORM_SOCKETS
        #define ADVCFG_PLATFORM_SOCKETS TN_SOCKETS
      #endif
    #endif
    #ifndef ADVCFG_PLATFORM_MODULES
      #define ADVCFG_PLATFORM_MODULES TN_MODULES
    #else
      #if ADVCFG_PLATFORM_MODULES < TN_MODULES
        #undef ADVCFG_PLATFORM_MODULES
        #define ADVCFG_PLATFORM_MODULES TN_MODULES
      #endif
    #endif

    #if GET_PATCHES == TRUE
      #define F15_TN_UCODE_10F
      #define F15_TN_UCODE_0E

      #if AGESA_ENTRY_INIT_EARLY == TRUE
        extern  CONST UINT8 ROMDATA CpuF15TnMicrocodePatch0600111F_Enc [];
        #undef F15_TN_UCODE_10F
        #define F15_TN_UCODE_10F CpuF15TnMicrocodePatch0600111F_Enc,

      #endif

      CONST UINT8 ROMDATA * CONST CpuF15TnMicroCodePatchArray[] =
      {
        F15_TN_UCODE_10F
        F15_TN_UCODE_0E
        NULL
      };

      CONST UINT8 ROMDATA CpuF15TnNumberOfMicrocodePatches = (UINT8) ((sizeof (CpuF15TnMicroCodePatchArray) / sizeof (CpuF15TnMicroCodePatchArray[0])) - 1);
    #endif



    #define OPT_F15_TN_CPU {AMD_FAMILY_15_TN, &cpuF15TnServices},

  #else  //  OPTION_FAMILY15H_TN == TRUE
    #define OPT_F15_TN_CPU
    #define OPT_F15_TN_ID
  #endif  //  OPTION_FAMILY15H_TN == TRUE
#else  //  defined (OPTION_FAMILY15H_TN)
  #define OPT_F15_TN_CPU
  #define OPT_F15_TN_ID
#endif  //  defined (OPTION_FAMILY15H_TN)



/*
 * Install unknown family 15h support
 */

#ifdef OPTION_FAMILY15H_UNKNOWN
  #if OPTION_FAMILY15H_UNKNOWN == TRUE
    #if USES_REGISTER_TABLES == TRUE
      CONST REGISTER_TABLE ROMDATA *F15UnknownRegisterTables[] =
      {
        #if BASE_FAMILY_PCI == TRUE
          &F15PciRegisterTable,
        #endif
        #if BASE_FAMILY_MSR == TRUE
          &F15MsrRegisterTable,
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
        {CompUnitCountsMsr, SetMsrForComputeUnitCountsEntry},
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
        (PF_CPU_GET_NB_FREQ) CommonAssert,
      #else
        (PF_CPU_GET_NB_FREQ) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        (PF_CPU_GET_MIN_MAX_NB_FREQ) CommonAssert,
      #else
        (PF_CPU_GET_MIN_MAX_NB_FREQ) CommonAssert,
      #endif
      #if GET_NB_FREQ == TRUE
        (PF_CPU_GET_NB_PSTATE_INFO) CommonAssert,
      #else
        (PF_CPU_GET_NB_PSTATE_INFO) CommonAssert,
      #endif
      #if IS_NBCOF_INIT_NEEDED == TRUE
        (PF_CPU_IS_NBCOF_INIT_NEEDED) CommonReturnFalse,
      #else
        (PF_CPU_IS_NBCOF_INIT_NEEDED) CommonAssert,
      #endif
      #if GET_NB_IDD_MAX == TRUE
        (PF_CPU_GET_NB_IDD_MAX) CommonAssert,
      #else
        (PF_CPU_GET_NB_IDD_MAX) CommonAssert,
      #endif
      #if AP_INITIAL_LAUNCH == TRUE
        F15LaunchApCore,
      #else
        (PF_CPU_AP_INITIAL_LAUNCH) CommonAssert,
      #endif
      #if (BRAND_STRING1 == TRUE) || (BRAND_STRING2 == TRUE)
        (PF_CPU_NUMBER_OF_PHYSICAL_CORES) CommonVoid,
      #else
        (PF_CPU_NUMBER_OF_PHYSICAL_CORES) CommonAssert,
      #endif
      #if GET_AP_MAILBOX_FROM_HW == TRUE
        (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE) CommonAssert,
      #else
        (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE) CommonAssert,
      #endif
      #if SET_AP_CORE_NUMBER == TRUE
        (PF_CPU_SET_AP_CORE_NUMBER) CommonAssert,
      #else
        (PF_CPU_SET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if GET_AP_CORE_NUMBER == TRUE
        (PF_CPU_GET_AP_CORE_NUMBER) CommonAssert,
      #else
        (PF_CPU_GET_AP_CORE_NUMBER) CommonAssert,
      #endif
      #if TRANSFER_AP_CORE_NUMBER == TRUE
        (PF_CPU_TRANSFER_AP_CORE_NUMBER) CommonAssert,
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
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonVoid,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_SYSTEM_PM_TABLE == TRUE
        GetEmptyArray,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_WHEA_INIT == TRUE
        GetF15WheaInitData,
      #else
        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) CommonAssert,
      #endif
      #if GET_PLATFORM_TYPE_SPECIFIC_INFO == TRUE
        (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO) CommonReturnAgesaSuccess,
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
        (PF_GET_EARLY_INIT_TABLE) CommonVoid
      #else
        (PF_GET_EARLY_INIT_TABLE) CommonVoid
      #endif
    };

    #define OPT_F15_UNKNOWN_CPU {(AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) , &cpuF15UnknownServices},

  #else  // OPTION_FAMILY15H_UNKNOWN == TRUE
    #define OPT_F15_UNKNOWN_CPU
    #define OPT_F15_UNKNOWN_ID
  #endif  // OPTION_FAMILY15H_UNKNOWN == TRUE
#else  // defined OPTION_FAMILY15H_UNKNOWN
  #define OPT_F15_UNKNOWN_CPU
  #define OPT_F15_UNKNOWN_ID
#endif  // defined OPTION_FAMILY15H_UNKNOWN


// Family 15h maximum base address is 48 bits. Limit BLDCFG to 48 bits, if appropriate.
#if (FAMILY_MMIO_BASE_MASK < 0xFFFF000000000000ull)
  #undef  FAMILY_MMIO_BASE_MASK
  #define FAMILY_MMIO_BASE_MASK (0xFFFF000000000000ull)
#endif


#undef OPT_F15_ID_TABLE
#define OPT_F15_ID_TABLE {0x15, {(AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) , (AMD_FAMILY_UNKNOWN | AMD_F15_OR_B2 | AMD_F15_TN_A0 | 0x0000000000100000ull ) }, F15LogicalIdTable, (sizeof (F15LogicalIdTable) / sizeof (F15LogicalIdTable[0]))},

#undef OPT_F15_TABLE
#define OPT_F15_TABLE   OPT_F15_OR_CPU OPT_F15_TN_CPU {0, NULL}, OPT_F15_UNKNOWN_CPU

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
#if OPTION_FS1_SOCKET_SUPPORT == TRUE
  #define F15_FS1_BRANDSTRING1 NULL,
  #define F15_FS1_BRANDSTRING2 NULL,
#else
  #define F15_FS1_BRANDSTRING1
  #define F15_FS1_BRANDSTRING2
#endif
#if OPTION_FM2_SOCKET_SUPPORT == TRUE
  #define F15_FM2_BRANDSTRING1 NULL,
  #define F15_FM2_BRANDSTRING2 NULL,
#else
  #define F15_FM2_BRANDSTRING1
  #define F15_FM2_BRANDSTRING2
#endif
#if OPTION_FP2_SOCKET_SUPPORT == TRUE
  #define F15_FP2_BRANDSTRING1 NULL,
  #define F15_FP2_BRANDSTRING2 NULL,
#else
  #define F15_FP2_BRANDSTRING1
  #define F15_FP2_BRANDSTRING2
#endif


#if BRAND_STRING1 == TRUE
  CONST CPU_BRAND_TABLE ROMDATA *F15BrandIdString1Tables[] =
  {
    F15_G34_BRANDSTRING1
    F15_C32_BRANDSTRING1
    F15_AM3_BRANDSTRING1
    F15_FS1_BRANDSTRING1
    NULL,
    F15_FM2_BRANDSTRING1
    NULL,
    F15_FP2_BRANDSTRING1
  };

  CONST UINT8 F15BrandIdString1TableCount = (sizeof (F15BrandIdString1Tables) / sizeof (F15BrandIdString1Tables[0]));
#endif

#if BRAND_STRING2 == TRUE
  CONST CPU_BRAND_TABLE ROMDATA *F15BrandIdString2Tables[] =
  {
    F15_G34_BRANDSTRING2
    F15_C32_BRANDSTRING2
    F15_AM3_BRANDSTRING2
    F15_FS1_BRANDSTRING2
    NULL,
    F15_FM2_BRANDSTRING2
    NULL,
    F15_FP2_BRANDSTRING2
  };

  CONST UINT8 F15BrandIdString2TableCount = (sizeof (F15BrandIdString2Tables) / sizeof (F15BrandIdString2Tables[0]));
#endif

CONST PF_CPU_GET_SUBFAMILY_ID_ARRAY ROMDATA F15LogicalIdTable[] =
{
  OPT_F15_OR_ID
  OPT_F15_TN_ID
  NULL
};

#endif  // _OPTION_FAMILY_15H_INSTALL_H_
