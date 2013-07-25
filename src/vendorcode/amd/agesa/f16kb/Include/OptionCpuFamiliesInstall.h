/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of all appropriate CPU family specific support.
 *
 * This file generates the defaults tables for all family specific
 * combinations.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
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

/*  Default all CPU Specific Service members to off.  They
    will be enabled as needed by cross referencing families
    with entry points in the family / model install files.  */
#define USES_REGISTER_TABLES        FALSE
#define BASE_FAMILY_PCI             FALSE
#define MODEL_SPECIFIC_PCI          FALSE
#define BASE_FAMILY_MSR             FALSE
#define MODEL_SPECIFIC_MSR          FALSE
#define BASE_FAMILY_HT_PCI          FALSE
#define MODEL_SPECIFIC_HT_PCI       FALSE
#define BASE_FAMILY_WORKAROUNDS     FALSE

/*
 * Pull in family specific services based on entry point
 */
#if AGESA_ENTRY_INIT_RESET == TRUE
#endif

#if AGESA_ENTRY_INIT_RECOVERY == TRUE
  #undef USES_REGISTER_TABLES
  #define USES_REGISTER_TABLES        TRUE
  #undef BASE_FAMILY_PCI
  #define BASE_FAMILY_PCI             TRUE
  #undef MODEL_SPECIFIC_PCI
  #define MODEL_SPECIFIC_PCI          TRUE
  #undef BASE_FAMILY_MSR
  #define BASE_FAMILY_MSR             TRUE
  #undef MODEL_SPECIFIC_MSR
  #define MODEL_SPECIFIC_MSR          TRUE
#endif

#if AGESA_ENTRY_INIT_EARLY == TRUE
  #undef USES_REGISTER_TABLES
  #define USES_REGISTER_TABLES        TRUE
  #undef BASE_FAMILY_PCI
  #define BASE_FAMILY_PCI             TRUE
  #undef MODEL_SPECIFIC_PCI
  #define MODEL_SPECIFIC_PCI          TRUE
  #undef BASE_FAMILY_MSR
  #define BASE_FAMILY_MSR             TRUE
  #undef MODEL_SPECIFIC_MSR
  #define MODEL_SPECIFIC_MSR          TRUE
  #undef BASE_FAMILY_HT_PCI
  #define BASE_FAMILY_HT_PCI          TRUE
  #undef MODEL_SPECIFIC_HT_PCI
  #define MODEL_SPECIFIC_HT_PCI       TRUE
  #undef BASE_FAMILY_WORKAROUNDS
  #define BASE_FAMILY_WORKAROUNDS     TRUE
#endif

#if AGESA_ENTRY_INIT_POST == TRUE
#endif

#if AGESA_ENTRY_INIT_ENV == TRUE
#endif

#if AGESA_ENTRY_INIT_MID == TRUE
#endif

#if AGESA_ENTRY_INIT_LATE == TRUE
#endif

#if AGESA_ENTRY_INIT_S3SAVE == TRUE
#endif

#if AGESA_ENTRY_INIT_RESUME == TRUE
#endif

#if AGESA_ENTRY_INIT_LATE_RESTORE == TRUE
#endif

#if AGESA_ENTRY_INIT_GENERAL_SERVICES == TRUE
#endif

/*
 * Initialize PCI MMIO mask to 0
 */
#define FAMILY_MMIO_BASE_MASK (0ull)


/*
 * Initialize all families to disabled
 */
#define OPT_F15_TABLE
#define OPT_F16_TABLE

#define OPT_F15_ID_TABLE
#define OPT_F16_ID_TABLE


/*
 * Install family specific support
 */
#if (OPTION_FAMILY15H_TN == TRUE)
  #include  "OptionFamily15hInstall.h"
#endif

#if (OPTION_FAMILY16H_KB == TRUE)
  #include  "OptionFamily16hInstall.h"
#endif
/*
 * Process PCI MMIO mask
 */

