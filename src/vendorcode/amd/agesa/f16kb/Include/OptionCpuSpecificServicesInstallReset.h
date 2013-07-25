/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of CPU specific services support
 *
 * This file resets and generates default services of CPU specific services.
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

//
// Undefine service table name
//
#undef CpuSrvcTableName

//
// Definitions types of a service to undefine
//
// * CpuSrvc<ServiceName>
// * DfltCpuSrvc<ServiceName>
// * DfltAssertCpuSrvc<ServiceName>
// * OvrdDfltCpuSrvc<ServiceName>
// * OvrdDfltAssertCpuSrvc<ServiceName>
// * FinalDfltCpuSrvc<ServiceName>
// * FinalDfltAssertCpuSrvc<ServiceName>
// * FinalCpuSrvc<ServiceName>
//

//
// Reset default services definitions
//
#undef CpuSrvcRevision
#undef CpuSrvcDisablePstate
#undef CpuSrvcTransitionPstate
#undef CpuSrvcGetProcIddMax
#undef CpuSrvcGetTscRate
#undef CpuSrvcGetCurrentNbFrequency
#undef CpuSrvcGetMinMaxNbFrequency
#undef CpuSrvcGetNbPstateInfo
#undef CpuSrvcIsNbCofInitNeeded
#undef CpuSrvcGetNbIddMax
#undef CpuSrvcLaunchApCore
#undef CpuSrvcGetNumberOfPhysicalCores
#undef CpuSrvcGetApMailboxFromHardware
#undef CpuSrvcSetApCoreNumber
#undef CpuSrvcGetApCoreNumber
#undef CpuSrvcTransferApCoreNumber
#undef CpuSrvcGetStoredNodeNumber
#undef CpuSrvcCoreIdPositionInInitialApicId
#undef CpuSrvcSaveFeatures
#undef CpuSrvcWriteFeatures
#undef CpuSrvcSetWarmResetFlag
#undef CpuSrvcGetWarmResetFlag
#undef CpuSrvcGetBrandString1
#undef CpuSrvcGetBrandString2
#undef CpuSrvcGetMicroCodePatchesStruct
#undef CpuSrvcGetMicrocodeEquivalenceTable
#undef CpuSrvcGetCacheInfo
#undef CpuSrvcGetSysPmTableStruct
#undef CpuSrvcGetWheaInitData
#undef CpuSrvcGetPlatformTypeSpecificInfo
#undef CpuSrvcIsNbPstateEnabled
#undef CpuSrvcNextLinkHasHtPhyFeats
#undef CpuSrvcSetHtPhyRegister
#undef CpuSrvcGetNextHtLinkFeatures
#undef CpuSrvcRegisterTableList
#undef CpuSrvcTableEntryTypeDescriptors
#undef CpuSrvcPackageLinkMap
#undef CpuSrvcComputeUnitMap
#undef CpuSrvcInitCacheDisabled
#undef CpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable
#undef CpuSrvcGetEarlyInitAfterApLaunchOnCoreTable
#undef CpuSrvcPatchLoaderIsSharedByCU

#undef DfltCpuSrvcRevision
#undef DfltCpuSrvcDisablePstate
#undef DfltCpuSrvcTransitionPstate
#undef DfltCpuSrvcGetProcIddMax
#undef DfltCpuSrvcGetTscRate
#undef DfltCpuSrvcGetCurrentNbFrequency
#undef DfltCpuSrvcGetMinMaxNbFrequency
#undef DfltCpuSrvcGetNbPstateInfo
#undef DfltCpuSrvcIsNbCofInitNeeded
#undef DfltCpuSrvcGetNbIddMax
#undef DfltCpuSrvcLaunchApCore
#undef DfltCpuSrvcGetNumberOfPhysicalCores
#undef DfltCpuSrvcGetApMailboxFromHardware
#undef DfltCpuSrvcSetApCoreNumber
#undef DfltCpuSrvcGetApCoreNumber
#undef DfltCpuSrvcTransferApCoreNumber
#undef DfltCpuSrvcGetStoredNodeNumber
#undef DfltCpuSrvcCoreIdPositionInInitialApicId
#undef DfltCpuSrvcSaveFeatures
#undef DfltCpuSrvcWriteFeatures
#undef DfltCpuSrvcSetWarmResetFlag
#undef DfltCpuSrvcGetWarmResetFlag
#undef DfltCpuSrvcGetBrandString1
#undef DfltCpuSrvcGetBrandString2
#undef DfltCpuSrvcGetMicroCodePatchesStruct
#undef DfltCpuSrvcGetMicrocodeEquivalenceTable
#undef DfltCpuSrvcGetCacheInfo
#undef DfltCpuSrvcGetSysPmTableStruct
#undef DfltCpuSrvcGetWheaInitData
#undef DfltCpuSrvcGetPlatformTypeSpecificInfo
#undef DfltCpuSrvcIsNbPstateEnabled
#undef DfltCpuSrvcNextLinkHasHtPhyFeats
#undef DfltCpuSrvcSetHtPhyRegister
#undef DfltCpuSrvcGetNextHtLinkFeatures
#undef DfltCpuSrvcRegisterTableList
#undef DfltCpuSrvcTableEntryTypeDescriptors
#undef DfltCpuSrvcPackageLinkMap
#undef DfltCpuSrvcComputeUnitMap
#undef DfltCpuSrvcInitCacheDisabled
#undef DfltCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable
#undef DfltCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable
#undef DfltCpuSrvcPatchLoaderIsSharedByCU

