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
#include  "Ids.h"
#include  "amdlib.h"
#include  "heapManager.h"
#include  "Gnb.h"
#include  "GnbPcieConfig.h"
#include  "GnbCommonLib.h"
#include  "Filecode.h"
#include  "GnbRegistersCommonV2.h"
#include  "GnbF1Table.h"
#define FILECODE PROC_GNB_MODULES_GNBNBINITLIBV5_GNBNBINITLIBV5_FILECODE
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

AGESA_STATUS
GnbSetTomV5 (
  IN      PCI_ADDR            NbPciAddress,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

UINT8
GnbLocateHighestVidCodeV5 (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );
UINT8
GnbLocateLowestVidCodeV5 (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

UINT32
GnbTranslateVidCodeToMillivoltV5 (
  IN       UINT8                 Vid,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

UINT8
GnbLocateHighestVidIndexV5 (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );
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
GnbSetTomV5 (
  IN      PCI_ADDR            NbPciAddress,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  AGESA_STATUS  Status;
  UINT64        MsrData;
  UINT64        GnbTom2;
  UINT64        GnbTom3;
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

  GnbTom2 = 0;
  GnbTom3 = 0;
  LibAmdMsrRead (SYS_CFG, &MsrData, StdHeader);
  if ((MsrData & BIT21) != 0) {
    //If SYS_CFG(MtrrTom2En) then configure GNB TOM2 and TOM3
    //Read memory size above 4G from TOP_MEM2 (MSR C001_001D)
    LibAmdMsrRead (TOP_MEM2, &MsrData, StdHeader);
    if ((MsrData & (UINT64)0x0000FFFFFFC00000) > ((UINT64)0x0000010000000000)) {
      // If TOP_MEM2 is above 1TB, enable GNB TOM2 and TOM3
      // Set TOM2 for below 1TB limit
      GnbTom2 = 0x000000FD00000000;
      // TOM3 is INCLUSIVE, so set it to TOM - 1 using bits 47:22
      GnbTom3 = (MsrData - 1) & (UINT64)0x0000FFFFFFC00000;
    } else {
      // If TOP_MEM2 is below 1TB, set TOM2 using bits 39:22
      GnbTom2 = MsrData & (UINT64)0x000000FFFFC00000;       //Keep bits 39:22
      // If TOP_MEM2 is below 1TB, disable GNB TOM3
      GnbTom3 = 0;
    }
  }

  if (GnbTom2 != 0) {
    // Write memory size[39:32] to indirect register 1A[7:0]
    Value = (UINT32) ((GnbTom2 >> 32) & 0xFF);
    GnbLibPciIndirectRMW (
      NbPciAddress.AddressValue | D0F0x60_ADDRESS,
      D0F0x64_x1A_ADDRESS | IOC_WRITE_ENABLE,
      AccessS3SaveWidth32,
      0xFFFFFF00,
      Value,
      StdHeader
    );
    // Write memory size[31:23] to indirect register 19[31:23] and enable memory through bit 0
    Value = (UINT32)GnbTom2 & 0xFF800000;      //Keep bits 31:23
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

  if (GnbTom3 != 0) {
    // Above 1TB addressing TOM3 if MSR TOM is above 1TB
    // Write memory size[47:22] to indirect register 4E[25:0] and enable memory through bit 31
    Value = (UINT32) (GnbTom3 >> 22);
    Value |= BIT31;
    GnbLibPciIndirectRMW (
      NbPciAddress.AddressValue | D0F0x60_ADDRESS,
      0x4e | IOC_WRITE_ENABLE,
      AccessS3SaveWidth32,
      0x83FFFFFF,
      Value,
      StdHeader
    );
  }
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get the index of highest SCLK VID
 *
 * @param[in] StdHeader       Standard configuration header
 * @retval                    NBVDD VID index
 */
UINT8
GnbLocateHighestVidIndexV5 (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8             MaxVid;
  UINT8             MaxVidIndex;
  UINTN             Index;
  PP_F1_ARRAY_V2     *PpF1Array;

  PpF1Array = (PP_F1_ARRAY_V2 *) GnbLocateHeapBuffer (AMD_PP_F1_TABLE_HANDLE, StdHeader);
  ASSERT (PpF1Array != NULL);
  if (PpF1Array == NULL) {
    IDS_HDT_CONSOLE (GNB_TRACE, "  ERROR!!! Heap Location\n");
    return 0;
  }

  MaxVidIndex = 0;
  MaxVid = 0xff;
  for (Index = 0; Index < 5; Index++) {
    if (PpF1Array->PP_FUSE_ARRAY_V2_fld32[Index] != 0 && PpF1Array->PP_FUSE_ARRAY_V2_fld32[Index] < MaxVid) {
      MaxVid = PpF1Array->PP_FUSE_ARRAY_V2_fld32[Index];
      MaxVidIndex = (UINT8) Index;
    }
  }
  ASSERT (PpF1Array->PP_FUSE_ARRAY_V2_fld32[MaxVidIndex] != 0);
  return MaxVidIndex;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get the index of lowest SCLK VID
 *
 * @param[in] StdHeader       Standard configuration header
 * @retval                    NBVDD VID index
 */
STATIC UINT8
GnbLocateLowestVidIndexV5 (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8             MinVidIndex;
  UINTN             Index;
  PP_F1_ARRAY_V2     *PpF1Array;

  PpF1Array = (PP_F1_ARRAY_V2 *) GnbLocateHeapBuffer (AMD_PP_F1_TABLE_HANDLE, StdHeader);
  ASSERT (PpF1Array != NULL);
  if (PpF1Array == NULL) {
    IDS_HDT_CONSOLE (GNB_TRACE, "  ERROR!!! Heap Location\n");
    return 0;
  }

  MinVidIndex = 0;

  for (Index = 0; Index < 5; Index++) {
    if (PpF1Array->PP_FUSE_ARRAY_V2_fld32[Index] > PpF1Array->PP_FUSE_ARRAY_V2_fld32[MinVidIndex]) {
      MinVidIndex = (UINT8) Index;
    }
  }
  ASSERT (PpF1Array->PP_FUSE_ARRAY_V2_fld32[MinVidIndex] != 0);
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
GnbLocateHighestVidCodeV5 (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8                MaxVidIndex;
  PP_F1_ARRAY_V2     *PpF1Array;

  PpF1Array = (PP_F1_ARRAY_V2 *) GnbLocateHeapBuffer (AMD_PP_F1_TABLE_HANDLE, StdHeader);
  ASSERT (PpF1Array != NULL);

  MaxVidIndex = GnbLocateHighestVidIndexV5 (StdHeader);
  ASSERT (PpF1Array->PP_FUSE_ARRAY_V2_fld32[MaxVidIndex] != 0);
  return PpF1Array->PP_FUSE_ARRAY_V2_fld32[MaxVidIndex];

}

/*----------------------------------------------------------------------------------------*/
/**
 * Get the lowest SCLK VID (low voltage)
 *
 * @param[in] StdHeader       Standard configuration header
 * @retval                    NBVDD VID
 */
UINT8
GnbLocateLowestVidCodeV5 (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8                MinVidIndex;
  PP_F1_ARRAY_V2     *PpF1Array;

  PpF1Array = (PP_F1_ARRAY_V2 *) GnbLocateHeapBuffer (AMD_PP_F1_TABLE_HANDLE, StdHeader);
  ASSERT (PpF1Array != NULL);
  MinVidIndex = GnbLocateLowestVidIndexV5 (StdHeader);
  ASSERT (PpF1Array->PP_FUSE_ARRAY_V2_fld32[MinVidIndex] != 0);
  return PpF1Array->PP_FUSE_ARRAY_V2_fld32[MinVidIndex];
}

/*----------------------------------------------------------------------------------------*/
/**
 * Translate VID code to millivolt with two fraction bits
 *
 *
 * @param[in] Vid             VID code
 * @param[in] StdHeader       Standard configuration header
 * @retval    100 millivolt with two fraction bits
 */

UINT32
GnbTranslateVidCodeToMillivoltV5 (
  IN       UINT8                 Vid,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32       Millivolt;

  // According to SVI2 spec, Vid Code 0xF7 is 6.25mv. 6.25mv is one boundary value.
  if (Vid > 0xF7) {
    Vid = 0xF7;
    ASSERT (FALSE);
  }

  // Equation: VID code increase/decrease 1, there is one 6.25mv change.
  //           6.25mv could be described using integer 625 with two fraction bits.
  Millivolt = (0xF7 - Vid + 1) * 625;

  return Millivolt;
}
