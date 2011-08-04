/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_14 MSR tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 48588 $   @e \$Date: 2011-03-10 08:57:36 -0700 (Thu, 10 Mar 2011) $
 *
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "cpuRegisters.h"
#include "Table.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FAMILY_0X14_CPUF14MSRTABLES_FILECODE

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
CONST MSR_TYPE_ENTRY_INITIALIZER ROMDATA F14MsrRegisters[] =
{

//  M S R    T a b l e s
// ----------------------

// MC0_CTL_MASK (0xC0010044)
// bit[6] = 1, erratum #628
  {
    MsrRegister,
    {
      AMD_FAMILY_14,                       // CpuFamily
      AMD_F14_ON_ALL                       // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_MC0_CTL_MASK,                      // MSR Address
      0x0000000000000040,                    // OR Mask
      0x0000000000000040,                    // NAND Mask
     }}
  },
// MSR_TOM2 (0xC001001D)
// bits[63:0] - TOP_MEM2 = 0
  {
    MsrRegister,
    {
      AMD_FAMILY_14,                       // CpuFamily
      AMD_F14_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_TOM2,                              // MSR Address
      0x0000000000000000,                    // OR Mask
      0xFFFFFFFFFFFFFFFF,                    // NAND Mask
    }}
  },
// MSR_SYS_CFG (0xC0010010)
// bit[21] - MtrrTom2En = 1
  {
    MsrRegister,
    {
      AMD_FAMILY_14,                       // CpuFamily
      AMD_F14_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_SYS_CFG,                           // MSR Address
      (1 << 21),                             // OR Mask
      (1 << 21),                             // NAND Mask
    }}
  },
// MSR_CPUID_EXT_FEATS (0xC0011005)
// bit[41] - OSVW = 0
  {
    MsrRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MSR_CPUID_EXT_FEATS,                   // MSR Address
      0x0000000000000000,                    // OR Mask
      0x0000020000000000,                    // NAND Mask
    }}
  },
// MSR_OSVW_ID_Length (0xC0010140)
// bit[15:0] = 4
  {
    MsrRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MSR_OSVW_ID_Length,                    // MSR Address
      0x0000000000000004,                    // OR Mask
      0x000000000000FFFF,                    // NAND Mask
    }}
  },
// MSR_HWCR (0xC0010015)
// Do not set bit[24] = 1, it will be set in AmdInitPost.

// This MSR should be set after the code that most errata would be applied in
// MSR_MC0_CTL (0x00000400)
// bits[63:0]  = 0xFFFFFFFFFFFFFFFF
  {
    MsrRegister,
    {
      AMD_FAMILY_14,                       // CpuFamily
      AMD_F14_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_MC0_CTL,                           // MSR Address
      0xFFFFFFFFFFFFFFFF,                    // OR Mask
      0xFFFFFFFFFFFFFFFF,                    // NAND Mask
    }}
  },
// MSR_LS_CFG (0xC0011020)
// bit[36]  Reserved = 1, workaround for erratum #530
// bit[25]  Reserved = 1, workaround for erratum #551
  {
    MsrRegister,
    {
      AMD_FAMILY_14,                       // CpuFamily
      AMD_F14_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_LS_CFG,                            // MSR Address
      0x0000001002000000,                    // OR Mask
      0x0000001002000000,                    // NAND Mask
     }}
  },
// MSR_DC_CFG (0xC0011022)
// bit[57:56]  Reserved = 2
  {
    MsrRegister,
    {
      AMD_FAMILY_14,                       // CpuFamily
      AMD_F14_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_DC_CFG,                            // MSR Address
      0x0200000000000000,                    // OR Mask
      0x0300000000000000,                    // NAND Mask
     }}
  }
};

CONST REGISTER_TABLE ROMDATA F14MsrRegisterTable = {
  AllCores,
  (sizeof (F14MsrRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) &F14MsrRegisters,
};


