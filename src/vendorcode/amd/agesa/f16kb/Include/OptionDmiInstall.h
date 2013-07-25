/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of build option: DMI
 *
 * Contains AMD AGESA install macros and test conditions. Output is the
 * defaults tables reflecting the User's build options selection.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Options
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
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

#ifndef _OPTION_DMI_INSTALL_H_
#define _OPTION_DMI_INSTALL_H_

#include "cpuLateInit.h"

/*  This option is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition
 */
OPTION_DMI_FEATURE                  GetDmiInfoStub;
OPTION_DMI_RELEASE_BUFFER           ReleaseDmiBufferStub;
#define USER_DMI_OPTION             GetDmiInfoStub
#define USER_DMI_RELEASE_BUFFER     ReleaseDmiBufferStub
#define CPU_DMI_AP_GET_TYPE4_TYPE7

#define  FAM15_TN_DMI_TABLE
#define  FAM16_KB_DMI_TABLE

#ifndef OPTION_DMI
  #error  BLDOPT: Option not defined: "OPTION_DMI"
#endif

#if OPTION_DMI == TRUE
  #if AGESA_ENTRY_INIT_LATE == TRUE
    OPTION_DMI_FEATURE          GetDmiInfoMain;
    OPTION_DMI_RELEASE_BUFFER   ReleaseDmiBuffer;
    #undef USER_DMI_OPTION
    #define USER_DMI_OPTION     &GetDmiInfoMain
    #undef USER_DMI_RELEASE_BUFFER
    #define USER_DMI_RELEASE_BUFFER &ReleaseDmiBuffer

    // This additional check keeps AP launch routines from being unnecessarily included
    // in single socket systems.
    #if OPTION_MULTISOCKET == TRUE
      #undef AGESA_ENTRY_LATE_RUN_AP_TASK
      #define AGESA_ENTRY_LATE_RUN_AP_TASK TRUE
      #undef CPU_DMI_AP_GET_TYPE4_TYPE7
      #define CPU_DMI_AP_GET_TYPE4_TYPE7 {AP_LATE_TASK_GET_TYPE4_TYPE7, (IMAGE_ENTRY) GetType4Type7Info},
    #endif

    // Family 15
    #ifdef OPTION_FAMILY15H
      #if OPTION_FAMILY15H == TRUE
        #if OPTION_FAMILY15H_TN == TRUE
          extern PROC_FAMILY_TABLE ProcFamily15TnDmiTable;
          #undef FAM15_TN_DMI_TABLE
          #define FAM15_TN_DMI_TABLE &ProcFamily15TnDmiTable,
        #endif
      #endif
    #endif

    // Family 16
    #ifdef OPTION_FAMILY16H
      #if OPTION_FAMILY16H == TRUE
        #if OPTION_FAMILY16H_KB
          extern PROC_FAMILY_TABLE ProcFamily16KbDmiTable;
          #undef FAM16_KB_DMI_TABLE
          #define FAM16_KB_DMI_TABLE &ProcFamily16KbDmiTable,
        #endif
      #endif
    #endif
  #endif
#endif

/*  Declare the Family List. An array of pointers to tables that each describe a family  */
CONST PROC_FAMILY_TABLE ROMDATA *ProcTables[] = {
  FAM15_TN_DMI_TABLE
  FAM16_KB_DMI_TABLE
  NULL
};

/*  Declare the instance of the DMI option configuration structure  */
CONST OPTION_DMI_CONFIGURATION ROMDATA OptionDmiConfiguration = {
  DMI_STRUCT_VERSION,
  USER_DMI_OPTION,
  USER_DMI_RELEASE_BUFFER,
  ((sizeof (ProcTables) / sizeof (PROC_FAMILY_TABLE *)) - 1),  // Including 'NULL' in above ProcTables would
                                        // cause one more entry is counted.
  (VOID *((*)[])) &ProcTables           // Compiler says array size must match struct decl
};

#endif  // _OPTION_DMI_INSTALL_H_
