/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of CPU specific services support
 *
 * This file generates the CPU specific services tables.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Include
 * @e \$Revision: 85962 $   @e \$Date: 2013-01-14 20:12:29 -0600 (Mon, 14 Jan 2013) $
 */
/*****************************************************************************
 *
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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


/*

NOTE:

1. This file is designed to be included multiple times in one file. So there's no includsion guard like below.

#ifndef _OPTION_CPU_FAMILY_SERVICE_INSTALL_DEFAULT_H_
#define _OPTION_CPU_FAMILY_SERVICE_INSTALL_DEFAULT_H_
#endif // _OPTION_CPU_FAMILY_SERVICE_INSTALL_DEFAULT_H_


2. This file is designed to be included in below form.

//
// 1st family model specific definitions
//
#include "OptionCpuSpecificServicesInstallReset.h"
#define CpuSrvcTableName                                        cpuFamilyModelServices
// Put your service definitions "CpuSrvc<ServiceName>" here.
#define CpuSrvcDisablePstate                                    cpuF1stM1stDisablePstate
#include "OptionCpuSpecificServicesInstall.h"
INSTALL_CPU_SPECIFIC_SERVICES_TABLE (CpuSrvcTableName);

//
// 2nd family model specific definitions
//
#include "OptionCpuSpecificServicesInstallReset.h"
#define CpuSrvcTableName                                        cpuFamilyModelServices
// Put your service definitions "CpuSrvc<ServiceName>" here.
#define CpuSrvcDisablePstate                                    cpuF2ndM2ndDisablePstate
#include "OptionCpuSpecificServicesInstall.h"
INSTALL_CPU_SPECIFIC_SERVICES_TABLE (CpuSrvcTableName);


Example of CPU specific services definitions:

NOTE: Members with type casting should use OvrdDfltCpuSrvc<ServiceName> instead due to automatical "extern" limitation.

#define CpuSrvcRevision                                        (UINT16)                                           Revision
#define CpuSrvcDisablePstate                                   (PF_CPU_DISABLE_PSTATE)                            DisablePstate
#define CpuSrvcTransitionPstate                                (PF_CPU_TRANSITION_PSTATE)                         TransitionPstate
#define CpuSrvcGetProcIddMax                                   (PF_CPU_GET_IDD_MAX)                               GetProcIddMax
#define CpuSrvcGetTscRate                                      (PF_CPU_GET_TSC_RATE)                              GetTscRate
#define CpuSrvcGetCurrentNbFrequency                           (PF_CPU_GET_NB_FREQ)                               GetCurrentNbFrequency
#define CpuSrvcGetMinMaxNbFrequency                            (PF_CPU_GET_MIN_MAX_NB_FREQ)                       GetMinMaxNbFrequency
#define CpuSrvcGetNbPstateInfo                                 (PF_CPU_GET_NB_PSTATE_INFO)                        GetNbPstateInfo
#define CpuSrvcIsNbCofInitNeeded                               (PF_CPU_IS_NBCOF_INIT_NEEDED)                      IsNbCofInitNeeded
#define CpuSrvcGetNbIddMax                                     (PF_CPU_GET_NB_IDD_MAX)                            GetNbIddMax
#define CpuSrvcLaunchApCore                                    (PF_CPU_AP_INITIAL_LAUNCH)                         LaunchApCore
#define CpuSrvcGetNumberOfPhysicalCores                        (PF_CPU_NUMBER_OF_PHYSICAL_CORES)                  GetNumberOfPhysicalCores
#define CpuSrvcGetApMailboxFromHardware                        (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE)          GetApMailboxFromHardware
#define CpuSrvcSetApCoreNumber                                 (PF_CPU_SET_AP_CORE_NUMBER)                        SetApCoreNumber
#define CpuSrvcGetApCoreNumber                                 (PF_CPU_GET_AP_CORE_NUMBER)                        GetApCoreNumber
#define CpuSrvcTransferApCoreNumber                            (PF_CPU_TRANSFER_AP_CORE_NUMBER)                   TransferApCoreNumber
#define CpuSrvcGetStoredNodeNumber                             (PF_CPU_GET_STORED_NODE_NUMBER)                    GetStoredNodeNumber
#define CpuSrvcCoreIdPositionInInitialApicId                   (PF_CORE_ID_POSITION_IN_INITIAL_APIC_ID)           CoreIdPositionInInitialApicId
#define CpuSrvcSaveFeatures                                    (PF_CPU_SAVE_FEATURES)                             SaveFeatures
#define CpuSrvcWriteFeatures                                   (PF_CPU_WRITE_FEATURES)                            WriteFeatures
#define CpuSrvcSetWarmResetFlag                                (PF_CPU_SET_WARM_RESET_FLAG)                       SetWarmResetFlag
#define CpuSrvcGetWarmResetFlag                                (PF_CPU_GET_WARM_RESET_FLAG)                       GetWarmResetFlag
#define CpuSrvcGetBrandString1                                 (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)                 GetBrandString1
#define CpuSrvcGetBrandString2                                 (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)                 GetBrandString2
#define CpuSrvcGetMicroCodePatchesStruct                       (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)                 GetMicroCodePatchesStruct
#define CpuSrvcGetMicrocodeEquivalenceTable                    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)                 GetMicrocodeEquivalenceTable
#define CpuSrvcGetCacheInfo                                    (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)                 GetCacheInfo
#define CpuSrvcGetSysPmTableStruct                             (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)                 GetSysPmTableStruct
#define CpuSrvcGetWheaInitData                                 (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)                 GetWheaInitData
#define CpuSrvcGetPlatformTypeSpecificInfo                     (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO)           GetPlatformTypeSpecificInfo
#define CpuSrvcIsNbPstateEnabled                               (PF_IS_NB_PSTATE_ENABLED)                          IsNbPstateEnabled
#define CpuSrvcNextLinkHasHtPhyFeats                           (PF_NEXT_LINK_HAS_HTFPY_FEATS)                     NextLinkHasHtPhyFeats
#define CpuSrvcSetHtPhyRegister                                (PF_SET_HT_PHY_REGISTER)                           SetHtPhyRegister
#define CpuSrvcGetNextHtLinkFeatures                           (PF_GET_NEXT_HT_LINK_FEATURES)                     GetNextHtLinkFeatures
#define CpuSrvcRegisterTableList                               (REGISTER_TABLE **)                                RegisterTableList
#define CpuSrvcTableEntryTypeDescriptors                       (TABLE_ENTRY_TYPE_DESCRIPTOR *)                    TableEntryTypeDescriptors
#define CpuSrvcPackageLinkMap                                  (PACKAGE_HTLINK_MAP)                               PackageLinkMap
#define CpuSrvcComputeUnitMap                                  (COMPUTE_UNIT_MAP *)                               ComputeUnitMap
#define CpuSrvcInitCacheDisabled                               (FAMILY_CACHE_INIT_POLICY)                         InitCacheDisabled
#define CpuSrvcGetEarlyBeforeApLaunchInitOnCoreTable           (PF_GET_EARLY_INIT_TABLE)                          GetEarlyInitBeforeApLaunchOnCoreTable
#define CpuSrvcGetEarlyAfterApLaunchInitOnCoreTable            (PF_GET_EARLY_INIT_TABLE)                          GetEarlyInitAfterApLaunchOnCoreTable
#define CpuSrvcPatchLoaderIsSharedByCU                         (BOOLEAN)                                          TRUE

  USAGE MODEL:

  //
  // If BUILD_CONFIG_SWITCH = TRUE
  //   If override service is defined, use it as the final service definition.
  //   If override service is not define, use default service (DlftCpuSrvc) as the final service definition.
  //
  // If BUILD_CONFIG_SWITCH = FALSE
  //   Use default assert service (AssertCpuSrvc) as the final service definition for making a assertion when unsupported service is called.
  //
  // "extern" will be used for function pointer (exclude data) type serive members automatically when CpuSrvc<ServiceName> is defined.
  //

// Member: (MEMBER_TYPE) ServiceName
#if BUILD_CONFIG_SWITCH == TRUE
  #ifdef CpuSrvcServiceName
    #define FinalCpuSrvcServiceName           CpuSrvcServiceName
    extern  MEMBER_TYPE                       FinalCpuSrvcServiceName;
  #else
    #define FinalCpuSrvcServiceName           DfltCpuSrvcServiceName
    #pragma message( STRING_MACRO_DEFAULT_USED(CpuSrvcServiceName) )
  #endif
#else
  #define FinalCpuSrvcServiceName           DfltAssertCpuSrvcServiceName
  #pragma message( STRING_MACRO_DEFAULT_ASSERT_USED(CpuSrvcServiceName) )
#endif

*/


#define STRING2(x)                                #x
#define STRING(x)                                 STRING2(x)
#define CAT_STRING(a, b)                          a##b
#define STRING_ARROW                              " --> "

//
// Verbose control for messaging CPU services at build time
//
#ifndef VERBOSE_CPU_SERVICES
  #define VERBOSE_CPU_SERVICES                    1 // 0 - disable messaging, 1 - enable debugging.
#endif

#if VERBOSE_CPU_SERVICES
  #define STRING_MACRO_VALUE(Name)                " " #Name " = " STRING(Name)
  #define STRING_OVERRIDE_MACRO_VALUE(Name)       "  OVERRIDE: " #Name " : " STRING(Name) STRING_ARROW STRING(Ovrd##Name)
  #define STRING_MACRO_DEFAULT_USED(Name)         "  NOT DEFINED: "  #Name STRING_ARROW " DEFAULT: " STRING(FinalDflt##Name)
  #define STRING_MACRO_DEFAULT_ASSERT_USED(Name)  "  DISABLED: " #Name ", assertion is used."

  #define STRING_SEPARATOR1                       "-----------------------------------------------------------------------------------------------"
  #define STRING_SEPARATOR2                       "==============================================================================================="
  #define STRING_SEPARATOR_POUND                  "###############################################################################################"
