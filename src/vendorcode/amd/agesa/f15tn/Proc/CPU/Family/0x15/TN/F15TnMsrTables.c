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
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
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

