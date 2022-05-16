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
 * @e \$Revision: 64211 $   @e \$Date: 2012-01-17 23:00:25 -0600 (Tue, 17 Jan 2012) $
 *
 */
/*
*****************************************************************************
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
#include  "GnbPcie.h"
#include  "GnbCommonLib.h"
#include  "GnbNbInitLibV4.h"
#include  "GnbRegisterAccTN.h"
#include  "GnbRegistersTN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITTN_GNBREGISTERACCTN_FILECODE
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
GnbRegisterWriteTNDump (
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       VOID                *Value
  );
AGESA_STATUS
GnbRegisterReadServiceTN (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
     OUT   VOID                *Value,
  IN       UINT32              Flags,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

AGESA_STATUS
GnbRegisterWriteServiceTN (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );
/*----------------------------------------------------------------------------------------*/
/*
 *  Config Dct and Mp.
 *
 *
 *
 * @param[in] DctCfgSel          Dct0/Dct1
 * @param[in] MemPsSel           Mp0/Mp1
 * @param[in] StdHeader          Standard configuration header
 *
 * @return          true - Memory Pstate context has been changed
 * @return          false - Memory Pstate context has not been changed
 */
STATIC BOOLEAN
GnbDctMpConfigTN (
  IN       UINT8               DctCfgSel,
  IN       UINT8               MemPsSel,
  IN       UINT32              Flags,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  // Select DCT and memory P-state, D18F1x10C[DctCfgSel], D18F1x10C[MemPsSel]
  D18F1x10C_STRUCT      D18F1x10C;
  BOOLEAN               MemPsChangd;
  ACCESS_WIDTH          Width;

  MemPsChangd = FALSE;
  Width = (Flags == GNB_REG_ACC_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;

  GnbLibPciRead (
    MAKE_SBDFO (0, 0, 0x18, 1, D18F1x10C_ADDRESS),
    Width,
    &D18F1x10C.Value,
    StdHeader
  );

  if ((DctCfgSel != 0xFF) && (DctCfgSel < 2)) {
    D18F1x10C.Field.DctCfgSel = DctCfgSel;
  }

  if ((MemPsSel != 0xFF) && (MemPsSel < 2) && (D18F1x10C.Field.MemPsSel != MemPsSel)) {
    //Switches Mem Pstate
    D18F1x10C.Field.MemPsSel = MemPsSel;
    MemPsChangd = TRUE;
  }

  GnbLibPciWrite (
    MAKE_SBDFO (0, 0, 0x18, 1, D18F1x10C_ADDRESS),
    Width,
    &D18F1x10C.Value,
    StdHeader
  );

  return MemPsChangd;

}

/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to Read Dct Additional Data.
 *
 *
 *
 * @param[in] Address            D18F2x9c Register offset
 * @param[in] DctCfgSel          Dct0/Dct1
 * @param[in] MemPsSel           Mp0/Mp1
 * @param[out] Value             Read value
 * @param[in] StdHeader          Standard configuration header
 */
STATIC VOID
GnbDctAdditionalDataReadTN (
  IN       UINT32              Address,
  IN       UINT8               DctCfgSel,
  IN       UINT8               MemPsSel,
  IN       UINT32              Flags,
     OUT   VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  D18F2x98_dct0_STRUCT  D18F2x98;
  BOOLEAN               PstateChanged;
  ACCESS_WIDTH          Width;

  Width = (Flags == GNB_REG_ACC_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;

  PstateChanged = GnbDctMpConfigTN (
                    DctCfgSel,
                    MemPsSel,
                    Flags,
                    StdHeader
                  );

  // Clear DctAccessWrite
  D18F2x98.Field.DctOffset = Address & 0x3FFFFFFF;
  D18F2x98.Field.DctAccessWrite = 0;

  GnbLibPciWrite (
    MAKE_SBDFO (0, 0, 0x18, 2, ((DctCfgSel == 0) ? D18F2x98_dct0_ADDRESS : D18F2x98_dct1_ADDRESS)),
    Width,
    &D18F2x98.Value,
    StdHeader
  );

  GnbLibPciRead (
    MAKE_SBDFO (0, 0, 0x18, 2, ((DctCfgSel == 0) ? D18F2x9C_dct0_ADDRESS : D18F2x9C_dct1_ADDRESS)),
    Width,
    Value,
    StdHeader
  );

  if (PstateChanged) {
    GnbDctMpConfigTN (
      DctCfgSel,
      ((MemPsSel == 0) ? 1 : 0),
      Flags,
      StdHeader
      );
  }
}

/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to Write Dct Additional Data.
 *
 *
 *
 * @param[in] Address            D18F2x9c Register offset
 * @param[in] DctCfgSel          Dct0/Dct1
 * @param[in] MemPsSel           Mp0/Mp1
 * @param[in] Value              Write value
 * @param[in] StdHeader          Standard configuration header
 */
STATIC VOID
GnbDctAdditionalDataWriteTN (
  IN       UINT32              Address,
  IN       UINT8               DctCfgSel,
  IN       UINT8               MemPsSel,
  IN       UINT32              Flags,
  CONST IN       VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  D18F2x98_dct0_STRUCT  D18F2x98;
  BOOLEAN               PstateChanged;
  ACCESS_WIDTH          Width;

  Width = (Flags == GNB_REG_ACC_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;

  PstateChanged = GnbDctMpConfigTN (
                    DctCfgSel,
                    MemPsSel,
                    Flags,
                    StdHeader
                  );

  // Put write data on
  GnbLibPciWrite (
    MAKE_SBDFO (0, 0, 0x18, 2, ((DctCfgSel == 0) ? D18F2x9C_dct0_ADDRESS : D18F2x9C_dct1_ADDRESS)),
    Width,
    Value,
    StdHeader
  );

  // Set DctAccessWrite
  D18F2x98.Field.DctOffset = Address & 0x3FFFFFFF;
  D18F2x98.Field.DctAccessWrite = 1;

  GnbLibPciWrite (
    MAKE_SBDFO (0, 0, 0x18, 2, ((DctCfgSel == 0) ? D18F2x98_dct0_ADDRESS : D18F2x98_dct1_ADDRESS)),
    Width,
    &D18F2x98.Value,
    StdHeader
  );

  if (PstateChanged) {
    GnbDctMpConfigTN (
      DctCfgSel,
      ((MemPsSel == 0) ? 1 : 0),
      Flags,
      StdHeader
      );
  }
}


/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to read all register spaces.
 *
 *
 *
 * @param[in] GnbHandle          GNB handle
 * @param[in] RegisterSpaceType  Register space type
 * @param[in] Address            Register offset, but PortDevice
 * @param[out] Value             Return value
 * @param[in] Flags              Flags - BIT0 indicates S3 save/restore
 * @param[in] StdHeader          Standard configuration header
 */
AGESA_STATUS
GnbRegisterReadServiceTN (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
     OUT   VOID                *Value,
  IN       UINT32              Flags,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  return GnbRegisterReadTN (RegisterSpaceType, Address, Value, Flags, StdHeader);
}
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
GnbRegisterReadTN (
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
     OUT   VOID                *Value,
  IN       UINT32              Flags,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  ACCESS_WIDTH          Width;
  UINT32                TempValue;
  UINT32                TempAddress;
  BOOLEAN               PstateChanged;

  Width = (Flags == GNB_REG_ACC_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;
  TempAddress = 0;
  TempValue = 0;


  switch (RegisterSpaceType) {
  case TYPE_D0F0:
    GnbLibPciRead (
      MAKE_SBDFO (0, 0, 0, 0, Address),
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_D0F2:
    GnbLibPciRead (
      MAKE_SBDFO (0, 0, 0, 2, Address),
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_D1F0:
    GnbLibPciRead (
      MAKE_SBDFO (0, 0, 1, 0, Address),
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_D1F1:
    GnbLibPciRead (
      MAKE_SBDFO (0, 0, 1, 1, Address),
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_DxF0:
    // Treat it as complete address for ports
    GnbLibPciRead (
      Address,
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

  case TYPE_D0F0x64:
    // Miscellaneous Index Data, access the registers D0F0x64_x[FF:00]
    // Write enable bit7
    GnbLibPciIndirectRead (
      MAKE_SBDFO (0, 0, 0, 0, D0F0x60_ADDRESS),
      Address,
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_D0F0x98:
    // Northbridge ORB Configuration Offset, access D0F0x98_x[FF:00]
    // Write enable bit8
    GnbLibPciIndirectRead (
      MAKE_SBDFO (0, 0, 0, 0, D0F0x94_ADDRESS),
      Address,
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_D0F0xBC:
    {
    //SMU, access D0F0xBC_x[FFFFFFFF:00000000]
    // No write enable
      UINT64  TempData;
      //ASSERT ((Address < 0xE0100000 || Address > 0xE0108FFFF) && (Address & 0x3) == 0);
      GnbLibPciIndirectRead (
        MAKE_SBDFO (0, 0, 0, 0, D0F0xB8_ADDRESS),
        (Address & (~0x3ull)),
        Width,
        &TempData,
        StdHeader
      );
      if ((Address & 0x3) != 0) {
        //Non aligned access allowed to fuse block
        GnbLibPciIndirectRead (
          MAKE_SBDFO (0, 0, 0, 0, D0F0xB8_ADDRESS),
          (Address & (~0x3ull)) + 4,
          Width,
          ((UINT32 *) &TempData) + 1,
          StdHeader
        );
      }
      * ((UINT32*) Value) = (UINT32) (TempData >> ((Address & 0x3) * 8));
      break;
    }
  case TYPE_D0F0xE4:
    // D0F0xE0 Link Index Address, access D0F0xE4_x[FFFF_FFFF:0000_0000]
    // No write enable
    GnbLibPciIndirectRead (
      MAKE_SBDFO (0, 0, 0, 0, D0F0xE0_ADDRESS),
      Address,
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_D0F2xF4:
    // IOMMU L2 Config Index, to access the registers D0F2xF4_x[FF:00].
    // Write enable bit8
    GnbLibPciIndirectRead (
      MAKE_SBDFO (0, 0, 0, 2, 0xF0),//D0F2xF0_ADDRESS
      Address,
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_D0F2xFC:
    // IOMMU L1 Config Index, access the registers D0F2xFC_x[FFFF:0000]_L1[3:0]
    // Write enable bit31
    GnbLibPciIndirectRead (
      MAKE_SBDFO (0, 0, 0, 2, 0xF8),//D0F2xF8_ADDRESS
      Address,
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_DxF0xE4:
    // D[8:2]F0xE0 Root Port Index, access the registers D[8:2]F0xE4_x[FF:00]
    // No write enable
    TempValue = ((Address >> 16) & 0xFF);
    TempAddress = Address & 0xFF;
    GnbLibPciIndirectRead (
      MAKE_SBDFO (0, 0, (TempValue), 0, 0xE0),//DxF0xE0_ADDRESS
      TempAddress,
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_MSR:
    LibAmdMsrRead  (Address, Value, StdHeader);
    break;

  case TYPE_GMM:
    ASSERT (Address < 0x40000);

    if ((Address >= 0x600) && (Address <= 0x8FF)) {
      // CG
      GnbLibPciIndirectRead (
        MAKE_SBDFO (0, 0, 0, 0, 0xB8),
        (0xE0002000 | (Address - 0x600)),
        Width,
        Value,
        StdHeader
      );
    } else {
      // SRBM
      GnbLibPciIndirectRead (
        MAKE_SBDFO (0, 0, 0, 0, 0xB8),
        (0x80080000 | (Address & 0x3FFFF)),
        Width,
        Value,
        StdHeader
      );
    }
    break;

  case TYPE_D18F2x9C_dct0:
    GnbDctAdditionalDataReadTN (
      Address,
      0,
      GNB_IGNORED_PARAM,
      Flags,
      Value,
      StdHeader
    );
    break;

  case TYPE_D18F2x9C_dct0_mp0:
    GnbDctAdditionalDataReadTN (
      Address,
      0,
      0,
      Flags,
      Value,
      StdHeader
    );
    break;

  case TYPE_D18F2x9C_dct0_mp1:
    GnbDctAdditionalDataReadTN (
      Address,
      0,
      1,
      Flags,
      Value,
      StdHeader
    );
    break;

  case TYPE_D18F2x9C_dct1:
    GnbDctAdditionalDataReadTN (
      Address,
      1,
      GNB_IGNORED_PARAM,
      Flags,
      Value,
      StdHeader
    );
    break;

  case TYPE_D18F2x9C_dct1_mp0:
    GnbDctAdditionalDataReadTN (
      Address,
      1,
      0,
      Flags,
      Value,
      StdHeader
    );
    break;

  case TYPE_D18F2x9C_dct1_mp1:
    GnbDctAdditionalDataReadTN (
      Address,
      1,
      1,
      Flags,
      Value,
      StdHeader
    );
    break;

  case TYPE_D18F2_dct0:
    GnbDctMpConfigTN (
      0,
      GNB_IGNORED_PARAM,
      Flags,
      StdHeader
    );
    GnbLibPciRead (
      MAKE_SBDFO (0, 0, 0x18, 2, Address),
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_D18F2_dct0_mp0:
    PstateChanged = GnbDctMpConfigTN (
                      0,
                      0,
                      Flags,
                      StdHeader
                    );
    GnbLibPciRead (
      MAKE_SBDFO (0, 0, 0x18, 2, Address),
      Width,
      Value,
      StdHeader
    );
    if (PstateChanged) {
      GnbDctMpConfigTN (
        0,
        1,
        Flags,
        StdHeader
        );
    }
    break;

  case TYPE_D18F2_dct0_mp1:
    PstateChanged = GnbDctMpConfigTN (
                      0,
                      1,
                      Flags,
                      StdHeader
                    );
    GnbLibPciRead (
      MAKE_SBDFO (0, 0, 0x18, 2, Address),
      Width,
      Value,
      StdHeader
    );
    if (PstateChanged) {
      GnbDctMpConfigTN (
        0,
        0,
        Flags,
        StdHeader
        );
    }
    break;

  case TYPE_D18F2_dct1:
    GnbDctMpConfigTN (
      1,
      GNB_IGNORED_PARAM,
      Flags,
      StdHeader
    );
    GnbLibPciRead (
      MAKE_SBDFO (0, 0, 0x18, 2, Address),
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_D18F2_dct1_mp0:
    PstateChanged = GnbDctMpConfigTN (
                      1,
                      0,
                      Flags,
                      StdHeader
                    );
    GnbLibPciRead (
      MAKE_SBDFO (0, 0, 0x18, 2, Address),
      Width,
      Value,
      StdHeader
    );
    if (PstateChanged) {
      GnbDctMpConfigTN (
        1,
        1,
        Flags,
        StdHeader
        );
    }
    break;

  case TYPE_D18F2_dct1_mp1:
    PstateChanged = GnbDctMpConfigTN (
                      1,
                      1,
                      Flags,
                      StdHeader
                    );
    GnbLibPciRead (
      MAKE_SBDFO (0, 0, 0x18, 2, Address),
      Width,
      Value,
      StdHeader
    );
    if (PstateChanged) {
      GnbDctMpConfigTN (
        1,
        0,
        Flags,
        StdHeader
        );
    }
    break;

  case TYPE_CGIND:
    // CG index
    GnbLibPciIndirectWrite (
      MAKE_SBDFO (0, 0, 0, 0, 0xB8),
      (0xE0002000 | (0x8F8 - 0x600)),
      Width,
      &Address,
      StdHeader
    );
    // CG data
    GnbLibPciIndirectRead (
      MAKE_SBDFO (0, 0, 0, 0, 0xB8),
      (0xE0002000 | (0x8FC - 0x600)),
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
 *
 * @param[in] GnbHandle          GnbHandle
 * @param[in] RegisterSpaceType  Register space type
 * @param[in] Address            Register offset, but PortDevice
 * @param[out] Value             The value to write
 * @param[in] Flags              Flags - BIT0 indicates S3 save/restore
 * @param[in] StdHeader          Standard configuration header
 */
AGESA_STATUS
GnbRegisterWriteServiceTN (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  return GnbRegisterWriteTN (RegisterSpaceType, Address, Value, Flags, StdHeader);
}

/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to write all register spaces.
 *
 *
 *
 * @param[in] RegisterSpaceType  Register space type
 * @param[in] Address            Register offset, but PortDevice
 * @param[out] Value             The value to write
 * @param[in] Flags              Flags - BIT0 indicates S3 save/restore
 * @param[in] StdHeader          Standard configuration header
 */
AGESA_STATUS
GnbRegisterWriteTN (
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  CONST IN       VOID                *Value,
  IN       UINT32              Flags,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{

  ACCESS_WIDTH          Width;
  UINT32                TempValue;
  UINT32                TempAddress;
  PCI_ADDR              PciAddress;
  BOOLEAN               PstateChanged;

  Width = (Flags == GNB_REG_ACC_FLAG_S3SAVE) ? AccessS3SaveWidth32 : AccessWidth32;
  TempAddress = 0;
  TempValue = 0;

  GNB_DEBUG_CODE (
    GnbRegisterWriteTNDump (RegisterSpaceType, Address, Value);
  );

  switch (RegisterSpaceType) {
  case TYPE_D0F0:
    GnbLibPciWrite (
      MAKE_SBDFO (0, 0, 0, 0, Address),
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_D0F2:
    GnbLibPciWrite (
      MAKE_SBDFO (0, 0, 0, 2, Address),
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_D1F0:
    GnbLibPciWrite (
      MAKE_SBDFO (0, 0, 1, 0, Address),
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_D1F1:
    GnbLibPciWrite (
      MAKE_SBDFO (0, 0, 1, 1, Address),
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_DxF0:
    // Treat it as complete address for ports
    GnbLibPciWrite (
      Address,
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

  case TYPE_D0F0x64:
    // Miscellaneous Index Data, access the registers D0F0x64_x[FF:00]
    // Write enable bit7
    GnbLibPciIndirectWrite (
      MAKE_SBDFO (0, 0, 0, 0, D0F0x60_ADDRESS),
      Address | IOC_WRITE_ENABLE,
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_D0F0x98:
    // Northbridge ORB Configuration Offset, access D0F0x98_x[FF:00]
    // Write enable bit8
    GnbLibPciIndirectWrite (
      MAKE_SBDFO (0, 0, 0, 0, D0F0x94_ADDRESS),
      Address | ORB_WRITE_ENABLE,
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_D0F0xBC:
    //SMU, access D0F0xBC_x[FFFFFFFF:00000000]
    // No write enable
    GnbLibPciIndirectWrite (
      MAKE_SBDFO (0, 0, 0, 0, 0xB8),
      Address,
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_D0F0xE4:
    // D0F0xE0 Link Index Address, access D0F0xE4_x[FFFF_FFFF:0000_0000]
    // No write enable
    GnbLibPciIndirectWrite (
      MAKE_SBDFO (0, 0, 0, 0, D0F0xE0_ADDRESS),
      Address,
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_D0F2xF4:
    // IOMMU L2 Config Index, to access the registers D0F2xF4_x[FF:00].
    // Write enable bit8
    GnbLibPciIndirectWrite (
      MAKE_SBDFO (0, 0, 0, 2, 0xF0),//D0F2xF0_ADDRESS
      Address | IOMMU_L2_WRITE_ENABLE,
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_D0F2xFC:
    // IOMMU L1 Config Index, access the registers D0F2xFC_x[FFFF:0000]_L1[3:0]
    // Write enable bit31
    GnbLibPciIndirectWrite (
      MAKE_SBDFO (0, 0, 0, 2, 0xF8),//D0F2xF8_ADDRESS
      Address | IOMMU_L1_WRITE_ENABLE,
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_DxF0xE4:
    // D[8:2]F0xE0 Root Port Index, access the registers D[8:2]F0xE4_x[FF:00]
    // No write enable
    TempValue = ((Address >> 16) & 0xFF);
    TempAddress = Address & 0xFF;
    GnbLibPciIndirectWrite (
      MAKE_SBDFO (0, 0, (TempValue), 0, 0xE0),//DxF0xE0_ADDRESS
      TempAddress,
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_MSR:
    LibAmdMsrWrite  (Address, Value, StdHeader);
    break;

  case TYPE_GMM:
    ASSERT (Address < 0x40000);

    if ((Address >= 0x600) && (Address <= 0x8FF)) {
      // CG
      GnbLibPciIndirectWrite (
        MAKE_SBDFO (0, 0, 0, 0, 0xB8),
        (0xE0002000 | (Address - 0x600)),
        Width,
        Value,
        StdHeader
      );
    } else {
      // SRBM
      GnbLibPciIndirectWrite (
        MAKE_SBDFO (0, 0, 0, 0, 0xB8),
        (0x80080000 | (Address & 0x3FFFF)),
        Width,
        Value,
        StdHeader
      );
    }
    break;

  case TYPE_D18F2x9C_dct0:
    GnbDctAdditionalDataWriteTN (
      Address,
      0,
      GNB_IGNORED_PARAM,
      Flags,
      Value,
      StdHeader
    );
    break;

  case TYPE_D18F2x9C_dct0_mp0:
    GnbDctAdditionalDataWriteTN (
      Address,
      0,
      0,
      Flags,
      Value,
      StdHeader
    );
    break;

  case TYPE_D18F2x9C_dct0_mp1:
    GnbDctAdditionalDataWriteTN (
      Address,
      0,
      1,
      Flags,
      Value,
      StdHeader
    );
    break;

  case TYPE_D18F2x9C_dct1:
    GnbDctAdditionalDataWriteTN (
      Address,
      1,
      GNB_IGNORED_PARAM,
      Flags,
      Value,
      StdHeader
    );
    break;

  case TYPE_D18F2x9C_dct1_mp0:
    GnbDctAdditionalDataWriteTN (
      Address,
      1,
      0,
      Flags,
      Value,
      StdHeader
    );
    break;

  case TYPE_D18F2x9C_dct1_mp1:
    GnbDctAdditionalDataWriteTN (
      Address,
      1,
      1,
      Flags,
      Value,
      StdHeader
    );
    break;

  case TYPE_D18F2_dct0:
    GnbDctMpConfigTN (
      0,
      GNB_IGNORED_PARAM,
      Flags,
      StdHeader
    );
    GnbLibPciWrite (
      MAKE_SBDFO (0, 0, 0x18, 2, Address),
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_D18F2_dct0_mp0:
    PstateChanged = GnbDctMpConfigTN (
                      0,
                      0,
                      Flags,
                      StdHeader
                    );
    GnbLibPciWrite (
      MAKE_SBDFO (0, 0, 0x18, 2, Address),
      Width,
      Value,
      StdHeader
    );
    if (PstateChanged) {
      GnbDctMpConfigTN (
        0,
        1,
        Flags,
        StdHeader
        );
    }
    break;

  case TYPE_D18F2_dct0_mp1:
    PstateChanged = GnbDctMpConfigTN (
                      0,
                      1,
                      Flags,
                      StdHeader
                    );
    GnbLibPciWrite (
      MAKE_SBDFO (0, 0, 0x18, 2, Address),
      Width,
      Value,
      StdHeader
    );
    if (PstateChanged) {
      GnbDctMpConfigTN (
        0,
        0,
        Flags,
        StdHeader
        );
    }
    break;

  case TYPE_D18F2_dct1:
    GnbDctMpConfigTN (
      1,
      GNB_IGNORED_PARAM,
      Flags,
      StdHeader
    );
    GnbLibPciWrite (
      MAKE_SBDFO (0, 0, 0x18, 2, Address),
      Width,
      Value,
      StdHeader
    );
    break;

  case TYPE_D18F2_dct1_mp0:
    PstateChanged = GnbDctMpConfigTN (
                      1,
                      0,
                      Flags,
                      StdHeader
                    );
    GnbLibPciWrite (
      MAKE_SBDFO (0, 0, 0x18, 2, Address),
      Width,
      Value,
      StdHeader
    );
    if (PstateChanged) {
      GnbDctMpConfigTN (
        1,
        1,
        Flags,
        StdHeader
        );
    }
    break;

  case TYPE_D18F2_dct1_mp1:
    PstateChanged = GnbDctMpConfigTN (
      1,
      1,
      Flags,
      StdHeader
    );
    GnbLibPciWrite (
      MAKE_SBDFO (0, 0, 0x18, 2, Address),
      Width,
      Value,
      StdHeader
    );
    if (PstateChanged) {
      GnbDctMpConfigTN (
        1,
        0,
        Flags,
        StdHeader
        );
    }
    break;

  case TYPE_CGIND:
    // CG index
    GnbLibPciIndirectWrite (
      MAKE_SBDFO (0, 0, 0, 0, 0xB8),
      (0xE0002000 | (0x8F8 - 0x600)),
      Width,
      &Address,
      StdHeader
    );
    // CG data
    GnbLibPciIndirectWrite (
      MAKE_SBDFO (0, 0, 0, 0, 0xB8),
      (0xE0002000 | (0x8FC - 0x600)),
      Width,
      Value,
      StdHeader
    );
    break;
  case TYPE_SMU_MSG:
    PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0);
    GnbSmuServiceRequestV4 (PciAddress, (UINT8) Address, Flags, StdHeader);
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
GnbRegisterWriteTNDump (
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       VOID                *Value
  )
{
  IDS_HDT_CONSOLE (NB_MISC, "  R WRITE Space %s  Address 0x%04x, Value 0x%04x\n",
    (RegisterSpaceType == TYPE_D0F0) ? "TYPE_D0F0" : (
    (RegisterSpaceType == TYPE_D0F0x64) ? "TYPE_D0F0x64" : (
    (RegisterSpaceType == TYPE_D0F0x98) ? "TYPE_D0F0x98" : (
    (RegisterSpaceType == TYPE_D0F0xBC) ? "TYPE_D0F0xBC" : (
    (RegisterSpaceType == TYPE_D0F0xE4) ? "TYPE_D0F0xE4" : (
    (RegisterSpaceType == TYPE_DxF0) ? "TYPE_DxF0" : (
    (RegisterSpaceType == TYPE_DxF0xE4) ? "TYPE_DxF0xE4" : (
    (RegisterSpaceType == TYPE_D0F2) ? "TYPE_D0F2" : (
    (RegisterSpaceType == TYPE_D0F2xF4) ? "TYPE_D0F2xF4" : (
    (RegisterSpaceType == TYPE_D0F2xFC) ? "TYPE_D0F2xFC" : (
    (RegisterSpaceType == TYPE_D18F1) ? "TYPE_D18F1" : (
    (RegisterSpaceType == TYPE_D18F2) ? "TYPE_D18F2" : (
    (RegisterSpaceType == TYPE_D18F3) ? "TYPE_D18F3" : (
    (RegisterSpaceType == TYPE_D18F4) ? "TYPE_D18F4" : (
    (RegisterSpaceType == TYPE_D18F5) ? "TYPE_D18F5" : (
    (RegisterSpaceType == TYPE_MSR) ? "TYPE_MSR" : (
    (RegisterSpaceType == TYPE_D1F0) ? "TYPE_D1F0" : (
    (RegisterSpaceType == TYPE_D1F1) ? "TYPE_D1F1" : (
    (RegisterSpaceType == TYPE_GMM) ? "TYPE_GMM" : (
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
    (RegisterSpaceType == TYPE_D18F2_dct1_mp1) ? "TYPE_D18F2_dct1_mp1" : "Invalid"))))))))))))))))))))))))))))))),
    Address,
    *((UINT32*)Value)
  );
}
