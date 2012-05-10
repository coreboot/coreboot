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
 * @e \$Revision: 49633 $   @e \$Date: 2011-03-26 06:52:29 +0800 (Sat, 26 Mar 2011) $
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
    RwMem (ACPI_MMIO_BASE + IOMUX_BASE + FCH_GEVENT_REG04, AccessWidth8, ~(BIT1 + BIT0), 0x02);
  }
}

/**
 * FchGppDynamicPowerSaving - GPP Dynamic Power Saving
 *
 *
 * @param[in] FchDataPtr
 *
 */
VOID
FchGppDynamicPowerSaving (
  IN  VOID     *FchDataPtr
  )
{
  FCH_GPP_PORT_CONFIG  *PortCfg;
  UINT8               FchGppLaneReversal;
  UINT8               FchAlinkPhyPllPowerDown;
  UINT8               FchGppPhyPllPowerDown;
  UINT32              GppData32;
  UINT32              HoldGppData32;
  UINT32              AbValue;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  if (!LocalCfgPtr->Gpp.GppDynamicPowerSaving || LocalCfgPtr->SerialDb.SerialDebugBusEnable) {
    return;
  }

  FchAlinkPhyPllPowerDown = (UINT8) LocalCfgPtr->Ab.UmiPhyPllPowerDown;
  FchGppLaneReversal =  (UINT8) LocalCfgPtr->Gpp.GppLaneReversal;
  FchGppPhyPllPowerDown =  (UINT8) LocalCfgPtr->Gpp.GppPhyPllPowerDown;

  if (LocalCfgPtr->Gpp.GppHardwareDownGrade) {
    PortCfg = &LocalCfgPtr->Gpp.PortCfg[LocalCfgPtr->Gpp.GppHardwareDownGrade - 1];
    PortCfg->PortDetected = TRUE;
  }

  GppData32 = 0;
  HoldGppData32 = 0;

  switch ( LocalCfgPtr->Gpp.GppLinkConfig ) {
  case PortA4:
    PortCfg = &LocalCfgPtr->Gpp.PortCfg[0];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= 0x0f0f;
      HoldGppData32 |= 0x1000;
    }
    break;

  case PortA2B2:
    PortCfg = &LocalCfgPtr->Gpp.PortCfg[0];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= ( FchGppLaneReversal )? 0x0c0c:0x0303;
      HoldGppData32 |= 0x1000;
    }

    PortCfg = &LocalCfgPtr->Gpp.PortCfg[1];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= ( FchGppLaneReversal )? 0x0303:0x0c0c;
      HoldGppData32 |= 0x2000;
    }
    break;

  case PortA2B1C1:
    PortCfg = &LocalCfgPtr->Gpp.PortCfg[0];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= ( FchGppLaneReversal )? 0x0c0c:0x0303;
      HoldGppData32 |= 0x1000;
    }

    PortCfg = &LocalCfgPtr->Gpp.PortCfg[1];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= ( FchGppLaneReversal )? 0x0202:0x0404;
      HoldGppData32 |= 0x2000;
    }

    PortCfg = &LocalCfgPtr->Gpp.PortCfg[2];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= ( FchGppLaneReversal )? 0x0101:0x0808;
      HoldGppData32 |= 0x4000;
    }
    break;

  case PortA1B1C1D1:
    PortCfg = &LocalCfgPtr->Gpp.PortCfg[0];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= ( FchGppLaneReversal )? 0x0808:0x0101;
      HoldGppData32 |= 0x1000;
    }

    PortCfg = &LocalCfgPtr->Gpp.PortCfg[1];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= ( FchGppLaneReversal )? 0x0404:0x0202;
      HoldGppData32 |= 0x2000;
    }

    PortCfg = &LocalCfgPtr->Gpp.PortCfg[2];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= ( FchGppLaneReversal )? 0x0202:0x0404;
      HoldGppData32 |= 0x4000;
    }

    PortCfg = &LocalCfgPtr->Gpp.PortCfg[3];
    if ( PortCfg->PortDetected == FALSE ) {
      GppData32 |= ( FchGppLaneReversal )? 0x0101:0x0808;
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
  if ( FchAlinkPhyPllPowerDown && FchGppPhyPllPowerDown ) {
    AbValue = ReadAlink (FCH_ABCFG_REGC0 | (UINT32) (ABCFG << 29), StdHeader);
    WriteAlink (FCH_ABCFG_REGC0 | (UINT32) (ABCFG << 29), (( AbValue | HoldGppData32 ) & (~ BIT8 )), StdHeader);
    RwAlink (FCH_AX_INDXC_REG40, ~(BIT9 + BIT4), (BIT0 + BIT3 + BIT12), StdHeader);
    RwAlink ((FCH_ABCFG_REG90 | (UINT32) (ABCFG << 29)), 0xFFFFFFFF, (BIT6 + BIT19), StdHeader);
    RwAlink (RC_INDXC_REG65, 0xFFFFFFFF, ((GppData32 & 0x0F) == 0x0F) ? GppData32 | 0x0CFF0000 : GppData32, StdHeader);
    RwAlink (RC_INDXC_REG40, ~(BIT9 + BIT4), (BIT0 + BIT3 + BIT12), StdHeader);
  }
}

