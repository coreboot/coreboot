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
    MAKE_SBDFO ( 0, 0, 0x18, 3, 0x15c ),
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
  ex488_STRUCT     ex488 ;
  ex489_STRUCT     ex489 ;

  //Enable voltage client
  GnbLibPciIndirectRead (
    MAKE_SBDFO (0, 0, 0, 0, D0F0x60_ADDRESS),
    0x6a  | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    &ex488.Value,
    StdHeader
  );

  ex488.Field.VoltageChangeEn = 0x1;

  GnbLibPciIndirectWrite (
    MAKE_SBDFO (0, 0, 0, 0, D0F0x60_ADDRESS),
    0x6a  | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    &ex488.Value,
    StdHeader
  );

  ex488.Field.VoltageLevel = VidIndex;
  ex488.Field.VoltageChangeReq = !ex488.Field.VoltageChangeReq;

  GnbLibPciIndirectWrite (
    MAKE_SBDFO (0, 0, 0, 0, D0F0x60_ADDRESS),
    0x6a  | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    &ex488.Value,
    StdHeader
    );
  do {
    GnbLibPciIndirectRead (
      MAKE_SBDFO (0, 0, 0, 0, D0F0x60_ADDRESS),
      0x6b  | IOC_WRITE_ENABLE,
      AccessS3SaveWidth32,
      &ex489.Value,
      StdHeader
    );
  } while (ex488.Field.VoltageChangeReq != ex489.Field.VoltageChangeAck);
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
    (UINT32)~(BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7),
    0x0,
    GnbLibGetHeader (Pcie)
    );
  GnbLibPciIndirectRMW (
    Silicon->Address.AddressValue | D0F0x60_ADDRESS,
    D0F0x64_x00_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    (UINT32)~BIT6,
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
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieSiliconHidePorts Enter\n");

  D0F0x64_x0C.Value =  BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7;
  WrapperList = PcieConfigGetChildWrapper (Silicon);
  while (WrapperList != NULL) {
    PCIe_ENGINE_CONFIG   *EngineList;
    EngineList = PcieConfigGetChildEngine (WrapperList);
    while (EngineList != NULL) {
      if (PcieConfigIsPcieEngine (EngineList)) {
        if (PcieConfigIsActivePcieEngine (EngineList) && !PcieConfigIsSbPcieEngine (EngineList)) {
          D0F0x64_x0C.Value &= ~(1 << EngineList->Type.Port.Address.Address.Device);
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
    (UINT32)~(BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7),
    D0F0x64_x0C.Value,
    GnbLibGetHeader (Pcie)
    );
  GnbLibPciIndirectRMW (
    Silicon->Address.AddressValue | D0F0x60_ADDRESS,
    D0F0x64_x00_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    (UINT32)~BIT6,
    0x0,
    GnbLibGetHeader (Pcie)
    );
  IDS_HDT_CONSOLE (GNB_TRACE, "Write D0F0x64_x0C.Value = %x\n", D0F0x64_x0C.Value);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieSiliconHidePorts Exit\n");
}

