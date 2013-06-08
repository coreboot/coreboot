/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Trinity Shared MSR table with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/TN
 * @e \$Revision: 64491 $   @e \$Date: 2012-01-23 12:37:30 -0600 (Mon, 23 Jan 2012) $
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
#include "amdlib.h"
#include "cpuRegisters.h"
#include "Table.h"
#include "cpuF15PowerMgmt.h"
#include "cpuF15TnPowerMgmt.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_TN_F15TNSHAREDMSRTABLE_FILECODE


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
F15TnFpCfgInit (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
STATIC
Update800MHzHtcPstateTo900MHz (
  IN       UINT32             Data,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
STATIC CONST MSR_TYPE_ENTRY_INITIALIZER ROMDATA F15TnSharedMsrRegisters[] =
{
//  M S R    T a b l e s
// ----------------------

// MSR_TOM2 (0xC001001D)
// bits[63:0] TOP_MEM2 = 0
  {
    MsrRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MSR_TOM2,                           // MSR Address - Shared
      0x0000000000000000,                 // OR Mask
      0xFFFFFFFFFFFFFFFF,                 // NAND Mask
    }}
  },

// MSR_SYS_CFG (0xC0010010)
// bit[21] MtrrTom2En = 1
  {
    MsrRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MSR_SYS_CFG,                        // MSR Address - Shared
      (1 << 21),                          // OR Mask
      (1 << 21),                          // NAND Mask
    }}
  },

// MSR_IC_CFG (0xC0011021)
// bit[39] DisLoopPredictor = 1
  {
    MsrRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MSR_IC_CFG,                         // MSR Address - Shared
      (1ull << 39),                       // OR Mask
      (1ull << 39),                       // NAND Mask
    }}
  },

// MSR_CU_CFG (0xC0011023)
  {
    MsrRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MSR_CU_CFG,                         // MSR Address - Shared
      0,                                  // OR Mask
      0x00000400,                         // NAND Mask
    }}
  },

// MSR_CU_CFG2 (0xC001102A)
// bit[50] RdMmExtCfgQwEn = 1
// bit[10] VicResyncChkEn = 1

  {
    MsrRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MSR_CU_CFG2,                        // MSR Address - Shared
      0x0004000000000400,                 // OR Mask
      0x0004000000000400,                 // NAND Mask
    }}
  },
// MSR_CU_CFG3 (0xC001102B)
// bit[42] PwcDisableWalkerSharing = 0
// bit[22] PfcDoubleStride = 1
  {
    MsrRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MSR_CU_CFG3,                        // MSR Address
      0x0000000000400000,                 // OR Mask
      0x0000040000400000,                 // NAND Mask
    }}
  },
};


// Compute Unit Count Dependent MSR Table

STATIC CONST MSR_CU_TYPE_ENTRY_INITIALIZER ROMDATA F15TnSharedMsrCuRegisters[] =
{
//  M S R    T a b l e s
// ----------------------

  // MSR_CU_CFG2 (0xC001102A)
  // bits[37:36] - ThrottleNbInterface[3:2] = 0
  // bits[7:6] - ThrottleNbInterface[1:0] = 0
  {
    CompUnitCountsMsr,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      {(COMPUTE_UNIT_RANGE_0 (1, 1) | COUNT_RANGE_NONE)}, // 1 compute unit
      {
        MSR_CU_CFG2,                      // MSR Address - Shared
        0x0000000000000000,               // OR Mask
        0x00000030000000C0,               // NAND Mask
      }
    }}
  },

  // MSR_CU_CFG2 (0xC001102A)
  // bits[37:36] - ThrottleNbInterface[3:2] = 0
  // bits[7:6] - ThrottleNbInterface[1:0] = 1
  {
    CompUnitCountsMsr,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      {(COMPUTE_UNIT_RANGE_0 (2, 2) | COUNT_RANGE_NONE)}, // 2 compute units
      {
        MSR_CU_CFG2,                      // MSR Address - Shared
        0x0000000000000040,               // OR Mask
        0x00000030000000C0,               // NAND Mask
      }
    }}
  }

};


// Shared MSRs with Special Programming Requirements Table

STATIC CONST FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_INITIALIZER ROMDATA F15TnSharedMsrWorkarounds[] =
{
  // MSR_FP_CFG (0xC0011028)
  // bit[16] -     DiDtMode = F3x1FC[0]
  // bits[22:18] - DiDtCfg0 = F3x1FC[5:1]
  // bits[34:27] - DiDtCfg1 = F3x1FC[13:6]
  // bits[26:25] - DiDtCfg2 = F3x1FC[15:14]
  // bits[44:42] - DiDtCfg4 = F3x1FC[19:17]
  {
    FamSpecificWorkaround,
    {
      AMD_FAMILY_15_TN,
      AMD_F15_TN_ALL
    },
    {AMD_PF_ALL},
    {{
      F15TnFpCfgInit,
      0x00000000
    }}
  },
};


