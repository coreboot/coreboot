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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 */
/*****************************************************************************
 *
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
 *
 ***************************************************************************/

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
        #if OPTION_FAMILY15H_OR == TRUE
          extern PROC_FAMILY_TABLE ProcFamily15OrDmiTable;
          #define FAM15_OR_DMI_SUPPORT FAM15_OR_ENABLED,
          #define FAM15_OR_DMI_TABLE &ProcFamily15OrDmiTable,
        #else
          #define FAM15_OR_DMI_SUPPORT
          #define FAM15_OR_DMI_TABLE
        #endif
        #if OPTION_FAMILY15H_TN == TRUE
          extern PROC_FAMILY_TABLE ProcFamily15TnDmiTable;
          #define FAM15_TN_DMI_SUPPORT FAM15_TN_ENABLED,
          #define FAM15_TN_DMI_TABLE &ProcFamily15TnDmiTable,
        #else
          #define FAM15_TN_DMI_SUPPORT
          #define FAM15_TN_DMI_TABLE
        #endif
      #else
        #define FAM15_OR_DMI_SUPPORT
        #define FAM15_OR_DMI_TABLE
        #define FAM15_TN_DMI_SUPPORT
        #define FAM15_TN_DMI_TABLE
      #endif
    #else
      #define FAM15_OR_DMI_SUPPORT
      #define FAM15_OR_DMI_TABLE
      #define FAM15_TN_DMI_SUPPORT
      #define FAM15_TN_DMI_TABLE
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
    #define FAM15_OR_DMI_SUPPORT
    #define FAM15_OR_DMI_TABLE
    #define FAM15_TN_DMI_SUPPORT
    #define FAM15_TN_DMI_TABLE
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
  #define FAM15_OR_DMI_SUPPORT
  #define FAM15_OR_DMI_TABLE
  #define FAM15_TN_DMI_SUPPORT
  #define FAM15_TN_DMI_TABLE
  #define CPU_DMI_AP_GET_TYPE4_TYPE7
#endif

/// DMI supported families enum
typedef enum {
  FAM10_DMI_SUPPORT                   ///< Conditionally define F10 support
  FAM12_DMI_SUPPORT                   ///< Conditionally define F12 support
  FAM14_DMI_SUPPORT                   ///< Conditionally define F14 support
  FAM15_OR_DMI_SUPPORT                ///< Conditionally define F15 OR support
  FAM15_TN_DMI_SUPPORT                ///< Conditionally define F15 TN support
  NUM_DMI_FAMILIES                    ///< Number of installed families
} AGESA_DMI_SUPPORTED_FAM;

/*  Declare the Family List. An array of pointers to tables that each describe a family  */
CONST PROC_FAMILY_TABLE ROMDATA *ProcTables[] = {
  FAM10_DMI_TABLE
  FAM12_DMI_TABLE
  FAM14_DMI_TABLE
  FAM15_OR_DMI_TABLE
  FAM15_TN_DMI_TABLE
  NULL,
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