// If size is 0, but base is not, break the build.
#if (CFG_PCI_MMIO_BASE != 0) && (CFG_PCI_MMIO_SIZE == 0)
  #error    BLDCFG: Invalid PCI MMIO size -- acceptable values are 1, 2, 4, 8, 16, 32, 64, 128, and 256
#endif

// If base is 0, but size is not, break the build.
#if (CFG_PCI_MMIO_BASE == 0) && (CFG_PCI_MMIO_SIZE != 0)
  #error    BLDCFG: Invalid PCI MMIO base -- must be 8MB or greater
#endif

#if (CFG_PCI_MMIO_BASE != 0) && (CFG_PCI_MMIO_SIZE != 0)
  // Both are non-zero, begin further processing.

  // Heap runs from 4MB to 8MB. Disallow any addresses below 8MB.
  #if (CFG_PCI_MMIO_BASE < 0x800000)
    #error    BLDCFG: Invalid PCI MMIO base -- must be 8MB or greater
  #endif

  // Break the build if the address is too high for the enabled families.
  #if ((CFG_PCI_MMIO_BASE & FAMILY_MMIO_BASE_MASK) != 0)
    #error       BLDCFG: Invalid PCI MMIO base address for the installed CPU families
  #endif

  // If the size parameter is not valid, break the build.
  #if (CFG_PCI_MMIO_SIZE != 1) && (CFG_PCI_MMIO_SIZE != 2) && (CFG_PCI_MMIO_SIZE != 4) && (CFG_PCI_MMIO_SIZE != 8) && (CFG_PCI_MMIO_SIZE != 16)
    #if (CFG_PCI_MMIO_SIZE != 32) && (CFG_PCI_MMIO_SIZE != 64) && (CFG_PCI_MMIO_SIZE != 128) && (CFG_PCI_MMIO_SIZE != 256)
      #error      BLDCFG: Invalid PCI MMIO size -- acceptable values are 1, 2, 4, 8, 16, 32, 64, 128, and 256
    #endif
  #endif

  #define PCI_MMIO_ALIGNMENT ((0x100000ul * CFG_PCI_MMIO_SIZE) - 1)
  // If the base is not aligned according to size, break the build.
  #if ((CFG_PCI_MMIO_BASE & PCI_MMIO_ALIGNMENT) != 0)
    #error      BLDCFG: Invalid PCI MMIO base -- must be properly aligned according to MMIO size
  #endif
  #undef PCI_MMIO_ALIGNMENT
#endif

/*
 * Process sockets / modules
 */
#ifndef ADVCFG_PLATFORM_SOCKETS
  #error BLDOPT Set Family supported sockets.
#endif
#ifndef ADVCFG_PLATFORM_MODULES
  #error BLDOPT Set Family supported modules.
#endif

CONST OPTIONS_CONFIG_TOPOLOGY ROMDATA TopologyConfiguration =
{
  ADVCFG_PLATFORM_SOCKETS,
  ADVCFG_PLATFORM_MODULES
};

/*
 * Instantiate global data needed for processor identification
 */
CONST CPU_SPECIFIC_SERVICES_XLAT ROMDATA CpuSupportedFamiliesArray[] =
{
  OPT_F15_TABLE
  OPT_F16_TABLE
};

CONST CPU_FAMILY_SUPPORT_TABLE ROMDATA CpuSupportedFamiliesTable =
{
  (sizeof (CpuSupportedFamiliesArray) / sizeof (CPU_SPECIFIC_SERVICES_XLAT)),
  &CpuSupportedFamiliesArray[0]
};


CONST CPU_LOGICAL_ID_FAMILY_XLAT ROMDATA CpuSupportedFamilyIdArray[] =
{
  OPT_F15_ID_TABLE
  OPT_F16_ID_TABLE
};

CONST CPU_FAMILY_ID_XLAT_TABLE ROMDATA CpuSupportedFamilyIdTable =
{
  (sizeof (CpuSupportedFamilyIdArray) / sizeof (CPU_LOGICAL_ID_FAMILY_XLAT)),
  CpuSupportedFamilyIdArray
};
