/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 DR, MSR tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 56307 $   @e \$Date: 2011-07-11 15:13:07 -0600 (Mon, 11 Jul 2011) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "Table.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X10_CPUF10MSRTABLES_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
STATIC CONST MSR_TYPE_ENTRY_INITIALIZER ROMDATA F10MsrRegisters[] =
{
//  M S R    T a b l e s
// ----------------------

// MSR_TOM2 (0xC001001D)
// bits[63:0] - TOP_MEM2 = 0
  {
    MsrRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_TOM2,                              // MSR Address
      0x0000000000000000,                    // OR Mask
      0xFFFFFFFFFFFFFFFF,                    // NAND Mask
    }}
  },
// MSR_SYS_CFG (0xC0010010)
// bit[21] = 1
  {
    MsrRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_SYS_CFG,                           // MSR Address
      (1 << 21),                             // OR Mask
      (1 << 21),                             // NAND Mask
    }}
  },
// MSR_HWCR (0xC0010015)
// Do not set bit[24] = 1, it will be set in AmdInitPost.
// bit[4] = 1
  {
    MsrRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_HWCR,                              // MSR Address
      0x0000000000000010,                    // OR Mask
      0x0000000000000010,                    // NAND Mask
    }}
  },
// MSR_MC4_CTL_MASK (0xC0010048)
// bit[10] = 1
// bits[22:19] = 1111b
  {
    MsrRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_MC4_CTL_MASK,                      // MSR Address
      0x0000000000780400,                    // OR Mask
      0x0000000000780400,                    // NAND Mask
    }}
  },
// MSR_DC_CFG (0xC0011022)
// bits[35:34] = 01
  {
    MsrRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_MULTI_LINK},                     // platformFeatures
    {{
      MSR_DC_CFG,                            // MSR Address
      0x0000000400000000,                    // OR Mask
      0x0000000C00000000,                    // NAND Mask
    }}
  },
// MSR_NB_CFG (0xC001001F)
// bit[54] = 1
// bit[52:51] = 11b for Erratum #372
  {
    MsrRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},   // platformFeatures
    {{
      MSR_NB_CFG,                            // MSR Address
      0x0058000000000000,                    // OR Mask
      0x0058000000000000,                    // NAND Mask
    }}
  },
// MSR_LS_CFG (0xC0011020)
// bit[8] = 1 for Erratum #670
  {
    MsrRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MSR_LS_CFG,                           // MSR Address
      (1 << 8),                             // OR Mask
      (1 << 8),                             // NAND Mask
    }}
  },
// MSR_DC_CFG (0xC0011022)
// bit[24] = 1
  {
    MsrRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_DC_CFG,                            // MSR Address
      (1 << 24),                             // OR Mask
      (1 << 24),                             // NAND Mask
    }}
  },
// MSR_CPUID_FEATS (0xC0011004)
// bit[28] = 1
  {
    MsrRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_MULTI_CORE | AMD_PF_DUAL_CORE) },  // platformFeatures
    {{
      MSR_CPUID_FEATS,                       // MSR Address
      (1 << 28),                             // OR Mask
      (1 << 28),                             // NAND Mask
    }}
  },
// MSR_CPUID_EXT_FEATS (0xC0011005)
// bit[33] = 1
  {
    MsrRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_DUAL_CORE},                     // platformFeatures
    {{
      MSR_CPUID_EXT_FEATS,                   // MSR Address
      0x0000000200000000,                    // OR Mask
      0x0000000200000000,                    // NAND Mask
    }}
  },
// MSR_OSVW_ID_Length (0xC0010140)
// bit[15:0] = 4
  {
    MsrRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_OSVW_ID_Length,                    // MSR Address
      0x0000000000000004,                    // OR Mask
      0x000000000000FFFF,                    // NAND Mask
    }}
  },
// MSR_OSVW_Status (0xC0010141)
// bit[3] = 1 for Erratum #383
// bit[2] = 1 for Erratum #415
  {
    MsrRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_OSVW_Status,                       // MSR Address
      0x000000000000000C,                    // OR Mask
      0x000000000000000C,                    // NAND Mask
    }}
  },
// This MSR should be set after the code that most errata would be applied in
// MSR_MC0_CTL (0x00000400)
// bits[63:0]  = 0xFFFFFFFFFFFFFFFF
  {
    MsrRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_MC0_CTL,                           // MSR Address
      0xFFFFFFFFFFFFFFFF,                    // OR Mask
      0xFFFFFFFFFFFFFFFF,                    // NAND Mask
    }}
  }
};

CONST REGISTER_TABLE ROMDATA F10MsrRegisterTable = {
  AllCores,
  (sizeof (F10MsrRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *)F10MsrRegisters,
};

