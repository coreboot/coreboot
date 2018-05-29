/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of build option: Low Power Pstate for PROCHOT_L Throttling.
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
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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

#ifndef _OPTION_LOW_PWR_PSTATE_INSTALL_H_
#define _OPTION_LOW_PWR_PSTATE_INSTALL_H_

#include "cpuLowPwrPstate.h"

/*  This option is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition
 */
#define OPTION_CPU_LOW_PWR_PSTATE_FOR_PROCHOT_FEAT
#define F15_OR_LOW_PWR_PSTATE_SUPPORT

#if OPTION_CPU_LOW_PWR_PSTATE_FOR_PROCHOT == TRUE
  #if (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_LATE == TRUE)
    // Family 15h
    #ifdef OPTION_FAMILY15H
      #if OPTION_FAMILY15H == TRUE
        #if OPTION_FAMILY15H_OR == TRUE
          #if (OPTION_G34_SOCKET_SUPPORT == TRUE) || (OPTION_C32_SOCKET_SUPPORT == TRUE)
            extern CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureLowPwrPstate;
            #undef OPTION_CPU_LOW_PWR_PSTATE_FOR_PROCHOT_FEAT
            #define OPTION_CPU_LOW_PWR_PSTATE_FOR_PROCHOT_FEAT &CpuFeatureLowPwrPstate,
            extern CONST LOW_PWR_PSTATE_FAMILY_SERVICES ROMDATA F15OrLowPwrPstateSupport;
            #undef F15_OR_LOW_PWR_PSTATE_SUPPORT
            #define F15_OR_LOW_PWR_PSTATE_SUPPORT {AMD_FAMILY_15_OR, &F15OrLowPwrPstateSupport},
          #endif
        #endif
      #endif
    #endif
  #endif
#endif

CONST CPU_SPECIFIC_SERVICES_XLAT ROMDATA LowPwrPstateFamilyServiceArray[] =
{
  F15_OR_LOW_PWR_PSTATE_SUPPORT
  {0, NULL}
};

CONST CPU_FAMILY_SUPPORT_TABLE ROMDATA LowPwrPstateFamilyServiceTable =
{
  (sizeof (LowPwrPstateFamilyServiceArray) / sizeof (CPU_SPECIFIC_SERVICES_XLAT)),
  &LowPwrPstateFamilyServiceArray[0]
};

#endif  // _OPTION_LOW_PWR_PSTATE_INSTALL_H_
