/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of family 16h support
 *
 * This file generates the defaults tables for family 16h processors.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
 * @e \$Revision: 87264 $   @e \$Date: 2013-01-31 09:26:23 -0600 (Thu, 31 Jan 2013) $
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

#ifndef _OPTION_FAMILY_16H_INSTALL_H_
#define _OPTION_FAMILY_16H_INSTALL_H_

#include "OptionFamily16hEarlySample.h"
#include "cpuFamilyTranslation.h"


/*
 * Pull in family specific services based on entry point
 */

/*
 * Common Family 16h routines
 */

/*
 * Install family 16h model 00h - 0Fh support
 */
#ifdef OPTION_FAMILY16H_KB
  #if OPTION_FAMILY16H_KB == TRUE
    extern CONST REGISTER_TABLE ROMDATA F16KbPciRegisterTableBeforeApLaunch;
    extern CONST REGISTER_TABLE ROMDATA F16KbPciRegisterTableAfterApLaunch;
    extern CONST REGISTER_TABLE ROMDATA F16KbPciWorkaroundTable;
    extern CONST REGISTER_TABLE ROMDATA F16KbMsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F16KbMsrWorkaroundTable;
    extern CONST REGISTER_TABLE ROMDATA F16KbSharedMsrRegisterTable;
    extern CONST REGISTER_TABLE ROMDATA F16KbSharedMsrWorkaroundTable;

    /**
     * Compute unit and Compute unit primary determination table.
     *
     * The four fields from the compute unit status hardware register can be used to determine whether
     * even number cores are primary or all cores are primary.  It can be extended if it is
     * decided to have other configs as well.  The other logically possible value sets are BitMapMapping,
     * but they are currently not supported by the processor.
     */
    CONST COMPUTE_UNIT_MAP ROMDATA HtFam16KbComputeUnitMapping[] =
    {
      {1, 'x', 'x', 1, QuadCoresMapping},                            ///< 1 Compute Unit with 4 Cores
      {1, 'x',  1,  0, TripleCoresMapping},                          ///< 1 Compute Unit with 3 Cores
      {1,  1,   0,  0, EvenCoresMapping},                            ///< 1 Compute Unit with 2 cores
      {1,  0,   0,  0, AllCoresMapping},                             ///< 1 Compute Unit with 1 Cores
      {HT_LIST_TERMINAL, HT_LIST_TERMINAL, HT_LIST_TERMINAL, HT_LIST_TERMINAL, MaxComputeUnitMapping}   ///< End
    };


    #if USES_REGISTER_TABLES == TRUE
      CONST REGISTER_TABLE ROMDATA * CONST F16KbRegisterTables[] =
      {
        #if MODEL_SPECIFIC_PCI == TRUE
          &F16KbPciRegisterTableBeforeApLaunch,
          &F16KbPciRegisterTableAfterApLaunch,
          &F16KbPciWorkaroundTable,
        #endif
        #if MODEL_SPECIFIC_MSR == TRUE
          &F16KbMsrRegisterTable,
          &F16KbMsrWorkaroundTable,
          &F16KbSharedMsrRegisterTable,
          &F16KbSharedMsrWorkaroundTable,
        #endif
        // the end.
        NULL
      };
    #endif

    #if USES_REGISTER_TABLES == TRUE
      CONST TABLE_ENTRY_TYPE_DESCRIPTOR ROMDATA F16KbTableEntryTypeDescriptors[] =
      {
        {MsrRegister, SetRegisterForMsrEntry},
        {PciRegister, SetRegisterForPciEntry},
        {FamSpecificWorkaround, SetRegisterForFamSpecificWorkaroundEntry},
        {ProfileFixup, SetRegisterForPerformanceProfileEntry},
        {CoreCountsPciRegister, SetRegisterForCoreCountsPerformanceEntry},
        // End
        {TableEntryTypeMax, (PF_DO_TABLE_ENTRY)CommonVoid}
      };
    #endif

    /**
     * Early Init Tables
     *
     */
    extern F_PERFORM_EARLY_INIT_ON_CORE SetRegistersFromTablesBeforeApLaunch;
    extern F_PERFORM_EARLY_INIT_ON_CORE SetRegistersFromTablesAfterApLaunch;
    extern F_PERFORM_EARLY_INIT_ON_CORE F16SetBrandIdRegistersAtEarly;
    extern F_PERFORM_EARLY_INIT_ON_CORE LocalApicInitializationAtEarly;
    extern F_PERFORM_EARLY_INIT_ON_CORE LoadMicrocodePatchAtEarly;
    extern F_PERFORM_EARLY_INIT_ON_CORE F16KbLoadMicrocodePatchAtEarly;

    CONST S_PERFORM_EARLY_INIT_ON_CORE ROMDATA F16KbEarlyInitBeforeApLaunchOnCoreTable[] =
    {
      {SetRegistersFromTablesBeforeApLaunch, PERFORM_EARLY_ANY_CONDITION},
      {LocalApicInitializationAtEarly, PERFORM_EARLY_ANY_CONDITION},
      #if OPTION_EARLY_SAMPLES == TRUE
        {LoadMicrocodePatchAtEarly, PERFORM_EARLY_ANY_CONDITION},
      #endif
      {NULL, 0}
    };

    CONST S_PERFORM_EARLY_INIT_ON_CORE ROMDATA F16KbEarlyInitAfterApLaunchOnCoreTable[] =
    {
      {SetRegistersFromTablesAfterApLaunch, PERFORM_EARLY_ANY_CONDITION},
      {F16SetBrandIdRegistersAtEarly, PERFORM_EARLY_ANY_CONDITION},
      #if OPTION_EARLY_SAMPLES == FALSE
        {F16KbLoadMicrocodePatchAtEarly, PERFORM_EARLY_ANY_CONDITION},
      #endif
      {NULL, 0}
    };

    #include "OptionCpuSpecificServicesInstallReset.h"
    #define CpuSrvcTableName                            cpuF16KbServices

    #define CpuSrvcDisablePstate                        F16DisablePstate
    #define CpuSrvcTransitionPstate                     F16TransitionPstate
    #define CpuSrvcGetProcIddMax                        F16KbGetProcIddMax
    #define CpuSrvcGetTscRate                           F16GetTscRate
    #define CpuSrvcGetCurrentNbFrequency                F16KbGetCurrentNbFrequency
    #define CpuSrvcGetMinMaxNbFrequency                 F16KbGetMinMaxNbFrequency
    #define CpuSrvcGetNbPstateInfo                      F16KbGetNbPstateInfo
    #define CpuSrvcIsNbCofInitNeeded                    F16GetNbCofVidUpdate
    #define CpuSrvcGetNbIddMax                          F16KbGetNbIddMax
    #define CpuSrvcLaunchApCore                         F16LaunchApCore
    #define CpuSrvcGetNumberOfPhysicalCores             F16KbGetNumberOfPhysicalCores
    #define CpuSrvcGetApMailboxFromHardware             F16KbGetApMailboxFromHardware
    #define CpuSrvcGetApCoreNumber                      F16KbGetApCoreNumber
    #define CpuSrvcCoreIdPositionInInitialApicId        F16CpuAmdCoreIdPositionInInitialApicId
    #define CpuSrvcSetWarmResetFlag                     F16SetAgesaWarmResetFlag
    #define CpuSrvcGetWarmResetFlag                     F16GetAgesaWarmResetFlag
    #define CpuSrvcGetMicroCodePatchesStruct            GetF16KbMicroCodePatchesStruct
    #define CpuSrvcGetMicrocodeEquivalenceTable         GetF16KbMicrocodeEquivalenceTable
    #define CpuSrvcGetCacheInfo                         GetF16CacheInfo
    #define CpuSrvcGetSysPmTableStruct                  GetF16KbSysPmTable
    #define CpuSrvcGetWheaInitData                      GetF16WheaInitData
    #define CpuSrvcIsNbPstateEnabled                    F16KbIsNbPstateEnabled
    #define CpuSrvcRegisterTableList                    (REGISTER_TABLE **) F16KbRegisterTables
    #define CpuSrvcTableEntryTypeDescriptors            (TABLE_ENTRY_TYPE_DESCRIPTOR *) F16KbTableEntryTypeDescriptors
    #define CpuSrvcComputeUnitMap                       (COMPUTE_UNIT_MAP *) &HtFam16KbComputeUnitMapping
    #define CpuSrvcInitCacheDisabled                    InitCacheEnabled
    #define CpuSrvcGetEarlyInitBeforeApLaunchOnCoreTable GetF16KbEarlyInitBeforeApLaunchOnCoreTable
    #define CpuSrvcGetEarlyInitAfterApLaunchOnCoreTable GetF16KbEarlyInitAfterApLaunchOnCoreTable
    #define CpuSrvcPatchLoaderIsSharedByCU              FALSE

    #include "OptionCpuSpecificServicesInstall.h"
    INSTALL_CPU_SPECIFIC_SERVICES_TABLE (CpuSrvcTableName);

    #define KB_SOCKETS 1
    #define KB_MODULES 1
    #define KB_RECOVERY_SOCKETS 1
    #define KB_RECOVERY_MODULES 1
    extern F_CPU_GET_SUBFAMILY_ID_ARRAY GetF16KbLogicalIdAndRev;
    #define OPT_F16_KB_ID (PF_CPU_GET_SUBFAMILY_ID_ARRAY) GetF16KbLogicalIdAndRev,
    #ifndef ADVCFG_PLATFORM_SOCKETS
      #define ADVCFG_PLATFORM_SOCKETS KB_SOCKETS
    #else
      #if ADVCFG_PLATFORM_SOCKETS < KB_SOCKETS
        #undef ADVCFG_PLATFORM_SOCKETS
        #define ADVCFG_PLATFORM_SOCKETS KB_SOCKETS
      #endif
    #endif
    #ifndef ADVCFG_PLATFORM_MODULES
      #define ADVCFG_PLATFORM_MODULES KB_MODULES
    #else
      #if ADVCFG_PLATFORM_MODULES < KB_MODULES
        #undef ADVCFG_PLATFORM_MODULES
        #define ADVCFG_PLATFORM_MODULES KB_MODULES
      #endif
    #endif

    #if (AGESA_ENTRY_INIT_EARLY == TRUE)
      #define F16_KB_UCODE_7000
      #define F16_KB_UCODE_7001

      #if AGESA_ENTRY_INIT_EARLY == TRUE
        #if OPTION_EARLY_SAMPLES == TRUE
          extern  CONST UINT8 ROMDATA  CpuF16KbId7000MicrocodePatch[];
          #undef F16_KB_UCODE_7000
          #define F16_KB_UCODE_7000 CpuF16KbId7000MicrocodePatch,
        #endif
        extern  CONST UINT8 ROMDATA  CpuF16KbId7001MicrocodePatch[];
        #undef F16_KB_UCODE_7001
        #define F16_KB_UCODE_7001 CpuF16KbId7001MicrocodePatch,
      #endif

      CONST UINT8 ROMDATA * CONST CpuF16KbMicroCodePatchArray[] =
      {
        F16_KB_UCODE_7001
        F16_KB_UCODE_7000
        NULL
      };

      CONST UINT8 ROMDATA CpuF16KbNumberOfMicrocodePatches = (UINT8) ((sizeof (CpuF16KbMicroCodePatchArray) / sizeof (CpuF16KbMicroCodePatchArray[0])) - 1);
    #endif

    #define OPT_F16_KB_CPU {AMD_FAMILY_16_KB, &cpuF16KbServices},

  #else  //  OPTION_FAMILY16H_KB == TRUE
    #define OPT_F16_KB_CPU
    #define OPT_F16_KB_ID
  #endif  //  OPTION_FAMILY16H_KB == TRUE
