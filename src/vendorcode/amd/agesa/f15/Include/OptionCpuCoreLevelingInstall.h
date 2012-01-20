/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of build option: CPU Core Leveling
 *
 * Contains AMD AGESA install macros and test conditions. Output is the
 * defaults tables reflecting the User's build options selection.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Options
 * @e \$Revision: 56186 $   @e \$Date: 2011-07-08 15:35:23 -0600 (Fri, 08 Jul 2011) $
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

#ifndef _OPTION_CPU_CORELEVELING_INSTALL_H_
#define _OPTION_CPU_CORELEVELING_INSTALL_H_


/*  This option is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition
 */
#define OPTION_CPU_CORE_LEVELING_FEAT
#define F10_REVE_CPU_CORELEVELING_SUPPORT
#define F10_REVD_CPU_CORELEVELING_SUPPORT
#define F10_REVC_CPU_CORELEVELING_SUPPORT
#define F15_OR_CPU_CORELEVELING_SUPPORT
#define F15_TN_CPU_CORELEVELING_SUPPORT

#if OPTION_CPU_CORELEVLING == TRUE
  #if (AGESA_ENTRY_INIT_EARLY == TRUE)
    extern CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureCoreLeveling;

    // Family 10h
    #if OPTION_FAMILY10H == TRUE
      #undef OPTION_CPU_CORE_LEVELING_FEAT
      #define OPTION_CPU_CORE_LEVELING_FEAT &CpuFeatureCoreLeveling,
      #if OPTION_FAMILY10H_HY == TRUE
        extern CONST CPU_CORE_LEVELING_FAMILY_SERVICES ROMDATA F10RevDCoreLeveling;
        #undef F10_REVD_CPU_CORELEVELING_SUPPORT
        #define F10_REVD_CPU_CORELEVELING_SUPPORT {AMD_FAMILY_10_HY, &F10RevDCoreLeveling},
      #endif

      #if (OPTION_FAMILY10H_RB == TRUE) || (OPTION_FAMILY10H_BL == TRUE) || (OPTION_FAMILY10H_DA == TRUE)
        extern CONST CPU_CORE_LEVELING_FAMILY_SERVICES ROMDATA F10RevCCoreLeveling;
        #undef F10_REVC_CPU_CORELEVELING_SUPPORT
        #define F10_REVC_CPU_CORELEVELING_SUPPORT {AMD_FAMILY_10_RB | AMD_FAMILY_10_BL | AMD_FAMILY_10_DA, &F10RevCCoreLeveling},
      #endif

      #if (OPTION_FAMILY10H_PH == TRUE)
        extern CONST CPU_CORE_LEVELING_FAMILY_SERVICES ROMDATA F10RevECoreLeveling;
        #undef F10_REVE_CPU_CORELEVELING_SUPPORT
        #define F10_REVE_CPU_CORELEVELING_SUPPORT {AMD_FAMILY_10_PH, &F10RevECoreLeveling},
      #endif
    #endif
    // Family 15h
    #if OPTION_FAMILY15H == TRUE
      #undef OPTION_CPU_CORE_LEVELING_FEAT
      #define OPTION_CPU_CORE_LEVELING_FEAT &CpuFeatureCoreLeveling,

      #if (OPTION_FAMILY15H_OR == TRUE)
        extern CONST CPU_CORE_LEVELING_FAMILY_SERVICES ROMDATA F15OrCoreLeveling;
        #undef F15_OR_CPU_CORELEVELING_SUPPORT
        #define F15_OR_CPU_CORELEVELING_SUPPORT {AMD_FAMILY_15_OR, &F15OrCoreLeveling},
      #endif
      #if (OPTION_FAMILY15H_TN == TRUE)
        extern CONST CPU_CORE_LEVELING_FAMILY_SERVICES ROMDATA F15TnCoreLeveling;
        #undef F15_TN_CPU_CORELEVELING_SUPPORT
        #define F15_TN_CPU_CORELEVELING_SUPPORT {AMD_FAMILY_15_TN, &F15TnCoreLeveling},
      #endif
    #endif
  #endif
#endif

CONST CPU_SPECIFIC_SERVICES_XLAT ROMDATA CoreLevelingFamilyServiceArray[] =
{
  F15_TN_CPU_CORELEVELING_SUPPORT
  F15_OR_CPU_CORELEVELING_SUPPORT
  F10_REVE_CPU_CORELEVELING_SUPPORT
  F10_REVD_CPU_CORELEVELING_SUPPORT
  F10_REVC_CPU_CORELEVELING_SUPPORT
  {0, NULL}
};
CONST CPU_FAMILY_SUPPORT_TABLE ROMDATA CoreLevelingFamilyServiceTable =
{
  (sizeof (CoreLevelingFamilyServiceArray) / sizeof (CPU_SPECIFIC_SERVICES_XLAT)),
  &CoreLevelingFamilyServiceArray[0]
};

#endif  // _OPTION_CPU_CORELEVELING_INSTALL_H_
