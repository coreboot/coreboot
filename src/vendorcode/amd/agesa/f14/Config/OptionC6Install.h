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
 * @e \$Revision: 37456 $   @e \$Date: 2010-09-04 04:17:05 +0800 (Sat, 04 Sep 2010) $
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

#ifndef _OPTION_C6_STATE_INSTALL_H_
#define _OPTION_C6_STATE_INSTALL_H_

#include "cpuC6State.h"

/*  This option is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition
 */
#define OPTION_C6_STATE_FEAT
#define F14_C6_STATE_SUPPORT

#if OPTION_C6_STATE == TRUE
  #if (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_LATE == TRUE)

    #ifdef OPTION_FAMILY14H
      #if OPTION_FAMILY14H == TRUE
        #if OPTION_FAMILY14H_ON == TRUE
          extern CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureC6State;
          #undef OPTION_C6_STATE_FEAT
          #define OPTION_C6_STATE_FEAT &CpuFeatureC6State,
          extern CONST C6_FAMILY_SERVICES ROMDATA F14C6Support;
          #undef F14_C6_STATE_SUPPORT
          #define F14_C6_STATE_SUPPORT {AMD_FAMILY_14_ON, &F14C6Support},

          #if OPTION_EARLY_SAMPLES == TRUE
            extern F_F14_ES_IS_C6_SUPPORTED F14IsC6DisabledEarlySample;
            extern F_F14_ES_C6_INIT F14C6A0Workaround;

            CONST F14_ES_C6_SUPPORT ROMDATA F14EarlySampleC6Support =
            {
              F14IsC6DisabledEarlySample,
              F14C6A0Workaround
            };
          #else
            CONST F14_ES_C6_SUPPORT ROMDATA F14EarlySampleC6Support =
            {
              (PF_F14_ES_IS_C6_SUPPORTED) CommonVoid,
              (PF_F14_ES_C6_INIT) CommonVoid
            };
          #endif

        #endif
      #endif
    #endif

  #endif
#endif

CONST CPU_SPECIFIC_SERVICES_XLAT ROMDATA C6FamilyServiceArray[] =
{
  F14_C6_STATE_SUPPORT
  {0, NULL}
};

CONST CPU_FAMILY_SUPPORT_TABLE ROMDATA C6FamilyServiceTable =
{
  (sizeof (C6FamilyServiceArray) / sizeof (CPU_SPECIFIC_SERVICES_XLAT)),
  &C6FamilyServiceArray[0]
};

#endif  // _OPTION_C6_STATE_INSTALL_H_
