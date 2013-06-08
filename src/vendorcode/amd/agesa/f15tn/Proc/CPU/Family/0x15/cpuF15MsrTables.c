/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 MSR tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
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
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_CPUF15MSRTABLES_FILECODE

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
STATIC CONST MSR_TYPE_ENTRY_INITIALIZER ROMDATA F15MsrRegisters[] =
{
//  M S R    T a b l e s
// ----------------------

// MSR_HWCR (0xC0010015)
// bit[4] = 1
  {
    MsrRegister,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                      // CpuFamily
      (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)                           // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_HWCR,                              // MSR Address
      0x0000000000000010,                    // OR Mask
      0x0000000000000010,                    // NAND Mask
    }}
  },
// MSR_NB_CFG (0xC001001F)
// bit[54] InitApicIdCpuIdLo = 1
  {
    MsrRegister,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                      // CpuFamily
      (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)                           // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_NB_CFG,                            // MSR Address
      0x0040000000000000,                    // OR Mask
      0x0040000000000000,                    // NAND Mask
    }}
  },
// This MSR should be set after the code that most errata would be applied in
// MSR_MC0_CTL (0x00000400)
// bits[63:0]  = 0xFFFFFFFFFFFFFFFF
  {
    MsrRegister,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                      // CpuFamily
      (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)                           // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_MC0_CTL,                           // MSR Address
      0xFFFFFFFFFFFFFFFF,                    // OR Mask
      0xFFFFFFFFFFFFFFFF,                    // NAND Mask
    }}
  }
};

CONST REGISTER_TABLE ROMDATA F15MsrRegisterTable = {
  AllCores,
  (sizeof (F15MsrRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *)F15MsrRegisters,
};

