/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe late post initialization.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 65061 $   @e \$Date: 2012-02-06 23:48:39 -0600 (Mon, 06 Feb 2012) $
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
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "PcieComplexDataTN.h"
#include  "GnbRegistersTN.h"

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


/*----------------------------------------------------------------------------------------
 *           T A B L E S
 *----------------------------------------------------------------------------------------
 */

STATIC CONST PCIE_HOST_REGISTER_ENTRY PcieInitEarlyTable ROMDATA[] = {
  {
    WRAP_SPACE (GPP_WRAP_ID, D0F0xE4_WRAP_8016_ADDRESS),
    D0F0xE4_WRAP_8016_CalibAckLatency_MASK,
    0
  },
  {
    PHY_SPACE (GPP_WRAP_ID, 0, D0F0xE4_PHY_2008_ADDRESS),
    D0F0xE4_PHY_2008_VdDetectEn_MASK,
    0x1 << D0F0xE4_PHY_2008_VdDetectEn_OFFSET
  },
  {
    PHY_SPACE (GFX_WRAP_ID, 0, D0F0xE4_PHY_2008_ADDRESS),
    D0F0xE4_PHY_2008_VdDetectEn_MASK,
    0x1 << D0F0xE4_PHY_2008_VdDetectEn_OFFSET
  },
  {
    PHY_SPACE (GFX_WRAP_ID, 1, D0F0xE4_PHY_2008_ADDRESS),
    D0F0xE4_PHY_2008_VdDetectEn_MASK,
    0x1 << D0F0xE4_PHY_2008_VdDetectEn_OFFSET
  },
  {
    PHY_SPACE (DDI_WRAP_ID, 0, D0F0xE4_PHY_2008_ADDRESS),
    D0F0xE4_PHY_2008_VdDetectEn_MASK,
    0x1 << D0F0xE4_PHY_2008_VdDetectEn_OFFSET
  },
  {
    PHY_SPACE (DDI2_WRAP_ID, 0, D0F0xE4_PHY_2008_ADDRESS),
    D0F0xE4_PHY_2008_VdDetectEn_MASK,
    0x1 << D0F0xE4_PHY_2008_VdDetectEn_OFFSET
  }
 };

CONST PCIE_HOST_REGISTER_TABLE_HEADER ROMDATA PcieInitEarlyTableTN  = {
  &PcieInitEarlyTable[0],
  ARRAY_SIZE(PcieInitEarlyTable)
  };

STATIC CONST PCIE_HOST_REGISTER_ENTRY ROMDATA CoreInitTable [] = {
  {
    D0F0xE4_CORE_0020_ADDRESS,
    D0F0xE4_CORE_0020_CiRcOrderingDis_MASK |
    D0F0xE4_CORE_0020_CiSlvOrderingDis_MASK,
    (0x1 << D0F0xE4_CORE_0020_CiRcOrderingDis_OFFSET)
  },
  {
    D0F0xE4_CORE_0010_ADDRESS,
    D0F0xE4_CORE_0010_RxSbAdjPayloadSize_MASK,
    (0x4 << D0F0xE4_CORE_0010_RxSbAdjPayloadSize_OFFSET)
  },
  {
    D0F0xE4_CORE_001C_ADDRESS,
    D0F0xE4_CORE_001C_TxArbRoundRobinEn_MASK |
    D0F0xE4_CORE_001C_TxArbSlvLimit_MASK |
    D0F0xE4_CORE_001C_TxArbMstLimit_MASK,
    (0x1 << D0F0xE4_CORE_001C_TxArbRoundRobinEn_OFFSET) |
    (0x4 << D0F0xE4_CORE_001C_TxArbSlvLimit_OFFSET) |
    (0x4 << D0F0xE4_CORE_001C_TxArbMstLimit_OFFSET)
  },
  {
    D0F0xE4_CORE_0040_ADDRESS,
    D0F0xE4_CORE_0040_PElecIdleMode_MASK,
    (0x2 << D0F0xE4_CORE_0040_PElecIdleMode_OFFSET)
  },
  {
    D0F0xE4_CORE_0002_ADDRESS,
    D0F0xE4_CORE_0002_HwDebug_0__MASK,
    (0x1 << D0F0xE4_CORE_0002_HwDebug_0__OFFSET)
  },
  {
    D0F0xE4_CORE_00C1_ADDRESS,
    D0F0xE4_CORE_00C1_StrapLinkBwNotificationCapEn_MASK |
    D0F0xE4_CORE_00C1_StrapGen2Compliance_MASK,
    (0x1 << D0F0xE4_CORE_00C1_StrapLinkBwNotificationCapEn_OFFSET) |
    (0x1 << D0F0xE4_CORE_00C1_StrapGen2Compliance_OFFSET)
  },
  {
    D0F0xE4_CORE_00B0_ADDRESS,
    D0F0xE4_CORE_00B0_StrapF0MsiEn_MASK,
    (0x1 << D0F0xE4_CORE_00B0_StrapF0MsiEn_OFFSET)
  }
};