#else // VERBOSE_CPU_SERVICES
  #define STRING_MACRO_VALUE(Name)                NULL_STRING
  #define STRING_OVERRIDE_MACRO_VALUE(Name)       NULL_STRING
  #define STRING_MACRO_DEFAULT_USED(Name)         NULL_STRING
  #define STRING_MACRO_DEFAULT_ASSERT_USED(Name)  NULL_STRING


  #define NULL_STRING                             ""
  #define STRING_SEPARATOR1                       NULL_STRING
  #define STRING_SEPARATOR2                       NULL_STRING
  #define STRING_SEPARATOR_POUND                  NULL_STRING
#endif // VERBOSE_CPU_SERVICES

//
// Handle override definitions on DfltAssertCpuSrvc<ServiceName>
//

#ifdef OvrdDfltAssertCpuSrvcRevision
  #define FinalDfltAssertCpuSrvcRevision                              OvrdDfltAssertCpuSrvcRevision
#else
  #define FinalDfltAssertCpuSrvcRevision                              DfltAssertCpuSrvcRevision
#endif

#ifdef OvrdDfltAssertCpuSrvcDisablePstate
  #define FinalDfltAssertCpuSrvcDisablePstate                         OvrdDfltAssertCpuSrvcDisablePstate
#else
  #define FinalDfltAssertCpuSrvcDisablePstate                         DfltAssertCpuSrvcDisablePstate
#endif

#ifdef OvrdDfltAssertCpuSrvcTransitionPstate
  #define FinalDfltAssertCpuSrvcTransitionPstate                      OvrdDfltAssertCpuSrvcTransitionPstate
#else
  #define FinalDfltAssertCpuSrvcTransitionPstate                      DfltAssertCpuSrvcTransitionPstate
#endif

#ifdef OvrdDfltAssertCpuSrvcGetProcIddMax
  #define FinalDfltAssertCpuSrvcGetProcIddMax                         OvrdDfltAssertCpuSrvcGetProcIddMax
#else
  #define FinalDfltAssertCpuSrvcGetProcIddMax                         DfltAssertCpuSrvcGetProcIddMax
#endif

#ifdef OvrdDfltAssertCpuSrvcGetTscRate
  #define FinalDfltAssertCpuSrvcGetTscRate                            OvrdDfltAssertCpuSrvcGetTscRate
#else
  #define FinalDfltAssertCpuSrvcGetTscRate                            DfltAssertCpuSrvcGetTscRate
#endif

#ifdef OvrdDfltAssertCpuSrvcGetCurrentNbFrequency
  #define FinalDfltAssertCpuSrvcGetCurrentNbFrequency                 OvrdDfltAssertCpuSrvcGetCurrentNbFrequency
#else
  #define FinalDfltAssertCpuSrvcGetCurrentNbFrequency                 DfltAssertCpuSrvcGetCurrentNbFrequency
#endif

#ifdef OvrdDfltAssertCpuSrvcGetMinMaxNbFrequency
  #define FinalDfltAssertCpuSrvcGetMinMaxNbFrequency                  OvrdDfltAssertCpuSrvcGetMinMaxNbFrequency
#else
  #define FinalDfltAssertCpuSrvcGetMinMaxNbFrequency                  DfltAssertCpuSrvcGetMinMaxNbFrequency
#endif

#ifdef OvrdDfltAssertCpuSrvcGetNbPstateInfo
  #define FinalDfltAssertCpuSrvcGetNbPstateInfo                       OvrdDfltAssertCpuSrvcGetNbPstateInfo
#else
  #define FinalDfltAssertCpuSrvcGetNbPstateInfo                       DfltAssertCpuSrvcGetNbPstateInfo
#endif

#ifdef OvrdDfltAssertCpuSrvcIsNbCofInitNeeded
  #define FinalDfltAssertCpuSrvcIsNbCofInitNeeded                     OvrdDfltAssertCpuSrvcIsNbCofInitNeeded
#else
  #define FinalDfltAssertCpuSrvcIsNbCofInitNeeded                     DfltAssertCpuSrvcIsNbCofInitNeeded
#endif

#ifdef OvrdDfltAssertCpuSrvcGetNbIddMax
  #define FinalDfltAssertCpuSrvcGetNbIddMax                           OvrdDfltAssertCpuSrvcGetNbIddMax
#else
  #define FinalDfltAssertCpuSrvcGetNbIddMax                           DfltAssertCpuSrvcGetNbIddMax
#endif

#ifdef OvrdDfltAssertCpuSrvcLaunchApCore
  #define FinalDfltAssertCpuSrvcLaunchApCore                          OvrdDfltAssertCpuSrvcLaunchApCore
#else
  #define FinalDfltAssertCpuSrvcLaunchApCore                          DfltAssertCpuSrvcLaunchApCore
#endif

#ifdef OvrdDfltAssertCpuSrvcGetNumberOfPhysicalCores
  #define FinalDfltAssertCpuSrvcGetNumberOfPhysicalCores              OvrdDfltAssertCpuSrvcGetNumberOfPhysicalCores
#else
  #define FinalDfltAssertCpuSrvcGetNumberOfPhysicalCores              DfltAssertCpuSrvcGetNumberOfPhysicalCores
#endif

#ifdef OvrdDfltAssertCpuSrvcGetApMailboxFromHardware
  #define FinalDfltAssertCpuSrvcGetApMailboxFromHardware              OvrdDfltAssertCpuSrvcGetApMailboxFromHardware
#else
  #define FinalDfltAssertCpuSrvcGetApMailboxFromHardware              DfltAssertCpuSrvcGetApMailboxFromHardware
#endif

#ifdef OvrdDfltAssertCpuSrvcSetApCoreNumber
  #define FinalDfltAssertCpuSrvcSetApCoreNumber                       OvrdDfltAssertCpuSrvcSetApCoreNumber
#else
  #define FinalDfltAssertCpuSrvcSetApCoreNumber                       DfltAssertCpuSrvcSetApCoreNumber
#endif

#ifdef OvrdDfltAssertCpuSrvcGetApCoreNumber
  #define FinalDfltAssertCpuSrvcGetApCoreNumber                       OvrdDfltAssertCpuSrvcGetApCoreNumber
#else
  #define FinalDfltAssertCpuSrvcGetApCoreNumber                       DfltAssertCpuSrvcGetApCoreNumber
#endif

#ifdef OvrdDfltAssertCpuSrvcTransferApCoreNumber
  #define FinalDfltAssertCpuSrvcTransferApCoreNumber                  OvrdDfltAssertCpuSrvcTransferApCoreNumber
#else
  #define FinalDfltAssertCpuSrvcTransferApCoreNumber                  DfltAssertCpuSrvcTransferApCoreNumber
#endif

#ifdef OvrdDfltAssertCpuSrvcGetStoredNodeNumber
  #define FinalDfltAssertCpuSrvcGetStoredNodeNumber                   OvrdDfltAssertCpuSrvcGetStoredNodeNumber
#else
  #define FinalDfltAssertCpuSrvcGetStoredNodeNumber                   DfltAssertCpuSrvcGetStoredNodeNumber
#endif

#ifdef OvrdDfltAssertCpuSrvcCoreIdPositionInInitialApicId
  #define FinalDfltAssertCpuSrvcCoreIdPositionInInitialApicId         OvrdDfltAssertCpuSrvcCoreIdPositionInInitialApicId
#else
  #define FinalDfltAssertCpuSrvcCoreIdPositionInInitialApicId         DfltAssertCpuSrvcCoreIdPositionInInitialApicId
#endif

#ifdef OvrdDfltAssertCpuSrvcSaveFeatures
  #define FinalDfltAssertCpuSrvcSaveFeatures                          OvrdDfltAssertCpuSrvcSaveFeatures
#else
  #define FinalDfltAssertCpuSrvcSaveFeatures                          DfltAssertCpuSrvcSaveFeatures
#endif

#ifdef OvrdDfltAssertCpuSrvcWriteFeatures
  #define FinalDfltAssertCpuSrvcWriteFeatures                         OvrdDfltAssertCpuSrvcWriteFeatures
#else
  #define FinalDfltAssertCpuSrvcWriteFeatures                         DfltAssertCpuSrvcWriteFeatures
#endif

#ifdef OvrdDfltAssertCpuSrvcSetWarmResetFlag
  #define FinalDfltAssertCpuSrvcSetWarmResetFlag                      OvrdDfltAssertCpuSrvcSetWarmResetFlag
#else
  #define FinalDfltAssertCpuSrvcSetWarmResetFlag                      DfltAssertCpuSrvcSetWarmResetFlag
#endif

#ifdef OvrdDfltAssertCpuSrvcGetWarmResetFlag
  #define FinalDfltAssertCpuSrvcGetWarmResetFlag                      OvrdDfltAssertCpuSrvcGetWarmResetFlag
#else
  #define FinalDfltAssertCpuSrvcGetWarmResetFlag                      DfltAssertCpuSrvcGetWarmResetFlag
#endif

#ifdef OvrdDfltAssertCpuSrvcGetBrandString1
  #define FinalDfltAssertCpuSrvcGetBrandString1                       OvrdDfltAssertCpuSrvcGetBrandString1
#else
  #define FinalDfltAssertCpuSrvcGetBrandString1                       DfltAssertCpuSrvcGetBrandString1
#endif

