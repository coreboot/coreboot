/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe PIF initialization routine
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 88079 $   @e \$Date: 2013-02-15 15:28:53 -0600 (Fri, 15 Feb 2013) $
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
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbRegistersCommonV2.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEINITLIBV5_PCIEPIFSERVICESV5_FILECODE
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
PciePifApplyGangingV5 (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PciePifPllPowerDownV5 (
  IN      UINT32                LaneBitmap,
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PciePifSetLs2ExitTimeV5 (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------*/
/**
 * Apply PIF ganging for all lanes for given wrapper
 *
 *
 *
 * @param[in]  Wrapper     Pointer to Wrapper config descriptor
 * @param[in]  Pcie        Pointer to PICe configuration data area
 */


VOID
PciePifApplyGangingV5 (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIe_ENGINE_CONFIG      *EngineList;
  UINT32                  LaneBitmap;
  D0F0xE4_PIF_0011_STRUCT D0F0xE4_PIF_0011;
  D0F0xE4_PIF_0011.Value = 0;

  IDS_HDT_CONSOLE (GNB_TRACE, "PciePifApplyGangingV5 Enter\n");
  EngineList = PcieConfigGetChildEngine (Wrapper);
  while (EngineList != NULL) {
    if (PcieLibIsEngineAllocated (EngineList)) {
      LaneBitmap = PcieUtilGetEngineLaneBitMap (LANE_TYPE_PCIE_PHY_NATIVE, 0, EngineList);
      switch (LaneBitmap) {
      case  0x0003:
        D0F0xE4_PIF_0011.Field.X2Lane10 = 0x1;
        break;
      case  0x000c:
        D0F0xE4_PIF_0011.Field.X2Lane32 = 0x1;
        break;
      case  0x0030:
        D0F0xE4_PIF_0011.Field.X2Lane54 = 0x1;
        break;
      case  0x00c0:
        D0F0xE4_PIF_0011.Field.X2Lane76 = 0x1;
        break;
      case  0x000f:
        D0F0xE4_PIF_0011.Field.X4Lane30 = 0x1;
        break;
      case  0x00f0:
        D0F0xE4_PIF_0011.Field.X4Lane74 = 0x1;
        break;
      case  0x00ff:
        D0F0xE4_PIF_0011.Field.X8Lane70 = 0x1;
        break;
      case  0x0300:
        D0F0xE4_PIF_0011.Field.X2Lane98 = 0x1;
        break;
      case  0x0c00:
        D0F0xE4_PIF_0011.Field.X2Lane1110 = 0x1;
        break;
      case  0x3000:
        D0F0xE4_PIF_0011.Field.X2Lane1312 = 0x1;
        break;
      case  0xc000:
        D0F0xE4_PIF_0011.Field.X2Lane1514 = 0x1;
        break;
      case  0x0f00:
        D0F0xE4_PIF_0011.Field.X4Lane118 = 0x1;
        break;
      case  0xf000:
        D0F0xE4_PIF_0011.Field.X4Lane1512 = 0x1;
        break;
      case  0xff00:
        D0F0xE4_PIF_0011.Field.X8Lane158 = 0x1;
        break;
      case  0xffff:
        D0F0xE4_PIF_0011.Field.X16Lane150 = 0x1;
        break;
      default:
        break;
      }
    }
    EngineList = PcieLibGetNextDescriptor (EngineList);
  }
  PcieRegisterWrite (
    Wrapper,
    PIF_SPACE (Wrapper->WrapId, 0, D0F0xE4_PIF_0011_ADDRESS),
    D0F0xE4_PIF_0011.Value,
    FALSE,
    Pcie
    );
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePifApplyGangingV5 Exit\n");
}


/*----------------------------------------------------------------------------------------*/
/**
 * PLL powerdown
 *
 *
 * @param[in]  LaneBitmap  Power down PLL for these lanes
 * @param[in]  Wrapper     Pointer to Wrapper config descriptor
 * @param[in]  Pcie        Pointer to PICe configuration data area
 */

VOID
PciePifPllPowerDownV5 (
  IN      UINT32                LaneBitmap,
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8                   Nibble;
  UINT16                  NibbleBitmap;
  UINT16                  PifRegAffress;
  D0F0xE4_PIF_0012_STRUCT D0F0xE4_PIF_0012;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePifPllPowerDownV5 Enter\n");
  for (Nibble = 0; Nibble < 4; Nibble++) {
    NibbleBitmap = (0xF << (Nibble * 4));
    if ((LaneBitmap & NibbleBitmap) == NibbleBitmap) {
      PifRegAffress = ((Nibble < 2) ? D0F0xE4_PIF_0012_ADDRESS : D0F0xE4_PIF_0017_ADDRESS) + (Nibble & 0x1);
      D0F0xE4_PIF_0012.Value = PcieRegisterRead (
                                 Wrapper,
                                 PIF_SPACE (Wrapper->WrapId, 0, PifRegAffress),
                                 Pcie
                                 );

      D0F0xE4_PIF_0012.Field.PllPowerStateInTxs2 = PifPowerStateOff;
      D0F0xE4_PIF_0012.Field.PllPowerStateInOff = PifPowerStateOff;
      D0F0xE4_PIF_0012.Field.TxPowerStateInTxs2 = PifPowerStateOff;
      D0F0xE4_PIF_0012.Field.RxPowerStateInRxs2 = PifPowerStateOff;
      PcieRegisterWrite (
        Wrapper,
        PIF_SPACE (Wrapper->WrapId, 0, PifRegAffress),
        D0F0xE4_PIF_0012.Value,
        TRUE,
        Pcie
        );
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePifPllPowerDownV5 Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Program LS2 exit time
 *
 *
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */

VOID
PciePifSetLs2ExitTimeV5 (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8 Pif;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePifSetLs2ExitTimeV5 Enter\n");
  for (Pif = 0; Pif < Wrapper->NumberOfPIFs; Pif++) {
    PcieRegisterWriteField (
        Wrapper,
        PIF_SPACE (Wrapper->WrapId, Pif, D0F0xE4_PIF_0010_ADDRESS),
        D0F0xE4_PIF_0010_Ls2ExitTime_OFFSET,
        D0F0xE4_PIF_0010_Ls2ExitTime_WIDTH,
        0x5,
        FALSE,
        Pcie
        );
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePifSetLs2ExitTimeV5 Exit\n");
}