#undef DfltAssertCpuSrvcRevision
#undef DfltAssertCpuSrvcDisablePstate
#undef DfltAssertCpuSrvcTransitionPstate
#undef DfltAssertCpuSrvcGetProcIddMax
#undef DfltAssertCpuSrvcGetTscRate
#undef DfltAssertCpuSrvcGetCurrentNbFrequency
#undef DfltAssertCpuSrvcGetMinMaxNbFrequency
#undef DfltAssertCpuSrvcGetNbPstateInfo
#undef DfltAssertCpuSrvcIsNbCofInitNeeded
#undef DfltAssertCpuSrvcGetNbIddMax
#undef DfltAssertCpuSrvcLaunchApCore
#undef DfltAssertCpuSrvcGetNumberOfPhysicalCores
#undef DfltAssertCpuSrvcGetApMailboxFromHardware
#undef DfltAssertCpuSrvcSetApCoreNumber
#undef DfltAssertCpuSrvcGetApCoreNumber
#undef DfltAssertCpuSrvcTransferApCoreNumber
#undef DfltAssertCpuSrvcGetStoredNodeNumber
#undef DfltAssertCpuSrvcCoreIdPositionInInitialApicId
#undef DfltAssertCpuSrvcSaveFeatures
#undef DfltAssertCpuSrvcWriteFeatures
#undef DfltAssertCpuSrvcSetWarmResetFlag
#undef DfltAssertCpuSrvcGetWarmResetFlag
#undef DfltAssertCpuSrvcGetBrandString1
#undef DfltAssertCpuSrvcGetBrandString2
#undef DfltAssertCpuSrvcGetMicroCodePatchesStruct
#undef DfltAssertCpuSrvcGetMicrocodeEquivalenceTable
#undef DfltAssertCpuSrvcGetCacheInfo
#undef DfltAssertCpuSrvcGetSysPmTableStruct
#undef DfltAssertCpuSrvcGetWheaInitData
#undef DfltAssertCpuSrvcGetPlatformTypeSpecificInfo
#undef DfltAssertCpuSrvcIsNbPstateEnabled
#undef DfltAssertCpuSrvcNextLinkHasHtPhyFeats
#undef DfltAssertCpuSrvcSetHtPhyRegister
#undef DfltAssertCpuSrvcGetNextHtLinkFeatures
#undef DfltAssertCpuSrvcRegisterTableList
#undef DfltAssertCpuSrvcTableEntryTypeDescriptors
#undef DfltAssertCpuSrvcPackageLinkMap
#undef DfltAssertCpuSrvcComputeUnitMap
#undef DfltAssertCpuSrvcInitCacheDisabled
#undef DfltAssertCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable
#undef DfltAssertCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable
#undef DfltAssertCpuSrvcPatchLoaderIsSharedByCU