#ifdef OvrdDfltAssertCpuSrvcGetBrandString2
  #define FinalDfltAssertCpuSrvcGetBrandString2                       OvrdDfltAssertCpuSrvcGetBrandString2
#else
  #define FinalDfltAssertCpuSrvcGetBrandString2                       DfltAssertCpuSrvcGetBrandString2
#endif

#ifdef OvrdDfltAssertCpuSrvcGetMicroCodePatchesStruct
  #define FinalDfltAssertCpuSrvcGetMicroCodePatchesStruct             OvrdDfltAssertCpuSrvcGetMicroCodePatchesStruct
#else
  #define FinalDfltAssertCpuSrvcGetMicroCodePatchesStruct             DfltAssertCpuSrvcGetMicroCodePatchesStruct
#endif

#ifdef OvrdDfltAssertCpuSrvcGetMicrocodeEquivalenceTable
  #define FinalDfltAssertCpuSrvcGetMicrocodeEquivalenceTable          OvrdDfltAssertCpuSrvcGetMicrocodeEquivalenceTable
#else
  #define FinalDfltAssertCpuSrvcGetMicrocodeEquivalenceTable          DfltAssertCpuSrvcGetMicrocodeEquivalenceTable
#endif

#ifdef OvrdDfltAssertCpuSrvcGetCacheInfo
  #define FinalDfltAssertCpuSrvcGetCacheInfo                          OvrdDfltAssertCpuSrvcGetCacheInfo
#else
  #define FinalDfltAssertCpuSrvcGetCacheInfo                          DfltAssertCpuSrvcGetCacheInfo
#endif

#ifdef OvrdDfltAssertCpuSrvcGetSysPmTableStruct
  #define FinalDfltAssertCpuSrvcGetSysPmTableStruct                   OvrdDfltAssertCpuSrvcGetSysPmTableStruct
#else
  #define FinalDfltAssertCpuSrvcGetSysPmTableStruct                   DfltAssertCpuSrvcGetSysPmTableStruct
#endif

#ifdef OvrdDfltAssertCpuSrvcGetWheaInitData
  #define FinalDfltAssertCpuSrvcGetWheaInitData                       OvrdDfltAssertCpuSrvcGetWheaInitData
#else
  #define FinalDfltAssertCpuSrvcGetWheaInitData                       DfltAssertCpuSrvcGetWheaInitData
#endif

#ifdef OvrdDfltAssertCpuSrvcGetPlatformTypeSpecificInfo
  #define FinalDfltAssertCpuSrvcGetPlatformTypeSpecificInfo           OvrdDfltAssertCpuSrvcGetPlatformTypeSpecificInfo
#else
  #define FinalDfltAssertCpuSrvcGetPlatformTypeSpecificInfo           DfltAssertCpuSrvcGetPlatformTypeSpecificInfo
#endif

#ifdef OvrdDfltAssertCpuSrvcIsNbPstateEnabled
  #define FinalDfltAssertCpuSrvcIsNbPstateEnabled                     OvrdDfltAssertCpuSrvcIsNbPstateEnabled
#else
  #define FinalDfltAssertCpuSrvcIsNbPstateEnabled                     DfltAssertCpuSrvcIsNbPstateEnabled
#endif

#ifdef OvrdDfltAssertCpuSrvcNextLinkHasHtPhyFeats
  #define FinalDfltAssertCpuSrvcNextLinkHasHtPhyFeats                 OvrdDfltAssertCpuSrvcNextLinkHasHtPhyFeats
#else
  #define FinalDfltAssertCpuSrvcNextLinkHasHtPhyFeats                 DfltAssertCpuSrvcNextLinkHasHtPhyFeats
#endif

#ifdef OvrdDfltAssertCpuSrvcSetHtPhyRegister
  #define FinalDfltAssertCpuSrvcSetHtPhyRegister                      OvrdDfltAssertCpuSrvcSetHtPhyRegister
#else
  #define FinalDfltAssertCpuSrvcSetHtPhyRegister                      DfltAssertCpuSrvcSetHtPhyRegister
#endif

#ifdef OvrdDfltAssertCpuSrvcGetNextHtLinkFeatures
  #define FinalDfltAssertCpuSrvcGetNextHtLinkFeatures                 OvrdDfltAssertCpuSrvcGetNextHtLinkFeatures
#else
  #define FinalDfltAssertCpuSrvcGetNextHtLinkFeatures                 DfltAssertCpuSrvcGetNextHtLinkFeatures
#endif

#ifdef OvrdDfltAssertCpuSrvcRegisterTableList
  #define FinalDfltAssertCpuSrvcRegisterTableList                     OvrdDfltAssertCpuSrvcRegisterTableList
#else
  #define FinalDfltAssertCpuSrvcRegisterTableList                     DfltAssertCpuSrvcRegisterTableList
#endif

#ifdef OvrdDfltAssertCpuSrvcTableEntryTypeDescriptors
  #define FinalDfltAssertCpuSrvcTableEntryTypeDescriptors             OvrdDfltAssertCpuSrvcTableEntryTypeDescriptors
#else
  #define FinalDfltAssertCpuSrvcTableEntryTypeDescriptors             DfltAssertCpuSrvcTableEntryTypeDescriptors
#endif

#ifdef OvrdDfltAssertCpuSrvcPackageLinkMap
  #define FinalDfltAssertCpuSrvcPackageLinkMap                        OvrdDfltAssertCpuSrvcPackageLinkMap
#else
  #define FinalDfltAssertCpuSrvcPackageLinkMap                        DfltAssertCpuSrvcPackageLinkMap
#endif

#ifdef OvrdDfltAssertCpuSrvcComputeUnitMap
  #define FinalDfltAssertCpuSrvcComputeUnitMap                        OvrdDfltAssertCpuSrvcComputeUnitMap
#else
  #define FinalDfltAssertCpuSrvcComputeUnitMap                        DfltAssertCpuSrvcComputeUnitMap
#endif

#ifdef OvrdDfltAssertCpuSrvcInitCacheDisabled
  #define FinalDfltAssertCpuSrvcInitCacheDisabled                     OvrdDfltAssertCpuSrvcInitCacheDisabled
#else
  #define FinalDfltAssertCpuSrvcInitCacheDisabled                     DfltAssertCpuSrvcInitCacheDisabled
#endif

#ifdef OvrdDfltAssertCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable
  #define FinalDfltAssertCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable OvrdDfltAssertCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable
#else
  #define FinalDfltAssertCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable DfltAssertCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable
#endif

#ifdef OvrdDfltAssertCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable
  #define FinalDfltAssertCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable  OvrdDfltAssertCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable
#else
  #define FinalDfltAssertCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable  DfltAssertCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable
#endif

#ifdef OvrdDfltAssertCpuSrvcPatchLoaderIsSharedByCU
  #define FinalDfltAssertCpuSrvcPatchLoaderIsSharedByCU               OvrdDfltAssertCpuSrvcPatchLoaderIsSharedByCU
#else
  #define FinalDfltAssertCpuSrvcPatchLoaderIsSharedByCU               DfltAssertCpuSrvcPatchLoaderIsSharedByCU
#endif

//
// Handle override definitions on DfltCpuSrvc<ServiceName>
//

#ifdef OvrdDfltCpuSrvcRevision
  #define FinalDfltCpuSrvcRevision                                    OvrdDfltCpuSrvcRevision
#else
  #define FinalDfltCpuSrvcRevision                                    DfltCpuSrvcRevision
#endif

#ifdef OvrdDfltCpuSrvcDisablePstate
  #define FinalDfltCpuSrvcDisablePstate                               OvrdDfltCpuSrvcDisablePstate
#else
  #define FinalDfltCpuSrvcDisablePstate                               DfltCpuSrvcDisablePstate
#endif

#ifdef OvrdDfltCpuSrvcTransitionPstate
  #define FinalDfltCpuSrvcTransitionPstate                            OvrdDfltCpuSrvcTransitionPstate
#else
  #define FinalDfltCpuSrvcTransitionPstate                            DfltCpuSrvcTransitionPstate
#endif

#ifdef OvrdDfltCpuSrvcGetProcIddMax
  #define FinalDfltCpuSrvcGetProcIddMax                               OvrdDfltCpuSrvcGetProcIddMax
#else
  #define FinalDfltCpuSrvcGetProcIddMax                               DfltCpuSrvcGetProcIddMax
#endif

#ifdef OvrdDfltCpuSrvcGetTscRate
  #define FinalDfltCpuSrvcGetTscRate                                  OvrdDfltCpuSrvcGetTscRate
#else
  #define FinalDfltCpuSrvcGetTscRate                                  DfltCpuSrvcGetTscRate
#endif

#ifdef OvrdDfltCpuSrvcGetCurrentNbFrequency
  #define FinalDfltCpuSrvcGetCurrentNbFrequency                       OvrdDfltCpuSrvcGetCurrentNbFrequency
#else
  #define FinalDfltCpuSrvcGetCurrentNbFrequency                       DfltCpuSrvcGetCurrentNbFrequency
#endif

#ifdef OvrdDfltCpuSrvcGetMinMaxNbFrequency
  #define FinalDfltCpuSrvcGetMinMaxNbFrequency                        OvrdDfltCpuSrvcGetMinMaxNbFrequency
#else
  #define FinalDfltCpuSrvcGetMinMaxNbFrequency                        DfltCpuSrvcGetMinMaxNbFrequency
#endif

#ifdef OvrdDfltCpuSrvcGetNbPstateInfo
  #define FinalDfltCpuSrvcGetNbPstateInfo                             OvrdDfltCpuSrvcGetNbPstateInfo
#else
  #define FinalDfltCpuSrvcGetNbPstateInfo                             DfltCpuSrvcGetNbPstateInfo
#endif

