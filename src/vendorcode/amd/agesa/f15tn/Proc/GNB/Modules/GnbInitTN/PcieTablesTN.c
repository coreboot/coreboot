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

STATIC PCIE_HOST_REGISTER_ENTRY PcieInitEarlyTable ROMDATA[] = {
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
  sizeof (PcieInitEarlyTable) / sizeof (PCIE_HOST_REGISTER_ENTRY)
};

STATIC PCIE_HOST_REGISTER_ENTRY ROMDATA CoreInitTable [] = {
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
  sizeof (CoreInitTable) / sizeof (PCIE_HOST_REGISTER_ENTRY)
};


STATIC PCIE_PORT_REGISTER_ENTRY ROMDATA PortInitEarlyTable [] = {
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
  sizeof (PortInitEarlyTable) / sizeof (PCIE_PORT_REGISTER_ENTRY)
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
  sizeof (PortInitMidTable) / sizeof (PCIE_PORT_REGISTER_ENTRY)
};
