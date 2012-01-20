/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Orochi Shared MSR table with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/OR
 * @e \$Revision: 53046 $   @e \$Date: 2011-05-13 20:20:37 -0600 (Fri, 13 May 2011) $
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
#include "amdlib.h"
#include "cpuRegisters.h"
#include "Table.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuF15OrPowerMgmt.h"
#include "OptionMultiSocket.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_OR_F15ORSHAREDMSRTABLE_FILECODE


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
F15OrFpCfgInit (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
STATIC CONST MSR_TYPE_ENTRY_INITIALIZER ROMDATA F15OrSharedMsrRegisters[] =
{
//  M S R    T a b l e s
// ----------------------

// MSR_TOM2 (0xC001001D)
// bits[63:0] - TOP_MEM2 = 0
  {
    MsrRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                         // platformFeatures
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
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                         // platformFeatures
    {{
      MSR_SYS_CFG,                        // MSR Address - Shared
      (1 << 21),                          // OR Mask
      (1 << 21),                          // NAND Mask
    }}
  },

// MSR_MC1_CTL_MASK (0xC0010045)
// bit[15] BSRP = 1, Erratum #593, OR-ALL
// bit[18] DEIBP = 1, Erratum #586, OR-ALL
  {
    MsrRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                         // platformFeatures
    {{
      MSR_MC1_CTL_MASK,                   // MSR Address
      0x0000000000048000,                 // OR Mask
      0x0000000000048000,                 // NAND Mask
    }}
  },

// MSR_CU_CFG (0xC0011023)
// bit[10] PbForceRespInOrder = 0
  {
    MsrRegister,
    {
      AMD_FAMILY_15,                    // CpuFamily
      AMD_F15_OR_ALL                    // CpuRevision
    },
    {AMD_PF_ALL},                       // platformFeatures
    {{
      MSR_CU_CFG,                       // MSR Address - Shared
      0,                                // OR Mask
      0x00000400,                       // NAND Mask
    }}
  },

// MSR_DE_CFG (0xC0011029)
// bit[10] ResyncPredSingleDispDis = 1
  {
    MsrRegister,
    {
      AMD_FAMILY_15,                    // CpuFamily
      AMD_F15_OR_ALL                    // CpuRevision
    },
    {AMD_PF_ALL},                       // platformFeatures
    {{
      MSR_DE_CFG,                       // MSR Address - Shared
      0x0000000000000400,               // OR Mask
      0x0000000000000400,               // NAND Mask
    }}
  },

// MSR_CU_CFG2 (0xC001102A)
// bit[50] = 1
// bit[11] = 1, Erratum #503, OR-ALL
// bit[10] = 1
  {
    MsrRegister,
    {
      AMD_FAMILY_15,                    // CpuFamily
      AMD_F15_OR_ALL                    // CpuRevision
    },
    {AMD_PF_ALL},                       // platformFeatures
    {{
      MSR_CU_CFG2,                      // MSR Address - Shared
      0x0004000000000C00,               // OR Mask
      0x0004000000000C00,               // NAND Mask
    }}
  },

// MSR_CU_CFG3 (0xC001102B)
// bit[42] PwcDisableWalkerSharing = 1
  {
    MsrRegister,
    {
      AMD_FAMILY_15,                    // CpuFamily
      AMD_F15_OR_ALL                    // CpuRevision
    },
    {AMD_PF_ALL},                       // platformFeatures
    {{
      MSR_CU_CFG3,                      // MSR Address
      0x0000040000000000,               // OR Mask
      0x0000040000000000,               // NAND Mask
    }}
  },
};


// Compute Unit Count Dependent MSR Table

STATIC CONST MSR_CU_TYPE_ENTRY_INITIALIZER ROMDATA F15OrSharedMsrCuRegisters[] =
{
//  M S R    T a b l e s
// ----------------------

  // MSR_CU_CFG2 (0xC001102A)
  // bits[7:6] - ThrottleNbInterface[1:0] = 0
  // bits[37:36] - ThrottleNbInterface[3:2] = 0
  {
    CompUnitCountsMsr,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                         // platformFeatures
    {{
	  {(COMPUTE_UNIT_RANGE_0 (1, 1) | COUNT_RANGE_NONE)}, // 1 compute unit
      {
        MSR_CU_CFG2,                        // MSR Address - Shared
        0x0000000000000000,                 // OR Mask
        0x00000030000000C0,                 // NAND Mask
      }
    }}
  },

  // MSR_CU_CFG2 (0xC001102A)
  // bits[7:6] - ThrottleNbInterface[1:0] = 1
  // bits[37:36] - ThrottleNbInterface[3:2] = 0
  {
    CompUnitCountsMsr,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                         // platformFeatures
    {{
	  {(COMPUTE_UNIT_RANGE_0 (2, 2) | COUNT_RANGE_NONE)}, // 2 compute units
      {
        MSR_CU_CFG2,                        // MSR Address - Shared
        0x0000000000000040,                 // OR Mask
        0x00000030000000C0,                 // NAND Mask
      }
    }}
  },

  // MSR_CU_CFG2 (0xC001102A)
  // bits[7:6] - ThrottleNbInterface[1:0] = 2
  // bits[37:36] - ThrottleNbInterface[3:2] = 0
  {
    CompUnitCountsMsr,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                         // platformFeatures
    {{
	  {(COMPUTE_UNIT_RANGE_0 (3, 3) | COUNT_RANGE_NONE)}, // 3 compute units
      {
        MSR_CU_CFG2,                        // MSR Address - Shared
        0x0000000000000080,                 // OR Mask
        0x00000030000000C0,                 // NAND Mask
      }
    }}
  },

  // MSR_CU_CFG2 (0xC001102A)
  // bits[7:6] - ThrottleNbInterface[1:0] = 3
  // bits[37:36] - ThrottleNbInterface[3:2] = 0
  {
    CompUnitCountsMsr,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                         // platformFeatures
    {{
	  {(COMPUTE_UNIT_RANGE_0 (4, 4) | COUNT_RANGE_NONE)}, // 4 compute units
      {
        MSR_CU_CFG2,                        // MSR Address - Shared
        0x00000000000000C0,                 // OR Mask
        0x00000030000000C0,                 // NAND Mask
      }
    }}
  },
};

// Shared MSRs with Special Programming Requirements Table

STATIC CONST FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_INITIALIZER ROMDATA F15OrSharedMsrWorkarounds[] =
{
  // MSR_FP_CFG (0xC0011028)
  // bit[16] -     DiDtMode = F3x1FC[0]
  // bits[22:18] - DiDtCfg0 = F3x1FC[5:1]
  // bits[34:27] - DiDtCfg1 = F3x1FC[13:6]
  {
    FamSpecificWorkaround,
    {
      AMD_FAMILY_15_OR,
      AMD_F15_OR_ALL
    },
    {AMD_PF_ALL},
    {{
      F15OrFpCfgInit,
      0x00000000
    }}
  },
};



CONST REGISTER_TABLE ROMDATA F15OrSharedMsrRegisterTable = {
  CorePairPrimary,
  (sizeof (F15OrSharedMsrRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) &F15OrSharedMsrRegisters,
};


CONST REGISTER_TABLE ROMDATA F15OrSharedMsrCuRegisterTable = {
  CorePairPrimary,
  (sizeof (F15OrSharedMsrCuRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) &F15OrSharedMsrCuRegisters,
};

CONST REGISTER_TABLE ROMDATA F15OrSharedMsrWorkaroundTable = {
  CorePairPrimary,
  (sizeof (F15OrSharedMsrWorkarounds) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) &F15OrSharedMsrWorkarounds,
};


/*---------------------------------------------------------------------------------------*/
/**
 * Update the FP_CFG MSR in current processor for Family15h OR.
 *
 * This function satisfies the programming requirements for the FP_CFG MSR.
 *
 * @param[in]   Data         The table data value, for example to indicate which CPU and Platform types matched.
 * @param[in]   StdHeader    Config handle for library and services.
 *
 */
VOID
F15OrFpCfgInit (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32       ProductInfo;
  UINT64       FpCfg;
  PCI_ADDR     PciAddress;

  if (IsWarmReset (StdHeader)) {
    OptionMultiSocketConfiguration.GetCurrPciAddr (&PciAddress, StdHeader);
    PciAddress.Address.Function = FUNC_3;
    PciAddress.Address.Register = PRCT_INFO_REG;
    LibAmdPciRead (AccessWidth32, PciAddress, &ProductInfo, StdHeader);

    LibAmdMsrRead (MSR_FP_CFG, &FpCfg, StdHeader);
    ((FP_CFG_MSR *) &FpCfg)->DiDtMode = ((PRODUCT_INFO_REGISTER *) &ProductInfo)->DiDtMode;
    ((FP_CFG_MSR *) &FpCfg)->DiDtCfg0 = ((PRODUCT_INFO_REGISTER *) &ProductInfo)->DiDtCfg0;
    ((FP_CFG_MSR *) &FpCfg)->DiDtCfg1 = ((PRODUCT_INFO_REGISTER *) &ProductInfo)->DiDtCfg1;
    ((FP_CFG_MSR *) &FpCfg)->AlwaysOnThrottle = ((PRODUCT_INFO_REGISTER *) &ProductInfo)->AlwaysOnThrottle;
    ((FP_CFG_MSR *) &FpCfg)->Pipe3ThrottleDis = ((PRODUCT_INFO_REGISTER *) &ProductInfo)->Pipe3ThrottleDis;
    LibAmdMsrWrite (MSR_FP_CFG, &FpCfg, StdHeader);
  }
}