#ifdef OvrdDfltCpuSrvcIsNbCofInitNeeded
  #define FinalDfltCpuSrvcIsNbCofInitNeeded                           OvrdDfltCpuSrvcIsNbCofInitNeeded
#else
  #define FinalDfltCpuSrvcIsNbCofInitNeeded                           DfltCpuSrvcIsNbCofInitNeeded
#endif

#ifdef OvrdDfltCpuSrvcGetNbIddMax
  #define FinalDfltCpuSrvcGetNbIddMax                                 OvrdDfltCpuSrvcGetNbIddMax
#else
  #define FinalDfltCpuSrvcGetNbIddMax                                 DfltCpuSrvcGetNbIddMax
#endif

#ifdef OvrdDfltCpuSrvcLaunchApCore
  #define FinalDfltCpuSrvcLaunchApCore                                OvrdDfltCpuSrvcLaunchApCore
#else
  #define FinalDfltCpuSrvcLaunchApCore                                DfltCpuSrvcLaunchApCore
#endif

#ifdef OvrdDfltCpuSrvcGetNumberOfPhysicalCores
  #define FinalDfltCpuSrvcGetNumberOfPhysicalCores                    OvrdDfltCpuSrvcGetNumberOfPhysicalCores
#else
  #define FinalDfltCpuSrvcGetNumberOfPhysicalCores                    DfltCpuSrvcGetNumberOfPhysicalCores
#endif

#ifdef OvrdDfltCpuSrvcGetApMailboxFromHardware
  #define FinalDfltCpuSrvcGetApMailboxFromHardware                    OvrdDfltCpuSrvcGetApMailboxFromHardware
#else
  #define FinalDfltCpuSrvcGetApMailboxFromHardware                    DfltCpuSrvcGetApMailboxFromHardware
#endif

#ifdef OvrdDfltCpuSrvcSetApCoreNumber
  #define FinalDfltCpuSrvcSetApCoreNumber                             OvrdDfltCpuSrvcSetApCoreNumber
#else
  #define FinalDfltCpuSrvcSetApCoreNumber                             DfltCpuSrvcSetApCoreNumber
#endif

#ifdef OvrdDfltCpuSrvcGetApCoreNumber
  #define FinalDfltCpuSrvcGetApCoreNumber                             OvrdDfltCpuSrvcGetApCoreNumber
#else
  #define FinalDfltCpuSrvcGetApCoreNumber                             DfltCpuSrvcGetApCoreNumber
#endif

#ifdef OvrdDfltCpuSrvcTransferApCoreNumber
  #define FinalDfltCpuSrvcTransferApCoreNumber                        OvrdDfltCpuSrvcTransferApCoreNumber
#else
  #define FinalDfltCpuSrvcTransferApCoreNumber                        DfltCpuSrvcTransferApCoreNumber
#endif

#ifdef OvrdDfltCpuSrvcGetStoredNodeNumber
  #define FinalDfltCpuSrvcGetStoredNodeNumber                         OvrdDfltCpuSrvcGetStoredNodeNumber
#else
  #define FinalDfltCpuSrvcGetStoredNodeNumber                         DfltCpuSrvcGetStoredNodeNumber
#endif

#ifdef OvrdDfltCpuSrvcCoreIdPositionInInitialApicId
  #define FinalDfltCpuSrvcCoreIdPositionInInitialApicId               OvrdDfltCpuSrvcCoreIdPositionInInitialApicId
#else
  #define FinalDfltCpuSrvcCoreIdPositionInInitialApicId               DfltCpuSrvcCoreIdPositionInInitialApicId
#endif

#ifdef OvrdDfltCpuSrvcSaveFeatures
  #define FinalDfltCpuSrvcSaveFeatures                                OvrdDfltCpuSrvcSaveFeatures
#else
  #define FinalDfltCpuSrvcSaveFeatures                                DfltCpuSrvcSaveFeatures
#endif

#ifdef OvrdDfltCpuSrvcWriteFeatures
  #define FinalDfltCpuSrvcWriteFeatures                               OvrdDfltCpuSrvcWriteFeatures
#else
  #define FinalDfltCpuSrvcWriteFeatures                               DfltCpuSrvcWriteFeatures
#endif

#ifdef OvrdDfltCpuSrvcSetWarmResetFlag
  #define FinalDfltCpuSrvcSetWarmResetFlag                            OvrdDfltCpuSrvcSetWarmResetFlag
#else
  #define FinalDfltCpuSrvcSetWarmResetFlag                            DfltCpuSrvcSetWarmResetFlag
#endif

#ifdef OvrdDfltCpuSrvcGetWarmResetFlag
  #define FinalDfltCpuSrvcGetWarmResetFlag                            OvrdDfltCpuSrvcGetWarmResetFlag
#else
  #define FinalDfltCpuSrvcGetWarmResetFlag                            DfltCpuSrvcGetWarmResetFlag
#endif

#ifdef OvrdDfltCpuSrvcGetBrandString1
  #define FinalDfltCpuSrvcGetBrandString1                             OvrdDfltCpuSrvcGetBrandString1
#else
  #define FinalDfltCpuSrvcGetBrandString1                             DfltCpuSrvcGetBrandString1
#endif

#ifdef OvrdDfltCpuSrvcGetBrandString2
  #define FinalDfltCpuSrvcGetBrandString2                             OvrdDfltCpuSrvcGetBrandString2
#else
  #define FinalDfltCpuSrvcGetBrandString2                             DfltCpuSrvcGetBrandString2
#endif

#ifdef OvrdDfltCpuSrvcGetMicroCodePatchesStruct
  #define FinalDfltCpuSrvcGetMicroCodePatchesStruct                   OvrdDfltCpuSrvcGetMicroCodePatchesStruct
#else
  #define FinalDfltCpuSrvcGetMicroCodePatchesStruct                   DfltCpuSrvcGetMicroCodePatchesStruct
#endif

#ifdef OvrdDfltCpuSrvcGetMicrocodeEquivalenceTable
  #define FinalDfltCpuSrvcGetMicrocodeEquivalenceTable                OvrdDfltCpuSrvcGetMicrocodeEquivalenceTable
#else
  #define FinalDfltCpuSrvcGetMicrocodeEquivalenceTable                DfltCpuSrvcGetMicrocodeEquivalenceTable
#endif

#ifdef OvrdDfltCpuSrvcGetCacheInfo
  #define FinalDfltCpuSrvcGetCacheInfo                                OvrdDfltCpuSrvcGetCacheInfo
#else
  #define FinalDfltCpuSrvcGetCacheInfo                                DfltCpuSrvcGetCacheInfo
#endif

#ifdef OvrdDfltCpuSrvcGetSysPmTableStruct
  #define FinalDfltCpuSrvcGetSysPmTableStruct                         OvrdDfltCpuSrvcGetSysPmTableStruct
#else
  #define FinalDfltCpuSrvcGetSysPmTableStruct                         DfltCpuSrvcGetSysPmTableStruct
#endif

#ifdef OvrdDfltCpuSrvcGetWheaInitData
  #define FinalDfltCpuSrvcGetWheaInitData                             OvrdDfltCpuSrvcGetWheaInitData
#else
  #define FinalDfltCpuSrvcGetWheaInitData                             DfltCpuSrvcGetWheaInitData
#endif

#ifdef OvrdDfltCpuSrvcGetPlatformTypeSpecificInfo
  #define FinalDfltCpuSrvcGetPlatformTypeSpecificInfo                 OvrdDfltCpuSrvcGetPlatformTypeSpecificInfo
#else
  #define FinalDfltCpuSrvcGetPlatformTypeSpecificInfo                 DfltCpuSrvcGetPlatformTypeSpecificInfo
#endif

#ifdef OvrdDfltCpuSrvcIsNbPstateEnabled
  #define FinalDfltCpuSrvcIsNbPstateEnabled                           OvrdDfltCpuSrvcIsNbPstateEnabled
#else
  #define FinalDfltCpuSrvcIsNbPstateEnabled                           DfltCpuSrvcIsNbPstateEnabled
#endif

#ifdef OvrdDfltCpuSrvcNextLinkHasHtPhyFeats
  #define FinalDfltCpuSrvcNextLinkHasHtPhyFeats                       OvrdDfltCpuSrvcNextLinkHasHtPhyFeats
#else
  #define FinalDfltCpuSrvcNextLinkHasHtPhyFeats                       DfltCpuSrvcNextLinkHasHtPhyFeats
#endif

#ifdef OvrdDfltCpuSrvcSetHtPhyRegister
  #define FinalDfltCpuSrvcSetHtPhyRegister                            OvrdDfltCpuSrvcSetHtPhyRegister
#else
  #define FinalDfltCpuSrvcSetHtPhyRegister                            DfltCpuSrvcSetHtPhyRegister
#endif

#ifdef OvrdDfltCpuSrvcGetNextHtLinkFeatures
  #define FinalDfltCpuSrvcGetNextHtLinkFeatures                       OvrdDfltCpuSrvcGetNextHtLinkFeatures
#else
  #define FinalDfltCpuSrvcGetNextHtLinkFeatures                       DfltCpuSrvcGetNextHtLinkFeatures
#endif

#ifdef OvrdDfltCpuSrvcRegisterTableList
  #define FinalDfltCpuSrvcRegisterTableList                           OvrdDfltCpuSrvcRegisterTableList
#else
  #define FinalDfltCpuSrvcRegisterTableList                           DfltCpuSrvcRegisterTableList
#endif

#ifdef OvrdDfltCpuSrvcTableEntryTypeDescriptors
  #define FinalDfltCpuSrvcTableEntryTypeDescriptors                   OvrdDfltCpuSrvcTableEntryTypeDescriptors
