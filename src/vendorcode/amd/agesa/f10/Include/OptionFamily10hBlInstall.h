/**
 * @file
 *
 * Install of family 10h model 5 support
 *
 * This file generates the defaults tables for family 10h model 5 processors.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
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
 *
 ***************************************************************************/

#ifdef OPTION_FAMILY10H
  #if OPTION_FAMILY10H == TRUE
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
        extern F_CPU_GET_NB_FREQ F10CommonRevCGetNbFrequency;
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
            // the end.
            NULL
          };
        #endif

        #if USES_REGISTER_TABLES == TRUE
          CONST TABLE_ENTRY_TYPE_DESCRIPTOR ROMDATA F10BlTableEntryTypeDescriptors[] =
          {
            {MSRREGISTER, SetRegisterForMsrEntry},
            {PCIREGISTER, SetRegisterForPciEntry},
            {ErrataWorkaround, SetRegisterForErrataWorkaroundEntry},
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
          #if GET_PSTATE_POWER == TRUE
            F10GetPstatePower,
          #else
            (PF_CPU_GET_PSTATE_POWER) CommonAssert,
          #endif
          #if GET_PSTATE_FREQ == TRUE
            F10GetPstateFrequency,
          #else
            (PF_CPU_GET_PSTATE_FREQ) CommonAssert,
          #endif
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
          #if PSTATE_TRANSITION_LATENCY == TRUE
            F10GetPstateTransLatency,
          #else
            (PF_CPU_PSTATE_TRANSITION_LATENCY) CommonAssert,
          #endif
          #if GET_PSTATE_REGISTER_INFO == TRUE
            F10GetPstateRegisterInfo,
          #else
            (PF_CPU_GET_PSTATE_REGISTER_INFO) CommonAssert,
          #endif
          #if GET_PSTATE_MAX_STATE == TRUE
            F10GetPstateMaxState,
          #else
            (PF_CPU_GET_PSTATE_MAX_STATE) CommonAssert,
          #endif
          #if SET_PSTATE_LEVELING_REG == TRUE
            F10PstateLevelingCoreMsrModify,
          #else
            (PF_CPU_SET_PSTATE_LEVELING_REG) CommonAssert,
          #endif
          #if GET_NB_FREQ == TRUE
            F10CommonRevCGetNbFrequency,
          #else
            (PF_CPU_GET_NB_FREQ) CommonAssert,
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
            F10DoesLinkHaveHtPhyFeats,
          #else
            (PF_DOES_LINK_HAVE_HTFPY_FEATS) CommonReturnFalse,
          #endif
          #if (BASE_FAMILY_HT_PCI == TRUE)
            F10SetHtPhyRegister,
          #else
            (PF_SET_HT_PHY_REGISTER) CommonAssert,
          #endif
          #if BASE_FAMILY_PCI == TRUE
            F10GetHtLinkFeatures,
          #else
            (PF_GET_HT_LINK_FEATURES) CommonAssert,
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
          #if AGESA_ENTRY_INIT_EARLY == TRUE
            GetCommonEarlyInitOnCoreTable
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
        #define OPT_F10_BL_CPU {AMD_FAMILY_10_BL, &cpuF10BlServices},
      #else
        #define OPT_F10_BL_CPU
        #define OPT_F10_BL_ID
      #endif
    #else
      #define OPT_F10_BL_CPU
      #define OPT_F10_BL_ID
    #endif
  #else
    #define OPT_F10_BL_CPU
    #define OPT_F10_BL_ID
  #endif
#else
  #define OPT_F10_BL_CPU
  #define OPT_F10_BL_ID
#endif