CONST PCIE_HOST_REGISTER_TABLE_HEADER ROMDATA CoreInitTableTN  = {
  &CoreInitTable[0],
  ARRAY_SIZE(CoreInitTable)
  };


STATIC CONST PCIE_PORT_REGISTER_ENTRY ROMDATA PortInitEarlyTable [] = {
  {
    DxF0xE4_x02_ADDRESS,
    DxF0xE4_x02_RegsLcAllowTxL1Control_MASK,
    (0x1 << DxF0xE4_x02_RegsLcAllowTxL1Control_OFFSET)
  },
  {
    DxF0xE4_x70_ADDRESS,
    DxF0xE4_x70_RxRcbCplTimeoutMode_MASK,
    (0x1 << DxF0xE4_x70_RxRcbCplTimeoutMode_OFFSET)
  },
  {
    DxF0xE4_xA0_ADDRESS,
    DxF0xE4_xA0_Lc16xClearTxPipe_MASK | DxF0xE4_xA0_LcL1ImmediateAck_MASK | DxF0xE4_xA0_LcL0sInactivity_MASK,
    (0x3 << DxF0xE4_xA0_Lc16xClearTxPipe_OFFSET) |
    (0x1 << DxF0xE4_xA0_LcL1ImmediateAck_OFFSET) |
    (0x6 << DxF0xE4_xA0_LcL0sInactivity_OFFSET)
  },
  {
    DxF0xE4_xA1_ADDRESS,
    DxF0xE4_xA1_LcDontGotoL0sifL1Armed_MASK,
    (0x1 << DxF0xE4_xA1_LcDontGotoL0sifL1Armed_OFFSET)
  },
  {
    DxF0xE4_xA2_ADDRESS,
    DxF0xE4_xA2_LcRenegotiateEn_MASK | DxF0xE4_xA2_LcUpconfigureSupport_MASK,
    (0x1 << DxF0xE4_xA2_LcRenegotiateEn_OFFSET) |
    (0x1 << DxF0xE4_xA2_LcUpconfigureSupport_OFFSET)
  },
  {
    DxF0xE4_xA3_ADDRESS,
    DxF0xE4_xA3_LcXmitFtsBeforeRecovery_MASK,
    (0x1 << DxF0xE4_xA3_LcXmitFtsBeforeRecovery_OFFSET)
  },
  {
    DxF0xE4_xB1_ADDRESS,
    DxF0xE4_xB1_LcDeassertRxEnInL0s_MASK | DxF0xE4_xB1_LcBlockElIdleinL0_MASK,
    (0x1 << DxF0xE4_xB1_LcDeassertRxEnInL0s_OFFSET) |
    (0x1 << DxF0xE4_xB1_LcBlockElIdleinL0_OFFSET)
  }
};

CONST PCIE_PORT_REGISTER_TABLE_HEADER ROMDATA PortInitEarlyTableTN  = {
  &PortInitEarlyTable[0],
  ARRAY_SIZE(PortInitEarlyTable)
  };


STATIC PCIE_PORT_REGISTER_ENTRY ROMDATA PortInitMidTable [] = {
  {
    DxF0xE4_xA2_ADDRESS,
    DxF0xE4_xA2_LcDynLanesPwrState_MASK,
    (0x3 << DxF0xE4_xA2_LcDynLanesPwrState_OFFSET)
  },
  {
    DxF0xE4_xC0_ADDRESS,
    DxF0xE4_xC0_StrapAutoRcSpeedNegotiationDis_MASK,
    (0x1 << DxF0xE4_xC0_StrapAutoRcSpeedNegotiationDis_OFFSET)
  }
};

CONST PCIE_PORT_REGISTER_TABLE_HEADER ROMDATA PortInitMidTableTN  = {
  &PortInitMidTable[0],
  ARRAY_SIZE(PortInitMidTable)
  };
