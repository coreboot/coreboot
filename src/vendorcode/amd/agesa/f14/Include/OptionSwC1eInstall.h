/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of build option: SW C1e
 *
 * Contains AMD AGESA install macros and test conditions. Output is the
 * defaults tables reflecting the User's build options selection.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Options
 * @e \$Revision: 37157 $   @e \$Date: 2010-09-01 03:24:07 +0800 (Wed, 01 Sep 2010) $
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

#ifndef _OPTION_SW_C1E_INSTALL_H_
#define _OPTION_SW_C1E_INSTALL_H_

#include "cpuSwC1e.h"

/*  This option is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition
 */
#define OPTION_SW_C1E_FEAT
#define F10_SW_C1E_SUPPORT
#if AGESA_ENTRY_INIT_EARLY == TRUE
  #ifdef OPTION_FAMILY10H
    #if OPTION_FAMILY10H == TRUE
      #if (OPTION_FAMILY10H_BL == TRUE) || (OPTION_FAMILY10H_DA == TRUE) || (OPTION_FAMILY10H_RB == TRUE) || (OPTION_FAMILY10H_PH == TRUE)
        extern CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureSwC1e;
        #undef OPTION_SW_C1E_FEAT
        #define OPTION_SW_C1E_FEAT &CpuFeatureSwC1e,
        extern CONST SW_C1E_FAMILY_SERVICES ROMDATA F10SwC1e;
        #undef F10_SW_C1E_SUPPORT
        #define F10_SW_C1E_SUPPORT {(AMD_FAMILY_10_BL | AMD_FAMILY_10_DA | AMD_FAMILY_10_RB | AMD_FAMILY_10_PH), &F10SwC1e},
      #endif
    #endif
  #endif
  CONST CPU_SPECIFIC_SERVICES_XLAT ROMDATA SwC1eFamilyServiceArray[] =
  {
    F10_SW_C1E_SUPPORT
    {0, NULL}
  };
  CONST CPU_FAMILY_SUPPORT_TABLE ROMDATA SwC1eFamilyServiceTable =
  {
    (sizeof (SwC1eFamilyServiceArray) / sizeof (CPU_SPECIFIC_SERVICES_XLAT)),
    &SwC1eFamilyServiceArray[0]
  };
#endif

#endif  // _OPTION_SW_C1E_INSTALL_H_
