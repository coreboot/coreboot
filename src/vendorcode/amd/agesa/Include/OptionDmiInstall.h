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

#ifndef _OPTION_DMI_INSTALL_H_
#define _OPTION_DMI_INSTALL_H_

#include "cpuLateInit.h"

/*  This option is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition
 */
#if AGESA_ENTRY_INIT_LATE == TRUE
  #ifndef OPTION_DMI
    #error  BLDOPT: Option not defined: "OPTION_DMI"
  #endif
  #if OPTION_DMI == TRUE
    OPTION_DMI_FEATURE          GetDmiInfoMain;
    OPTION_DMI_RELEASE_BUFFER   ReleaseDmiBuffer;
    #define USER_DMI_OPTION     &GetDmiInfoMain
    #define USER_DMI_RELEASE_BUFFER &ReleaseDmiBuffer

    // This additional check keeps AP launch routines from being unnecessarily included
    // in single socket systems.
    #if OPTION_MULTISOCKET == TRUE
      #undef AGESA_ENTRY_LATE_RUN_AP_TASK
      #define AGESA_ENTRY_LATE_RUN_AP_TASK TRUE
      #define CPU_DMI_AP_GET_TYPE4_TYPE7 {AP_LATE_TASK_GET_TYPE4_TYPE7, (IMAGE_ENTRY) GetType4Type7Info},
    #else
      #define CPU_DMI_AP_GET_TYPE4_TYPE7
    #endif

    // Family 10
    #ifdef OPTION_FAMILY10H
      #if OPTION_FAMILY10H == TRUE
        extern PROC_FAMILY_TABLE ProcFamily10DmiTable;
        #define FAM10_DMI_SUPPORT FAM10_ENABLED,
        #define FAM10_DMI_TABLE &ProcFamily10DmiTable,
      #else
        #define FAM10_DMI_SUPPORT
        #define FAM10_DMI_TABLE
      #endif
    #else
      #define FAM10_DMI_SUPPORT
      #define FAM10_DMI_TABLE
    #endif

    // Family 12
    #ifdef OPTION_FAMILY12H
      #if OPTION_FAMILY12H == TRUE
        extern PROC_FAMILY_TABLE ProcFamily12DmiTable;
        #define FAM12_DMI_SUPPORT FAM12_ENABLED,
        #define FAM12_DMI_TABLE &ProcFamily12DmiTable,
      #else
        #define FAM12_DMI_SUPPORT
        #define FAM12_DMI_TABLE
      #endif
    #else
      #define FAM12_DMI_SUPPORT
      #define FAM12_DMI_TABLE
    #endif

    // Family 14
    #ifdef OPTION_FAMILY14H
      #if OPTION_FAMILY14H == TRUE
        extern PROC_FAMILY_TABLE ProcFamily14DmiTable;
        #define FAM14_DMI_SUPPORT FAM14_ENABLED,
        #define FAM14_DMI_TABLE &ProcFamily14DmiTable,
      #else
        #define FAM14_DMI_SUPPORT
        #define FAM14_DMI_TABLE
      #endif
    #else
      #define FAM14_DMI_SUPPORT
      #define FAM14_DMI_TABLE
    #endif

    // Family 15
    #ifdef OPTION_FAMILY15H
      #if OPTION_FAMILY15H == TRUE
        extern PROC_FAMILY_TABLE ProcFamily15DmiTable;
        #define FAM15_DMI_SUPPORT FAM15_ENABLED,
        #define FAM15_DMI_TABLE &ProcFamily15DmiTable,
      #else
        #define FAM15_DMI_SUPPORT
        #define FAM15_DMI_TABLE
      #endif
    #else
      #define FAM15_DMI_SUPPORT
      #define FAM15_DMI_TABLE
    #endif

  #else
    OPTION_DMI_FEATURE          GetDmiInfoStub;
    OPTION_DMI_RELEASE_BUFFER   ReleaseDmiBufferStub;
    #define USER_DMI_OPTION     GetDmiInfoStub
    #define USER_DMI_RELEASE_BUFFER ReleaseDmiBufferStub
    #define FAM10_DMI_SUPPORT
    #define FAM10_DMI_TABLE
    #define FAM12_DMI_SUPPORT
    #define FAM12_DMI_TABLE
    #define FAM14_DMI_SUPPORT
    #define FAM14_DMI_TABLE
    #define FAM15_DMI_SUPPORT
    #define FAM15_DMI_TABLE
    #define CPU_DMI_AP_GET_TYPE4_TYPE7
  #endif
#else
  OPTION_DMI_FEATURE          GetDmiInfoStub;
  OPTION_DMI_RELEASE_BUFFER   ReleaseDmiBufferStub;
  #define USER_DMI_OPTION     GetDmiInfoStub
  #define USER_DMI_RELEASE_BUFFER ReleaseDmiBufferStub
  #define FAM10_DMI_SUPPORT
  #define FAM10_DMI_TABLE
  #define FAM12_DMI_SUPPORT
  #define FAM12_DMI_TABLE
  #define FAM14_DMI_SUPPORT
  #define FAM14_DMI_TABLE
  #define FAM15_DMI_SUPPORT
  #define FAM15_DMI_TABLE
  #define CPU_DMI_AP_GET_TYPE4_TYPE7
#endif

/// DMI supported families enum
typedef enum {
  FAM10_DMI_SUPPORT                   ///< Conditionally define F10 support
  FAM12_DMI_SUPPORT                   ///< Conditionally define F12 support
  FAM14_DMI_SUPPORT                   ///< Conditionally define F14 support
  FAM15_DMI_SUPPORT                   ///< Conditionally define F15 support
  NUM_DMI_FAMILIES                    ///< Number of installed families
} AGESA_DMI_SUPPORTED_FAM;

/*  Declare the Family List. An array of pointers to tables that each describe a family  */
CONST PROC_FAMILY_TABLE ROMDATA *ProcTables[] = {
  FAM10_DMI_TABLE
  FAM12_DMI_TABLE
  FAM14_DMI_TABLE
  FAM15_DMI_TABLE
  NULL
};

/*  Declare the instance of the DMI option configuration structure  */
CONST OPTION_DMI_CONFIGURATION ROMDATA OptionDmiConfiguration = {
  DMI_STRUCT_VERSION,
  USER_DMI_OPTION,
  USER_DMI_RELEASE_BUFFER,
  NUM_DMI_FAMILIES,
  (VOID *((*)[])) &ProcTables           // Compiler says array size must match struct decl
};

#endif  // _OPTION_DMI_INSTALL_H_