#undef OvrdDfltCpuSrvcRevision
#undef OvrdDfltCpuSrvcDisablePstate
#undef OvrdDfltCpuSrvcTransitionPstate
#undef OvrdDfltCpuSrvcGetProcIddMax
#undef OvrdDfltCpuSrvcGetTscRate
#undef OvrdDfltCpuSrvcGetCurrentNbFrequency
#undef OvrdDfltCpuSrvcGetMinMaxNbFrequency
#undef OvrdDfltCpuSrvcGetNbPstateInfo
#undef OvrdDfltCpuSrvcIsNbCofInitNeeded
#undef OvrdDfltCpuSrvcGetNbIddMax
#undef OvrdDfltCpuSrvcLaunchApCore
#undef OvrdDfltCpuSrvcGetNumberOfPhysicalCores
#undef OvrdDfltCpuSrvcGetApMailboxFromHardware
#undef OvrdDfltCpuSrvcSetApCoreNumber
#undef OvrdDfltCpuSrvcGetApCoreNumber
#undef OvrdDfltCpuSrvcTransferApCoreNumber
#undef OvrdDfltCpuSrvcGetStoredNodeNumber
#undef OvrdDfltCpuSrvcCoreIdPositionInInitialApicId
#undef OvrdDfltCpuSrvcSaveFeatures
#undef OvrdDfltCpuSrvcWriteFeatures
#undef OvrdDfltCpuSrvcSetWarmResetFlag
#undef OvrdDfltCpuSrvcGetWarmResetFlag
#undef OvrdDfltCpuSrvcGetBrandString1
#undef OvrdDfltCpuSrvcGetBrandString2
#undef OvrdDfltCpuSrvcGetMicroCodePatchesStruct
#undef OvrdDfltCpuSrvcGetMicrocodeEquivalenceTable
#undef OvrdDfltCpuSrvcGetCacheInfo
#undef OvrdDfltCpuSrvcGetSysPmTableStruct
#undef OvrdDfltCpuSrvcGetWheaInitData
#undef OvrdDfltCpuSrvcGetPlatformTypeSpecificInfo
#undef OvrdDfltCpuSrvcIsNbPstateEnabled
#undef OvrdDfltCpuSrvcNextLinkHasHtPhyFeats
#undef OvrdDfltCpuSrvcSetHtPhyRegister
#undef OvrdDfltCpuSrvcGetNextHtLinkFeatures
#undef OvrdDfltCpuSrvcRegisterTableList
#undef OvrdDfltCpuSrvcTableEntryTypeDescriptors
#undef OvrdDfltCpuSrvcPackageLinkMap
#undef OvrdDfltCpuSrvcComputeUnitMap
#undef OvrdDfltCpuSrvcInitCacheDisabled
#undef OvrdDfltCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable
#undef OvrdDfltCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable
#undef OvrdDfltCpuSrvcPatchLoaderIsSharedByCU

#undef OvrdDfltAssertCpuSrvcRevision
#undef OvrdDfltAssertCpuSrvcDisablePstate
#undef OvrdDfltAssertCpuSrvcTransitionPstate
#undef OvrdDfltAssertCpuSrvcGetProcIddMax
#undef OvrdDfltAssertCpuSrvcGetTscRate
#undef OvrdDfltAssertCpuSrvcGetCurrentNbFrequency
#undef OvrdDfltAssertCpuSrvcGetMinMaxNbFrequency
#undef OvrdDfltAssertCpuSrvcGetNbPstateInfo
#undef OvrdDfltAssertCpuSrvcIsNbCofInitNeeded
#undef OvrdDfltAssertCpuSrvcGetNbIddMax
#undef OvrdDfltAssertCpuSrvcLaunchApCore
#undef OvrdDfltAssertCpuSrvcGetNumberOfPhysicalCores
#undef OvrdDfltAssertCpuSrvcGetApMailboxFromHardware
#undef OvrdDfltAssertCpuSrvcSetApCoreNumber
#undef OvrdDfltAssertCpuSrvcGetApCoreNumber
#undef OvrdDfltAssertCpuSrvcTransferApCoreNumber
#undef OvrdDfltAssertCpuSrvcGetStoredNodeNumber
#undef OvrdDfltAssertCpuSrvcCoreIdPositionInInitialApicId
#undef OvrdDfltAssertCpuSrvcSaveFeatures
#undef OvrdDfltAssertCpuSrvcWriteFeatures
#undef OvrdDfltAssertCpuSrvcSetWarmResetFlag
#undef OvrdDfltAssertCpuSrvcGetWarmResetFlag
#undef OvrdDfltAssertCpuSrvcGetBrandString1
#undef OvrdDfltAssertCpuSrvcGetBrandString2
#undef OvrdDfltAssertCpuSrvcGetMicroCodePatchesStruct
#undef OvrdDfltAssertCpuSrvcGetMicrocodeEquivalenceTable
#undef OvrdDfltAssertCpuSrvcGetCacheInfo
#undef OvrdDfltAssertCpuSrvcGetSysPmTableStruct
#undef OvrdDfltAssertCpuSrvcGetWheaInitData
#undef OvrdDfltAssertCpuSrvcGetPlatformTypeSpecificInfo
#undef OvrdDfltAssertCpuSrvcIsNbPstateEnabled
#undef OvrdDfltAssertCpuSrvcNextLinkHasHtPhyFeats
#undef OvrdDfltAssertCpuSrvcSetHtPhyRegister
#undef OvrdDfltAssertCpuSrvcGetNextHtLinkFeatures
#undef OvrdDfltAssertCpuSrvcRegisterTableList
#undef OvrdDfltAssertCpuSrvcTableEntryTypeDescriptors
#undef OvrdDfltAssertCpuSrvcPackageLinkMap
#undef OvrdDfltAssertCpuSrvcComputeUnitMap
#undef OvrdDfltAssertCpuSrvcInitCacheDisabled
#undef OvrdDfltAssertCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable
#undef OvrdDfltAssertCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable
#undef OvrdDfltAssertCpuSrvcPatchLoaderIsSharedByCU

