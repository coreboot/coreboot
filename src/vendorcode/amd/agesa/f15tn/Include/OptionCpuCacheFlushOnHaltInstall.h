/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of build option: CPU Cache Flush On Halt
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

#ifndef _OPTION_CPU_CACHEFLUSHONHALT_INSTALL_H_
#define _OPTION_CPU_CACHEFLUSHONHALT_INSTALL_H_

#include "cpuPostInit.h"

/*  This option is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition
 */
#define OPTION_CPU_CACHE_FLUSH_ON_HALT_FEAT
#define F10_BL_CPU_CFOH_SUPPORT
#define F10_DA_CPU_CFOH_SUPPORT
#define F10_CPU_CFOH_SUPPORT
#define F15_OR_CPU_CFOH_SUPPORT
#define F15_TN_CPU_CFOH_SUPPORT

#if OPTION_CPU_CFOH == TRUE
  #if (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_RESUME == TRUE)
    #ifdef OPTION_FAMILY10H
      #if OPTION_FAMILY10H == TRUE
        extern CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureCacheFlushOnHalt;
        #undef OPTION_CPU_CACHE_FLUSH_ON_HALT_FEAT
        #define OPTION_CPU_CACHE_FLUSH_ON_HALT_FEAT &CpuFeatureCacheFlushOnHalt,

        #if OPTION_FAMILY10H_BL == TRUE
          extern CONST CPU_CFOH_FAMILY_SERVICES ROMDATA F10BlCacheFlushOnHalt;
          #undef F10_BL_CPU_CFOH_SUPPORT
          #define F10_BL_CPU_CFOH_SUPPORT {AMD_FAMILY_10_BL, &F10BlCacheFlushOnHalt},
        #endif

        #if OPTION_FAMILY10H_DA == TRUE
          extern CONST CPU_CFOH_FAMILY_SERVICES ROMDATA F10DaCacheFlushOnHalt;
          #undef F10_DA_CPU_CFOH_SUPPORT
          #define F10_DA_CPU_CFOH_SUPPORT {AMD_FAMILY_10_DA, &F10DaCacheFlushOnHalt},
        #endif

        #if (OPTION_FAMILY10H_RB == TRUE) || (OPTION_FAMILY10H_HY == TRUE) || (OPTION_FAMILY10H_PH == TRUE)
          extern CONST CPU_CFOH_FAMILY_SERVICES ROMDATA F10CacheFlushOnHalt;
          #undef F10_CPU_CFOH_SUPPORT
          #define F10_CPU_CFOH_SUPPORT {AMD_FAMILY_10_RB | AMD_FAMILY_10_HY | AMD_FAMILY_10_PH, &F10CacheFlushOnHalt},
        #endif
      #endif
    #endif

    #ifdef OPTION_FAMILY15H
      #if OPTION_FAMILY15H == TRUE
        extern CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureCacheFlushOnHalt;
        #undef OPTION_CPU_CACHE_FLUSH_ON_HALT_FEAT
        #define OPTION_CPU_CACHE_FLUSH_ON_HALT_FEAT &CpuFeatureCacheFlushOnHalt,

        #if OPTION_FAMILY15H_OR == TRUE
          extern CONST CPU_CFOH_FAMILY_SERVICES ROMDATA F15OrCacheFlushOnHalt;
          #undef F15_OR_CPU_CFOH_SUPPORT
          #define F15_OR_CPU_CFOH_SUPPORT {AMD_FAMILY_15_OR, &F15OrCacheFlushOnHalt},
        #endif

        #if OPTION_FAMILY15H_TN == TRUE
          extern CONST CPU_CFOH_FAMILY_SERVICES ROMDATA F15TnCacheFlushOnHalt;
          #undef F15_TN_CPU_CFOH_SUPPORT
          #define F15_TN_CPU_CFOH_SUPPORT {AMD_FAMILY_15_TN, &F15TnCacheFlushOnHalt},
        #endif

      #endif
    #endif
  #endif
#endif

CONST CPU_SPECIFIC_SERVICES_XLAT ROMDATA CacheFlushOnHaltFamilyServiceArray[] =
{
  F10_BL_CPU_CFOH_SUPPORT
  F10_DA_CPU_CFOH_SUPPORT
  F10_CPU_CFOH_SUPPORT
  F15_OR_CPU_CFOH_SUPPORT
  F15_TN_CPU_CFOH_SUPPORT
  {0, NULL},
  {0, NULL}
};
CONST CPU_FAMILY_SUPPORT_TABLE ROMDATA CacheFlushOnHaltFamilyServiceTable =
{
  (sizeof (CacheFlushOnHaltFamilyServiceArray) / sizeof (CPU_SPECIFIC_SERVICES_XLAT)),
  &CacheFlushOnHaltFamilyServiceArray[0]
};

#endif  // _OPTION_CPU_CACHEFLUSHONHALT_INSTALL_H_