CONST REGISTER_TABLE ROMDATA F15TnSharedMsrRegisterTable = {
  CorePairPrimary,
  (sizeof (F15TnSharedMsrRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) &F15TnSharedMsrRegisters,
};


CONST REGISTER_TABLE ROMDATA F15TnSharedMsrCuRegisterTable = {
  CorePairPrimary,
  (sizeof (F15TnSharedMsrCuRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) &F15TnSharedMsrCuRegisters,
};

CONST REGISTER_TABLE ROMDATA F15TnSharedMsrWorkaroundTable = {
  CorePairPrimary,
  (sizeof (F15TnSharedMsrWorkarounds) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) &F15TnSharedMsrWorkarounds,
};

/*---------------------------------------------------------------------------------------*/
/**
 * Update the FP_CFG MSR in current processor for Family15h TN.
 *
 * This function satisfies the programming requirements for the FP_CFG MSR.
 *
 * @param[in]   Data         The table data value, for example to indicate which CPU and Platform types matched.
 * @param[in]   StdHeader    Config handle for library and services.
 *
 */
VOID
F15TnFpCfgInit (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32       ProductInfo;
  UINT64       FpCfg;
  PCI_ADDR     PciAddress;

  PciAddress.AddressValue = PRCT_INFO_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &ProductInfo, StdHeader);

  LibAmdMsrRead (MSR_FP_CFG, &FpCfg, StdHeader);
  ((FP_CFG_MSR *) &FpCfg)->DiDtMode = ((PRODUCT_INFO_REGISTER *) &ProductInfo)->DiDtMode;
  ((FP_CFG_MSR *) &FpCfg)->DiDtCfg0 = ((PRODUCT_INFO_REGISTER *) &ProductInfo)->DiDtCfg0;
  ((FP_CFG_MSR *) &FpCfg)->DiDtCfg1 = ((PRODUCT_INFO_REGISTER *) &ProductInfo)->DiDtCfg1;
  ((FP_CFG_MSR *) &FpCfg)->DiDtCfg2 = ((PRODUCT_INFO_REGISTER *) &ProductInfo)->DiDtCfg2;
  ((FP_CFG_MSR *) &FpCfg)->DiDtCfg4 = ((PRODUCT_INFO_REGISTER *) &ProductInfo)->DiDtCfg4;
  ((FP_CFG_MSR *) &FpCfg)->DiDtCfg5 = ((PRODUCT_INFO_REGISTER *) &ProductInfo)->DiDtCfg5;
  LibAmdMsrWrite (MSR_FP_CFG, &FpCfg, StdHeader);
}


// Per-Node MSR with Special Programming Requirements Table
STATIC CONST FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_INITIALIZER ROMDATA F15TnPerNodeMsrWorkarounds[] =
{
// MSR C001_00[6B:64]
  {
    FamSpecificWorkaround,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                        // CpuFamily
      AMD_F15_TN_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      Update800MHzHtcPstateTo900MHz,         // function call
      0x00000000,                           // data
    }}
  }
};


CONST REGISTER_TABLE ROMDATA F15TnPerNodeMsrWorkaroundTable = {
  PrimaryCores,
  (sizeof (F15TnPerNodeMsrWorkarounds) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) F15TnPerNodeMsrWorkarounds,
};


/*---------------------------------------------------------------------------------------*/
/**
 *  Workaround for CPUs with a minimum P-state = 800MHz.
 *
 *  AGESA should change the frequency of 800MHz P-states to 900MHz.
 *
 * @param[in]   Data         The table data value, for example to indicate which CPU and Platform types matched.
 * @param[in]   StdHeader    Config handle for library and services.
 *
 */
VOID
STATIC
Update800MHzHtcPstateTo900MHz (
  IN       UINT32             Data,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  PCI_ADDR      PciAddress;
  PSTATE_MSR    HtcPstate;
  PSTATE_MSR    HtcPstateMinus1;
  HTC_REGISTER  HtcRegister;

  PciAddress.AddressValue = HTC_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, (VOID *) &HtcRegister, StdHeader);

  LibAmdMsrRead ((HtcRegister.HtcPstateLimit + MSR_PSTATE_0), (UINT64 *) &HtcPstate, StdHeader);

  if (HtcPstate.CpuFid == 0 && HtcPstate.CpuDid == 1) {
    if (HtcRegister.HtcPstateLimit == 0) {
      HtcPstateMinus1 = HtcPstate;
    } else {
      LibAmdMsrRead ((HtcRegister.HtcPstateLimit + MSR_PSTATE_0 - 1), (UINT64 *) &HtcPstateMinus1, StdHeader);
    }
    HtcPstate.CpuVid = HtcPstateMinus1.CpuVid;
    HtcPstate.CpuFid = 2;
    LibAmdMsrWrite ((HtcRegister.HtcPstateLimit + MSR_PSTATE_0), (UINT64 *) &HtcPstate, StdHeader);
  }
}

