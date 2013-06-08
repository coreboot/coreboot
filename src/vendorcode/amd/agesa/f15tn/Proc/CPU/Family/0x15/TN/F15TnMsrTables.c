/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Trinity MSR tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/TN
 * @e \$Revision: 63495 $   @e \$Date: 2011-12-23 01:30:59 -0600 (Fri, 23 Dec 2011) $
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
#include "cpuF15TnPowerMgmt.h"
#include "F15TnPackageType.h"
#include "Table.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_TN_F15TNMSRTABLES_FILECODE


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
STATIC
SetTopologyExtensions (
  IN       UINT32            Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
SetForceSmcCheckFlwStDis (
  IN       UINT32            Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
STATIC CONST MSR_TYPE_ENTRY_INITIALIZER ROMDATA F15TnMsrRegisters[] =
{
//  M S R    T a b l e s
// ----------------------

// MSR_NB_CFG (0xC001001F)
// bit[23] = 1, erratum #663
  {
    MsrRegister,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                      // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                            // platformFeatures
    {{
      MSR_NB_CFG,                            // MSR Address
      0x0000000000800000,                    // OR Mask
      0x0000000000800000,                    // NAND Mask
    }}
  },

// MSR_LS_CFG2 (0xC001102D)
// bit[23] DisScbThreshold = 1
  {
    MsrRegister,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                      // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MSR_LS_CFG2,                           // MSR Address
      0x0000000000800000,                    // OR Mask
      0x0000000000800000,                    // NAND Mask
    }}
  },
// MSR_HWCR (0xC0010015)
// bit[27] EffFreqReadOnlyLock = 1
// bit[12] HltXSpCycEn = 1
  {
    MsrRegister,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                      // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MSR_HWCR,                              // MSR Address
      0x0000000008001000,                    // OR Mask
      0x0000000008001000,                    // NAND Mask
    }}
  },
// MSR_OSVW_ID_Length (0xC0010140)
// bit[15:0] = 4
  {
    MsrRegister,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                      // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MSR_OSVW_ID_Length,                    // MSR Address
      0x0000000000000004,                    // OR Mask
      0x000000000000FFFF,                    // NAND Mask
    }}
  },
// MSR 0xC0011000
// bit[17] = 1, Disable Erratum #671
// bit[16] = 1, Erratum #608 for all TN revisions
  {
    MsrRegister,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                      // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                         // platformFeatures
    {{
      0xC0011000,                            // MSR Address
      0x0000000000030000,                    // OR Mask
      0x0000000000030000,                    // NAND Mask
    }}
  },
// MSR_CPUID_EXT_FEATS (0xC0011005)
// bit[51]  NodeId = 1
  {
    MsrRegister,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                      // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                         // platformFeatures
    {{
      MSR_CPUID_EXT_FEATS,                   // MSR Address
      0x0008000000000000,                    // OR Mask
      0x0008000000000000,                    // NAND Mask
    }}
  },
};

CONST REGISTER_TABLE ROMDATA F15TnMsrRegisterTable = {
  AllCores,
  (sizeof (F15TnMsrRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) &F15TnMsrRegisters,
};

// MSR with Special Programming Requirements Table

STATIC CONST FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_INITIALIZER ROMDATA F15TnMsrWorkarounds[] =
{
// MSR_C001_1005
  {
    FamSpecificWorkaround,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                        // CpuFamily
      AMD_F15_TN_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      SetTopologyExtensions,                // function call
      0x00000000,                           // data
    }}
  },
// MSR_C001_102D
  {
    FamSpecificWorkaround,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                        // CpuFamily
      AMD_F15_TN_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      SetForceSmcCheckFlwStDis,             // function call
      0x00000000,                           // data
    }}
  },
};

CONST REGISTER_TABLE ROMDATA F15TnMsrWorkaroundTable = {
  AllCores,
  (sizeof (F15TnMsrWorkarounds) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) F15TnMsrWorkarounds,
};

/*---------------------------------------------------------------------------------------*/
/**
 *  MSR special programming requirements for MSR_C001_1005
 *
 *  AGESA should program MSR_C001_1005[54, TopologyExtensions] as follows:
 *  IF (CPUID Fn8000_0001_EBX[PkgType]==0010b) THEN 0 ELSE 1 ENDIF.
 *
 * @param[in]   Data         The table data value, for example to indicate which CPU and Platform types matched.
 * @param[in]   StdHeader    Config handle for library and services.
 *
 */
VOID
STATIC
SetTopologyExtensions (
  IN       UINT32            Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 PkgType;
  UINT64 CpuMsrData;

  PkgType = LibAmdGetPackageType (StdHeader);
  LibAmdMsrRead (MSR_CPUID_EXT_FEATS, &CpuMsrData, StdHeader);
  CpuMsrData &= ~(BIT54);
  if (PkgType == PACKAGE_TYPE_FM2) {
    CpuMsrData |= BIT54;
  }
  LibAmdMsrWrite (MSR_CPUID_EXT_FEATS, &CpuMsrData, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  MSR special programming requirements for MSR_C001_102D
 *
 *  AGESA should program MSR_C001_102D[14] with the fused value from F3x1FC[23]
 *
 * @param[in]   Data         The table data value, for example to indicate which CPU and Platform types matched.
 * @param[in]   StdHeader    Config handle for library and services.
 *
 */
VOID
STATIC
SetForceSmcCheckFlwStDis (
  IN       UINT32            Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  PCI_ADDR PciAddress;
  PRODUCT_INFO_REGISTER ProductInfo;
  LS_CFG2_MSR LsCfg2;

  PciAddress.AddressValue = PRCT_INFO_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, (VOID *) &ProductInfo, StdHeader);

  LibAmdMsrRead (MSR_LS_CFG2, (UINT64 *) &LsCfg2, StdHeader);

  LsCfg2.ForceSmcCheckFlwStDis = ProductInfo.ForceSmcCheckFlwStDis;
  LibAmdMsrWrite (MSR_LS_CFG2, (UINT64 *) &LsCfg2, StdHeader);

  return;
}

