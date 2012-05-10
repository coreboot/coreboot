/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_12 MSR tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "cpuRegisters.h"
#include "cpuF12PowerMgmt.h"
#include "Table.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FAMILY_0X12_CPUF12MSRTABLES_FILECODE

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
CONST MSR_TYPE_ENTRY_INITIALIZER ROMDATA F12MsrRegisters[] =
{

//  M S R    T a b l e s
// ----------------------

// MSR_TOM2 (0xC001001D)
// bits[63:0] - TOP_MEM2 = 0
  {
    MsrRegister,
    {
      AMD_FAMILY_12,                       // CpuFamily
      AMD_F12_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_TOM2,                              // MSR Address
      0x0000000000000000ull,                    // OR Mask
      0xFFFFFFFFFFFFFFFFull,                    // NAND Mask
    }}
  },
// MSR_SYS_CFG (0xC0010010)
// bit[21] - MtrrTom2En = 1
  {
    MsrRegister,
    {
      AMD_FAMILY_12,                       // CpuFamily
      AMD_F12_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_SYS_CFG,                           // MSR Address
      (UINT64) (1 << 21),                             // OR Mask
      (UINT64) (1 << 21),                             // NAND Mask
    }}
  },
// MSR_HWCR (0xC0010015)
// bit[4] - INVD_WBINVD = 1
  {
    MsrRegister,
    {
      AMD_FAMILY_12,                       // CpuFamily
      AMD_F12_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_HWCR,                              // MSR Address
      0x0000000000000010ull,                    // OR Mask
      0x0000000000000010ull,                    // NAND Mask
    }}
  },
// MSR_CSTATE_ADDRESS (0xC0010073)
// bit[15:0] - CstateAddr = 0
  {
    MsrRegister,
    {
      AMD_FAMILY_12,                       // CpuFamily
      AMD_F12_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_CSTATE_ADDRESS,                    // MSR Address
      0x0000000000000000ull,                    // OR Mask
      0x000000000000FFFFull,                    // NAND Mask
    }}
  },
// MSR_BU_CFG2 (0xC001102A)
// bit[50]     - RdMmExtCfgDwDis = 1
// bit[56]     - L2ClkGatingEn = 1
// bits[58:57] - L2HystCnt = 3
  {
    MsrRegister,
    {
      AMD_FAMILY_12,                       // CpuFamily
      AMD_F12_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_BU_CFG2,                           // MSR Address
      0x0704000000000000ull,                    // OR Mask
      0x0704000000000000ull,                    // NAND Mask
    }}
  },
// MSR_OSVW_ID_Length (0xC0010140)
// bit[15:0] = 4
  {
    MsrRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_OSVW_ID_Length,                    // MSR Address
      0x0000000000000004ull,                    // OR Mask
      0x000000000000FFFFull,                    // NAND Mask
    }}
  },
// MSR_OSVW_Status (0xC0010141)
// bits[2:0] = 0 reserved, must be zero
// bit[3]    = 1 for Erratum #383
  {
    MsrRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_LN_Ax                       // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_OSVW_Status,                       // MSR Address
      0x0000000000000008ull,                    // OR Mask
      0x000000000000000Full,                    // NAND Mask
    }}
  },
// This MSR should be set after the code that most errata would be applied in
// MSR_MC0_CTL (0x00000400)
// bits[63:0]  = 0xFFFFFFFFFFFFFFFF
  {
    MsrRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_MC0_CTL,                              // MSR Address
      0xFFFFFFFFFFFFFFFFull,                    // OR Mask
      0xFFFFFFFFFFFFFFFFull,                    // NAND Mask
    }}
  }
};

CONST REGISTER_TABLE ROMDATA F12MsrRegisterTable = {
  AllCores,
  (sizeof (F12MsrRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) &F12MsrRegisters,
};


