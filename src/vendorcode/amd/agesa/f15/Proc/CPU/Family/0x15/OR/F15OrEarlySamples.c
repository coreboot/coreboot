/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 OR early sample support.
 *
 * Provides the code and data required to support pre-production silicon.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/OR
 * @e \$Revision: 56279 $   @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
 *
 */
/*
 *****************************************************************************
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
#include "Ids.h"
#include "cpuRegisters.h"
#include "Table.h"
#include "cpuEarlyInit.h"
#include "cpuFamilyTranslation.h"
#include "F15OrUtilities.h"
#include "cpuF15Utilities.h"
#include "cpuF15PowerMgmt.h"
#include "cpuF15OrPowerMgmt.h"
#include "GeneralServices.h"
#include "OptionMultiSocket.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_OR_F15OREARLYSAMPLES_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
typedef union {
  UINT64           RawData;
  PATCH_LOADER_MSR BitFields;
} PATCH_LOADER;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
BOOLEAN
F15OrEarlySamplesLoadMicrocode (
  IN       MICROCODE_PATCH    *MicrocodePatchPtr,
  IN OUT   AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
F15OrHtcInitEarlySampleHook (
  IN OUT   UINT32  *HtcRegister,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
F15OrIsCpbDisabledEarlySample (
  IN OUT   BOOLEAN            *IsEnabled,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
F15OrIsC6DisabledEarlySample (
  IN OUT   BOOLEAN            *IsEnabled,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
F15OrEarlySamplesAvoidNbCyclesStart (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   UINT64            *SavedMsrValue
  );

VOID
F15OrEarlySamplesAvoidNbCyclesEnd (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       UINT64            *SavedMsrValue
  );

VOID
F15OrEarlySamplesAfterPatchLoaded (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       BOOLEAN IsPatchLoaded
  );

BOOLEAN
F15OrEarlySamplesLoadMicrocodePatch (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );




/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
F15OrB0WeightsInit (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          D A T A    D E C L A R A T I O N S
 *----------------------------------------------------------------------------------------
 */

/*-----------------------------
 * Early Sample PCI registers
 *-----------------------------
 */

STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F15OrEarlySamplePciRegisters[] =
{
// F3x188 - NB Configuration 2 Register
// bit[30] Reserved = 1 Erratum #620, only on OR A0, A1 and B0
  {
    PciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_LT_B1                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x188), // Address
      0x40000000,                           // regData
      0x40000000,                           // regMask
    }}
  },
// F3x18C - Reserved
// bit[31] Reserved = 1 Erratum #603, only on OR A0, A1 and B0
  {
    PciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_LT_B1                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x18C), // Address
      0x80000000,                           // regData
      0x80000000,                           // regMask
    }}
  },

// F3x1B8 - L3 Control 1
// bit[7]  Reserved = 1, Erratum #574
// bit[29] Reserved = 1, Erratum #574
  {
    ProfileFixup,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_Ax                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      PERFORMANCE_L3_CACHE,                 // Features
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x1B8), // Address
      0x20000080,                           // regData
      0x20000080,                           // regMask
    }}
  },
// F4x110 - Sample and Residency Timers
// bits[20:13] MinResTmr = 0x64
  {
    PciRegister,
    {
      AMD_FAMILY_15_OR,                   // CpuFamily
      AMD_F15_OR_Ax                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x110), // Address
      0x000C8000,                           // regData
      0x001FE000,                           // regMask
    }}
  },
// F4x1A0 - Reserved
// bits[31:0]  Reserved = 4
  {
    ProfileFixup,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_A0                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      PERFORMANCE_L3_CACHE,                 // Features
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x1A0),  // Address
      0x00000004,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
// F4x1A4 - Reserved
// bits[31:0]  Reserved = 0x24 Erratum #553
  {
    ProfileFixup,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_A0                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      PERFORMANCE_L3_CACHE,                 // Features
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x1A4),  // Address
      0x00000024,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
};

