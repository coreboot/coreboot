/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Hudson2 Pcie controller
 *
 * Init GPP (pcie Controller) features.
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
#define FILECODE PROC_FCH_PCIE_FAMILY_HUDSON2_HUDSON2GPPSERVICE_FILECODE

/**
 * ProgramGppTogglePcieReset - Toggle PCIE_RST2#
 *
 *
 * @param[in] DoToggling
 * @param[in] StdHeader
 *
 */
VOID
ProgramGppTogglePcieReset (
  IN     BOOLEAN                DoToggling,
  IN     AMD_CONFIG_PARAMS      *StdHeader
  )
{
  if (DoToggling) {
    FchResetPcie (FchBlock, AssertReset, StdHeader);
    FchStall (500, StdHeader);
    FchResetPcie (FchBlock, DeassertReset, StdHeader);
  } else {
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + FCH_GEVENT_REG04, AccessWidth8, (UINT32)~(BIT1 + BIT0), 0x02);
  }
}

/**
 * FchGppDynamicPowerSaving - GPP Dynamic Power Saving
 *
 *
 * @param[in] FchGpp      Pointer to Fch GPP configuration structure
 * @param[in] StdHeader   Pointer to AMD_CONFIG_PARAMS
 *
 */
VOID
FchGppDynamicPowerSaving (
  IN       FCH_GPP             *FchGpp,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  FCH_GPP_PORT_CONFIG    *PortCfg;
  UINT32                 GppData32;
  UINT32                 HoldGppData32;
  UINT32                 AbValue;

  if (!FchGpp->GppDynamicPowerSaving || FchGpp->SerialDebugBusEnable) {
    return;
  }

  if (FchGpp->GppHardwareDownGrade) {
    PortCfg = &FchGpp->PortCfg[FchGpp->GppHardwareDownGrade - 1];
    PortCfg->PortDetected = TRUE;
  }

  GppData32 = 0;
  HoldGppData32 = 0;

  switch ( FchGpp->GppLinkConfig ) {
  case PortA4:
    PortCfg = &FchGpp->PortCfg[0];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= 0x0f0f;
      HoldGppData32 |= 0x1000;
    }
    break;

  case PortA2B2:
    PortCfg = &FchGpp->PortCfg[0];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= ( FchGpp->GppLaneReversal )? 0x0c0c:0x0303;
      HoldGppData32 |= 0x1000;
    }

    PortCfg = &FchGpp->PortCfg[1];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= ( FchGpp->GppLaneReversal )? 0x0303:0x0c0c;
      HoldGppData32 |= 0x2000;
    }
    break;

  case PortA2B1C1:
    PortCfg = &FchGpp->PortCfg[0];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= ( FchGpp->GppLaneReversal )? 0x0c0c:0x0303;
      HoldGppData32 |= 0x1000;
    }

    PortCfg = &FchGpp->PortCfg[1];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= ( FchGpp->GppLaneReversal )? 0x0202:0x0404;
      HoldGppData32 |= 0x2000;
    }

    PortCfg = &FchGpp->PortCfg[2];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= ( FchGpp->GppLaneReversal )? 0x0101:0x0808;
      HoldGppData32 |= 0x4000;
    }
    break;

  case PortA1B1C1D1:
    PortCfg = &FchGpp->PortCfg[0];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= ( FchGpp->GppLaneReversal )? 0x0808:0x0101;
      HoldGppData32 |= 0x1000;
    }

    PortCfg = &FchGpp->PortCfg[1];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= ( FchGpp->GppLaneReversal )? 0x0404:0x0202;
      HoldGppData32 |= 0x2000;
    }

    PortCfg = &FchGpp->PortCfg[2];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= ( FchGpp->GppLaneReversal )? 0x0202:0x0404;
      HoldGppData32 |= 0x4000;
    }

    PortCfg = &FchGpp->PortCfg[3];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= ( FchGpp->GppLaneReversal )? 0x0101:0x0808;
      HoldGppData32 |= 0x8000;
    }
    break;

  default:
    ASSERT (FALSE);
    break;
  }

  //
  // Power Saving With GPP Disable
  // ABCFG 0xC0[8] = 0x0
  // ABCFG 0xC0[15:12] = 0xF
  // Enable "Power Saving Feature for A-Link Express Lanes"
  // Enable "Power Saving Feature for GPP Lanes"
  // ABCFG 0x90[19] = 1
  // ABCFG 0x90[6] = 1
  // RCINDC_Reg 0x65 [27:0] = 0xFFFFFFF
  // ABCFG 0xC0[7:4] = 0x0
  //
  if (FchGpp->UmiPhyPllPowerDown && FchGpp->GppPhyPllPowerDown ) {
    AbValue = ReadAlink (FCH_ABCFG_REGC0 | (UINT32) (ABCFG << 29), StdHeader);
    WriteAlink (FCH_ABCFG_REGC0 | (UINT32) (ABCFG << 29), (( AbValue | HoldGppData32 ) & (~ BIT8 )), StdHeader);
    RwAlink (FCH_AX_INDXC_REG40, (UINT32)~(BIT9 + BIT4), (BIT0 + BIT3 + BIT12), StdHeader);
    RwAlink ((FCH_ABCFG_REG90 | (UINT32) (ABCFG << 29)), 0xFFFFFFFF, (BIT6 + BIT19), StdHeader);
    RwAlink (FCH_RCINDXC_REG65, 0xFFFFFFFF, ((GppData32 & 0x0F) == 0x0F) ? GppData32 | 0x0CFF0000 : GppData32, StdHeader);
  }
}