#undef FinalDfltCpuSrvcRevision
#undef FinalDfltCpuSrvcDisablePstate
#undef FinalDfltCpuSrvcTransitionPstate
#undef FinalDfltCpuSrvcGetProcIddMax
#undef FinalDfltCpuSrvcGetTscRate
#undef FinalDfltCpuSrvcGetCurrentNbFrequency
#undef FinalDfltCpuSrvcGetMinMaxNbFrequency
#undef FinalDfltCpuSrvcGetNbPstateInfo
#undef FinalDfltCpuSrvcIsNbCofInitNeeded
#undef FinalDfltCpuSrvcGetNbIddMax
#undef FinalDfltCpuSrvcLaunchApCore
#undef FinalDfltCpuSrvcGetNumberOfPhysicalCores
#undef FinalDfltCpuSrvcGetApMailboxFromHardware
#undef FinalDfltCpuSrvcSetApCoreNumber
#undef FinalDfltCpuSrvcGetApCoreNumber
#undef FinalDfltCpuSrvcTransferApCoreNumber
#undef FinalDfltCpuSrvcGetStoredNodeNumber
#undef FinalDfltCpuSrvcCoreIdPositionInInitialApicId
#undef FinalDfltCpuSrvcSaveFeatures
#undef FinalDfltCpuSrvcWriteFeatures
#undef FinalDfltCpuSrvcSetWarmResetFlag
#undef FinalDfltCpuSrvcGetWarmResetFlag
#undef FinalDfltCpuSrvcGetBrandString1
#undef FinalDfltCpuSrvcGetBrandString2
#undef FinalDfltCpuSrvcGetMicroCodePatchesStruct
#undef FinalDfltCpuSrvcGetMicrocodeEquivalenceTable
#undef FinalDfltCpuSrvcGetCacheInfo
#undef FinalDfltCpuSrvcGetSysPmTableStruct
#undef FinalDfltCpuSrvcGetWheaInitData
#undef FinalDfltCpuSrvcGetPlatformTypeSpecificInfo
#undef FinalDfltCpuSrvcIsNbPstateEnabled
#undef FinalDfltCpuSrvcNextLinkHasHtPhyFeats
#undef FinalDfltCpuSrvcSetHtPhyRegister
#undef FinalDfltCpuSrvcGetNextHtLinkFeatures
#undef FinalDfltCpuSrvcRegisterTableList
#undef FinalDfltCpuSrvcTableEntryTypeDescriptors
#undef FinalDfltCpuSrvcPackageLinkMap
#undef FinalDfltCpuSrvcComputeUnitMap
#undef FinalDfltCpuSrvcInitCacheDisabled
#undef FinalDfltCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable
#undef FinalDfltCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable
#undef FinalDfltCpuSrvcPatchLoaderIsSharedByCU

#undef FinalDfltAssertCpuSrvcRevision
#undef FinalDfltAssertCpuSrvcDisablePstate
#undef FinalDfltAssertCpuSrvcTransitionPstate
#undef FinalDfltAssertCpuSrvcGetProcIddMax
#undef FinalDfltAssertCpuSrvcGetTscRate
#undef FinalDfltAssertCpuSrvcGetCurrentNbFrequency
#undef FinalDfltAssertCpuSrvcGetMinMaxNbFrequency
#undef FinalDfltAssertCpuSrvcGetNbPstateInfo
#undef FinalDfltAssertCpuSrvcIsNbCofInitNeeded
#undef FinalDfltAssertCpuSrvcGetNbIddMax
#undef FinalDfltAssertCpuSrvcLaunchApCore
#undef FinalDfltAssertCpuSrvcGetNumberOfPhysicalCores
#undef FinalDfltAssertCpuSrvcGetApMailboxFromHardware
#undef FinalDfltAssertCpuSrvcSetApCoreNumber
#undef FinalDfltAssertCpuSrvcGetApCoreNumber
#undef FinalDfltAssertCpuSrvcTransferApCoreNumber
#undef FinalDfltAssertCpuSrvcGetStoredNodeNumber
#undef FinalDfltAssertCpuSrvcCoreIdPositionInInitialApicId
#undef FinalDfltAssertCpuSrvcSaveFeatures
#undef FinalDfltAssertCpuSrvcWriteFeatures
#undef FinalDfltAssertCpuSrvcSetWarmResetFlag
#undef FinalDfltAssertCpuSrvcGetWarmResetFlag
#undef FinalDfltAssertCpuSrvcGetBrandString1
#undef FinalDfltAssertCpuSrvcGetBrandString2
#undef FinalDfltAssertCpuSrvcGetMicroCodePatchesStruct
#undef FinalDfltAssertCpuSrvcGetMicrocodeEquivalenceTable
#undef FinalDfltAssertCpuSrvcGetCacheInfo
#undef FinalDfltAssertCpuSrvcGetSysPmTableStruct
#undef FinalDfltAssertCpuSrvcGetWheaInitData
#undef FinalDfltAssertCpuSrvcGetPlatformTypeSpecificInfo
#undef FinalDfltAssertCpuSrvcIsNbPstateEnabled
#undef FinalDfltAssertCpuSrvcNextLinkHasHtPhyFeats
#undef FinalDfltAssertCpuSrvcSetHtPhyRegister
#undef FinalDfltAssertCpuSrvcGetNextHtLinkFeatures
#undef FinalDfltAssertCpuSrvcRegisterTableList
#undef FinalDfltAssertCpuSrvcTableEntryTypeDescriptors
#undef FinalDfltAssertCpuSrvcPackageLinkMap
#undef FinalDfltAssertCpuSrvcComputeUnitMap
#undef FinalDfltAssertCpuSrvcInitCacheDisabled
#undef FinalDfltAssertCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable
#undef FinalDfltAssertCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable
#undef FinalDfltAssertCpuSrvcPatchLoaderIsSharedByCU