#else  //  defined (OPTION_FAMILY16H_KB)
  #define OPT_F16_KB_CPU
  #define OPT_F16_KB_ID
#endif  //  defined (OPTION_FAMILY16H_KB)


/*
 * Install unknown family 16h support
 */


#if USES_REGISTER_TABLES == TRUE
  extern CONST REGISTER_TABLE ROMDATA F16PciUnknownRegisterTable;
  extern CONST REGISTER_TABLE ROMDATA F16MsrUnknownRegisterTable;
  CONST REGISTER_TABLE ROMDATA * CONST F16UnknownRegisterTables[] =
  {
    &F16PciUnknownRegisterTable,
    &F16MsrUnknownRegisterTable
    // the end.
  };
#endif

#if USES_REGISTER_TABLES == TRUE
  CONST TABLE_ENTRY_TYPE_DESCRIPTOR ROMDATA F16UnknownTableEntryTypeDescriptors[] =
  {
    {MsrRegister, SetRegisterForMsrEntry},
    {PciRegister, SetRegisterForPciEntry},
    // End
    {TableEntryTypeMax, (PF_DO_TABLE_ENTRY)CommonVoid}
  };
#endif



#include "OptionCpuSpecificServicesInstallReset.h"
#define CpuSrvcTableName                            cpuF16UnknownServices