#else
  #define FinalDfltCpuSrvcTableEntryTypeDescriptors                   DfltCpuSrvcTableEntryTypeDescriptors
#endif

#ifdef OvrdDfltCpuSrvcPackageLinkMap
  #define FinalDfltCpuSrvcPackageLinkMap                              OvrdDfltCpuSrvcPackageLinkMap
#else
  #define FinalDfltCpuSrvcPackageLinkMap                              DfltCpuSrvcPackageLinkMap
#endif

#ifdef OvrdDfltCpuSrvcComputeUnitMap
  #define FinalDfltCpuSrvcComputeUnitMap                              OvrdDfltCpuSrvcComputeUnitMap
#else
  #define FinalDfltCpuSrvcComputeUnitMap                              DfltCpuSrvcComputeUnitMap
#endif

#ifdef OvrdDfltCpuSrvcInitCacheDisabled
  #define FinalDfltCpuSrvcInitCacheDisabled                           OvrdDfltCpuSrvcInitCacheDisabled
#else
  #define FinalDfltCpuSrvcInitCacheDisabled                           DfltCpuSrvcInitCacheDisabled
#endif

#ifdef OvrdDfltCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable
  #define FinalDfltCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable       OvrdDfltCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable
#else
  #define FinalDfltCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable       DfltCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable
#endif

#ifdef OvrdDfltCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable
  #define FinalDfltCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable        OvrdDfltCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable
#else
  #define FinalDfltCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable        DfltCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable
#endif

#ifdef OvrdDfltCpuSrvcPatchLoaderIsSharedByCU
  #define FinalDfltCpuSrvcPatchLoaderIsSharedByCU                     OvrdDfltCpuSrvcPatchLoaderIsSharedByCU
#else
  #define FinalDfltCpuSrvcPatchLoaderIsSharedByCU                     DfltCpuSrvcPatchLoaderIsSharedByCU
#endif


//
// Make final service definition with considerations of below:
// 1. Common build config swich control
// 2. Override service definition
//

// Member: (UINT16) Revision
#ifdef CpuSrvcRevision
  #define FinalCpuSrvcRevision                                CpuSrvcRevision
#else
  #define FinalCpuSrvcRevision                                FinalDfltCpuSrvcRevision
#endif

// Member: (PF_CPU_DISABLE_PSTATE) DisablePstate
#if AGESA_ENTRY_INIT_EARLY == TRUE
  #ifdef CpuSrvcDisablePstate
    #define FinalCpuSrvcDisablePstate                           CpuSrvcDisablePstate
    #ifndef EXTERN_FINALCPUSRVCDISABLEPSTATE
    #define EXTERN_FINALCPUSRVCDISABLEPSTATE
        extern  F_CPU_DISABLE_PSTATE                                FinalCpuSrvcDisablePstate;
    #endif
  #else
    #define FinalCpuSrvcDisablePstate                           FinalDfltCpuSrvcDisablePstate
  #endif
#else
  #define FinalCpuSrvcDisablePstate                           FinalDfltAssertCpuSrvcDisablePstate
#endif

// Member: (PF_CPU_TRANSITION_PSTATE) TransitionPstate
#if (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_LATE == TRUE)
  #ifdef CpuSrvcTransitionPstate
    #define FinalCpuSrvcTransitionPstate                        CpuSrvcTransitionPstate
    #ifndef EXTERN_FINALCPUSRVCTRANSITIONPSTATE
    #define EXTERN_FINALCPUSRVCTRANSITIONPSTATE
        extern  F_CPU_TRANSITION_PSTATE                             FinalCpuSrvcTransitionPstate;
    #endif
  #else
    #define FinalCpuSrvcTransitionPstate                        FinalDfltCpuSrvcTransitionPstate
  #endif
#else
  #define FinalCpuSrvcTransitionPstate                        FinalDfltAssertCpuSrvcTransitionPstate
#endif

// Member: (PF_CPU_GET_IDD_MAX) GetProcIddMax
#if (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE)
  #ifdef CpuSrvcGetProcIddMax
    #define FinalCpuSrvcGetProcIddMax                           CpuSrvcGetProcIddMax
    extern  F_CPU_GET_IDD_MAX                                   FinalCpuSrvcGetProcIddMax;
  #else
    #define FinalCpuSrvcGetProcIddMax                           FinalDfltCpuSrvcGetProcIddMax
  #endif
#else
  #define FinalCpuSrvcGetProcIddMax                           FinalDfltAssertCpuSrvcGetProcIddMax
#endif

// Member: (PF_CPU_GET_TSC_RATE) GetTscRate
#if (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_LATE == TRUE)
  #ifdef CpuSrvcGetTscRate
    #define FinalCpuSrvcGetTscRate                              CpuSrvcGetTscRate
    #ifndef EXTERN_FINALCPUSRVCGETTSCRATE
    #define EXTERN_FINALCPUSRVCGETTSCRATE
        extern  F_CPU_GET_TSC_RATE                                  FinalCpuSrvcGetTscRate;
    #endif
  #else
    #define FinalCpuSrvcGetTscRate                              FinalDfltCpuSrvcGetTscRate
  #endif
#else
  #define FinalCpuSrvcGetTscRate                              FinalDfltAssertCpuSrvcGetTscRate
#endif

// Member: (PF_CPU_GET_NB_FREQ) GetCurrentNbFrequency
#if AGESA_ENTRY_INIT_EARLY == TRUE
  #ifdef CpuSrvcGetCurrentNbFrequency
    #define FinalCpuSrvcGetCurrentNbFrequency                   CpuSrvcGetCurrentNbFrequency
    extern  F_CPU_GET_NB_FREQ                                   FinalCpuSrvcGetCurrentNbFrequency;
  #else
    #define FinalCpuSrvcGetCurrentNbFrequency                   FinalDfltCpuSrvcGetCurrentNbFrequency
  #endif
#else
  #define FinalCpuSrvcGetCurrentNbFrequency                   FinalDfltAssertCpuSrvcGetCurrentNbFrequency
#endif


// Member: (PF_CPU_GET_MIN_MAX_NB_FREQ) GetMinMaxNbFrequency
#if AGESA_ENTRY_INIT_EARLY == TRUE
  #ifdef CpuSrvcGetMinMaxNbFrequency
    #define FinalCpuSrvcGetMinMaxNbFrequency                    CpuSrvcGetMinMaxNbFrequency
    extern  F_CPU_GET_MIN_MAX_NB_FREQ                           FinalCpuSrvcGetMinMaxNbFrequency;
  #else
    #define FinalCpuSrvcGetMinMaxNbFrequency                    FinalDfltCpuSrvcGetMinMaxNbFrequency
  #endif
#else
  #define FinalCpuSrvcGetMinMaxNbFrequency                    FinalDfltAssertCpuSrvcGetMinMaxNbFrequency
#endif

// Member: (PF_CPU_GET_NB_PSTATE_INFO) GetNbPstateInfo
#if AGESA_ENTRY_INIT_EARLY == TRUE
  #ifdef CpuSrvcGetNbPstateInfo
    #define FinalCpuSrvcGetNbPstateInfo                         CpuSrvcGetNbPstateInfo
    extern  F_CPU_GET_NB_PSTATE_INFO                            FinalCpuSrvcGetNbPstateInfo;
  #else
    #define FinalCpuSrvcGetNbPstateInfo                         FinalDfltCpuSrvcGetNbPstateInfo
  #endif
#else
  #define FinalCpuSrvcGetNbPstateInfo                         FinalDfltAssertCpuSrvcGetNbPstateInfo
#endif

// Member: (PF_CPU_IS_NBCOF_INIT_NEEDED) IsNbCofInitNeeded
#if AGESA_ENTRY_INIT_EARLY == TRUE
  #ifdef CpuSrvcIsNbCofInitNeeded
    #define FinalCpuSrvcIsNbCofInitNeeded                       CpuSrvcIsNbCofInitNeeded
    extern  F_CPU_IS_NBCOF_INIT_NEEDED                          FinalCpuSrvcIsNbCofInitNeeded;
  #else
    #define FinalCpuSrvcIsNbCofInitNeeded                       FinalDfltCpuSrvcIsNbCofInitNeeded
  #endif
#else
  #define FinalCpuSrvcIsNbCofInitNeeded                       FinalDfltAssertCpuSrvcIsNbCofInitNeeded
#endif

// Member:    (PF_CPU_GET_NB_IDD_MAX) GetNbIddMax
#if AGESA_ENTRY_INIT_EARLY == TRUE
  #ifdef CpuSrvcGetNbIddMax
    #define FinalCpuSrvcGetNbIddMax                             CpuSrvcGetNbIddMax
    extern  F_CPU_GET_NB_IDD_MAX                                FinalCpuSrvcGetNbIddMax;
  #else
    #define FinalCpuSrvcGetNbIddMax                             FinalDfltCpuSrvcGetNbIddMax
  #endif
#else
  #define FinalCpuSrvcGetNbIddMax                             FinalDfltAssertCpuSrvcGetNbIddMax
#endif

// Member:  (PF_CPU_AP_INITIAL_LAUNCH) LaunchApCore
#if AGESA_ENTRY_INIT_EARLY == TRUE
  #ifdef CpuSrvcLaunchApCore
    #define FinalCpuSrvcLaunchApCore                            CpuSrvcLaunchApCore
    #ifndef EXTERN_FINALCPUSRVCLAUNCHAPCORE
    #define EXTERN_FINALCPUSRVCLAUNCHAPCORE
        extern  F_CPU_AP_INITIAL_LAUNCH                             FinalCpuSrvcLaunchApCore;
    #endif
  #else
    #define FinalCpuSrvcLaunchApCore                            FinalDfltCpuSrvcLaunchApCore
  #endif
