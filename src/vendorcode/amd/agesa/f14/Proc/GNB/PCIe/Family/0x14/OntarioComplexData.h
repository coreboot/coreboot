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
 * @e \$Revision: 37710 $   @e \$Date: 2010-09-10 11:08:20 +0800 (Fri, 10 Sep 2010) $
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
 *
 * ***************************************************************************
 *
 */
#ifndef _ONTARIOCOMPLEXDATA_H_
#define _ONTARIOCOMPLEXDATA_H_

STATIC
CONST F14_COMPLEX_CONFIG ComplexData = {
  //Silicon
  {
    DESCRIPTOR_TERMINATE_LIST,
    {0},
    (VOID *)(offsetof (F14_COMPLEX_CONFIG, GppWrapper)),
    NULL
  },
  //Gpp Wrapper
  {
    DESCRIPTOR_PCIE_WRAPPER,
    GPP_WRAP_ID,
    GPP_NUMBER_OF_PIFs,
    GPP_START_PHY_LANE,
    GPP_END_PHY_LANE,
    GPP_CORE_ID,
    GPP_CORE_ID,
    {
      1,  //PowerOffUnusedLanesEnabled,
      1,  //PowerOffUnusedPllsEnabled
      1,  //ClkGating
      1,  //LclkGating
      1,  //TxclkGatingPllPowerDown
      1   //PllOffInL1
    },
    (VOID *)(offsetof (F14_COMPLEX_CONFIG, Port4)),
    (VOID *)(offsetof (F14_COMPLEX_CONFIG, Silicon)),
    (VOID *)(offsetof (F14_COMPLEX_CONFIG, FmGppWrapper))
  },
  //Virtual DDI Wrapper
  {
    DESCRIPTOR_DDI_WRAPPER | DESCRIPTOR_VIRTUAL | DESCRIPTOR_TERMINATE_LIST,
    DDI_WRAP_ID,
    0,
    DDI_START_PHY_LANE,
    DDI_END_PHY_LANE,
    0xff,
    0x0,
    {
      1,  //PowerOffUnusedLanesEnabled,
      1,  //PowerOffUnusedPllsEnabled
      1,  //ClkGating
      1,  //LclkGating
      1,  //TxclkGatingPllPowerDown
      0   //PllOffInL1
    },
    (VOID *)(offsetof (F14_COMPLEX_CONFIG, Dpa)),
    (VOID *)(offsetof (F14_COMPLEX_CONFIG, Silicon)),
    NULL
  },
  //Port 4
  {
    DESCRIPTOR_PCIE_ENGINE,
    (VOID *)(offsetof (F14_COMPLEX_CONFIG, GppWrapper)),
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
        FALSE,
        LinkStateResetExit
      },
    },
  },
  //Port 5
  {
    DESCRIPTOR_PCIE_ENGINE,
    (VOID *)(offsetof (F14_COMPLEX_CONFIG, GppWrapper)),
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
        FALSE,
        LinkStateResetExit
      },
    },
  },
  //Port 6
  {
    DESCRIPTOR_PCIE_ENGINE,
    (VOID *)(offsetof (F14_COMPLEX_CONFIG, GppWrapper)),
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
        FALSE,
        LinkStateResetExit
      },
    },
  },
  //Port 7
  {
    DESCRIPTOR_PCIE_ENGINE,
    (VOID *)(offsetof (F14_COMPLEX_CONFIG, GppWrapper)),
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
        FALSE,
        LinkStateResetExit
      },
    },
  },
  //Port 8
  {
    DESCRIPTOR_PCIE_ENGINE | DESCRIPTOR_TERMINATE_LIST,
    (VOID *)(offsetof (F14_COMPLEX_CONFIG, GppWrapper)),
    { PciePortEngine, 0, 3 },
    0,                                              //Initialization Status
    0xFF,                                           //Scratch
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
        TRUE,
        LinkStateTrainingSuccess
      },
    },
  },
  //Virtual DpA
  {
    DESCRIPTOR_DDI_ENGINE | DESCRIPTOR_VIRTUAL,
    (VOID *)(offsetof (F14_COMPLEX_CONFIG, DdiWrapper)),
    {PcieDdiEngine},
    0,                                              //Initialization Status
    0xFF,                                           //Scratch
  },
  //Virtual DpB
  {
    DESCRIPTOR_DDI_ENGINE | DESCRIPTOR_VIRTUAL,
    (VOID *)(offsetof (F14_COMPLEX_CONFIG, DdiWrapper)),
    {PcieDdiEngine},
    0,                                              //Initialization Status
    0xFF,                                           //Scratch
  },
  //Virtual VGA
  {
    DESCRIPTOR_DDI_ENGINE | DESCRIPTOR_VIRTUAL | DESCRIPTOR_TERMINATE_LIST,
    (VOID *)(offsetof (F14_COMPLEX_CONFIG, DdiWrapper)),
    {PcieDdiEngine},
    0,                                              //Initialization Status
    0xFF,                                           //Scratch
  },
  //Native Gen Support
  //Set to TRUE after bringup
  {
    TRUE,
  }

};
#endif
