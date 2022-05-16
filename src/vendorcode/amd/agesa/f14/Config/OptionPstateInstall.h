/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of build option: PState
 *
 * Contains AMD AGESA install macros and test conditions. Output is the
 * defaults tables reflecting the User's build options selection.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Options
 * @e \$Revision: 34897 $   @e \$Date: 2010-07-14 10:07:10 +0800 (Wed, 14 Jul 2010) $
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

#ifndef _OPTION_PSTATE_INSTALL_H_
#define _OPTION_PSTATE_INSTALL_H_

#include "cpuPstateTables.h"

/*  This option is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition
 */

#define F14_PSTATE_SERVICE_SUPPORT

#if ((AGESA_ENTRY_INIT_LATE == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE))
  //
  //Define Pstate CPU Family service
  //

  #ifdef OPTION_FAMILY14H
    #if OPTION_FAMILY14H == TRUE
      extern CONST PSTATE_CPU_FAMILY_SERVICES ROMDATA F14PstateServices;
      #undef F14_PSTATE_SERVICE_SUPPORT
      #define F14_PSTATE_SERVICE_SUPPORT {AMD_FAMILY_14, &F14PstateServices},
    #endif
  #endif

  //
  //Define ACPI Pstate objects.
  //
  #ifndef OPTION_ACPI_PSTATES
    #error  BLDOPT: Option not defined: "OPTION_ACPI_PSTATES"
  #endif
  #if (OPTION_ACPI_PSTATES == TRUE)
//    OPTION_SSDT_FEATURE               GenerateSsdt;
    #define USER_SSDT_MAIN            GenerateSsdt
    #ifndef OPTION_MULTISOCKET
      #error  BLDOPT: Option not defined: "OPTION_MULTISOCKET"
    #endif

    OPTION_ACPI_FEATURE               CreatePStateAcpiTables;
    OPTION_PSTATE_GATHER              PStateGatherMain;
    #if ((OPTION_MULTISOCKET == TRUE) && (AGESA_ENTRY_INIT_POST == TRUE))
      OPTION_PSTATE_LEVELING          PStateLevelingMain;
      #define USER_PSTATE_OPTION_LEVEL  PStateLevelingMain
    #else
      OPTION_PSTATE_LEVELING            PStateLevelingStub;
      #define USER_PSTATE_OPTION_LEVEL  PStateLevelingStub
    #endif
    #if AGESA_ENTRY_INIT_LATE == TRUE
      #define USER_PSTATE_OPTION_MAIN   CreatePStateAcpiTables
    #else
      OPTION_ACPI_FEATURE               CreateAcpiTablesStub;
      #define USER_PSTATE_OPTION_MAIN   CreateAcpiTablesStub
    #endif
    #if AGESA_ENTRY_INIT_POST == TRUE
      #define USER_PSTATE_OPTION_GATHER PStateGatherMain
    #else
      OPTION_PSTATE_GATHER              PStateGatherStub;
      #define USER_PSTATE_OPTION_GATHER PStateGatherStub
    #endif
    #if CFG_ACPI_PSTATES_PPC == TRUE
      #define USER_PSTATE_CFG_PPC   TRUE
    #else
      #define USER_PSTATE_CFG_PPC   FALSE
    #endif
    #if CFG_ACPI_PSTATES_PCT == TRUE
      #define USER_PSTATE_CFG_PCT   TRUE
    #else
      #define USER_PSTATE_CFG_PCT   FALSE
    #endif
    #if CFG_ACPI_PSTATES_PSD == TRUE
      #define USER_PSTATE_CFG_PSD   TRUE
    #else
      #define USER_PSTATE_CFG_PSD   FALSE
    #endif
    #if CFG_ACPI_PSTATES_PSS == TRUE
      #define USER_PSTATE_CFG_PSS   TRUE
    #else
      #define USER_PSTATE_CFG_PSS   FALSE
    #endif
    #if CFG_ACPI_PSTATES_XPSS == TRUE
      #define USER_PSTATE_CFG_XPSS   TRUE
    #else
      #define USER_PSTATE_CFG_XPSS   FALSE
    #endif

    #if OPTION_IO_CSTATE == TRUE
      OPTION_ACPI_FEATURE             CreateCStateAcpiTables;
      #define USER_CSTATE_OPTION_MAIN CreateCStateAcpiTables
    #else
      OPTION_ACPI_FEATURE             CreateAcpiTablesStub;
      #define USER_CSTATE_OPTION_MAIN CreateAcpiTablesStub
    #endif
  #else
    OPTION_SSDT_FEATURE               GenerateSsdtStub;
    OPTION_ACPI_FEATURE               CreateAcpiTablesStub;
    OPTION_PSTATE_GATHER              PStateGatherStub;
    OPTION_PSTATE_LEVELING            PStateLevelingStub;
    #define USER_SSDT_MAIN            GenerateSsdtStub
    #define USER_PSTATE_OPTION_MAIN   CreateAcpiTablesStub
    #define USER_CSTATE_OPTION_MAIN   CreateAcpiTablesStub
    #define USER_PSTATE_OPTION_GATHER PStateGatherStub
    #define USER_PSTATE_OPTION_LEVEL  PStateLevelingStub
    #define USER_PSTATE_CFG_PPC    FALSE
    #define USER_PSTATE_CFG_PCT    FALSE
    #define USER_PSTATE_CFG_PSD    FALSE
    #define USER_PSTATE_CFG_PSS    FALSE
    #define USER_PSTATE_CFG_XPSS   FALSE

    // If ACPI Objects are disabled for PStates, we still need to check
    // whether ACPI Objects are enabled for CStates
    #if OPTION_IO_CSTATE == TRUE
      OPTION_SSDT_FEATURE               GenerateSsdt;
      OPTION_PSTATE_GATHER              PStateGatherMain;
      OPTION_ACPI_FEATURE               CreateCStateAcpiTables;
      #undef  USER_SSDT_MAIN
      #define USER_SSDT_MAIN            GenerateSsdt
      #undef  USER_PSTATE_OPTION_GATHER
      #define USER_PSTATE_OPTION_GATHER PStateGatherMain
      #undef  USER_CSTATE_OPTION_MAIN
      #define USER_CSTATE_OPTION_MAIN   CreateCStateAcpiTables
    #endif
  #endif
