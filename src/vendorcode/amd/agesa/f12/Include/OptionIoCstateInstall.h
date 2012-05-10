/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of build option: IO C-state
 *
 * Contains AMD AGESA install macros and test conditions. Output is the
 * defaults tables reflecting the User's build options selection.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Options
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
 */
/*****************************************************************************
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
 ***************************************************************************/

#ifndef _OPTION_IO_CSTATE_INSTALL_H_
#define _OPTION_IO_CSTATE_INSTALL_H_

#include "cpuIoCstate.h"

/*  This option is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition
 */

#define OPTION_IO_CSTATE_FEAT
#define F10_IO_CSTATE_SUPPORT
#define F12_IO_CSTATE_SUPPORT
#define F14_IO_CSTATE_SUPPORT
#define F15_OR_IO_CSTATE_SUPPORT

#if OPTION_IO_CSTATE == TRUE
  #if (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_RESUME == TRUE) || (AGESA_ENTRY_INIT_LATE == TRUE)
    #ifdef OPTION_FAMILY10H
      #if OPTION_FAMILY10H == TRUE
        #if OPTION_FAMILY10H_PH == TRUE
          extern CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureIoCstate;
          #undef OPTION_IO_CSTATE_FEAT
          #define OPTION_IO_CSTATE_FEAT &CpuFeatureIoCstate,
          extern CONST IO_CSTATE_FAMILY_SERVICES ROMDATA F10IoCstateSupport;
          #undef F10_IO_CSTATE_SUPPORT
          #define F10_IO_CSTATE_SUPPORT {AMD_FAMILY_10_PH, &F10IoCstateSupport},
        #endif
      #endif
    #endif

    #ifdef OPTION_FAMILY12H
      #if OPTION_FAMILY12H == TRUE
        #if OPTION_FAMILY12H_LN == TRUE
          extern CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureIoCstate;
          #undef OPTION_IO_CSTATE_FEAT
          #define OPTION_IO_CSTATE_FEAT &CpuFeatureIoCstate,
          extern CONST IO_CSTATE_FAMILY_SERVICES ROMDATA F12IoCstateSupport;
          #undef F12_IO_CSTATE_SUPPORT
          #define F12_IO_CSTATE_SUPPORT {AMD_FAMILY_12_LN, &F12IoCstateSupport},
        #endif
      #endif
    #endif

    #ifdef OPTION_FAMILY14H
      #if OPTION_FAMILY14H == TRUE
        #if OPTION_FAMILY14H_ON == TRUE
          extern CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureIoCstate;
          #undef OPTION_IO_CSTATE_FEAT
          #define OPTION_IO_CSTATE_FEAT &CpuFeatureIoCstate,
          extern CONST IO_CSTATE_FAMILY_SERVICES ROMDATA F14IoCstateSupport;
          #undef F14_IO_CSTATE_SUPPORT
          #define F14_IO_CSTATE_SUPPORT {AMD_FAMILY_14, &F14IoCstateSupport},
        #endif
      #endif
    #endif

    #ifdef OPTION_FAMILY15H
      #if OPTION_FAMILY15H == TRUE
        #if OPTION_FAMILY15H_OR == TRUE
          extern CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureIoCstate;
          #undef OPTION_IO_CSTATE_FEAT
          #define OPTION_IO_CSTATE_FEAT &CpuFeatureIoCstate,
          extern CONST IO_CSTATE_FAMILY_SERVICES ROMDATA F15OrIoCstateSupport;
          #undef F15_OR_IO_CSTATE_SUPPORT
          #define F15_OR_IO_CSTATE_SUPPORT {AMD_FAMILY_15_OR, &F15OrIoCstateSupport},
        #endif
      #endif
    #endif

  #endif
#endif

CONST CPU_SPECIFIC_SERVICES_XLAT ROMDATA IoCstateFamilyServiceArray[] =
{
  F10_IO_CSTATE_SUPPORT
  F12_IO_CSTATE_SUPPORT
  F14_IO_CSTATE_SUPPORT
  F15_OR_IO_CSTATE_SUPPORT
  {0, NULL}
};

CONST CPU_FAMILY_SUPPORT_TABLE ROMDATA IoCstateFamilyServiceTable =
{
  (sizeof (IoCstateFamilyServiceArray) / sizeof (CPU_SPECIFIC_SERVICES_XLAT)),
  &IoCstateFamilyServiceArray[0]
};

#endif // _OPTION_IO_CSTATE_INSTALL_H_
