/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch GPP controller
 *
 * Init GPP features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
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
****************************************************************************
*/
#include "FchPlatform.h"
#include "Ids.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_PCIE_GPPHP_FILECODE

VOID
FchGppHotplugSmiCallback (
  IN       VOID           *DataPtr
  );

/**
 * GPP hot plug handler
 *
 *
 * @param[in] FchGpp      Pointer to Fch GPP configuration structure
 * @param[in] HpPort      The hot plug port number
 * @param[in] StdHeader   Pointer to AMD_CONFIG_PARAMS
 *
 */
STATIC VOID
FchGppHotPlugSmiProcess (
  IN       FCH_GPP             *FchGpp,
  IN       UINT32              HpPort,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8                  FailedPort;
  UINT8                  GppS3Data;

  GppS3Data = 0x00;
  ReadMem ( ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x0D, AccessWidth8, &GppS3Data);
  RwAlink (FCH_RCINDXC_REG40, (UINT32)~BIT3, 0, StdHeader);

  //
  // First restore GPP pads if needed
  //
  if (FchGpp->GppDynamicPowerSaving && FchGpp->UmiPhyPllPowerDown && FchGpp->GppPhyPllPowerDown) {
    RwAlink (0xC0  | (UINT32) (ABCFG << 29), ~(UINT32) (1 << (12 + HpPort)), 0, StdHeader);
    RwAlink (FCH_RCINDXC_REG65, ~(UINT32) (0x101 << HpPort), 0, StdHeader);
    FchStall (1000, StdHeader);
  }

  FailedPort = (UINT8) (1 << HpPort);
  if (FchGpp->GppGen2 && FchGpp->GppGen2Strap) {
    GppS3Data &= (UINT8) !(1 << HpPort);
    if (GppPortPollingLtssm (FchGpp, FailedPort, TRUE, StdHeader)) {
      FchGppForceGen1 (FchGpp, FailedPort, StdHeader);
      FailedPort = GppPortPollingLtssm (FchGpp, FailedPort, FALSE, StdHeader);
      GppS3Data |= (UINT8) (1 << HpPort);
    }
  } else {
    FchGppForceGen1 (FchGpp, FailedPort, StdHeader);
    FailedPort = GppPortPollingLtssm (FchGpp, FailedPort, FALSE, StdHeader);
    GppS3Data |= (UINT8) (1 << HpPort);
  }
  GppS3Data |= (UINT8) (1 << (HpPort + 4));
  RwMem (ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x0D, AccessWidth8, 0, GppS3Data);
  GppGen2Workaround (FchGpp, StdHeader);
}


/**
 * GPP hot-unplug handler
 *
 *
 * @param[in] FchGpp      Pointer to Fch GPP configuration structure
 * @param[in] HpPort      The hot plug port number.
 * @param[in] StdHeader   Pointer to AMD_CONFIG_PARAMS
 *
 */
STATIC VOID
FchGppHotUnplugSmiProcess (
  IN       FCH_GPP             *FchGpp,
  IN       UINT32              HpPort,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8                  GppS3Data;

  GppS3Data = 0x00;
  ReadMem ( ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x0D, AccessWidth8, &GppS3Data);
  FchGpp->PortCfg[HpPort].PortDetected = FALSE;
  GppS3Data &= (UINT8) !(1 << (HpPort + 4));

  if (FchGpp->GppGen2 && FchGpp->GppGen2Strap) {
    FchGppForceGen2 (FchGpp, (UINT8) (1 << HpPort), StdHeader);
  }

  if (FchGpp->GppDynamicPowerSaving && FchGpp->UmiPhyPllPowerDown && FchGpp->GppPhyPllPowerDown) {
    RwAlink (FCH_RCINDXP_REGA2 | HpPort << 24, ~(UINT32) (BIT17), BIT17, StdHeader);
    RwAlink (FCH_RCINDXP_REGA2 | HpPort << 24, ~(UINT32) (BIT8), BIT8, StdHeader);
    RwAlink (0xC0  | (UINT32) (ABCFG << 29), ~(UINT32) (1 << (12 + HpPort)), (1 << (12 + HpPort)), StdHeader);
    RwAlink (FCH_RCINDXP_REGA2 | HpPort << 24, ~(UINT32) (BIT17), 0, StdHeader);

    GppGen2Workaround (FchGpp, StdHeader);

    // Finally re-configure GPP pads if needed
    FchGppDynamicPowerSaving (FchGpp, StdHeader);
  }
  RwMem (ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x0D, AccessWidth8, 0, GppS3Data);
}


/**
 * SMI handler for GPP hot-plug
 *
 *
 * @param[in] DataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchGppHotplugSmiCallback (
  IN       VOID           *DataPtr
  )
{
  UINT32             PortNum;
  UINT32             HpPort;
  FCH_DATA_BLOCK     *FchDb;
  UINT8              HpGeventNum;
  UINT8              GpioPinState;

  FchDb = (FCH_DATA_BLOCK*) DataPtr;
  if (!FchDb->Gpp.GppFunctionEnable) {
    return;
  }

  HpPort = 0xff;
  for (PortNum = 0; PortNum < MAX_GPP_PORTS; PortNum++) {
    if (FchDb->Gpp.PortCfg[PortNum].PortHotPlug == TRUE) {
      HpPort = PortNum;
      break;
    }
  }

  if (HpPort == 0xff) {
    return;
  }

  HpGeventNum = FchDb->Gpp.GppHotPlugGeventNum & 31;
  GpioPinState = ACPIMMIO8 (ACPI_MMIO_BASE + GPIO_BASE + FCH_GEVENT_REG00 + HpGeventNum) >> 7;
  if (!GpioPinState) {
    AGESA_TESTPOINT (TpFchGppHotPlugging, FchDb->StdHeader);
    FchGppHotPlugSmiProcess (&FchDb->Gpp, HpPort, FchDb->StdHeader);
  } else {
    AGESA_TESTPOINT (TpFchGppHotUnplugging, FchDb->StdHeader);
    FchGppHotUnplugSmiProcess (&FchDb->Gpp, HpPort, FchDb->StdHeader);
  }

  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98) ^= (1 << HpGeventNum);     // Swap SmiTrig
}
