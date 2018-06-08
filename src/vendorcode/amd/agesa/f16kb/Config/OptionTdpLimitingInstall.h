/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of build option: TDP Limiting.
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

#ifndef _OPTION_TDP_LIMITING_INSTALL_H_
#define _OPTION_TDP_LIMITING_INSTALL_H_

#include "cpuTdpLimiting.h"

/*  This option is designed to be included into the CPU features install
 *  file. The CPU features install file will define the options status.
 *  Check to validate the definition
 */
#define OPTION_TDP_LIMIT_FEAT
#define F15_TDP_LIMIT_SUPPORT

#if OPTION_CPU_TDP_LIMITING == TRUE
  #if (AGESA_ENTRY_INIT_EARLY == TRUE)
    // Family 15h
    #ifdef OPTION_FAMILY15H
      #if OPTION_FAMILY15H == TRUE
        extern CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureTdpLimit;
        #undef OPTION_TDP_LIMIT_FEAT
        #define OPTION_TDP_LIMIT_FEAT &CpuFeatureTdpLimit,
        extern CONST TDP_LIMIT_FAMILY_SERVICES ROMDATA F15TdpLimitSupport;
        #undef F15_TDP_LIMIT_SUPPORT
        #define F15_TDP_LIMIT_SUPPORT {AMD_FAMILY_15, &F15TdpLimitSupport},
      #endif
    #endif
  #endif
#endif

CONST CPU_SPECIFIC_SERVICES_XLAT ROMDATA TdpLimitFamilyServiceArray[] =
{
  F15_TDP_LIMIT_SUPPORT
  {0, NULL}
};

CONST CPU_FAMILY_SUPPORT_TABLE ROMDATA TdpLimitFamilyServiceTable =
{
  (sizeof (TdpLimitFamilyServiceArray) / sizeof (CPU_SPECIFIC_SERVICES_XLAT)),
  &TdpLimitFamilyServiceArray[0]
};

#endif  // _OPTION_TDP_LIMITING_INSTALL_H_