#undef FinalCpuSrvcRevision
#undef FinalCpuSrvcDisablePstate
#undef FinalCpuSrvcTransitionPstate
#undef FinalCpuSrvcGetProcIddMax
#undef FinalCpuSrvcGetTscRate
#undef FinalCpuSrvcGetCurrentNbFrequency
#undef FinalCpuSrvcGetMinMaxNbFrequency
#undef FinalCpuSrvcGetNbPstateInfo
#undef FinalCpuSrvcIsNbCofInitNeeded
#undef FinalCpuSrvcGetNbIddMax
#undef FinalCpuSrvcLaunchApCore
#undef FinalCpuSrvcGetNumberOfPhysicalCores
#undef FinalCpuSrvcGetApMailboxFromHardware
#undef FinalCpuSrvcSetApCoreNumber
#undef FinalCpuSrvcGetApCoreNumber
#undef FinalCpuSrvcTransferApCoreNumber
#undef FinalCpuSrvcGetStoredNodeNumber
#undef FinalCpuSrvcCoreIdPositionInInitialApicId
#undef FinalCpuSrvcSaveFeatures
#undef FinalCpuSrvcWriteFeatures
#undef FinalCpuSrvcSetWarmResetFlag
#undef FinalCpuSrvcGetWarmResetFlag
#undef FinalCpuSrvcGetBrandString1
#undef FinalCpuSrvcGetBrandString2
#undef FinalCpuSrvcGetMicroCodePatchesStruct
#undef FinalCpuSrvcGetMicrocodeEquivalenceTable
#undef FinalCpuSrvcGetCacheInfo
#undef FinalCpuSrvcGetSysPmTableStruct
#undef FinalCpuSrvcGetWheaInitData
#undef FinalCpuSrvcGetPlatformTypeSpecificInfo
#undef FinalCpuSrvcIsNbPstateEnabled
#undef FinalCpuSrvcNextLinkHasHtPhyFeats
#undef FinalCpuSrvcSetHtPhyRegister
#undef FinalCpuSrvcGetNextHtLinkFeatures
#undef FinalCpuSrvcRegisterTableList
#undef FinalCpuSrvcTableEntryTypeDescriptors
#undef FinalCpuSrvcPackageLinkMap
#undef FinalCpuSrvcComputeUnitMap
#undef FinalCpuSrvcInitCacheDisabled
#undef FinalCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable
#undef FinalCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable
#undef FinalCpuSrvcPatchLoaderIsSharedByCU