CONST REGISTER_TABLE ROMDATA F15OrEarlySamplePciRegisterTable = {
  PrimaryCores,
  (sizeof (F15OrEarlySamplePciRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F15OrEarlySamplePciRegisters,
};

/*-----------------------------
 * Early Sample MSR registers
 *-----------------------------
 */

STATIC CONST MSR_TYPE_ENTRY_INITIALIZER ROMDATA F15OrEarlySampleMsrRegisters[] =
{
// MSR_LS_CFG (0xC0011020)
// bit[0] = 1, Erratum #500 for OR-A0 only
// bit[4] = 1, Erratum #501 for OR-A0 only
// bit[28] DisSS = 1, Erratum #495, #496 for OR-A0 only
// bit[30] = 1, Erratum #544 for OR-A0 only
// bit[62] = 1, Erratum #494 for OR-A0 only
  {
    MsrRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_A0                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MSR_LS_CFG,                           // MSR Address
      0x4000000050000011,                   // OR Mask
      0x4000000050000011,                   // NAND Mask
    }}
  },
// MSR_DC_CFG (0xC0011022)
// bit[13] DisHwPf = 1, Erratum #498, OR-A0 only
// bit[10] = 1, Erratum #575, OR-A0 only
  {
    MsrRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_A0                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MSR_DC_CFG,                           // MSR Address
      0x0000000000002400,                   // OR Mask
      0x0000000000002400,                   // NAND Mask
    }}
  },
// MSR_DE_CFG (0xC0011029)
// bit[7:2] = 111111b, Erratum #497, OR-A0 only
  {
    MsrRegister,
    {
      AMD_FAMILY_15,                    // CpuFamily
      AMD_F15_OR_A0                     // CpuRevision
    },
    {AMD_PF_ALL},                       // platformFeatures
    {{
      MSR_DE_CFG,                       // MSR Address - Shared
      0x00000000000000FC,               // OR Mask
      0x00000000000000FC,               // NAND Mask
    }}
  },
};

