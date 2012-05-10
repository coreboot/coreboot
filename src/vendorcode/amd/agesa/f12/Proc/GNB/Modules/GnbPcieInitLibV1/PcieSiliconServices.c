/* $NoKeywords:$ */
/**
 * @file
 *
 * Family specific PCIe complex initialization services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 49532 $   @e \$Date: 2011-03-25 02:54:43 +0800 (Fri, 25 Mar 2011) $
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
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbRegistersLN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEINITLIBV1_PCIESILICONSERVICES_FILECODE
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
 * Get Gen1 voltage Index
 *
 *
 *
 *
 * @param[in]  StdHeader           Standard configuration header
 */
UINT8
PcieSiliconGetGen1VoltageIndex (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT8   Index;
  UINT8   Gen1VidIndex;
  UINT8   SclkVidArray[4];
  GnbLibPciRead (
    MAKE_SBDFO ( 0, 0, 0x18, 3, D18F3x15C_ADDRESS),
    AccessWidth32,
    &SclkVidArray[0],
    StdHeader
    );
  Gen1VidIndex = 0;
  for (Index = 0; Index < 4; Index++) {
    if (SclkVidArray[Index] > SclkVidArray[Gen1VidIndex]) {
      Gen1VidIndex = Index;
    }
  }
  return Gen1VidIndex;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Request Pcie voltage change
 *
 *
 *
 * @param[in]  VidIndex            The request VID index
 * @param[in]  StdHeader           Standard configuration header
 */
VOID
PcieSiliconRequestVoltage (
  IN      UINT8                 VidIndex,
  IN      AMD_CONFIG_PARAMS     *StdHeader
  )
{
  D0F0x64_x6A_STRUCT    D0F0x64_x6A;
  D0F0x64_x6B_STRUCT    D0F0x64_x6B;

  //Enable voltage client
  GnbLibPciIndirectRead (
    MAKE_SBDFO (0, 0, 0, 0, D0F0x60_ADDRESS),
    D0F0x64_x6A_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    &D0F0x64_x6A.Value,
    StdHeader
  );

  D0F0x64_x6A.Field.VoltageChangeEn = 0x1;

  GnbLibPciIndirectWrite (
    MAKE_SBDFO (0, 0, 0, 0, D0F0x60_ADDRESS),
    D0F0x64_x6A_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    &D0F0x64_x6A.Value,
    StdHeader
  );

  D0F0x64_x6A.Field.VoltageLevel = VidIndex;
  D0F0x64_x6A.Field.VoltageChangeReq = !D0F0x64_x6A.Field.VoltageChangeReq;

  GnbLibPciIndirectWrite (
    MAKE_SBDFO (0, 0, 0, 0, D0F0x60_ADDRESS),
    D0F0x64_x6A_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    &D0F0x64_x6A.Value,
    StdHeader
    );
  do {
    GnbLibPciIndirectRead (
      MAKE_SBDFO (0, 0, 0, 0, D0F0x60_ADDRESS),
      D0F0x64_x6B_ADDRESS | IOC_WRITE_ENABLE,
      AccessS3SaveWidth32,
      &D0F0x64_x6B.Value,
      StdHeader
    );
  } while (D0F0x64_x6A.Field.VoltageChangeReq != D0F0x64_x6B.Field.VoltageChangeAck);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Unhide all ports
 *
 *
 *
 * @param[in]  Silicon             Pointer to silicon configuration descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */

VOID
PcieSiliconUnHidePorts (
  IN      PCIe_SILICON_CONFIG   *Silicon,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  GnbLibPciIndirectRMW (
    Silicon->Address.AddressValue | D0F0x60_ADDRESS,
    D0F0x64_x0C_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    ~(UINT32)(BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7),
    0x0,
    GnbLibGetHeader (Pcie)
    );
  GnbLibPciIndirectRMW (
    Silicon->Address.AddressValue | D0F0x60_ADDRESS,
    D0F0x64_x00_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    ~(UINT32)BIT6,
    BIT6,
    GnbLibGetHeader (Pcie)
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Hide unused ports
 *
 *
 *
 * @param[in]  Silicon             Pointer to silicon configuration data area
 * @param[in]  Pcie                Pointer to data area up to 256 byte
 */

VOID
PcieSiliconHidePorts (
  IN      PCIe_SILICON_CONFIG   *Silicon,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  D0F0x64_x0C_STRUCT    D0F0x64_x0C;
  PCIe_WRAPPER_CONFIG   *WrapperList;
  D0F0x64_x0C.Value =  0;
  WrapperList = PcieConfigGetChildWrapper (Silicon);
  while (WrapperList != NULL) {
    PCIe_ENGINE_CONFIG   *EngineList;
    EngineList = PcieConfigGetChildEngine (WrapperList);
    while (EngineList != NULL) {
      if (EngineList->EngineData.EngineType == PciePortEngine) {
        if (!PcieConfigCheckPortStatus (EngineList, INIT_STATUS_PCIE_TRAINING_SUCCESS) &&
          ((EngineList->Type.Port.PortData.LinkHotplug == HotplugDisabled) || (EngineList->Type.Port.PortData.LinkHotplug == HotplugInboard)) &&
          !PcieConfigIsSbPcieEngine (EngineList)) {
          D0F0x64_x0C.Value |= 1 << EngineList->Type.Port.NativeDevNumber;
        }
      }
      EngineList = PcieLibGetNextDescriptor (EngineList);
    }
    WrapperList = PcieLibGetNextDescriptor (WrapperList);
  }

  GnbLibPciIndirectRMW (
    Silicon->Address.AddressValue | D0F0x60_ADDRESS,
    D0F0x64_x0C_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    ~(UINT32)(BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7),
    D0F0x64_x0C.Value,
    GnbLibGetHeader (Pcie)
    );
  GnbLibPciIndirectRMW (
    Silicon->Address.AddressValue | D0F0x60_ADDRESS,
    D0F0x64_x00_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    ~(UINT32)BIT6,
    0x0,
    GnbLibGetHeader (Pcie)
    );
}