#define CpuSrvcDisablePstate                        F16DisablePstate
#define CpuSrvcTransitionPstate                     F16TransitionPstate
#define CpuSrvcGetTscRate                           F16GetTscRate
#define CpuSrvcLaunchApCore                         F16LaunchApCore
#define CpuSrvcCoreIdPositionInInitialApicId        F16CpuAmdCoreIdPositionInInitialApicId
#define CpuSrvcSetWarmResetFlag                     F16SetAgesaWarmResetFlag
#define CpuSrvcGetWarmResetFlag                     F16GetAgesaWarmResetFlag
#define CpuSrvcGetMicroCodePatchesStruct            GetEmptyArray
#define CpuSrvcGetMicrocodeEquivalenceTable         GetEmptyArray
#define CpuSrvcGetWheaInitData                      GetF16WheaInitData
#define CpuSrvcIsNbPstateEnabled                    F16IsNbPstateEnabled
#define CpuSrvcRegisterTableList                    (REGISTER_TABLE **) F16UnknownRegisterTables
#define CpuSrvcTableEntryTypeDescriptors            (TABLE_ENTRY_TYPE_DESCRIPTOR *) F16UnknownTableEntryTypeDescriptors
#define CpuSrvcInitCacheDisabled                    InitCacheEnabled
#define CpuSrvcPatchLoaderIsSharedByCU              FALSE