CONST REGISTER_TABLE ROMDATA F15OrEarlySampleMsrRegisterTable = {
  AllCores,
  (sizeof (F15OrEarlySampleMsrRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) &F15OrEarlySampleMsrRegisters,
};

/*-----------------------------
 * Early Sample Shared MSR registers
 *-----------------------------
 */

STATIC CONST MSR_TYPE_ENTRY_INITIALIZER ROMDATA F15OrEarlySampleSharedMsrRegisters[] =
{
// MSR_CU_CFG2 (0xC001102A)
// bit[27] = 1, Erratum #572, OR-Ax only
  {
    MsrRegister,
    {
      AMD_FAMILY_15,                    // CpuFamily
      AMD_F15_OR_Ax                     // CpuRevision
    },
    {AMD_PF_ALL},                       // platformFeatures
    {{
      MSR_CU_CFG2,                      // MSR Address - Shared
      0x0000000008000000,               // OR Mask
      0x0000000008000000,               // NAND Mask
    }}
  },

// MSR_CU_CFG3 (0xC001102B)
// bit[34] Reserved = 1, Erratum #568, OR-Ax only
  {
    MsrRegister,
    {
      AMD_FAMILY_15,                    // CpuFamily
      AMD_F15_OR_Ax                     // CpuRevision
    },
    {AMD_PF_ALL},                       // platformFeatures
    {{
      MSR_CU_CFG3,                      // MSR Address
      0x0000000400000000,               // OR Mask
      0x0000000400000000,               // NAND Mask
    }}
  },
// MSR_C001_1070
// bit[41] = 0, Erratum #597, OR-Ax only
  {
    MsrRegister,
    {
      AMD_FAMILY_15,                    // CpuFamily
      AMD_F15_OR_Ax                     // CpuRevision
    },
    {AMD_PF_ALL},                       // platformFeatures
    {{
      MSR_C001_1070,                    // MSR Address - Shared
      0x0000000000000000,               // OR Mask
      0x0000020000000000,               // NAND Mask
    }}
  },
};

CONST REGISTER_TABLE ROMDATA F15OrEarlySampleSharedMsrRegisterTable = {
  CorePairPrimary,
  (sizeof (F15OrEarlySampleSharedMsrRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) &F15OrEarlySampleSharedMsrRegisters,
};

/*-----------------------------
 * Early Sample Workarounds
 *-----------------------------
 */

STATIC CONST FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_INITIALIZER ROMDATA F15OrEarlySampleWorkarounds[] =
{
  // HT PHY DLL Compensation setting for Ax
  {
    FamSpecificWorkaround,
    {
      AMD_FAMILY_15,
      AMD_F15_OR_Ax
    },
    {AMD_PF_ALL},
    {{
      F15HtPhyOverrideDllCompensation,
      0x00000000
    }}
  },
  // CPU TDP Limit 2 setting for Ax
  {
    FamSpecificWorkaround,
    {
      AMD_FAMILY_15,
      AMD_F15_OR_Ax
    },
    {AMD_PF_ALL},
    {{
      F15OrOverrideNodeTdpLimit,
      0x00000000
    }}
  },
  // CPU Node TDP Accumulator Throttle Threshold setting for Ax
  {
    FamSpecificWorkaround,
    {
      AMD_FAMILY_15_OR,
      AMD_F15_OR_Ax
    },
    {AMD_PF_ALL},
    {{
      F15OrOverrideNodeTdpAccumulatorThrottleThreshold,
      0x00000000
    }}
  },
};

CONST REGISTER_TABLE ROMDATA F15OrEarlySampleWorkaroundsTable = {
  PrimaryCores,
  (sizeof (F15OrEarlySampleWorkarounds) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *)F15OrEarlySampleWorkarounds,
};

/*-----------------------------
 * Early Sample shared MSRs with Special Programming Requirements Table
 *-----------------------------
 */

STATIC CONST FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_INITIALIZER ROMDATA F15OrEarlySampleSharedMsrWorkarounds[] =
{
  // MSRC001_1072
  {
    FamSpecificWorkaround,
    {
      AMD_FAMILY_15_OR,
      AMD_F15_OR_B0
    },
    {AMD_PF_ALL},
    {{
      F15OrB0WeightsInit,
      0x00000000
    }}
  }
};

CONST REGISTER_TABLE ROMDATA F15OrEarlySampleSharedMsrWorkaroundTable = {
  CorePairPrimary,
  (sizeof (F15OrEarlySampleSharedMsrWorkarounds) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) &F15OrEarlySampleSharedMsrWorkarounds,
};


CONST UINT32 ROMDATA F15OrB0WeightsTable [] = {
  0x1300005A,              //MSRC001_1072_x00
  0x10ABD100,              //MSRC001_1072_x01
  0xBF1A1A44,              //MSRC001_1072_x02
  0xC4DABEA4,              //MSRC001_1072_x03
  0x147B7B6A,              //MSRC001_1072_x04
  0x320C0C00,              //MSRC001_1072_x05
  0xE6D6C6DC,              //MSRC001_1072_x06
  0x00911C06,              //MSRC001_1072_x07
  0x1F473727,              //MSRC001_1072_x08
  0x9FA3A32B,              //MSRC001_1072_x09
  0xDFCFBFAF,              //MSRC001_1072_x0A
  0xCFBFAF9F,              //MSRC001_1072_x0B
  0x606060DF,              //MSRC001_1072_x0C
  0x00000060,              //MSRC001_1072_x0D
  0xBAAA9A00,              //MSRC001_1072_x0E
  0xFF00DACA,              //MSRC001_1072_x0F
  0xFEFEFF64,              //MSRC001_1072_x10
  0x41FCFEFE,              //MSRC001_1072_x11
  0xE14C2F0D,              //MSRC001_1072_x12
  0x95A371EA,              //MSRC001_1072_x13
  0x002EE260,              //MSRC001_1072_x14
  0x00F907D2,              //MSRC001_1072_x15
  0xF9F2A5A5,              //MSRC001_1072_x16
  0x97C100E3,              //MSRC001_1072_x17
  0x91C5B577,              //MSRC001_1072_x18
  0x95C1B1A1,              //MSRC001_1072_x19
  0x68584800,              //MSRC001_1072_x1A
  0x67000000,              //MSRC001_1072_x1B
  0xB2003109,              //MSRC001_1072_x1C
  0x3F8DCDC4,              //MSRC001_1072_x1D
  0xD2D4D409,              //MSRC001_1072_x1E
  0x090000D2,              //MSRC001_1072_x1F
  0x00160000,              //MSRC001_1072_x20
  0x0000E300               //MSRC001_1072_x21
};


/*---------------------------------------------------------------------------------------*/
/**
 *  Early sample hook point during HTC initialization
 *
 *  @param[in,out] HtcRegister             Value of F3x64 to be written.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
VOID
F15OrHtcInitEarlySampleHook (
  IN OUT   UINT32  *HtcRegister,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 MsrAddr;
  UINT64 Msr;

  if (((HTC_REGISTER *) HtcRegister)->HtcPstateLimit == 0) {
    // HtcPstateLimit is set to Pb0.  Reprogram it to the minimum enabled P-state with
    // with NbPstate = 0
    for (MsrAddr = PS_MAX_REG; MsrAddr > PS_MIN_REG; MsrAddr--) {
      LibAmdMsrRead (MsrAddr, &Msr, StdHeader);
      if ((((PSTATE_MSR *) &Msr)->PsEnable == 1) && (((PSTATE_MSR *) &Msr)->NbPstate == 0)) {
        break;
      }
    }
    ((HTC_REGISTER *) HtcRegister)->HtcPstateLimit = (MsrAddr - PS_MIN_REG);
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Is CPB supported on this CPU
 *
 * @param[in,out] IsEnabled          Whether or not CPB should be enabled.
 * @param[in]     StdHeader          Config Handle for library, services.
 *
 */
VOID
F15OrIsCpbDisabledEarlySample (
  IN OUT   BOOLEAN            *IsEnabled,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  CPU_LOGICAL_ID LogicalId;

  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  // Check if this CPU is OR A0, then disable CPB support.
  if ((LogicalId.Revision & AMD_F15_OR_A0) != 0) {
    *IsEnabled = FALSE;
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Is C6 supported on this CPU
 *
 * @param[in,out] IsEnabled          Whether or not C6 should be enabled.
 * @param[in]     StdHeader          Config Handle for library, services.
 *
 */
VOID
F15OrIsC6DisabledEarlySample (
  IN OUT   BOOLEAN            *IsEnabled,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  CPU_LOGICAL_ID LogicalId;

  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  // Check if this CPU is OR A0, then disable C6 support.
  if ((LogicalId.Revision & AMD_F15_OR_A0) != 0) {
    *IsEnabled = FALSE;
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Update the weights for affected OR B0 CPUs.
 *
 * This function implements a workaround for OR B0 when applicable.
 *
 * @param[in]   Data         The table data value, for example to indicate which CPU and Platform types matched.
 * @param[in]   StdHeader    Config handle for library and services.
 *
 */
VOID
F15OrB0WeightsInit (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32       i;
  UINT32       ProductInfo;
  UINT64       LocalMsr;
  PCI_ADDR     PciAddress;

  if (IsWarmReset (StdHeader)) {
    OptionMultiSocketConfiguration.GetCurrPciAddr (&PciAddress, StdHeader);
    PciAddress.Address.Function = FUNC_3;
    PciAddress.Address.Register = PRCT_INFO_REG;
    LibAmdPciRead (AccessWidth32, PciAddress, &ProductInfo, StdHeader);

    if ((ProductInfo & BIT31) == 0) {
      for (i = 0; i < ((sizeof F15OrB0WeightsTable) / (sizeof F15OrB0WeightsTable[0])); i++) {
        LocalMsr = (((((UINT64) F15OrB0WeightsTable[i]) << 32) | i) | BIT14);
        LibAmdMsrWrite (0xC0011072, &LocalMsr, StdHeader);
      }
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *  Workaround to avoid patch loading from causing NB cycles
 *
 *
 *  @param[in,out] StdHeader     - Config handle for library and services.
 *  @param[in,out] SavedMsrValue - Saved a MSR value
 *
 */
VOID
F15OrEarlySamplesAvoidNbCyclesStart (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   UINT64            *SavedMsrValue
  )
{
  UINT64         MsrValue;
  CPU_LOGICAL_ID LogicalId;

  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  // Check if this CPU is OR Ax, expected fix in OR-B0
  if ((LogicalId.Revision & AMD_F15_OR_Ax) != 0) {
    // Workaround for F15 OR-Ax workaround to avoid patch loading from causing NB cycles
    // Start - Set MSR C001_102A [8]
    LibAmdMsrRead (MSR_BU_CFG2, SavedMsrValue, StdHeader);
    MsrValue = *SavedMsrValue | BIT8;
    LibAmdMsrWrite (MSR_BU_CFG2, &MsrValue, StdHeader);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *  Workaround to avoid patch loading from causing NB cycles
 *
 *
 *  @param[in,out] StdHeader     - Config handle for library and services.
 *  @param[in] SavedMsrValue - Saved a MSR value
 *
 */
VOID
F15OrEarlySamplesAvoidNbCyclesEnd (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       UINT64            *SavedMsrValue
  )
{
  CPU_LOGICAL_ID LogicalId;

  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  if ((LogicalId.Revision & AMD_F15_OR_Ax) != 0) {
    // Restore Workaround for F15 OR-Ax workaround to avoid patch loading from causing NB cycles
    // End - Restore MSR C001_102A
    LibAmdMsrWrite (MSR_BU_CFG2, SavedMsrValue, StdHeader);
  }

}

/* -----------------------------------------------------------------------------*/
/**
 *  Workaround for Ax processors after patch is loaded.
 *
 *
 *  @param[in] StdHeader     - Config handle for library and services.
 *  @param[in] IsPatchLoaded - Is patch loaded
 *
 */
VOID
F15OrEarlySamplesAfterPatchLoaded (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       BOOLEAN IsPatchLoaded
  )
{
  UINT64         MsrValue;
  CPU_LOGICAL_ID LogicalId;

  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);

  // MSR C001_1023[4:3] = 11b
  // Erratum #502, OR-A0 only after microcode patch has been loaded
  if (((LogicalId.Revision & AMD_F15_OR_A0) != 0) && (IsPatchLoaded)) {
    LibAmdMsrRead (MSR_CU_CFG, &MsrValue, StdHeader);
    MsrValue |= 0x18;
    LibAmdMsrWrite (MSR_CU_CFG, &MsrValue, StdHeader);
  }

  // Erratum #590, OR-A1 only, if any patch is applied
  // MSR C001_0028 = 0x2E00_0080
  // MSR C001_0029 = 0xFE00_0080
  // MSR C001_002C = 0x0400_1029
  if (((LogicalId.Revision & AMD_F15_OR_A1) != 0) && (IsPatchLoaded)) {
    MsrValue = 0x2E000080;
    LibAmdMsrWrite (0xC0010028, &MsrValue, StdHeader);

    MsrValue = 0xFE000080;
    LibAmdMsrWrite (0xC0010029, &MsrValue, StdHeader);

    MsrValue = 0x04001029;
    LibAmdMsrWrite (0xC001002C, &MsrValue, StdHeader);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *  Update microcode patch in current processor.
 *
 *  Then reads the patch id, and compare it to the expected, in the Microprocessor
 *  patch block.
 *
 *  @param[in] StdHeader   - Config handle for library and services.
 *
 *  @retval    TRUE   - Patch Loaded Successfully.
 *  @retval    FALSE  - Patch Did Not Get Loaded.
 *
 */
BOOLEAN
F15OrEarlySamplesLoadMicrocodePatch (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8    PatchNumber;
  UINT8    TotalPatches;
  UINT16   ProcessorEquivalentId;
  BOOLEAN  Status;
  MICROCODE_PATCH **MicrocodePatchPtr;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  Status = FALSE;

  if (IsCorePairPrimary (FirstCoreIsComputeUnitPrimary, StdHeader)) {
    // Get the patch pointer
    GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
    FamilySpecificServices->GetMicroCodePatchesStruct (FamilySpecificServices, (const VOID **) &MicrocodePatchPtr, &TotalPatches, StdHeader);

    IDS_OPTION_HOOK (IDS_UCODE, &TotalPatches, StdHeader);

    // Get the processor microcode path equivalent ID
    if (GetPatchEquivalentId (&ProcessorEquivalentId, StdHeader)) {
      // parse the patch table to see if we have one for the current cpu
      for (PatchNumber = 0; PatchNumber < TotalPatches; PatchNumber++) {
        if (ValidateMicrocode (MicrocodePatchPtr[PatchNumber], ProcessorEquivalentId, StdHeader)) {
          if (F15OrEarlySamplesLoadMicrocode (MicrocodePatchPtr[PatchNumber], StdHeader)) {
            Status = TRUE;
          } else {
            PutEventLog (AGESA_ERROR,
                         CPU_ERROR_MICRO_CODE_PATCH_IS_NOT_LOADED,
                         0, 0, 0, 0, StdHeader);
          }
          break; // Once we find a microcode patch that matches the processor, exit the for loop
        }
      }
    }
  }
  return Status;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  F15OrEarlySamplesLoadMicrocode
 *
 *    Update microcode patch in current processor, then reads the
 *    patch id, and compare it to the expected, in the Microprocessor
 *    patch block.
 *
 *    Note: This is a special version of the normal LoadMicrocode()
 *    function which lives in cpuMicrocodePatch.c. This version
 *    implements a workaround (on Or-B0 only) before applying the
 *    microcode patch.
 *
 *    @param[in]       MicrocodePatchPtr  - Pointer to Microcode Patch.
 *    @param[in,out]   StdHeader          - Pointer to AMD_CONFIG_PARAMS struct.
 *
 *    @retval          TRUE  - Patch Loaded Successfully.
 *    @retval          FALSE - Patch Did Not Get Loaded.
 *
 */
BOOLEAN
F15OrEarlySamplesLoadMicrocode (
  IN       MICROCODE_PATCH    *MicrocodePatchPtr,
  IN OUT   AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32       MicrocodeVersion;
  UINT64       MsrData;
  PATCH_LOADER PatchLoaderMsr;
  CPU_LOGICAL_ID LogicalId;

  // Load microcode patch into CPU
  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  PatchLoaderMsr.RawData = (UINT64)(intptr_t) MicrocodePatchPtr;
  PatchLoaderMsr.BitFields.SBZ = 0;
  // Check if this CPU is OR-B0, expected fix in OR-B1
  if ((LogicalId.Revision & AMD_F15_OR_B0) != 0) {
    LibAmdMsrRead (MSR_BR_FROM, &MsrData, StdHeader);
  }

  LibAmdMsrWrite (MSR_PATCH_LOADER, &PatchLoaderMsr.RawData, StdHeader);

  // Do ucode patch Authentication
  // Read microcode version back from CPU, determine if
  // it is the same patch level as contained in the source
  // microprocessor patch block passed in
  GetMicrocodeVersion (&MicrocodeVersion, StdHeader);
  if (MicrocodeVersion == MicrocodePatchPtr->PatchID) {
    return (TRUE);
  } else {
    return (FALSE);
  }
}

/*---------------------------------------------------------------------------------------
 *                           L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

