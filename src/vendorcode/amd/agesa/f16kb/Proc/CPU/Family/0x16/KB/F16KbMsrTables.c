/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_16 KB MSR tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x16/KB
 * @e \$Revision: 87267 $   @e \$Date: 2013-01-31 09:34:00 -0600 (Thu, 31 Jan 2013) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuRegisters.h"
#include "F16KbPowerMgmt.h"
#include "Table.h"
#include "Filecode.h"
CODE_GROUP  (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X16_KB_F16KBMSRTABLES_FILECODE

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
VOID
NbMcaLock (
  IN       UINT32            Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

//   M S R   T a b l e s
// ----------------------

STATIC CONST MSR_TYPE_ENTRY_INITIALIZER ROMDATA F16KbMsrRegisters[] =
{
// MSR_TOM2 (0xC001001D)
// bits[39:23]   TOP_MEM2            = 0x0
  {
    MsrRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                             // PlatformFeatures
    {{
      MSR_TOM2,                             // Address
      0x0000000000000000,                   // RegData
      0xFFFFFFFFFF800000,                   // RegMask
    }}
  },

// MSR_SYS_CFG (0xC0010010)
// bits[21]      MtrrTom2En          = 0x1
  {
    MsrRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                             // PlatformFeatures
    {{
      MSR_SYS_CFG,                          // Address
      (1 << 21),                            // RegData
      (1 << 21),                            // RegMask
    }}
  },
// MC4_MISC_1 (0xC0000408)
// Clear to 0
  {
    MsrRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                             // PlatformFeatures
    {{
      0xC0000408,                           // Address
      0x0000000000000000,                   // RegData
      0xFF0FFFFFFFFFFFFF,                   // RegMask
    }}
  },
// MSR_LS_CFG (C0011020)
// bits[26]                          = 0x1
  {
    MsrRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MSR_LS_CFG,                           // Address
      0x0000000004000000,                   // RegData
      0x0000000004000000,                   // RegMask
    }}
  },
// MSR_IC_CFG (C0011021)
// bits[26]      DIS_WIDEREAD_PWR_SAVE = 0x1
  {
    MsrRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MSR_IC_CFG,                           // Address
      0x0000000004000000,                   // RegData
      0x0000000004000000,                   // RegMask
    }}
  },
// Processor Feedback Constants 0 (C0011090)
// bits[15:8]    RefCountScale = 0x64
// bits[7:0]     ActualCountScale = 0xA5
  {
    MsrRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      0xC0011090,                           // Address
      0x00000000000064A5,                   // RegData
      0x000000000000FFFF,                   // RegMask
    }}
  },
// MSR_L2I_CFG (C00110A0)
// bits[56:45]   L2ScrubberInterval  = 0x100
// bits[44]      PbDisObeysThrottleNb = 0x1
// bits[43:40]   ThrottleNbInterface = 0x0
// bits[19]      McaToMstCoreEn      = 0x1
  {
    MsrRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MSR_L2I_CFG,                          // Address
      0x0020100000080000,                   // RegData
      0x01FFFF0000080000,                   // RegMask
    }}
  },
};

//   MSR with Special Programming Requirements Table
// ----------------------

STATIC CONST FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_INITIALIZER ROMDATA F16KbMsrWorkarounds[] =
{
// MSR_0000_0413
  {
    FamSpecificWorkaround,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      NbMcaLock,                            // Function call
      0x00000000,                           // Data
    }}
  },
};

CONST REGISTER_TABLE ROMDATA F16KbMsrRegisterTable = {
  AllCores,
  PERFORM_TP_AFTER_AP_LAUNCH,
  (sizeof (F16KbMsrRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) &F16KbMsrRegisters,
};

CONST REGISTER_TABLE ROMDATA F16KbMsrWorkaroundTable = {
  AllCores,
  PERFORM_TP_AFTER_AP_LAUNCH,
  (sizeof (F16KbMsrWorkarounds) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) F16KbMsrWorkarounds,
};


/*---------------------------------------------------------------------------------------*/
/**
 *  MSR special programming requirements for MSR_0000_0413
 *
 *
 * @param[in]   Data         The table data value, for example to indicate which CPU and Platform types matched.
 * @param[in]   StdHeader    Config handle for library and services.
 *
 */
VOID
NbMcaLock (
  IN       UINT32            Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  MC4_MISC0_MSR Mc4Misc0;

  LibAmdMsrRead (MSR_MC4_MISC, (UINT64 *) &Mc4Misc0, StdHeader);

  if (Mc4Misc0.IntType == 0x2) {
    Mc4Misc0.Locked = 1;
  } else {
    Mc4Misc0.Locked = 0;
  }
  LibAmdMsrWrite (MSR_MC4_MISC, (UINT64 *) &Mc4Misc0, StdHeader);

  return;
}