#else
  #define FinalCpuSrvcLaunchApCore                            FinalDfltAssertCpuSrvcLaunchApCore
#endif

// Member:(PF_CPU_NUMBER_OF_PHYSICAL_CORES) GetNumberOfPhysicalCores
#if (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_LATE == TRUE)
  #ifdef CpuSrvcGetNumberOfPhysicalCores
    #define FinalCpuSrvcGetNumberOfPhysicalCores                CpuSrvcGetNumberOfPhysicalCores
    extern  F_CPU_NUMBER_OF_PHYSICAL_CORES                      FinalCpuSrvcGetNumberOfPhysicalCores;
  #else
    #define FinalCpuSrvcGetNumberOfPhysicalCores                FinalDfltCpuSrvcGetNumberOfPhysicalCores
  #endif
#else
  #define FinalCpuSrvcGetNumberOfPhysicalCores                FinalDfltAssertCpuSrvcGetNumberOfPhysicalCores
#endif

// Member: (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE) GetApMailboxFromHardware
#if (AGESA_ENTRY_INIT_RESET == TRUE) || (AGESA_ENTRY_INIT_EARLY == TRUE)
  #ifdef CpuSrvcGetApMailboxFromHardware
    #define FinalCpuSrvcGetApMailboxFromHardware                CpuSrvcGetApMailboxFromHardware
    extern  F_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE              FinalCpuSrvcGetApMailboxFromHardware;
  #else
    #define FinalCpuSrvcGetApMailboxFromHardware                FinalDfltCpuSrvcGetApMailboxFromHardware
  #endif
#else
  #define FinalCpuSrvcGetApMailboxFromHardware                FinalDfltAssertCpuSrvcGetApMailboxFromHardware
#endif

// Member: (PF_CPU_SET_AP_CORE_NUMBER) SetApCoreNumber
#if (AGESA_ENTRY_INIT_RESET == TRUE) || (AGESA_ENTRY_INIT_RECOVERY == TRUE) || (AGESA_ENTRY_INIT_EARLY == TRUE)
  #ifdef CpuSrvcSetApCoreNumber
    #define FinalCpuSrvcSetApCoreNumber                         CpuSrvcSetApCoreNumber
    extern  F_CPU_SET_AP_CORE_NUMBER                            FinalCpuSrvcSetApCoreNumber;
  #else
    #define FinalCpuSrvcSetApCoreNumber                         FinalDfltCpuSrvcSetApCoreNumber
  #endif
#else
  #define FinalCpuSrvcSetApCoreNumber                         FinalDfltAssertCpuSrvcSetApCoreNumber
#endif

// Member: (PF_CPU_GET_AP_CORE_NUMBER) GetApCoreNumber
#if (AGESA_ENTRY_INIT_RESET == TRUE) || (AGESA_ENTRY_INIT_RECOVERY == TRUE) || (AGESA_ENTRY_INIT_EARLY == TRUE) || \
    (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_LATE == TRUE)
  #ifdef CpuSrvcGetApCoreNumber
    #define FinalCpuSrvcGetApCoreNumber                         CpuSrvcGetApCoreNumber
    extern  F_CPU_GET_AP_CORE_NUMBER                            FinalCpuSrvcGetApCoreNumber;
  #else
    #define FinalCpuSrvcGetApCoreNumber                         FinalDfltCpuSrvcGetApCoreNumber
  #endif
#else
  #define FinalCpuSrvcGetApCoreNumber                         FinalDfltAssertCpuSrvcGetApCoreNumber
#endif

// Member: (PF_CPU_TRANSFER_AP_CORE_NUMBER) TransferApCoreNumber
#if (AGESA_ENTRY_INIT_RESET == TRUE) || (AGESA_ENTRY_INIT_RECOVERY == TRUE) || (AGESA_ENTRY_INIT_EARLY == TRUE)
  #ifdef CpuSrvcTransferApCoreNumber
    #define FinalCpuSrvcTransferApCoreNumber                    CpuSrvcTransferApCoreNumber
    extern  F_CPU_TRANSFER_AP_CORE_NUMBER                       FinalCpuSrvcTransferApCoreNumber;
  #else
    #define FinalCpuSrvcTransferApCoreNumber                    FinalDfltCpuSrvcTransferApCoreNumber
  #endif
#else
  #define FinalCpuSrvcTransferApCoreNumber                    FinalDfltAssertCpuSrvcTransferApCoreNumber
#endif

// Member: (PF_CPU_GET_STORED_NODE_NUMBER) GetStoredNodeNumber
#if (AGESA_ENTRY_INIT_RESET == TRUE) || (AGESA_ENTRY_INIT_RECOVERY == TRUE) || (AGESA_ENTRY_INIT_EARLY == TRUE) || \
    (AGESA_ENTRY_INIT_ENV == TRUE) || (AGESA_ENTRY_INIT_MID == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE) || \
    (AGESA_ENTRY_INIT_LATE == TRUE) || (AGESA_ENTRY_INIT_S3SAVE == TRUE) || (AGESA_ENTRY_INIT_RESUME == TRUE) || \
    (AGESA_ENTRY_INIT_LATE_RESTORE == TRUE) || (AGESA_ENTRY_INIT_GENERAL_SERVICES == TRUE)
  #ifdef CpuSrvcGetStoredNodeNumber
    #define FinalCpuSrvcGetStoredNodeNumber                     CpuSrvcGetStoredNodeNumber
    extern  F_CPU_GET_STORED_NODE_NUMBER                        FinalCpuSrvcGetStoredNodeNumber;
  #else
    #define FinalCpuSrvcGetStoredNodeNumber                     FinalDfltCpuSrvcGetStoredNodeNumber
  #endif
#else
  #define FinalCpuSrvcGetStoredNodeNumber                     FinalDfltAssertCpuSrvcGetStoredNodeNumber
#endif

// Member: (PF_CORE_ID_POSITION_IN_INITIAL_APIC_ID) CoreIdPositionInInitialApicId
#if (AGESA_ENTRY_INIT_RESET == TRUE) || (AGESA_ENTRY_INIT_RECOVERY == TRUE) || (AGESA_ENTRY_INIT_EARLY == TRUE) || \
    (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_GENERAL_SERVICES == TRUE)
  #ifdef CpuSrvcCoreIdPositionInInitialApicId
    #define FinalCpuSrvcCoreIdPositionInInitialApicId           CpuSrvcCoreIdPositionInInitialApicId
    #ifndef EXTERN_FINALCPUSRVCCOREIDPOSITIONININITIALAPICID
    #define EXTERN_FINALCPUSRVCCOREIDPOSITIONININITIALAPICID
        extern  F_CORE_ID_POSITION_IN_INITIAL_APIC_ID               FinalCpuSrvcCoreIdPositionInInitialApicId;
    #endif
  #else
    #define FinalCpuSrvcCoreIdPositionInInitialApicId           FinalDfltCpuSrvcoreIdPositionInInitialApicId
  #endif
#else
  #define FinalCpuSrvcCoreIdPositionInInitialApicId           FinalDfltAssertCpuSrvcCoreIdPositionInInitialApicId
#endif

// Member: (PF_CPU_SAVE_FEATURES) SaveFeatures
#if (AGESA_ENTRY_INIT_POST == TRUE)
  #ifdef CpuSrvcSaveFeatures
    #define FinalCpuSrvcSaveFeatures                            CpuSrvcSaveFeatures
    extern  F_CPU_SAVE_FEATURES                                 FinalCpuSrvcSaveFeatures;
  #else
    #define FinalCpuSrvcSaveFeatures                            FinalDfltCpuSrvcSaveFeatures
  #endif
#else
  #define FinalCpuSrvcSaveFeatures                            FinalDfltAssertCpuSrvcSaveFeatures
#endif

// Member: (PF_CPU_WRITE_FEATURES) WriteFeatures
#if (AGESA_ENTRY_INIT_POST == TRUE)
  #ifdef CpuSrvcWriteFeatures
    #define FinalCpuSrvcWriteFeatures                           CpuSrvcWriteFeatures
    extern  F_CPU_WRITE_FEATURES                                FinalCpuSrvcWriteFeatures;
  #else
    #define FinalCpuSrvcWriteFeatures                           FinalDfltCpuSrvcWriteFeatures
  #endif
#else
  #define FinalCpuSrvcWriteFeatures                           FinalDfltAssertCpuSrvcWriteFeatures
#endif

// Member: (PF_CPU_SET_WARM_RESET_FLAG) SetWarmResetFlag
#if (AGESA_ENTRY_INIT_RESET == TRUE) || (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE)
  #ifdef CpuSrvcSetWarmResetFlag
    #define FinalCpuSrvcSetWarmResetFlag                        CpuSrvcSetWarmResetFlag
    #ifndef EXTERN_FINALCPUSRVCSETWARMRESETFLAG
    #define EXTERN_FINALCPUSRVCSETWARMRESETFLAG
        extern  F_CPU_SET_WARM_RESET_FLAG                           FinalCpuSrvcSetWarmResetFlag;
    #endif
  #else
    #define FinalCpuSrvcSetWarmResetFlag                        FinalDfltCpuSrvcSetWarmResetFlag
  #endif
#else
  #define FinalCpuSrvcSetWarmResetFlag                        FinalDfltAssertCpuSrvcSetWarmResetFlag
#endif