//
// Define null CPU specific services - no redefinition is expected.
//
#define NullCpuSrvcRevision                                   (UINT16)                                      0
#define NullCpuSrvcDisablePstate                              (PF_CPU_DISABLE_PSTATE)                       CommonReturnAgesaSuccess
#define NullCpuSrvcTransitionPstate                           (PF_CPU_TRANSITION_PSTATE)                    CommonReturnAgesaSuccess
#define NullCpuSrvcGetProcIddMax                              (PF_CPU_GET_IDD_MAX)                          CommonReturnFalse
#define NullCpuSrvcGetTscRate                                 (PF_CPU_GET_TSC_RATE)                         CommonReturnAgesaSuccess
#define NullCpuSrvcGetCurrentNbFrequency                      (PF_CPU_GET_NB_FREQ)                          CommonReturnAgesaError
#define NullCpuSrvcGetMinMaxNbFrequency                       (PF_CPU_GET_MIN_MAX_NB_FREQ)                  CommonReturnAgesaError
#define NullCpuSrvcGetNbPstateInfo                            (PF_CPU_GET_NB_PSTATE_INFO)                   CommonReturnFalse
#define NullCpuSrvcIsNbCofInitNeeded                          (PF_CPU_IS_NBCOF_INIT_NEEDED)                 CommonReturnAgesaSuccess
#define NullCpuSrvcGetNbIddMax                                (PF_CPU_GET_NB_IDD_MAX)                       CommonReturnFalse
#define NullCpuSrvcLaunchApCore                               (PF_CPU_AP_INITIAL_LAUNCH)                    CommonReturnFalse
#define NullCpuSrvcGetNumberOfPhysicalCores                   (PF_CPU_NUMBER_OF_PHYSICAL_CORES)             CommonReturnZero8
#define NullCpuSrvcGetApMailboxFromHardware                   (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE)     CommonReturnAgesaSuccess
#define NullCpuSrvcSetApCoreNumber                            (PF_CPU_SET_AP_CORE_NUMBER)                   CommonVoid
#define NullCpuSrvcGetApCoreNumber                            (PF_CPU_GET_AP_CORE_NUMBER)                   CommonReturnOne32
#define NullCpuSrvcTransferApCoreNumber                       (PF_CPU_TRANSFER_AP_CORE_NUMBER)              CommonVoid
#define NullCpuSrvcGetStoredNodeNumber                        (PF_CPU_GET_STORED_NODE_NUMBER)               CommonReturnZero32
#define NullCpuSrvcCoreIdPositionInInitialApicId              (PF_CORE_ID_POSITION_IN_INITIAL_APIC_ID)      CommonReturnAgesaSuccess
#define NullCpuSrvcSaveFeatures                               (PF_CPU_SAVE_FEATURES)                        CommonVoid
#define NullCpuSrvcWriteFeatures                              (PF_CPU_WRITE_FEATURES)                       CommonVoid
#define NullCpuSrvcSetWarmResetFlag                           (PF_CPU_SET_WARM_RESET_FLAG)                  CommonReturnAgesaSuccess
#define NullCpuSrvcGetWarmResetFlag                           (PF_CPU_GET_WARM_RESET_FLAG)                  CommonReturnAgesaSuccess
#define NullCpuSrvcGetBrandString1                            (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)            GetEmptyArray
#define NullCpuSrvcGetBrandString2                            (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)            GetEmptyArray
#define NullCpuSrvcGetMicroCodePatchesStruct                  (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)            GetEmptyArray
#define NullCpuSrvcGetMicrocodeEquivalenceTable               (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)            GetEmptyArray
#define NullCpuSrvcGetCacheInfo                               (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)            GetEmptyArray
#define NullCpuSrvcGetSysPmTableStruct                        (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)            GetEmptyArray
#define NullCpuSrvcGetWheaInitData                            (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)            GetEmptyArray
#define NullCpuSrvcGetPlatformTypeSpecificInfo                (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO)      CommonReturnAgesaSuccess
#define NullCpuSrvcIsNbPstateEnabled                          (PF_IS_NB_PSTATE_ENABLED)                     CommonReturnFalse
#define NullCpuSrvcNextLinkHasHtPhyFeats                      (PF_NEXT_LINK_HAS_HTPHY_FEATS)                CommonReturnFalse
#define NullCpuSrvcSetHtPhyRegister                           (PF_SET_HT_PHY_REGISTER)                      CommonVoid
#define NullCpuSrvcGetNextHtLinkFeatures                      (PF_GET_NEXT_HT_LINK_FEATURES)                CommonVoid
#define NullCpuSrvcRegisterTableList                          (REGISTER_TABLE **)                           NULL
#define NullCpuSrvcTableEntryTypeDescriptors                  (TABLE_ENTRY_TYPE_DESCRIPTOR *)               NULL
#define NullCpuSrvcPackageLinkMap                             (PACKAGE_HTLINK_MAP)                          NULL
#define NullCpuSrvcComputeUnitMap                             (COMPUTE_UNIT_MAP *)                          NULL
#define NullCpuSrvcInitCacheDisabled                          (FAMILY_CACHE_INIT_POLICY)                    InitCacheDisabled
#define NullCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable      (PF_GET_EARLY_INIT_TABLE)                     CommonVoid
#define NullCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable       (PF_GET_EARLY_INIT_TABLE)                     CommonVoid
#define NullCpuSrvcPatchLoaderIsSharedByCU                    (BOOLEAN)                                     FALSE
//
// Define default cpu specific services assertion if possible
//
#define DfltAssertCpuSrvcRevision                             NullCpuSrvcRevision
#define DfltAssertCpuSrvcDisablePstate                        (PF_CPU_DISABLE_PSTATE)                       CommonAssert
#define DfltAssertCpuSrvcTransitionPstate                     (PF_CPU_TRANSITION_PSTATE)                    CommonAssert
#define DfltAssertCpuSrvcGetProcIddMax                        (PF_CPU_GET_IDD_MAX)                          CommonAssert
#define DfltAssertCpuSrvcGetTscRate                           (PF_CPU_GET_TSC_RATE)                         CommonAssert
#define DfltAssertCpuSrvcGetCurrentNbFrequency                (PF_CPU_GET_NB_FREQ)                          CommonAssert
#define DfltAssertCpuSrvcGetMinMaxNbFrequency                 (PF_CPU_GET_MIN_MAX_NB_FREQ)                  CommonAssert
#define DfltAssertCpuSrvcGetNbPstateInfo                      (PF_CPU_GET_NB_PSTATE_INFO)                   CommonAssert
#define DfltAssertCpuSrvcIsNbCofInitNeeded                    (PF_CPU_IS_NBCOF_INIT_NEEDED)                 CommonAssert
#define DfltAssertCpuSrvcGetNbIddMax                          (PF_CPU_GET_NB_IDD_MAX)                       CommonAssert
#define DfltAssertCpuSrvcLaunchApCore                         (PF_CPU_AP_INITIAL_LAUNCH)                    CommonAssert
#define DfltAssertCpuSrvcGetNumberOfPhysicalCores             (PF_CPU_NUMBER_OF_PHYSICAL_CORES)             CommonAssert
#define DfltAssertCpuSrvcGetApMailboxFromHardware             (PF_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE)     CommonAssert
#define DfltAssertCpuSrvcSetApCoreNumber                      (PF_CPU_SET_AP_CORE_NUMBER)                   CommonAssert
#define DfltAssertCpuSrvcGetApCoreNumber                      (PF_CPU_GET_AP_CORE_NUMBER)                   CommonAssert
#define DfltAssertCpuSrvcTransferApCoreNumber                 (PF_CPU_TRANSFER_AP_CORE_NUMBER)              CommonAssert
#define DfltAssertCpuSrvcGetStoredNodeNumber                  (PF_CPU_GET_STORED_NODE_NUMBER)               CommonAssert
#define DfltAssertCpuSrvcCoreIdPositionInInitialApicId        (PF_CORE_ID_POSITION_IN_INITIAL_APIC_ID)      CommonAssert
#define DfltAssertCpuSrvcSaveFeatures                         (PF_CPU_SAVE_FEATURES)                        CommonAssert
#define DfltAssertCpuSrvcWriteFeatures                        (PF_CPU_WRITE_FEATURES)                       CommonAssert
#define DfltAssertCpuSrvcSetWarmResetFlag                     (PF_CPU_SET_WARM_RESET_FLAG)                  CommonAssert
#define DfltAssertCpuSrvcGetWarmResetFlag                     (PF_CPU_GET_WARM_RESET_FLAG)                  CommonAssert
#define DfltAssertCpuSrvcGetBrandString1                      (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)            CommonAssert
#define DfltAssertCpuSrvcGetBrandString2                      (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)            CommonAssert
#define DfltAssertCpuSrvcGetMicroCodePatchesStruct            (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)            CommonAssert
#define DfltAssertCpuSrvcGetMicrocodeEquivalenceTable         (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)            CommonAssert
#define DfltAssertCpuSrvcGetCacheInfo                         (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)            CommonAssert
#define DfltAssertCpuSrvcGetSysPmTableStruct                  (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)            CommonAssert
#define DfltAssertCpuSrvcGetWheaInitData                      (PF_CPU_GET_FAMILY_SPECIFIC_ARRAY)            CommonAssert
#define DfltAssertCpuSrvcGetPlatformTypeSpecificInfo          (PF_CPU_GET_PLATFORM_TYPE_SPECIFIC_INFO)      CommonAssert
#define DfltAssertCpuSrvcIsNbPstateEnabled                    (PF_IS_NB_PSTATE_ENABLED)                     CommonAssert
#define DfltAssertCpuSrvcNextLinkHasHtPhyFeats                (PF_NEXT_LINK_HAS_HTPHY_FEATS)                CommonAssert
#define DfltAssertCpuSrvcSetHtPhyRegister                     (PF_SET_HT_PHY_REGISTER)                      CommonAssert
#define DfltAssertCpuSrvcGetNextHtLinkFeatures                (PF_GET_NEXT_HT_LINK_FEATURES)                CommonAssert
#define DfltAssertCpuSrvcRegisterTableList                    NullCpuSrvcRegisterTableList
#define DfltAssertCpuSrvcTableEntryTypeDescriptors            NullCpuSrvcTableEntryTypeDescriptors
#define DfltAssertCpuSrvcPackageLinkMap                       NullCpuSrvcPackageLinkMap
#define DfltAssertCpuSrvcComputeUnitMap                       NullCpuSrvcComputeUnitMap
#define DfltAssertCpuSrvcInitCacheDisabled                    (FAMILY_CACHE_INIT_POLICY)                    InitCacheDisabled
#define DfltAssertCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable (PF_GET_EARLY_INIT_TABLE)                    CommonAssert
#define DfltAssertCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable (PF_GET_EARLY_INIT_TABLE)                     CommonAssert
#define DfltAssertCpuSrvcPatchLoaderIsSharedByCU              NullCpuSrvcPatchLoaderIsSharedByCU

