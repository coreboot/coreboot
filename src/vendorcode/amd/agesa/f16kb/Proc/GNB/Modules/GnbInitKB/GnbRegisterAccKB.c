/* $NoKeywords:$ */
/**
 * @file
 *
 * Initialize PP/DPM fuse table.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "amdlib.h"
#include  "Ids.h"
#include  "Gnb.h"
#include  "GnbPcieConfig.h"
#include  "GnbCommonLib.h"
#include  "GnbRegisterAccKB.h"
#include  "GnbRegistersKB.h"
#include  "GnbSmuInitLibV7.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITKB_GNBREGISTERACCKB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define GNB_IGNORED_PARAM             0xFF
#define ORB_WRITE_ENABLE              0x100
#define IOMMU_L1_WRITE_ENABLE         0x80000000ul
#define IOMMU_L2_WRITE_ENABLE         0x100


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
GnbRegisterWriteKBDump (
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       VOID                *Value
  );


/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to read all register spaces.
 *
 *
 *
 * @param[in] RegisterSpaceType  Register space type
 * @param[in] Address            Register offset, but PortDevice
 * @param[out] Value             Return value
 * @param[in] Flags              Flags - BIT0 indicates S3 save/restore
 * @param[in] StdHeader          Standard configuration header
 */
AGESA_STATUS
GnbRegisterReadKB (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
     OUT   VOID                *Value,
  IN       UINT32              Flags,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  ACCESS_WIDTH  Width;
  UINT32        TempValue;
  UINT32        TempAddress;
  PCI_ADDR      GnbPciAddress;

  GnbPciAddress = GnbGetHostPciAddress (GnbHandle);
  Width = (Flags == GNB_REG_ACC_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;
  TempAddress = 0;
  TempValue = 0;


  switch (RegisterSpaceType) {
  case 0x1:
    GnbLibPciRead (
      GnbPciAddress.AddressValue | Address,
      Width,
      Value,
      StdHeader
    );
    break;
  case 0x8:
    GnbPciAddress.Address.Function = 2;
    GnbLibPciRead (
      GnbPciAddress.AddressValue | Address,
      Width,
      Value,
      StdHeader
    );
    break;
  case 0x6:  ///@todo has to be DxFx
    GnbLibPciRead (
      Address,
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_D18F0:
    GnbLibPciRead (
      MAKE_SBDFO (0, 0, 0x18, 0, Address),
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_D18F1:
    GnbLibPciRead (
      MAKE_SBDFO (0, 0, 0x18, 1, Address),
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_D18F2:
  case TYPE_D18F2_dct0:
    GnbLibPciRead (
      MAKE_SBDFO (0, 0, 0x18, 2, Address),
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_D18F3:
    GnbLibPciRead (
      MAKE_SBDFO (0, 0, 0x18, 3, Address),
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_D18F4:
    GnbLibPciRead (
      MAKE_SBDFO (0, 0, 0x18, 4, Address),
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_D18F5:
    GnbLibPciRead (
      MAKE_SBDFO (0, 0, 0x18, 5, Address),
      Width,
      Value,
      StdHeader
    );
    break;

  case 0x2:
    // Miscellaneous Index Data, access the registers D0F0x64_x[FF:00]
    GnbLibPciIndirectRead (
      GnbPciAddress.AddressValue | D0F0x60_ADDRESS,
      Address,
      Width,
      Value,
      StdHeader
    );
    break;

  case 0x3:
    // Northbridge ORB Configuration Offset, access D0F0x98_x[FF:00]
    GnbLibPciIndirectRead (
      GnbPciAddress.AddressValue | D0F0x94_ADDRESS,
      Address,
      Width,
      Value,
      StdHeader
    );
    break;

  case 0x4:
    {
      UINT64  TempData;
      GnbLibPciIndirectRead (
        GnbPciAddress.AddressValue | D0F0xB8_ADDRESS,
        (Address & (~0x3ull)),
        Width,
        &TempData,
        StdHeader
      );
      if ((Address & 0x3) != 0) {
        //Non aligned access allowed to fuse block
        GnbLibPciIndirectRead (
          GnbPciAddress.AddressValue | D0F0xB8_ADDRESS,
          (Address & (~0x3ull)) + 4,
          Width,
          ((UINT32 *) &TempData) + 1,
          StdHeader
        );
      }
      * ((UINT32*) Value) = (UINT32) (TempData >> ((Address & 0x3) * 8));
      break;
    }
  case 0x22:
    // D0F0xD0 Link Index Address, access D0F0xD4_x[0130_14BF:0109_0000]
    GnbLibPciIndirectRead (
      GnbPciAddress.AddressValue | 0xD0,
      Address,
      Width,
      Value,
      StdHeader
    );
    break;

  case 0x5:
    // D0F0xE0 Link Index Address, access D0F0xE4_x[FFFF_FFFF:0000_0000]
    GnbLibPciIndirectRead (
      GnbPciAddress.AddressValue | D0F0xE0_ADDRESS,
      Address,
      Width,
      Value,
      StdHeader
    );
    break;

  case 0x9:
    GnbPciAddress.Address.Function = 2;
    GnbLibPciIndirectRead (
      GnbPciAddress.AddressValue | 0xF0,
      Address,
      Width,
      Value,
      StdHeader
    );
    break;

  case 0xa:
    GnbPciAddress.Address.Function = 2;
    GnbLibPciIndirectRead (
      GnbPciAddress.AddressValue | 0xF8,
      Address,
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_MSR:
    LibAmdMsrRead  (Address, Value, StdHeader);
    break;

  case 0x12:
    ASSERT (Address < 0x40000);
    // SRBM
    GnbLibPciIndirectRead (
      MAKE_SBDFO (0, 0, 0, 0, 0xB8),
      (0x80080000 | (Address & 0x3FFFF)),
      Width,
      Value,
      StdHeader
    );
    break;

  default:
    ASSERT (FALSE);
    break;
  }
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to write all register spaces.
 *
 *
 * @param[in] GnbHandle          GnbHandle
 * @param[in] RegisterSpaceType  Register space type
 * @param[in] Address            Register offset, but PortDevice
 * @param[out] Value             The value to write
 * @param[in] Flags              Flags - BIT0 indicates S3 save/restore
 * @param[in] StdHeader          Standard configuration header
 */
AGESA_STATUS
GnbRegisterWriteKB (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{

  ACCESS_WIDTH    Width;
  UINT32          TempValue;
  UINT32          TempAddress;
  PCI_ADDR        GnbPciAddress;
  DEV_OBJECT      DevObject;

  GnbPciAddress = GnbGetHostPciAddress (GnbHandle);
  Width = (Flags == GNB_REG_ACC_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;
  TempAddress = 0;
  TempValue = 0;

  GNB_DEBUG_CODE (
    GnbRegisterWriteKBDump (RegisterSpaceType, Address, Value);
  );

  switch (RegisterSpaceType) {
  case 0x1:
    GnbLibPciWrite (
      GnbPciAddress.AddressValue | Address,
      Width,
      Value,
      StdHeader
    );
    break;
  case 0x8:
    GnbPciAddress.Address.Function = 2;
    GnbLibPciWrite (
      GnbPciAddress.AddressValue | Address,
      Width,
      Value,
      StdHeader
    );
    break;
  case 0x6:  /// @todo needs to be DxFx
    GnbLibPciWrite (
      Address,
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_D18F0:
    GnbLibPciWrite (
      MAKE_SBDFO (0, 0, 0x18, 0, Address),
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_D18F1:
    GnbLibPciWrite (
      MAKE_SBDFO (0, 0, 0x18, 1, Address),
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_D18F2:
  case TYPE_D18F2_dct0:
    GnbLibPciWrite (
      MAKE_SBDFO (0, 0, 0x18, 2, Address),
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_D18F3:
    GnbLibPciWrite (
      MAKE_SBDFO (0, 0, 0x18, 3, Address),
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_D18F4:
    GnbLibPciWrite (
      MAKE_SBDFO (0, 0, 0x18, 4, Address),
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_D18F5:
    GnbLibPciWrite (
      MAKE_SBDFO (0, 0, 0x18, 5, Address),
      Width,
      Value,
      StdHeader
    );
    break;

  case 0x2:
    // Miscellaneous Index Data, access the registers D0F0x64_x[FF:00]
    GnbLibPciIndirectWrite (
      GnbPciAddress.AddressValue | D0F0x60_ADDRESS,
      Address | IOC_WRITE_ENABLE,
      Width,
      Value,
      StdHeader
    );
    break;

  case 0x3:
    // Northbridge ORB Configuration Offset, access D0F0x98_x[FF:00]
    GnbLibPciIndirectWrite (
      GnbPciAddress.AddressValue | D0F0x94_ADDRESS,
      Address | ORB_WRITE_ENABLE,
      Width,
      Value,
      StdHeader
    );
    break;

  case 0x4:
    //SMU, access D0F0xBC_x[FFFFFFFF:00000000]
    GnbLibPciIndirectWrite (
      GnbPciAddress.AddressValue | D0F0xB8_ADDRESS,
      Address,
      Width,
      Value,
      StdHeader
    );
    break;

  case 0x22:
    // D0F0xD0 Link Index Address, access D0F0xD4_x[0130_14BF:0109_0000]
    GnbLibPciIndirectWrite (
      GnbPciAddress.AddressValue | 0xD0,
      Address,
      Width,
      Value,
      StdHeader
    );
    break;

  case 0x5:
    // D0F0xE0 Link Index Address, access D0F0xE4_x[FFFF_FFFF:0000_0000]
    GnbLibPciIndirectWrite (
      GnbPciAddress.AddressValue | D0F0xE0_ADDRESS,
      Address,
      Width,
      Value,
      StdHeader
    );
    break;

  case 0x9:
    // IOMMU L2 Config Index, to access the registers D0F2xF4_x[FF:00].
    GnbPciAddress.Address.Function = 2;
    GnbLibPciIndirectWrite (
      GnbPciAddress.AddressValue | 0xF0,
      Address | IOMMU_L2_WRITE_ENABLE,
      Width,
      Value,
      StdHeader
    );
    break;

  case 0xa:
    GnbPciAddress.Address.Function = 2;
    GnbLibPciIndirectWrite (
      GnbPciAddress.AddressValue | 0xF8,
      Address | IOMMU_L1_WRITE_ENABLE,
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_MSR:
    LibAmdMsrWrite  (Address, Value, StdHeader);
    break;

  case 0x12:
    ASSERT (Address < 0x40000);
    // SRBM
    GnbLibPciIndirectWrite (
      MAKE_SBDFO (0, 0, 0, 0, 0xB8),
      (0x80080000 | (Address & 0x3FFFF)),
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_SMU_MSG:
    DevObject.StdHeader = StdHeader;
    DevObject.GnbHandle = GnbGetHandle (StdHeader);
    DevObject.DevPciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0);
    GnbSmuServiceRequestV7 (&DevObject, (UINT8) Address, *((UINT32 *)Value), Flags);
    break;

  default:
    ASSERT (FALSE);
    break;
  }

  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to dump all write register spaces.
 *
 *
 *
 * @param[in] RegisterSpaceType  Register space type
 * @param[in] Address            Register offset
 * @param[in] Value              The value to write
 */
VOID
GnbRegisterWriteKBDump (
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       VOID                *Value
  )
{
  IDS_HDT_CONSOLE (NB_MISC, "  R WRITE Space %s  Address 0x%04x, Value 0x%04x\n",
    (RegisterSpaceType == 0x1) ? "0x1" : (
    (RegisterSpaceType == 0x2) ? "0x2" : (
    (RegisterSpaceType == 0x3) ? "0x3" : (
    (RegisterSpaceType == 0x4) ? "0x4" : (
    (RegisterSpaceType == 0x22) ? "0x22" : (
    (RegisterSpaceType == 0x5) ? "0x5" : (
    (RegisterSpaceType == 0x6) ? "0x6" : (
    (RegisterSpaceType == TYPE_D18F0) ? "TYPE_D18F0" : (
    (RegisterSpaceType == TYPE_D18F1) ? "TYPE_D18F1" : (
    (RegisterSpaceType == TYPE_D18F2) ? "TYPE_D18F2" : (
    (RegisterSpaceType == TYPE_D18F3) ? "TYPE_D18F3" : (
    (RegisterSpaceType == TYPE_D18F4) ? "TYPE_D18F4" : (
    (RegisterSpaceType == TYPE_D18F5) ? "TYPE_D18F5" : (
    (RegisterSpaceType == TYPE_MSR) ? "TYPE_MSR" : (
    (RegisterSpaceType == TYPE_D1F0) ? "TYPE_D1F0" : (
    (RegisterSpaceType == TYPE_D1F1) ? "TYPE_D1F1" : (
    (RegisterSpaceType == 0x12) ? "0x12" : (
    (RegisterSpaceType == TYPE_D18F2x9C_dct0) ? "TYPE_D18F2x9C_dct0" : (
    (RegisterSpaceType == TYPE_D18F2x9C_dct0_mp0) ? "TYPE_D18F2x9C_dct0_mp0" : (
    (RegisterSpaceType == TYPE_D18F2x9C_dct0_mp1) ? "TYPE_D18F2x9C_dct0_mp1" : (
    (RegisterSpaceType == TYPE_D18F2x9C_dct1) ? "TYPE_D18F2x9C_dct1" : (
    (RegisterSpaceType == TYPE_D18F2x9C_dct1_mp0) ? "TYPE_D18F2x9C_dct1_mp0" : (
    (RegisterSpaceType == TYPE_D18F2x9C_dct1_mp1) ? "TYPE_D18F2x9C_dct1_mp1" : (
    (RegisterSpaceType == TYPE_D18F2_dct0) ? "TYPE_D18F2_dct0" : (
    (RegisterSpaceType == TYPE_D18F2_dct0_mp0) ? "TYPE_D18F2_dct0_mp0" : (
    (RegisterSpaceType == TYPE_D18F2_dct0_mp1) ? "TYPE_D18F2_dct0_mp1" : (
    (RegisterSpaceType == TYPE_D18F2_dct1) ? "TYPE_D18F2_dct1" : (
    (RegisterSpaceType == TYPE_D18F2_dct1_mp0) ? "TYPE_D18F2_dct1_mp0" : (
    (RegisterSpaceType == TYPE_SMU_MSG) ? "TYPE_SMU_MSG" : (
    (RegisterSpaceType == TYPE_D18F2_dct1_mp1) ? "TYPE_D18F2_dct1_mp1" : "Invalid"))))))))))))))))))))))))))))),
    Address,
    *((UINT32*)Value)
  );
}