// Member: (PF_CPU_GET_WARM_RESET_FLAG) GetWarmResetFlag
#if (AGESA_ENTRY_INIT_RESET == TRUE) || (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE)
  #ifdef CpuSrvcGetWarmResetFlag
    #define FinalCpuSrvcGetWarmResetFlag                        CpuSrvcGetWarmResetFlag
    #ifndef EXTERN_FINALCPUSRVCGETWARMRESETFLAG
    #define EXTERN_FINALCPUSRVCGETWARMRESETFLAG
        extern  F_CPU_GET_WARM_RESET_FLAG                           FinalCpuSrvcGetWarmResetFlag;
    #endif
  #else
    #define FinalCpuSrvcGetWarmResetFlag                        FinalDfltCpuSrvcGetWarmResetFlag
  #endif
#else
  #define FinalCpuSrvcGetWarmResetFlag                        FinalDfltAssertCpuSrvcGetWarmResetFlag
#endif

// Member: (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) GetBrandString1
#if (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_LATE == TRUE)
  #ifdef CpuSrvcGetBrandString1
    #define FinalCpuSrvcGetBrandString1                         CpuSrvcGetBrandString1
    extern  F_CPU_GET_FAMILY_SPECIFIC_ARRAY                     FinalCpuSrvcGetBrandString1;
  #else
    #define FinalCpuSrvcGetBrandString1                         FinalDfltCpuSrvcGetBrandString1
  #endif
#else
  #define FinalCpuSrvcGetBrandString1                         FinalDfltAssertCpuSrvcGetBrandString1
#endif

// Member: (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) GetBrandString2
#if (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_LATE == TRUE)
  #ifdef CpuSrvcGetBrandString2
    #define FinalCpuSrvcGetBrandString2                         CpuSrvcGetBrandString2
    extern  F_CPU_GET_FAMILY_SPECIFIC_ARRAY                     FinalCpuSrvcGetBrandString2;
  #else
    #define FinalCpuSrvcGetBrandString2                         FinalDfltCpuSrvcGetBrandString2
  #endif
#else
  #define FinalCpuSrvcGetBrandString2                         FinalDfltAssertCpuSrvcGetBrandString2
#endif

// Member: (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) GetMicroCodePatchesStruct
#if (AGESA_ENTRY_INIT_RECOVERY == TRUE) || (AGESA_ENTRY_INIT_EARLY == TRUE)
  #ifdef CpuSrvcGetMicroCodePatchesStruct
    #define FinalCpuSrvcGetMicroCodePatchesStruct               CpuSrvcGetMicroCodePatchesStruct
    #ifndef EXTERN_FINALCPUSRVCGETMICROCODEPATCHESSTRUCT
    #define EXTERN_FINALCPUSRVCGETMICROCODEPATCHESSTRUCT
        extern  F_CPU_GET_FAMILY_SPECIFIC_ARRAY                     FinalCpuSrvcGetMicroCodePatchesStruct;
    #endif
  #else
    #define FinalCpuSrvcGetMicroCodePatchesStruct               FinalDfltCpuSrvcGetMicroCodePatchesStruct
  #endif
#else
  #define FinalCpuSrvcGetMicroCodePatchesStruct               FinalDfltAssertCpuSrvcGetMicroCodePatchesStruct
#endif

// Member: (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) GetMicrocodeEquivalenceTable
#if (AGESA_ENTRY_INIT_RECOVERY == TRUE) || (AGESA_ENTRY_INIT_EARLY == TRUE)
  #ifdef CpuSrvcGetMicrocodeEquivalenceTable
    #define FinalCpuSrvcGetMicrocodeEquivalenceTable            CpuSrvcGetMicrocodeEquivalenceTable
    #ifndef EXTERN_FINALCPUSRVCGETMICROCODEEQUIVALENCETABLE
    #define EXTERN_FINALCPUSRVCGETMICROCODEEQUIVALENCETABLE
        extern  F_CPU_GET_FAMILY_SPECIFIC_ARRAY                     FinalCpuSrvcGetMicrocodeEquivalenceTable;
    #endif
  #else
    #define FinalCpuSrvcGetMicrocodeEquivalenceTable            FinalDfltCpuSrvcGetMicrocodeEquivalenceTable
  #endif
#else
  #define FinalCpuSrvcGetMicrocodeEquivalenceTable            FinalDfltAssertCpuSrvcGetMicrocodeEquivalenceTable
#endif

// Member: (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) GetCacheInfo
#if (AGESA_ENTRY_INIT_RESET == TRUE) || (AGESA_ENTRY_INIT_RECOVERY == TRUE) || (AGESA_ENTRY_INIT_EARLY == TRUE)
  #ifdef CpuSrvcGetCacheInfo
    #define FinalCpuSrvcGetCacheInfo                            CpuSrvcGetCacheInfo
    extern  F_CPU_GET_FAMILY_SPECIFIC_ARRAY                     FinalCpuSrvcGetCacheInfo;
  #else
    #define FinalCpuSrvcGetCacheInfo                            FinalDfltCpuSrvcGetCacheInfo
  #endif
#else
  #define FinalCpuSrvcGetCacheInfo                            FinalDfltAssertCpuSrvcGetCacheInfo
#endif

// Member: (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) GetSysPmTableStruct
#if AGESA_ENTRY_INIT_EARLY == TRUE
  #ifdef CpuSrvcGetSysPmTableStruct
    #define FinalCpuSrvcGetSysPmTableStruct                     CpuSrvcGetSysPmTableStruct
    extern  F_CPU_GET_FAMILY_SPECIFIC_ARRAY                     FinalCpuSrvcGetSysPmTableStruct;
  #else
    #define FinalCpuSrvcGetSysPmTableStruct                     FinalDfltCpuSrvcGetSysPmTableStruct
  #endif
#else
  #define FinalCpuSrvcGetSysPmTableStruct                     FinalDfltAssertCpuSrvcGetSysPmTableStruct
#endif

// Member: (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY) GetWheaInitData
#if AGESA_ENTRY_INIT_LATE == TRUE
  #ifdef CpuSrvcGetWheaInitData
    #define FinalCpuSrvcGetWheaInitData                         CpuSrvcGetWheaInitData
    #ifndef EXTERN_FINALCPUSRVCGETWHEAINITDATA
    #define EXTERN_FINALCPUSRVCGETWHEAINITDATA
        extern  F_CPU_GET_FAMILY_SPECIFIC_ARRAY                     FinalCpuSrvcGetWheaInitData;
    #endif
  #else
    #define FinalCpuSrvcGetWheaInitData                         FinalDfltCpuSrvcGetWheaInitData
  #endif
#else
  #define FinalCpuSrvcGetWheaInitData                         FinalDfltAssertCpuSrvcGetWheaInitData
#endif

// Member: (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO) GetPlatformTypeSpecificInfo
#if (AGESA_ENTRY_INIT_RECOVERY == TRUE) || (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_LATE == TRUE)
  #ifdef CpuSrvcGetPlatformTypeSpecificInfo
    #define FinalCpuSrvcGetPlatformTypeSpecificInfo             CpuSrvcGetPlatformTypeSpecificInfo
    extern  F_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO               FinalCpuSrvcGetPlatformTypeSpecificInfo;
  #else
    #define FinalCpuSrvcGetPlatformTypeSpecificInfo             FinalDfltCpuSrvcGetPlatformTypeSpecificInfo
  #endif
#else
  #define FinalCpuSrvcGetPlatformTypeSpecificInfo             FinalDfltAssertCpuSrvcGetPlatformTypeSpecificInfo
#endif

// Member: (PF_IS_NB_PSTATE_ENABLED) IsNbPstateEnabled
#if (AGESA_ENTRY_INIT_RECOVERY == TRUE) || (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE)
  #ifdef CpuSrvcIsNbPstateEnabled
    #define FinalCpuSrvcIsNbPstateEnabled                       CpuSrvcIsNbPstateEnabled
    extern  F_IS_NB_PSTATE_ENABLED                              FinalCpuSrvcIsNbPstateEnabled;
  #else
    #define FinalCpuSrvcIsNbPstateEnabled                       FinalDfltCpuSrvcIsNbPstateEnabled
  #endif
#else
  #define FinalCpuSrvcIsNbPstateEnabled                       FinalDfltAssertCpuSrvcIsNbPstateEnabled
#endif

// Member: (PF_NEXT_LINK_HAS_HTFPY_FEATS) NextLinkHasHtPhyFeats
#if BASE_FAMILY_HT_PCI == TRUE
  #ifdef CpuSrvcNextLinkHasHtPhyFeats
    #define FinalCpuSrvcNextLinkHasHtPhyFeats                   CpuSrvcNextLinkHasHtPhyFeats
    extern  F_NEXT_LINK_HAS_HTPHY_FEATS                         FinalCpuSrvcNextLinkHasHtPhyFeats;
  #else
    #define FinalCpuSrvcNextLinkHasHtPhyFeats                   FinalDfltCpuSrvcNextLinkHasHtPhyFeats
  #endif
#else
  #define FinalCpuSrvcNextLinkHasHtPhyFeats                   FinalDfltAssertCpuSrvcNextLinkHasHtPhyFeats
#endif

// Member: (PF_SET_HT_PHY_REGISTER) SetHtPhyRegister
#if BASE_FAMILY_HT_PCI == TRUE
  #ifdef CpuSrvcSetHtPhyRegister
    #define FinalCpuSrvcSetHtPhyRegister                        CpuSrvcSetHtPhyRegister
    extern  F_SET_HT_PHY_REGISTER                               FinalCpuSrvcSetHtPhyRegister;
  #else
    #define FinalCpuSrvcSetHtPhyRegister                        FinalDfltCpuSrvcSetHtPhyRegister
  #endif
#else
  #define FinalCpuSrvcSetHtPhyRegister                        FinalDfltAssertCpuSrvcSetHtPhyRegister
#endif

