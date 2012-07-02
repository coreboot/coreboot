/* $NoKeywords:$ */
/**
 * @file
 *
 * Family specific PCIe configuration data definition
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
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbPcieFamServices.h"
#include  "PcieComplexDataTN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITTN_PCIECOMPLEXDATATN_FILECODE
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


TN_COMPLEX_CONFIG ComplexDataTN = {
  //Silicon
  {
    {
      DESCRIPTOR_SILICON | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY,
      0,
      0,
      offsetof (TN_COMPLEX_CONFIG, GfxWrapper) - offsetof (TN_COMPLEX_CONFIG, Silicon)
    },
    0,
    0
  },
  //Gfx Wrapper
  {
    {
      DESCRIPTOR_PCIE_WRAPPER | DESCRIPTOR_DDI_WRAPPER,
      offsetof (TN_COMPLEX_CONFIG, GfxWrapper) - offsetof (TN_COMPLEX_CONFIG, Silicon),
      offsetof (TN_COMPLEX_CONFIG, GppWrapper) - offsetof (TN_COMPLEX_CONFIG, GfxWrapper),
      offsetof (TN_COMPLEX_CONFIG, Port2) - offsetof (TN_COMPLEX_CONFIG, GfxWrapper)
    },

    GFX_WRAP_ID,
    GFX_NUMBER_OF_PIFs,
    GFX_START_PHY_LANE,
    GFX_END_PHY_LANE,
    GFX_CORE_ID,
    GFX_CORE_ID,
    16,
    {
      1,  //PowerOffUnusedLanesEnabled,
      1,  //PowerOffUnusedPllsEnabled
      1,  //ClkGating
      1,  //LclkGating
      1,  //TxclkGatingPllPowerDown
      1,  //PllOffInL1
      0   //AccessEncoding
    },
  },
  //Gpp Wrapper
  {
    {
      DESCRIPTOR_PCIE_WRAPPER,
      offsetof (TN_COMPLEX_CONFIG, GppWrapper) - offsetof (TN_COMPLEX_CONFIG, Silicon),
      offsetof (TN_COMPLEX_CONFIG, DdiWrapper) - offsetof (TN_COMPLEX_CONFIG, GppWrapper),
      offsetof (TN_COMPLEX_CONFIG, Port4) - offsetof (TN_COMPLEX_CONFIG, GppWrapper)
    },
    GPP_WRAP_ID,
    GPP_NUMBER_OF_PIFs,
    GPP_START_PHY_LANE,
    GPP_END_PHY_LANE,
    GPP_CORE_ID,
    GPP_CORE_ID,
    8,
    {
      1,  //PowerOffUnusedLanesEnabled,
      1,  //PowerOffUnusedPllsEnabled
      1,  //ClkGating
      1,  //LclkGating
      1,  //TxclkGatingPllPowerDown
      1,  //PllOffInL1
      0   //AccessEncoding
    },
  },
  //DDI Wrapper
  {
    {
      DESCRIPTOR_DDI_WRAPPER,
      offsetof (TN_COMPLEX_CONFIG, DdiWrapper) - offsetof (TN_COMPLEX_CONFIG, Silicon),
      offsetof (TN_COMPLEX_CONFIG, Ddi2Wrapper) - offsetof (TN_COMPLEX_CONFIG, DdiWrapper),
      offsetof (TN_COMPLEX_CONFIG, DpE) - offsetof (TN_COMPLEX_CONFIG, DdiWrapper)
    },
    DDI_WRAP_ID,
    DDI_NUMBER_OF_PIFs,
    DDI_START_PHY_LANE,
    DDI_END_PHY_LANE,
    0xf,
    0x0,
    8,
    {
      1,  //PowerOffUnusedLanesEnabled,
      1,  //PowerOffUnusedPllsEnabled
      1,  //ClkGating
      1,  //LclkGating
      1,  //TxclkGatingPllPowerDown
      0,  //PllOffInL1
      0   //AccessEncoding
    },
  },
  //DDI2 Wrapper
  {
    {
      DESCRIPTOR_DDI_WRAPPER | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY,
      offsetof (TN_COMPLEX_CONFIG, Ddi2Wrapper) - offsetof (TN_COMPLEX_CONFIG, Silicon),
      0,
      offsetof (TN_COMPLEX_CONFIG, DpA) - offsetof (TN_COMPLEX_CONFIG, Ddi2Wrapper)
    },
    DDI2_WRAP_ID,
    DDI2_NUMBER_OF_PIFs,
    DDI2_START_PHY_LANE,
    DDI2_END_PHY_LANE,
    0xf,
    0x0,
    8,
    {
      1,  //PowerOffUnusedLanesEnabled,
      1,  //PowerOffUnusedPllsEnabled
      1,  //ClkGating
      1,  //LclkGating
      1,  //TxclkGatingPllPowerDown
      0,  //PllOffInL1
      0   //AccessEncoding
    },
  },
  //Port 2
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (TN_COMPLEX_CONFIG, Port2) - offsetof (TN_COMPLEX_CONFIG, GfxWrapper),
      offsetof (TN_COMPLEX_CONFIG, Port3) - offsetof (TN_COMPLEX_CONFIG, Port2),
      0
    },
    { PciePortEngine, 8, 23},
    0,                                              //Initialization Status
    0xFF,                                           //Scratch
    {
      {
        {0},
        0,
        15,
        2,
        0,
        GFX_CORE_ID,
        0,
        {0},
        LinkStateResetExit,
        0,
        2,
        1
      },
    },
  },
  //Port 3
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (TN_COMPLEX_CONFIG, Port3) - offsetof (TN_COMPLEX_CONFIG, GfxWrapper),
      offsetof (TN_COMPLEX_CONFIG, DpB) - offsetof (TN_COMPLEX_CONFIG, Port3),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID },
    0,                                              //Initialization Status
    0xFF,                                           //Scratch
    {
      {
        {0},
        UNUSED_LANE_ID,
        UNUSED_LANE_ID,
        3,
        0,
        GFX_CORE_ID,
        1,
        {0},
        LinkStateResetExit,
        1,
        3,
        1
      },
    },
  },
  //DdiB
  {
    {
      DESCRIPTOR_DDI_ENGINE,
      offsetof (TN_COMPLEX_CONFIG, DpB) - offsetof (TN_COMPLEX_CONFIG, GfxWrapper),
      offsetof (TN_COMPLEX_CONFIG, DpC) - offsetof (TN_COMPLEX_CONFIG, DpB),
      0
    },
    {PcieDdiEngine},
    0,                                              //Initialization Status
    0xFF                                            //Scratch
  },
  //DdiC
  {
    {
      DESCRIPTOR_DDI_ENGINE,
      offsetof (TN_COMPLEX_CONFIG, DpC) - offsetof (TN_COMPLEX_CONFIG, GfxWrapper),
      offsetof (TN_COMPLEX_CONFIG, DpD) - offsetof (TN_COMPLEX_CONFIG, DpC),
      0
    },
    {PcieDdiEngine},
    0,                                               //Initialization Status
    0xFF                                             //Scratch
  },
  //DdiD
  {
    {
      DESCRIPTOR_DDI_ENGINE | DESCRIPTOR_TERMINATE_LIST,
      offsetof (TN_COMPLEX_CONFIG, DpD) - offsetof (TN_COMPLEX_CONFIG, GfxWrapper),
      offsetof (TN_COMPLEX_CONFIG, Port4) - offsetof (TN_COMPLEX_CONFIG, DpD),
      0
    },
    {PcieDdiEngine},
    0,                                               //Initialization Status
    0xFF                                             //Scratch
  },

  //Port 4
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (TN_COMPLEX_CONFIG, Port4) - offsetof (TN_COMPLEX_CONFIG, GppWrapper),
      offsetof (TN_COMPLEX_CONFIG, Port5) - offsetof (TN_COMPLEX_CONFIG, Port4),
      0
    },
    { PciePortEngine, 4, 4},
    0,                                              //Initialization Status
    0xFF,                                           //Scratch
    {
      {
        {0},
        4,
        4,
        4,
        0,
        GPP_CORE_ID,
        1,
        {0},
        LinkStateResetExit,
        2,
        0,
        0
      },
    },
  },
  //Port 5
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (TN_COMPLEX_CONFIG, Port5) - offsetof (TN_COMPLEX_CONFIG, GppWrapper),
      offsetof (TN_COMPLEX_CONFIG, Port6) - offsetof (TN_COMPLEX_CONFIG, Port5),
      0
    },
    { PciePortEngine, 5, 5},
    0,                                              //Initialization Status
    0xFF,                                           //Scratch
    {
      {
        {0},
        5,
        5,
        5,
        0,
        GPP_CORE_ID,
        2,
        {0},
        LinkStateResetExit,
        3,
        0,
        0
      },
    },
  },
  //Port 6
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (TN_COMPLEX_CONFIG, Port6) - offsetof (TN_COMPLEX_CONFIG, GppWrapper),
      offsetof (TN_COMPLEX_CONFIG, Port7) - offsetof (TN_COMPLEX_CONFIG, Port6),
      0
    },
    { PciePortEngine, 6, 6 },
    0,                                              //Initialization Status
    0xFF,                                           //Scratch
    {
      {
        {0},
        6,
        6,
        6,
        0,
        GPP_CORE_ID,
        3,
        {0},
        LinkStateResetExit,
        4,
        0,
        0
      },
    },
  },
  //Port 7
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (TN_COMPLEX_CONFIG, Port7) - offsetof (TN_COMPLEX_CONFIG, GppWrapper),
      offsetof (TN_COMPLEX_CONFIG, Port8) - offsetof (TN_COMPLEX_CONFIG, Port7),
      0
    },
    { PciePortEngine, 7, 7 },
    0,                                              //Initialization Status
    0xFF,                                           //Scratch
    {
      {
        {0},
        7,
        7,
        7,
        0,
        GPP_CORE_ID,
        4,
        {0},
        LinkStateResetExit,
        5,
        0,
        0
      },
    },
  },
  //Port 8
  {
    {
      DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE | DESCRIPTOR_TERMINATE_LIST,
      offsetof (TN_COMPLEX_CONFIG, Port8) - offsetof (TN_COMPLEX_CONFIG, GppWrapper),
      offsetof (TN_COMPLEX_CONFIG, DpE) - offsetof (TN_COMPLEX_CONFIG, Port8),
      0
    },
    { PciePortEngine, 0, 3 },
    INIT_STATUS_PCIE_TRAINING_SUCCESS,            //Initialization Status
    0xFF,                                         //Scratch
    {
      {
        {PortEnabled, 0, 8, 0, PcieGenMaxSupported, AspmL0sL1, HotplugDisabled, 0x0, {0}},
        0,
        3,
        8,
        0,
        GPP_CORE_ID,
        0,
        {MAKE_SBDFO (0, 0, 8, 0, 0)},
        LinkStateTrainingSuccess,
        6,
        0,
        0
      },
    },
  },
  //DpE
  {
    {
      DESCRIPTOR_DDI_ENGINE,
      offsetof (TN_COMPLEX_CONFIG, DpE) - offsetof (TN_COMPLEX_CONFIG, DdiWrapper),
      offsetof (TN_COMPLEX_CONFIG, DpF) - offsetof (TN_COMPLEX_CONFIG, DpE),
      0
    },
   {PcieDdiEngine},
    0,                                              //Initialization Status
    0xFF                                            //Scratch
  },
  //DpF
  {
    {
      DESCRIPTOR_DDI_ENGINE | DESCRIPTOR_TERMINATE_LIST,
      offsetof (TN_COMPLEX_CONFIG, DpF) - offsetof (TN_COMPLEX_CONFIG, DdiWrapper),
      offsetof (TN_COMPLEX_CONFIG, DpA) - offsetof (TN_COMPLEX_CONFIG, DpF),
      0
    },
    {PcieDdiEngine},
    0,                                              //Initialization Status
    0xFF                                            //Scratch
  },
  //DpA
  {
    {
      DESCRIPTOR_DDI_ENGINE | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY,
      offsetof (TN_COMPLEX_CONFIG, DpA) - offsetof (TN_COMPLEX_CONFIG, Ddi2Wrapper),
      0,
      0
    },
    {PcieDdiEngine},
    0,                                              //Initialization Status
    0xFF                                            //Scratch
  },
  //F12 specific Silicon
  {
    OscFuses,
    {0, 0, 0, 0, 0, 0}
  }
};

