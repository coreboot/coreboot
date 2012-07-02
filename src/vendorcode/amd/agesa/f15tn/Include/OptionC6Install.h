/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of build option: C6 C-state
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

#ifndef _OPTION_C6_STATE_INSTALL_H_
#define _OPTION_C6_STATE_INSTALL_H_

#include "cpuC6State.h"

/*  This option is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition
 */
#define OPTION_C6_STATE_FEAT
#define F12_C6_STATE_SUPPORT
#define F14_ON_C6_STATE_SUPPORT
#define F15_OR_C6_STATE_SUPPORT
#define F15_TN_C6_STATE_SUPPORT

#if OPTION_C6_STATE == TRUE
  #if (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_LATE == TRUE)
    #ifdef OPTION_FAMILY12H
      #if OPTION_FAMILY12H == TRUE
        #if OPTION_FAMILY12H_LN == TRUE
          extern CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureC6State;
          #undef OPTION_C6_STATE_FEAT
          #define OPTION_C6_STATE_FEAT &CpuFeatureC6State,
          extern CONST C6_FAMILY_SERVICES ROMDATA F12C6Support;
          #undef F12_C6_STATE_SUPPORT
          #define F12_C6_STATE_SUPPORT {AMD_FAMILY_12_LN, &F12C6Support},

          #if OPTION_EARLY_SAMPLES == TRUE
            extern F_F12_ES_C6_INIT F12C6A0Workaround;

            CONST F12_ES_C6_SUPPORT ROMDATA F12EarlySampleC6Support =
            {
              F12C6A0Workaround
            };
          #else
            CONST F12_ES_C6_SUPPORT ROMDATA F12EarlySampleC6Support =
            {
              (PF_F12_ES_C6_INIT) CommonVoid
            };
          #endif

        #endif
      #endif
    #endif

    #ifdef OPTION_FAMILY14H
      #if OPTION_FAMILY14H == TRUE
        #if (OPTION_FAMILY14H_ON == TRUE)
          extern CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureC6State;
          #undef OPTION_C6_STATE_FEAT
          #define OPTION_C6_STATE_FEAT &CpuFeatureC6State,
          extern CONST C6_FAMILY_SERVICES ROMDATA F14OnC6Support;
          #undef F14_ON_C6_STATE_SUPPORT
          #define F14_ON_C6_STATE_SUPPORT {AMD_FAMILY_14_ON, &F14OnC6Support},

          #if (OPTION_EARLY_SAMPLES == TRUE)
            extern F_F14_ON_ES_IS_C6_SUPPORTED F14IsC6DisabledEarlySample;
            extern F_F14_ON_ES_C6_INIT F14C6A0Workaround;

            CONST F14_ON_ES_C6_SUPPORT ROMDATA F14OnEarlySampleC6Support =
            {
              F14IsC6DisabledEarlySample,
              F14C6A0Workaround
            };
          #else
            CONST F14_ON_ES_C6_SUPPORT ROMDATA F14OnEarlySampleC6Support =
            {
              (PF_F14_ON_ES_IS_C6_SUPPORTED) CommonVoid,
              (PF_F14_ON_ES_C6_INIT) CommonVoid
            };
          #endif
        #endif


      #endif
    #endif

    #ifdef OPTION_FAMILY15H
      #if OPTION_FAMILY15H == TRUE
        #if (OPTION_FAMILY15H_OR == TRUE)
          extern CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureC6State;
          #undef OPTION_C6_STATE_FEAT
          #define OPTION_C6_STATE_FEAT &CpuFeatureC6State,
          extern CONST C6_FAMILY_SERVICES ROMDATA F15OrC6Support;
          #undef F15_OR_C6_STATE_SUPPORT
          #define F15_OR_C6_STATE_SUPPORT {AMD_FAMILY_15_OR, &F15OrC6Support},

          #if (OPTION_EARLY_SAMPLES == TRUE)
            extern F_F15_OR_ES_IS_C6_SUPPORTED F15OrIsC6DisabledEarlySample;

            CONST F15_OR_ES_C6_SUPPORT ROMDATA F15OrEarlySampleC6Support =
            {
              F15OrIsC6DisabledEarlySample
            };
          #else
            CONST F15_OR_ES_C6_SUPPORT ROMDATA F15OrEarlySampleC6Support =
            {
              (PF_F15_OR_ES_IS_C6_SUPPORTED) CommonVoid
            };
          #endif
        #endif

        #if (OPTION_FAMILY15H_TN == TRUE)
          extern CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureC6State;
          #undef OPTION_C6_STATE_FEAT
          #define OPTION_C6_STATE_FEAT &CpuFeatureC6State,
          extern CONST C6_FAMILY_SERVICES ROMDATA F15TnC6Support;
          #undef F15_TN_C6_STATE_SUPPORT
          #define F15_TN_C6_STATE_SUPPORT {AMD_FAMILY_15_TN, &F15TnC6Support},
        #endif


      #endif
    #endif
  #endif
#endif

CONST CPU_SPECIFIC_SERVICES_XLAT ROMDATA C6FamilyServiceArray[] =
{
  F12_C6_STATE_SUPPORT
  F14_ON_C6_STATE_SUPPORT
  {0, NULL},
  F15_OR_C6_STATE_SUPPORT
  F15_TN_C6_STATE_SUPPORT
  {0, NULL},
  {0, NULL}
};

CONST CPU_FAMILY_SUPPORT_TABLE ROMDATA C6FamilyServiceTable =
{
  (sizeof (C6FamilyServiceArray) / sizeof (CPU_SPECIFIC_SERVICES_XLAT)),
  &C6FamilyServiceArray[0]
};

#endif  // _OPTION_C6_STATE_INSTALL_H_
