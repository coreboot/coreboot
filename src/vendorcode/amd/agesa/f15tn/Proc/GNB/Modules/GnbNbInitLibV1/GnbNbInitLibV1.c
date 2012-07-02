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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
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
#include  "OptionGnb.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBNBINITLIBV1_GNBNBINITLIBV1_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern GNB_BUILD_OPTIONS      GnbBuildOptions;

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
    1 << 9 ,
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

  ex495_STRUCT     ex495  ;

  GnbLibPciIndirectRead (
    NbPciAddress.AddressValue | D0F0x94_ADDRESS,
    0x2c  | (1 << D0F0x94_OrbIndWrEn_OFFSET),
    AccessWidth32,
    &ex495.Value,
    StdHeader
    );

  // Enable Dynamic wake
  // Wake Hysteresis timer value.  Specifies the number of SMU pulses to count.
  if (GnbBuildOptions.CfgOrbDynWakeEnable) {
    ex495.Field.ex495_1  = 1;
  } else {
    ex495.Field.ex495_1  = 0;
  }
  ex495.Field.ex495_3  = 0x64;

  IDS_OPTION_HOOK (IDS_GNB_ORBDYNAMIC_WAKE, &ex495, StdHeader);

  GnbLibPciIndirectWrite (
    NbPciAddress.AddressValue | D0F0x94_ADDRESS,
    0x2c  | (1 << D0F0x94_OrbIndWrEn_OFFSET),
    AccessS3SaveWidth32,
    &ex495.Value,
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
        0x3a  | (1 << D0F0x94_OrbIndWrEn_OFFSET),
        AccessS3SaveWidth32,
        0xFFFFFFFF,
        1 << 3 /* D0F0x98_x3A_ClumpingEn_OFFSET*/,
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