//
// Define family specific services default when feature is enabled in the build
//
#define DfltCpuSrvcRevision                                   NullCpuSrvcRevision
#define DfltCpuSrvcDisablePstate                              DfltAssertCpuSrvcDisablePstate
#define DfltCpuSrvcTransitionPstate                           DfltAssertCpuSrvcTransitionPstate
#define DfltCpuSrvcGetProcIddMax                              NullCpuSrvcGetProcIddMax
#define DfltCpuSrvcGetTscRate                                 DfltAssertCpuSrvcGetTscRate
#define DfltCpuSrvcGetCurrentNbFrequency                      DfltAssertCpuSrvcGetCurrentNbFrequency
#define DfltCpuSrvcGetMinMaxNbFrequency                       NullCpuSrvcGetMinMaxNbFrequency
#define DfltCpuSrvcGetNbPstateInfo                            NullCpuSrvcGetNbPstateInfo
#define DfltCpuSrvcIsNbCofInitNeeded                          (PF_CPU_IS_NBCOF_INIT_NEEDED)                 CommonReturnFalse
#define DfltCpuSrvcGetNbIddMax                                DfltAssertCpuSrvcGetNbIddMax
#define DfltCpuSrvcLaunchApCore                               NullCpuSrvcLaunchApCore
#define DfltCpuSrvcGetNumberOfPhysicalCores                   (PF_CPU_NUMBER_OF_PHYSICAL_CORES)             CommonReturnOne8
#define DfltCpuSrvcGetApMailboxFromHardware                   DfltAssertCpuSrvcGetApMailboxFromHardware
#define DfltCpuSrvcSetApCoreNumber                            NullCpuSrvcSetApCoreNumber
#define DfltCpuSrvcGetApCoreNumber                            NullCpuSrvcGetApCoreNumber
#define DfltCpuSrvcTransferApCoreNumber                       NullCpuSrvcTransferApCoreNumber
#define DfltCpuSrvcGetStoredNodeNumber                        NullCpuSrvcGetStoredNodeNumber
#define DfltCpuSrvcCoreIdPositionInInitialApicId              (PF_CORE_ID_POSITION_IN_INITIAL_APIC_ID)      CommonReturnOne32
#define DfltCpuSrvcSaveFeatures                               NullCpuSrvcSaveFeatures
#define DfltCpuSrvcWriteFeatures                              NullCpuSrvcWriteFeatures
#define DfltCpuSrvcSetWarmResetFlag                           (PF_CPU_SET_WARM_RESET_FLAG)                  CommonVoid
#define DfltCpuSrvcGetWarmResetFlag                           DfltAssertCpuSrvcGetWarmResetFlag
#define DfltCpuSrvcGetBrandString1                            NullCpuSrvcGetBrandString1
#define DfltCpuSrvcGetBrandString2                            NullCpuSrvcGetBrandString2
#define DfltCpuSrvcGetMicroCodePatchesStruct                  NullCpuSrvcGetMicroCodePatchesStruct
#define DfltCpuSrvcGetMicrocodeEquivalenceTable               NullCpuSrvcGetMicrocodeEquivalenceTable
#define DfltCpuSrvcGetCacheInfo                               NullCpuSrvcGetCacheInfo
#define DfltCpuSrvcGetSysPmTableStruct                        NullCpuSrvcGetSysPmTableStruct
#define DfltCpuSrvcGetWheaInitData                            NullCpuSrvcGetWheaInitData
#define DfltCpuSrvcGetPlatformTypeSpecificInfo                NullCpuSrvcGetPlatformTypeSpecificInfo
#define DfltCpuSrvcIsNbPstateEnabled                          NullCpuSrvcIsNbPstateEnabled
#define DfltCpuSrvcNextLinkHasHtPhyFeats                      NullCpuSrvcNextLinkHasHtPhyFeats
#define DfltCpuSrvcSetHtPhyRegister                           NullCpuSrvcSetHtPhyRegister
#define DfltCpuSrvcGetNextHtLinkFeatures                      (PF_GET_NEXT_HT_LINK_FEATURES)                CommonReturnFalse
#define DfltCpuSrvcRegisterTableList                          NullCpuSrvcRegisterTableList
#define DfltCpuSrvcTableEntryTypeDescriptors                  NullCpuSrvcTableEntryTypeDescriptors
#define DfltCpuSrvcPackageLinkMap                             NullCpuSrvcPackageLinkMap
#define DfltCpuSrvcComputeUnitMap                             NullCpuSrvcComputeUnitMap
// NOTE: From CPUs with compute units and moving forward, we use InitCacheEnabled.
#define DfltCpuSrvcInitCacheDisabled                          (FAMILY_CACHE_INIT_POLICY)                    InitCacheEnabled
#define DfltCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable      NullCpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable
#define DfltCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable       NullCpuSrvcGetEarlyInitAfterApLaunchOnCoreTable
#define DfltCpuSrvcPatchLoaderIsSharedByCU                    NullCpuSrvcPatchLoaderIsSharedByCU

