/* $NoKeywords:$ */
/**
 * @file
 *
 * NB services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 49911 $   @e \$Date: 2011-03-30 17:43:29 +0800 (Wed, 30 Mar 2011) $
 *
 */
/*
*****************************************************************************
*
* Copyright (c) 2011, Advanced Micro Devices, Inc.
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
#include  "Ids.h"
#include  "amdlib.h"
#include  "heapManager.h"
#include  "Gnb.h"
#include  "GnbFuseTable.h"
#include  "GnbCommonLib.h"
#include  "GnbNbInitLibV1.h"
#include  "GnbRegistersLN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBNBINITLIBV1_GNBNBINITLIBV1_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Init NB set top of memory
 *
 *
 *
 * @param[in] NbPciAddress    Gnb PCI address
 * @param[in] StdHeader       Standard Configuration Header
 */

AGESA_STATUS
GnbSetTom (
  IN      PCI_ADDR            NbPciAddress,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  AGESA_STATUS  Status;
  UINT64        MsrData;
  UINT32        Value;
  Status = AGESA_SUCCESS;
  //Read memory size below 4G from MSR C001_001A
  LibAmdMsrRead (TOP_MEM, &MsrData, StdHeader);
  //Write to NB register 0x90
  Value = (UINT32)MsrData & 0xFF800000;     //Keep bits 31:23
  GnbLibPciRMW (
    NbPciAddress.AddressValue | D0F0x90_ADDRESS,
    AccessS3SaveWidth32,
    0x007FFFFF,
    Value,
    StdHeader
    );
  if (Value == 0) {
    Status = AGESA_WARNING;
  }

  LibAmdMsrRead (SYS_CFG, &MsrData, StdHeader);
  if ((MsrData & BIT21) != 0) {
    //Read memory size above 4G from MSR C001_001D
    LibAmdMsrRead (TOP_MEM2, &MsrData, StdHeader);
    // Write memory size[39:32] to indirect register 1A[7:0]
    Value = (UINT32) ((MsrData >> 32) & 0xFF);
    GnbLibPciIndirectRMW (
      NbPciAddress.AddressValue | D0F0x60_ADDRESS,
      D0F0x64_x1A_ADDRESS | IOC_WRITE_ENABLE,
      AccessS3SaveWidth32,
      0xFFFFFF00,
      Value,
      StdHeader
    );

    // Write memory size[31:23] to indirect register 19[31:23] and enable memory through bit 0
    Value = (UINT32)MsrData & 0xFF800000;     //Keep bits 31:23
    Value |= BIT0;                            // Enable top of memory
    GnbLibPciIndirectRMW (
      NbPciAddress.AddressValue | D0F0x60_ADDRESS,
      D0F0x64_x19_ADDRESS | IOC_WRITE_ENABLE,
      AccessS3SaveWidth32,
      0x007FFFFF,
      Value,
      StdHeader
    );
  }
  return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Avoid LPC DMA transaction deadlock
 *
 *
 *
 * @param[in] NbPciAddress    Gnb PCI address
 * @param[in] StdHeader       Standard Configuration Header
 */

VOID
GnbLpcDmaDeadlockPrevention (
  IN      PCI_ADDR            NbPciAddress,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  // For GPP Link core, enable special NP memory write protocol on the  processor side PCIE controller
  GnbLibPciIndirectRMW (
    NbPciAddress.AddressValue | D0F0xE0_ADDRESS,
    CORE_SPACE (1, D0F0xE4_CORE_0010_ADDRESS),
    AccessWidth32,
    0xFFFFFFFF,
    1 << D0F0xE4_CORE_0010_UmiNpMemWrite_OFFSET,
    StdHeader
  );

  //Enable special NP memory write protocol in ORB
  GnbLibPciIndirectRMW (
    NbPciAddress.AddressValue | D0F0x94_ADDRESS,
    D0F0x98_x06_ADDRESS | (1 << D0F0x94_OrbIndWrEn_OFFSET),
    AccessS3SaveWidth32,
    0xFFFFFFFF,
    1 << D0F0x98_x06_UmiNpMemWrEn_OFFSET,
    StdHeader
  );
}

/*----------------------------------------------------------------------------------------*/
/**
 * NB Dynamic Wake
 * ORB_CNB_Wake signal is used to inform the CNB NCLK controller and GNB LCLK controller
 *   that ORB is (or will soon) push data into the synchronizer FIFO (i.e. wake is high).
 *
 * @param[in] NbPciAddress    Gnb PCI address
 * @param[in] StdHeader       Standard Configuration Header
 */

VOID
GnbOrbDynamicWake (
  IN      PCI_ADDR            NbPciAddress,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{

  D0F0x98_x2C_STRUCT D0F0x98_x2C;

  GnbLibPciIndirectRead (
    NbPciAddress.AddressValue | D0F0x94_ADDRESS,
    D0F0x98_x2C_ADDRESS | (1 << D0F0x94_OrbIndWrEn_OFFSET),
    AccessWidth32,
    &D0F0x98_x2C.Value,
    StdHeader
    );

  // Enable Dynamic wake
  // Wake Hysteresis timer value.  Specifies the number of SMU pulses to count.
  D0F0x98_x2C.Field.DynWakeEn = 1;
  D0F0x98_x2C.Field.WakeHysteresis = 0x64;

  IDS_OPTION_HOOK (IDS_GNB_ORBDYNAMIC_WAKE, &D0F0x98_x2C, StdHeader);

  GnbLibPciIndirectWrite (
    NbPciAddress.AddressValue | D0F0x94_ADDRESS,
    D0F0x98_x2C_ADDRESS | (1 << D0F0x94_OrbIndWrEn_OFFSET),
    AccessS3SaveWidth32,
    &D0F0x98_x2C.Value,
    StdHeader
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Lock NB registers
 *
 *
 *
 * @param[in] NbPciAddress    Gnb PCI address
 * @param[in] StdHeader       Standard Configuration Header
 */

VOID
GnbLock (
  IN      PCI_ADDR            NbPciAddress,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  GnbLibPciIndirectWriteField (
    NbPciAddress.AddressValue | D0F0x60_ADDRESS,
    D0F0x64_x00_ADDRESS | IOC_WRITE_ENABLE,
    D0F0x64_x00_HwInitWrLock_OFFSET,
    D0F0x64_x00_HwInitWrLock_WIDTH,
    0x1,
    TRUE,
    StdHeader
    );
}


/*----------------------------------------------------------------------------------------*/
/**
 * UnitID Clumping
 *
 *
 * @param[in] NbPciAddress    Gnb PCI address
 * @param[in] StdHeader       Standard Configuration Header
 */

VOID
GnbClumpUnitID (
  IN      PCI_ADDR            NbPciAddress,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32  Value;
  GnbLibPciRead (MAKE_SBDFO (0, NbPciAddress.Address.Bus, 2, 0, 0), AccessWidth32, &Value, StdHeader);
  if (Value != 0xFFFFFFFF) {
    GnbLibPciRead (MAKE_SBDFO (0, NbPciAddress.Address.Bus, 3, 0, 0), AccessWidth32, &Value, StdHeader);
    if (Value == 0xFFFFFFFF) {
      GnbLibPciIndirectRMW (
        NbPciAddress.AddressValue | D0F0x94_ADDRESS,
        D0F0x98_x3A_ADDRESS | (1 << D0F0x94_OrbIndWrEn_OFFSET),
        AccessS3SaveWidth32,
        0xFFFFFFFF,
        1 << D0F0x98_x3A_ClumpingEn_OFFSET,
        StdHeader
        );
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get the index of highest SCLK VID
 *
 * @param[in] StdHeader       Standard configuration header
 * @retval                    NBVDD VID index
 */
UINT8
GnbLocateHighestVidIndex (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8             MaxVid;
  UINT8             MaxVidIndex;
  UINTN             Index;
  PP_FUSE_ARRAY     *PpFuseArray;

  PpFuseArray = (PP_FUSE_ARRAY *) GnbLocateHeapBuffer (AMD_PP_FUSE_TABLE_HANDLE, StdHeader);
  ASSERT (PpFuseArray != NULL);
  if (PpFuseArray == NULL) {
    IDS_HDT_CONSOLE (GNB_TRACE, "  ERROR!!! Heap Location\n");
    return 0;
  }

  MaxVidIndex = 0;
  MaxVid = 0xff;
  for (Index = 0; Index < 4; Index++) {
    if (PpFuseArray->SclkVid[Index] != 0 && PpFuseArray->SclkVid[Index] < MaxVid) {
      MaxVid = PpFuseArray->SclkVid[Index];
      MaxVidIndex = (UINT8) Index;
    }
  }
  ASSERT (PpFuseArray->SclkVid[MaxVidIndex] != 0);
  return MaxVidIndex;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get the index of lowest SCLK VID
 *
 * @param[in] StdHeader       Standard configuration header
 * @retval                    NBVDD VID index
 */
UINT8
GnbLocateLowestVidIndex (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8             MinVidIndex;
  UINTN             Index;
  PP_FUSE_ARRAY     *PpFuseArray;

  PpFuseArray = (PP_FUSE_ARRAY *) GnbLocateHeapBuffer (AMD_PP_FUSE_TABLE_HANDLE, StdHeader);
  ASSERT (PpFuseArray != NULL);
  if (PpFuseArray == NULL) {
    IDS_HDT_CONSOLE (GNB_TRACE, "  ERROR!!! Heap Location\n");
    return 0;
  }

  MinVidIndex = 0;

  for (Index = 0; Index < 4; Index++) {
    if (PpFuseArray->SclkVid[Index] > PpFuseArray->SclkVid[MinVidIndex]) {
      MinVidIndex = (UINT8) Index;
    }
  }
  ASSERT (PpFuseArray->SclkVid[MinVidIndex] != 0);
  return MinVidIndex;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get the highest SCLK VID (high voltage)
 *
 * @param[in] StdHeader       Standard configuration header
 * @retval                    NBVDD VID
 */
UINT8
GnbLocateHighestVidCode (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8                         MaxVidIndex;
  PP_FUSE_ARRAY     *PpFuseArray;

  PpFuseArray = (PP_FUSE_ARRAY *) GnbLocateHeapBuffer (AMD_PP_FUSE_TABLE_HANDLE, StdHeader);
  ASSERT (PpFuseArray != NULL);

  MaxVidIndex = GnbLocateHighestVidIndex (StdHeader);
  ASSERT (PpFuseArray->SclkVid[MaxVidIndex] != 0);
  return PpFuseArray->SclkVid[MaxVidIndex];

}

/*----------------------------------------------------------------------------------------*/
/**
 * Get the lowest SCLK VID (low voltage)
 *
 * @param[in] StdHeader       Standard configuration header
 * @retval                    NBVDD VID
 */
UINT8
GnbLocateLowestVidCode (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8                         MinVidIndex;
  PP_FUSE_ARRAY     *PpFuseArray;

  PpFuseArray = (PP_FUSE_ARRAY *) GnbLocateHeapBuffer (AMD_PP_FUSE_TABLE_HANDLE, StdHeader);
  ASSERT (PpFuseArray != NULL);
  MinVidIndex = GnbLocateLowestVidIndex (StdHeader);
  ASSERT (PpFuseArray->SclkVid[MinVidIndex] != 0);
  return PpFuseArray->SclkVid[MinVidIndex];
}