#else
  OPTION_SSDT_FEATURE               GenerateSsdtStub;
  OPTION_ACPI_FEATURE               CreateAcpiTablesStub;
  OPTION_PSTATE_GATHER              PStateGatherStub;
  OPTION_PSTATE_LEVELING            PStateLevelingStub;
  #define USER_SSDT_MAIN            GenerateSsdtStub
  #define USER_PSTATE_OPTION_MAIN   CreateAcpiTablesStub
  #define USER_CSTATE_OPTION_MAIN   CreateAcpiTablesStub
  #define USER_PSTATE_OPTION_GATHER PStateGatherStub
  #define USER_PSTATE_OPTION_LEVEL  PStateLevelingStub
  #define USER_PSTATE_CFG_PPC    FALSE
  #define USER_PSTATE_CFG_PCT    FALSE
  #define USER_PSTATE_CFG_PSD    FALSE
  #define USER_PSTATE_CFG_PSS    FALSE
  #define USER_PSTATE_CFG_XPSS   FALSE
#endif

/*  Declare the instance of the PSTATE option configuration structure  */
CONST OPTION_PSTATE_POST_CONFIGURATION      OptionPstatePostConfiguration = {
  PSTATE_STRUCT_VERSION,
  USER_PSTATE_OPTION_GATHER,
  USER_PSTATE_OPTION_LEVEL
};

CONST OPTION_PSTATE_LATE_CONFIGURATION      OptionPstateLateConfiguration = {
  PSTATE_STRUCT_VERSION,
  USER_SSDT_MAIN,
  USER_PSTATE_OPTION_MAIN,
  USER_CSTATE_OPTION_MAIN,
  USER_PSTATE_CFG_PPC,
  USER_PSTATE_CFG_PCT,
  USER_PSTATE_CFG_PSD,
  USER_PSTATE_CFG_PSS,
  USER_PSTATE_CFG_XPSS
};

CONST CPU_SPECIFIC_SERVICES_XLAT ROMDATA PstateCpuFamilyServiceArray[] =
{
  F14_PSTATE_SERVICE_SUPPORT
  {0, NULL}
};
CONST CPU_FAMILY_SUPPORT_TABLE ROMDATA PstateFamilyServiceTable =
{
  (sizeof (PstateCpuFamilyServiceArray) / sizeof (CPU_SPECIFIC_SERVICES_XLAT)),
  &PstateCpuFamilyServiceArray[0]
};
#endif  // _OPTION_PSTATE_INSTALL_H_
