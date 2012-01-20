/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Orochi MSR tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/OR
 * @e \$Revision: 60740 $   @e \$Date: 2011-10-20 19:47:10 -0600 (Thu, 20 Oct 2011) $
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
#include "F15PackageType.h"
#include "cpuF15OrPowerMgmt.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_OR_F15ORMSRTABLES_FILECODE


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
F15OrDisUcodeWorkaroundForErratum671 (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
STATIC CONST MSR_TYPE_ENTRY_INITIALIZER ROMDATA F15OrMsrRegisters[] =
{
//  M S R    T a b l e s
// ----------------------

// MSR_MC4_CTL_MASK (0xC0010048)
// bit[10] GartTblWkEn = 1
// bits[22:19] RtryHtEn = 1111b
  {
    MsrRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_MC4_CTL_MASK,                      // MSR Address
      0x0000000000780400,                    // OR Mask
      0x0000000000780400,                    // NAND Mask
    }}
  },
// MSR 0xC0011000
// bit[16] = 1, Erratum #608 for all OR revisions
  {
    MsrRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                         // platformFeatures
    {{
      0xC0011000,                            // MSR Address
      0x0000000000010000,                    // OR Mask
      0x0000000000010000,                    // NAND Mask
    }}
  },
// MSR_CPUID_EXT_FEATS (0xC0011005)
// bit[56]  PerfCtrExtNB = 1
// bit[55]  PerfCtrExtCore = 1
// bit[51]  NodeId = 1
  {
    MsrRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                         // platformFeatures
    {{
      MSR_CPUID_EXT_FEATS,                   // MSR Address
      0x0188000000000000,                    // OR Mask
      0x0188000000000000,                    // NAND Mask
    }}
  },
// MSR_OSVW_ID_Length (0xC0010140)
// bit[15:0] = 4
  {
    MsrRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MSR_OSVW_ID_Length,                    // MSR Address
      0x0000000000000004,                    // OR Mask
      0x000000000000FFFF,                    // NAND Mask
    }}
  },
// MSR_IBS_OP_DATA3 (0xC0011037)
// bit[16] IbsDcMabHit = 0
  {
    MsrRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MSR_IBS_OP_DATA3,                      // MSR Address
      0x0000000000000000,                    // OR Mask
      0x0000000000010000,                    // NAND Mask
    }}
  }
};

// MSRs with Special Programming Requirements Table

STATIC CONST FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_INITIALIZER ROMDATA F15OrAM3MsrWorkarounds[] =
{
  // Disable Microcode workaround for Erratum #671
  {
    FamSpecificWorkaround,
    {
      AMD_FAMILY_15_OR,
      AMD_F15_OR_B2
    },
    {AMD_PF_ALL},
    {{
      F15OrDisUcodeWorkaroundForErratum671,
      0x00000000
    }}
  },
};


CONST REGISTER_TABLE ROMDATA F15OrMsrRegisterTable = {
  AllCores,
  (sizeof (F15OrMsrRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) &F15OrMsrRegisters,
};

CONST REGISTER_TABLE ROMDATA F15OrAM3MsrWorkaroundTable = {
  AllCores,
  (sizeof (F15OrAM3MsrWorkarounds) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) &F15OrAM3MsrWorkarounds,
};

/*---------------------------------------------------------------------------------------*/
/**
 * A Family Specific Workaround method, to disable the microcode workaround for Erratum #671
 *
 * \@TableTypeFamSpecificInstances.
 *
 * @param[in]     Data       The table data value, for example to indicate which CPU and Platform types matched.
 * @param[in]     StdHeader  Config params for library, services.
 */
VOID
F15OrDisUcodeWorkaroundForErratum671 (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT64        MsrData;
  UINT32        PackageType;

  // Is this processor AM3?
  PackageType = LibAmdGetPackageType (StdHeader);

  if (PackageType == PACKAGE_TYPE_AM3r2) {
    // Apply the enhancement.
    LibAmdMsrRead (0xC0011000, &MsrData, StdHeader);
    MsrData = (MsrData | BIT17);
    LibAmdMsrWrite (0xC0011000, &MsrData, StdHeader);
  }
}



