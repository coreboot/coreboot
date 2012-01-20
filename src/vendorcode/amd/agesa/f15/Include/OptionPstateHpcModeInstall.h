/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of build option: Pstate HPC mode.
 *
 * Contains AMD AGESA install macros and test conditions. Output is the
 * defaults tables reflecting the User's build options selection.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Options
 * @e \$Revision: 52150 $   @e \$Date: 2011-05-03 01:01:08 -0600 (Tue, 03 May 2011) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *
 ***************************************************************************/

#ifndef _OPTION_PSTATE_HPC_MODE_INSTALL_H_
#define _OPTION_PSTATE_HPC_MODE_INSTALL_H_

#include "cpuPstateHpcMode.h"

/*  This option is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition
 */
#define OPTION_CPU_PSTATE_HPC_MODE_FEAT
#define F15_PSTATE_HPC_MODE_SUPPORT

#if OPTION_CPU_PSTATE_HPC_MODE == TRUE
  #if (AGESA_ENTRY_INIT_POST == TRUE)
    // Family 15h
    #ifdef OPTION_FAMILY15H
      #if OPTION_FAMILY15H == TRUE
        // Orochi and Komodo
        #if (OPTION_FAMILY15H_OR == TRUE) || (OPTION_FAMILY15H_KM == TRUE)
          extern CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeaturePstateHpcMode;
          #undef OPTION_CPU_PSTATE_HPC_MODE_FEAT
          #define OPTION_CPU_PSTATE_HPC_MODE_FEAT &CpuFeaturePstateHpcMode,
          extern CONST PSTATE_HPC_MODE_FAMILY_SERVICES ROMDATA F15PstateHpcSupport;
          #undef F15_PSTATE_HPC_MODE_SUPPORT
          #define F15_PSTATE_HPC_MODE_SUPPORT {(AMD_FAMILY_15_OR | AMD_FAMILY_15_KM), &F15PstateHpcSupport},
        #endif
      #endif
    #endif
  #endif
#endif

CONST CPU_SPECIFIC_SERVICES_XLAT ROMDATA PstateHpcModeFamilyServiceArray[] =
{
  F15_PSTATE_HPC_MODE_SUPPORT
  {0, NULL}
};

CONST CPU_FAMILY_SUPPORT_TABLE ROMDATA PstateHpcModeFamilyServiceTable =
{
  (sizeof (PstateHpcModeFamilyServiceArray) / sizeof (CPU_SPECIFIC_SERVICES_XLAT)),
  &PstateHpcModeFamilyServiceArray[0]
};

#endif  // _OPTION_PSTATE_HPC_MODE_INSTALL_H_