// Member: (PF_GET_NEXT_HT_LINK_FEATURES) GetNextHtLinkFeatures
#if BASE_FAMILY_PCI == TRUE
  #ifdef CpuSrvcGetNextHtLinkFeatures
    #define FinalCpuSrvcGetNextHtLinkFeatures                   CpuSrvcGetNextHtLinkFeatures
    extern  F_GET_NEXT_HT_LINK_FEATURES                         FinalCpuSrvcGetNextHtLinkFeatures;
  #else
    #define FinalCpuSrvcGetNextHtLinkFeatures                   FinalDfltCpuSrvcGetNextHtLinkFeatures
  #endif
#else
  #define FinalCpuSrvcGetNextHtLinkFeatures                   FinalDfltAssertCpuSrvcGetNextHtLinkFeatures
#endif

// Member: (REGISTER_TABLE **) RegisterTableList
#if USES_REGISTER_TABLES == TRUE // (AGESA_ENTRY_INIT_RECOVERY == TRUE) || (AGESA_ENTRY_INIT_EARLY == TRUE)
  #ifdef CpuSrvcRegisterTableList
    #define FinalCpuSrvcRegisterTableList                       CpuSrvcRegisterTableList
  #else
    #define FinalCpuSrvcRegisterTableList                       FinalDfltCpuSrvcRegisterTableList
  #endif
#else
  #define FinalCpuSrvcRegisterTableList                       FinalDfltAssertCpuSrvcRegisterTableList
#endif

// Member: (TABLE_ENTRY_TYPE_DESCRIPTOR *) TableEntryTypeDescriptors
#if USES_REGISTER_TABLES == TRUE // (AGESA_ENTRY_INIT_RECOVERY == TRUE) || (AGESA_ENTRY_INIT_EARLY == TRUE)
  #ifdef CpuSrvcTableEntryTypeDescriptors
    #define FinalCpuSrvcTableEntryTypeDescriptors               CpuSrvcTableEntryTypeDescriptors
  #else
    #define FinalCpuSrvcTableEntryTypeDescriptors               FinalDfltCpuSrvcTableEntryTypeDescriptors
  #endif
#else
  #define FinalCpuSrvcTableEntryTypeDescriptors               FinalDfltAssertCpuSrvcTableEntryTypeDescriptors
#endif

// Member: (PACKAGE_HTLINK_MAP) PackageLinkMap
#if MODEL_SPECIFIC_HT_PCI == TRUE
  #ifdef CpuSrvcPackageLinkMap
    #define FinalCpuSrvcPackageLinkMap                          CpuSrvcPackageLinkMap
  #else
    #define FinalCpuSrvcPackageLinkMap                          FinalDfltCpuSrvcPackageLinkMap
  #endif
#else
  #define FinalCpuSrvcPackageLinkMap                          FinalDfltAssertCpuSrvcPackageLinkMap
#endif

// Member: (COMPUTE_UNIT_MAP *) ComputeUnitMap
#ifdef CpuSrvcComputeUnitMap
  #define FinalCpuSrvcComputeUnitMap                            CpuSrvcComputeUnitMap
#else
  #define FinalCpuSrvcComputeUnitMap                            FinalDfltCpuSrvcComputeUnitMap
#endif

// Member: (FAMILY_CACHE_INIT_POLICY) InitCacheDisabled
#ifdef CpuSrvcInitCacheDisabled
  #define FinalCpuSrvcInitCacheDisabled                         CpuSrvcInitCacheDisabled
#else
  #define FinalCpuSrvcInitCacheDisabled                         FinalDfltCpuSrvcInitCacheDisabled
#endif

// Member: (PF_GET_EARLY_INIT_TABLE) GetEarlyInitBeforeApLaunchOnCoreTable
#if AGESA_ENTRY_INIT_EARLY == TRUE
  #ifdef CpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable
    #define FinalCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable                 CpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable
    extern  F_GET_EARLY_INIT_TABLE                              FinalCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable;
  #else
    #define FinalCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable                 FinalDfltCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable
  #endif
#else
  #define FinalCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable                   FinalDfltAssertCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable
#endif

// Member: (PF_GET_EARLY_INIT_TABLE) GetEarlyInitAfterApLaunchOnCoreTable
#if AGESA_ENTRY_INIT_EARLY == TRUE
  #ifdef CpuSrvcGetEarlyInitAfterApLaunchOnCoreTable
    #define FinalCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable                 CpuSrvcGetEarlyInitAfterApLaunchOnCoreTable
    extern  F_GET_EARLY_INIT_TABLE                              FinalCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable;
  #else
    #define FinalCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable                 FinalDfltCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable
  #endif
#else
  #define FinalCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable                   FinalDfltAssertCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable
#endif

// Member: (BOOLEAN) PatchLoaderIsSharedByCU
#if (AGESA_ENTRY_INIT_RECOVERY == TRUE) || (AGESA_ENTRY_INIT_EARLY == TRUE)
  #ifdef CpuSrvcPatchLoaderIsSharedByCU
    #define FinalCpuSrvcPatchLoaderIsSharedByCU                 CpuSrvcPatchLoaderIsSharedByCU
  #else
    #define FinalCpuSrvcPatchLoaderIsSharedByCU                 FinalDfltCpuSrvcPatchLoaderIsSharedByCU
  #endif
#else
  #define FinalCpuSrvcPatchLoaderIsSharedByCU                 FinalDfltAssertCpuSrvcPatchLoaderIsSharedByCU
#endif

//
// NOTE: All CPU family specific service members should be all defined now.
//

//
// Define CPU specific services installation macro
//
#ifndef _INSTALL_CPU_SPECIFIC_SERVICES_TABLE_NAME_
  #define _INSTALL_CPU_SPECIFIC_SERVICES_TABLE_NAME_

  #define INSTALL_CPU_SPECIFIC_SERVICES_TABLE_NAME(MacroPrefix, TableName) \
  CONST CPU_SPECIFIC_SERVICES ROMDATA TableName =           \
  {                                                         \
    MacroPrefix##CpuSrvcRevision,                           \
    MacroPrefix##CpuSrvcDisablePstate,                      \
    MacroPrefix##CpuSrvcTransitionPstate,                   \
    MacroPrefix##CpuSrvcGetProcIddMax,                      \
    MacroPrefix##CpuSrvcGetTscRate,                         \
    MacroPrefix##CpuSrvcGetCurrentNbFrequency,              \
    MacroPrefix##CpuSrvcGetMinMaxNbFrequency,               \
    MacroPrefix##CpuSrvcGetNbPstateInfo,                    \
    MacroPrefix##CpuSrvcIsNbCofInitNeeded,                  \
    MacroPrefix##CpuSrvcGetNbIddMax,                        \
    MacroPrefix##CpuSrvcLaunchApCore,                       \
    MacroPrefix##CpuSrvcGetNumberOfPhysicalCores,           \
    MacroPrefix##CpuSrvcGetApMailboxFromHardware,           \
    MacroPrefix##CpuSrvcSetApCoreNumber,                    \
    MacroPrefix##CpuSrvcGetApCoreNumber,                    \
    MacroPrefix##CpuSrvcTransferApCoreNumber,               \
    MacroPrefix##CpuSrvcGetStoredNodeNumber,                \
    MacroPrefix##CpuSrvcCoreIdPositionInInitialApicId,      \
    MacroPrefix##CpuSrvcSaveFeatures,                       \
    MacroPrefix##CpuSrvcWriteFeatures,                      \
    MacroPrefix##CpuSrvcSetWarmResetFlag,                   \
    MacroPrefix##CpuSrvcGetWarmResetFlag,                   \
    MacroPrefix##CpuSrvcGetBrandString1,                    \
    MacroPrefix##CpuSrvcGetBrandString2,                    \
    MacroPrefix##CpuSrvcGetMicroCodePatchesStruct,          \
    MacroPrefix##CpuSrvcGetMicrocodeEquivalenceTable,       \
    MacroPrefix##CpuSrvcGetCacheInfo,                       \
    MacroPrefix##CpuSrvcGetSysPmTableStruct,                \
    MacroPrefix##CpuSrvcGetWheaInitData,                    \
    MacroPrefix##CpuSrvcGetPlatformTypeSpecificInfo,        \
    MacroPrefix##CpuSrvcIsNbPstateEnabled,                  \
    MacroPrefix##CpuSrvcNextLinkHasHtPhyFeats,              \
    MacroPrefix##CpuSrvcSetHtPhyRegister,                   \
    MacroPrefix##CpuSrvcGetNextHtLinkFeatures,              \
    MacroPrefix##CpuSrvcRegisterTableList,                  \
    MacroPrefix##CpuSrvcTableEntryTypeDescriptors,          \
    MacroPrefix##CpuSrvcPackageLinkMap,                     \
    MacroPrefix##CpuSrvcComputeUnitMap,                     \
    MacroPrefix##CpuSrvcInitCacheDisabled,                  \
    MacroPrefix##CpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable, \
    MacroPrefix##CpuSrvcGetEarlyInitAfterApLaunchOnCoreTable, \
    MacroPrefix##CpuSrvcPatchLoaderIsSharedByCU             \
  }
#endif // _INSTALL_CPU_SPECIFIC_SERVICES_TABLE_NAME_

#ifndef _INSTALL_CPU_SPECIFIC_SERVICES_TABLE_
  #define _INSTALL_CPU_SPECIFIC_SERVICES_TABLE_

  #define INSTALL_CPU_SPECIFIC_SERVICES_TABLE(TableName) \
            INSTALL_CPU_SPECIFIC_SERVICES_TABLE_NAME (Final, TableName)
#endif // _INSTALL_CPU_SPECIFIC_SERVICES_TABLE_

//
// Message out the final table definitions
//

