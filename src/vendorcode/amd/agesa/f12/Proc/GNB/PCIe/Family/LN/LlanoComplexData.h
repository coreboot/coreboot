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
#ifndef _LLANOCOMPLEXDATA_H_
#define _LLANOCOMPLEXDATA_H_


F12_COMPLEX_CONFIG ComplexData = {
  //Silicon
  {
    {
      DESCRIPTOR_SILICON | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY,
      0,
      0,
      offsetof (F12_COMPLEX_CONFIG, GfxWrapper) - offsetof (F12_COMPLEX_CONFIG, Silicon)
    },
    0,
  },
  //Gfx Wrapper
  {
    {
      DESCRIPTOR_PCIE_WRAPPER | DESCRIPTOR_DDI_WRAPPER,
      offsetof (F12_COMPLEX_CONFIG, GfxWrapper) - offsetof (F12_COMPLEX_CONFIG, Silicon),
      offsetof (F12_COMPLEX_CONFIG, GppWrapper) - offsetof (F12_COMPLEX_CONFIG, GfxWrapper),
      offsetof (F12_COMPLEX_CONFIG, Port2) - offsetof (F12_COMPLEX_CONFIG, GfxWrapper)
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
      1   //PllOffInL1
    },
  },
  //Gpp Wrapper
  {
    {
      DESCRIPTOR_PCIE_WRAPPER,
      offsetof (F12_COMPLEX_CONFIG, GppWrapper) - offsetof (F12_COMPLEX_CONFIG, Silicon),
      offsetof (F12_COMPLEX_CONFIG, DdiWrapper) - offsetof (F12_COMPLEX_CONFIG, GppWrapper),
      offsetof (F12_COMPLEX_CONFIG, Port4) - offsetof (F12_COMPLEX_CONFIG, GppWrapper)
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
      1   //PllOffInL1
    },
  },
  //DDI Wrapper
  {
    {
      DESCRIPTOR_DDI_WRAPPER | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY,
      offsetof (F12_COMPLEX_CONFIG, DdiWrapper) - offsetof (F12_COMPLEX_CONFIG, Silicon),
      0,
      offsetof (F12_COMPLEX_CONFIG, Dpa) - offsetof (F12_COMPLEX_CONFIG, DdiWrapper)
    },
    DDI_WRAP_ID,
    DDI_NUMBER_OF_PIFs,
    DDI_START_PHY_LANE,
    DDI_END_PHY_LANE,
    0x0f,
    0x0,
    8,
    {
      1,  //PowerOffUnusedLanesEnabled,
      1,  //PowerOffUnusedPllsEnabled
      1,  //ClkGating
      1,  //LclkGating
      1,  //TxclkGatingPllPowerDown
      0   //PllOffInL1
    },
  },
  //Port 2
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (F12_COMPLEX_CONFIG, Port2) - offsetof (F12_COMPLEX_CONFIG, GfxWrapper),
      offsetof (F12_COMPLEX_CONFIG, Port3) - offsetof (F12_COMPLEX_CONFIG, Port2),
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
        LinkStateResetExit
      },
    },
  },
  //Port 3
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (F12_COMPLEX_CONFIG, Port3) - offsetof (F12_COMPLEX_CONFIG, GfxWrapper),
      offsetof (F12_COMPLEX_CONFIG, Dp1) - offsetof (F12_COMPLEX_CONFIG, Port3),
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
        LinkStateResetExit
      },
    },
  },
  //Ddi1
  {
    {
      DESCRIPTOR_DDI_ENGINE,
      offsetof (F12_COMPLEX_CONFIG, Dp1) - offsetof (F12_COMPLEX_CONFIG, GfxWrapper),
      offsetof (F12_COMPLEX_CONFIG, Dp2) - offsetof (F12_COMPLEX_CONFIG, Dp1),
      0
    },
    {PcieDdiEngine},
    0,                                              //Initialization Status
    0xFF                                            //Scratch
  },
  //Ddi2
  {
    {
      DESCRIPTOR_DDI_ENGINE,
      offsetof (F12_COMPLEX_CONFIG, Dp2) - offsetof (F12_COMPLEX_CONFIG, GfxWrapper),
      offsetof (F12_COMPLEX_CONFIG, Dp3) - offsetof (F12_COMPLEX_CONFIG, Dp2),
      0
    },
    {PcieDdiEngine},
    0,                                               //Initialization Status
    0xFF                                             //Scratch
  },
  //Ddi3
  {
    {
      DESCRIPTOR_DDI_ENGINE,
      offsetof (F12_COMPLEX_CONFIG, Dp3) - offsetof (F12_COMPLEX_CONFIG, GfxWrapper),
      offsetof (F12_COMPLEX_CONFIG, Dp4) - offsetof (F12_COMPLEX_CONFIG, Dp3),
      0
    },
    {PcieDdiEngine},
    0,                                               //Initialization Status
    0xFF                                             //Scratch
  },
  //Ddi4
  {
    {
      DESCRIPTOR_DDI_ENGINE | DESCRIPTOR_TERMINATE_LIST,
      offsetof (F12_COMPLEX_CONFIG, Dp4) - offsetof (F12_COMPLEX_CONFIG, GfxWrapper),
      offsetof (F12_COMPLEX_CONFIG, Port4) - offsetof (F12_COMPLEX_CONFIG, Dp4),
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
      offsetof (F12_COMPLEX_CONFIG, Port4) - offsetof (F12_COMPLEX_CONFIG, GppWrapper),
      offsetof (F12_COMPLEX_CONFIG, Port5) - offsetof (F12_COMPLEX_CONFIG, Port4),
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
        LinkStateResetExit
      },
    },
  },
  //Port 5
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (F12_COMPLEX_CONFIG, Port5) - offsetof (F12_COMPLEX_CONFIG, GppWrapper),
      offsetof (F12_COMPLEX_CONFIG, Port6) - offsetof (F12_COMPLEX_CONFIG, Port5),
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
        LinkStateResetExit
      },
    },
  },
  //Port 6
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (F12_COMPLEX_CONFIG, Port6) - offsetof (F12_COMPLEX_CONFIG, GppWrapper),
      offsetof (F12_COMPLEX_CONFIG, Port7) - offsetof (F12_COMPLEX_CONFIG, Port6),
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
        LinkStateResetExit
      },
    },
  },
  //Port 7
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (F12_COMPLEX_CONFIG, Port7) - offsetof (F12_COMPLEX_CONFIG, GppWrapper),
      offsetof (F12_COMPLEX_CONFIG, Port8) - offsetof (F12_COMPLEX_CONFIG, Port7),
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
        LinkStateResetExit
      },
    },
  },
  //Port 8
  {
    {
      DESCRIPTOR_PCIE_ENGINE | DESCRIPTOR_TERMINATE_LIST,
      offsetof (F12_COMPLEX_CONFIG, Port8) - offsetof (F12_COMPLEX_CONFIG, GppWrapper),
      offsetof (F12_COMPLEX_CONFIG, Dpa) - offsetof (F12_COMPLEX_CONFIG, Port8),
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
        LinkStateTrainingSuccess
      },
    },
  },
  //DpA
  {
    {
      DESCRIPTOR_DDI_ENGINE,
      offsetof (F12_COMPLEX_CONFIG, Dpa) - offsetof (F12_COMPLEX_CONFIG, DdiWrapper),
      offsetof (F12_COMPLEX_CONFIG, Dpb) - offsetof (F12_COMPLEX_CONFIG, Dpa),
      0
    },
    {PcieDdiEngine},
    0,                                              //Initialization Status
    0xFF                                            //Scratch
  },
  //DpB
  {
    {
      DESCRIPTOR_DDI_ENGINE | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY,
      offsetof (F12_COMPLEX_CONFIG, Dpb) - offsetof (F12_COMPLEX_CONFIG, DdiWrapper),
      0,
      0
    },
    {PcieDdiEngine},
    0,                                              //Initialization Status
    0xFF                                            //Scratch
  },
  //F12 specific Silicon
  {
    OscFuses
  }
};

PCIe_PORT_DESCRIPTOR DefaultSbPort = {
  0,
  PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 0, 3),
  PCIE_PORT_DATA_INITIALIZER (PortEnabled, ChannelTypeLowLoss, 8, HotplugDisabled, PcieGenMaxSupported, PcieGenMaxSupported, AspmL0sL1, 0)
};

#endif