#include "OptionCpuSpecificServicesInstall.h"
INSTALL_CPU_SPECIFIC_SERVICES_TABLE (CpuSrvcTableName);

// Family 16h maximum base address is 40 bits. Limit BLDCFG to 40 bits, if appropriate.

#if (FAMILY_MMIO_BASE_MASK < 0xFFFFFF0000000000ull)

  #undef  FAMILY_MMIO_BASE_MASK

  #define FAMILY_MMIO_BASE_MASK (0xFFFFFF0000000000ull)

#endif



#undef OPT_F16_ID_TABLE

#define OPT_F16_ID_TABLE {0x16, {AMD_FAMILY_16, AMD_F16_UNKNOWN}, F16LogicalIdTable, (sizeof (F16LogicalIdTable) / sizeof (F16LogicalIdTable[0]))},

#define OPT_F16_UNKNOWN_CPU {AMD_FAMILY_16, &cpuF16UnknownServices},


#undef OPT_F16_TABLE

#define OPT_F16_TABLE   OPT_F16_KB_CPU OPT_F16_UNKNOWN_CPU



CONST PF_CPU_GET_SUBFAMILY_ID_ARRAY ROMDATA F16LogicalIdTable[] =

{

  OPT_F16_KB_ID

};


#endif  // _OPTION_FAMILY_16H_INSTALL_H_
