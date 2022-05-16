/* $NoKeywords:$ */
/**
 * @file
 *
 * Family specific PCIe configuration data
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
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
#include  "GnbPcieFamServices.h"
#include  "PcieComplexDataKB.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITKB_PCIECOMPLEXDATAKB_FILECODE
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
AGESA_STATUS
PcieGetComplexDataLengthKB (
  IN       UINT8                         SocketId,
     OUT   UINTN                         *Length,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  );

AGESA_STATUS
PcieBuildComplexConfigurationKB (
  IN       UINT8                         SocketId,
     OUT   VOID                          *Buffer,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  );

UINT32
PcieGetNativePhyLaneBitmapKB (
  IN       UINT32                        PhyLaneBitmap,
  IN       PCIe_ENGINE_CONFIG            *Engine
  );
//
// Complex configuration
//

CONST KB_COMPLEX_CONFIG ComplexDataKB = {
  //Silicon
  {
    {
      DESCRIPTOR_SILICON | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY,
      0,
      0,
      offsetof (KB_COMPLEX_CONFIG, GppWrapper) - offsetof (KB_COMPLEX_CONFIG, Silicon)
    },
    0,
    0xFF,
    0xFF
  },
  //Gpp Wrapper
  {
    {
      DESCRIPTOR_PCIE_WRAPPER,
      offsetof (KB_COMPLEX_CONFIG, GppWrapper) - offsetof (KB_COMPLEX_CONFIG, Silicon),
      offsetof (KB_COMPLEX_CONFIG, DdiWrapper) - offsetof (KB_COMPLEX_CONFIG, GppWrapper),
      offsetof (KB_COMPLEX_CONFIG, PortPBR4) - offsetof (KB_COMPLEX_CONFIG, GppWrapper)
    },
    GPP_WRAP_ID,
    GPP_NUMBER_OF_PIFs,
    GPP_START_PHY_LANE,
    GPP_END_PHY_LANE,
    GPP_CORE_ID,
    GPP_CORE_ID,
    GPP_END_PHY_LANE - GPP_START_PHY_LANE + 1,
    {
      1,
      1,
      1,
      1,
      1,
      1,
      1
    },
  },
  //Virtual DDI Wrapper
  {
    {
      DESCRIPTOR_DDI_WRAPPER | DESCRIPTOR_VIRTUAL | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY,
      offsetof (KB_COMPLEX_CONFIG, DdiWrapper) - offsetof (KB_COMPLEX_CONFIG, Silicon),
      0,
      offsetof (KB_COMPLEX_CONFIG, Ddi1) - offsetof (KB_COMPLEX_CONFIG, DdiWrapper)
    },
    DDI_WRAP_ID,
    0,
    DDI_START_PHY_LANE,
    DDI_END_PHY_LANE,
    0x0F, // Start PCIe Core ID (4 bits) - Should be 'UINT8_MAX' however clang complains of truncation
    0,    // End PCIe Core ID   (4 bits)
    0,
    {
      1,
      1,
      1,
      1,
      1,
      0,
      1
    },
  },
//------------------------------ GPP WRAPPER START-------------------------------------
  //Port PBR4
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (KB_COMPLEX_CONFIG, PortPBR4) - offsetof (KB_COMPLEX_CONFIG, GppWrapper),
      offsetof (KB_COMPLEX_CONFIG, PortPBR3) - offsetof (KB_COMPLEX_CONFIG, PortPBR4),
      0
    },
    { PciePortEngine, GPP_START_PHY_LANE, GPP_END_PHY_LANE },
    0,                                              //Initialization Status
    0xFF,                                           //Scratch
    {
      {
        {0},
        7,
        7,
        PBR4_NATIVE_PCI_DEV,
        PBR4_NATIVE_PCI_FUN,
        PBR4_CORE_ID,
        PBR4_PORT_ID,
        {(UINT32)PBR4_PCI_ADDRESS},
        LinkStateResetExit,
        PBR4,
        PBR4_UNIT_ID,
        PBR4_NUM_UNIT_IDs
      },
    },
  },
  //Port PBR3
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (KB_COMPLEX_CONFIG, PortPBR3) - offsetof (KB_COMPLEX_CONFIG, GppWrapper),
      offsetof (KB_COMPLEX_CONFIG, PortPBR2) - offsetof (KB_COMPLEX_CONFIG, PortPBR3),
      0
    },
    { PciePortEngine, GPP_START_PHY_LANE, GPP_END_PHY_LANE },
    0,                                              //Initialization Status
    0xFF,                                           //Scratch
    {
      {
        {0},
        6,
        6,
        PBR3_NATIVE_PCI_DEV,
        PBR3_NATIVE_PCI_FUN,
        PBR3_CORE_ID,
        PBR3_PORT_ID,
        {(UINT32)PBR3_PCI_ADDRESS},
        LinkStateResetExit,
        PBR3,
        PBR3_UNIT_ID,
        PBR3_NUM_UNIT_IDs
      },
    },
  },
  //Port PBR2
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (KB_COMPLEX_CONFIG, PortPBR2) - offsetof (KB_COMPLEX_CONFIG, GppWrapper),
      offsetof (KB_COMPLEX_CONFIG, PortPBR1) - offsetof (KB_COMPLEX_CONFIG, PortPBR2),
      0
    },
    { PciePortEngine, GPP_START_PHY_LANE, GPP_END_PHY_LANE},
    0,                                              //Initialization Status
    0xFF,                                           //Scratch
    {
      {
        {0},
        5,
        5,
        PBR2_NATIVE_PCI_DEV,
        PBR2_NATIVE_PCI_FUN,
        PBR2_CORE_ID,
        PBR2_PORT_ID,
        {(UINT32)PBR2_PCI_ADDRESS},
        LinkStateResetExit,
        PBR2,
        PBR2_UNIT_ID,
        PBR2_NUM_UNIT_IDs
      },
    },
  },
  //Port PBR1
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (KB_COMPLEX_CONFIG, PortPBR1) - offsetof (KB_COMPLEX_CONFIG, GppWrapper),
      offsetof (KB_COMPLEX_CONFIG, PortPBR0) - offsetof (KB_COMPLEX_CONFIG, PortPBR1),
      0
    },
    { PciePortEngine, GPP_START_PHY_LANE, GPP_END_PHY_LANE },
    0,                                              //Initialization Status
    0xFF,                                           //Scratch
    {
      {
        {0},
        4,
        4,
        PBR1_NATIVE_PCI_DEV,
        PBR1_NATIVE_PCI_FUN,
        PBR1_CORE_ID,
        PBR1_PORT_ID,
        {(UINT32)PBR1_PCI_ADDRESS},
        LinkStateResetExit,
        PBR1,
        PBR1_UNIT_ID,
        PBR1_NUM_UNIT_IDs
      },
    },
  },
  //Port PBR0
  {
    {
      DESCRIPTOR_PCIE_ENGINE | DESCRIPTOR_TERMINATE_LIST,
      offsetof (KB_COMPLEX_CONFIG, PortPBR0) - offsetof (KB_COMPLEX_CONFIG, GppWrapper),
      offsetof (KB_COMPLEX_CONFIG, Ddi1) - offsetof (KB_COMPLEX_CONFIG, PortPBR0),
      0
    },
    { PciePortEngine, GPP_START_PHY_LANE, GPP_END_PHY_LANE },
    0,                                              //Initialization Status
    0xFF,                                           //Scratch
    {
      {
        {0},
        0,
        3,
        PBR0_NATIVE_PCI_DEV,
        PBR0_NATIVE_PCI_FUN,
        PBR0_CORE_ID,
        PBR0_PORT_ID,
        {(UINT32)PBR0_PCI_ADDRESS},
        LinkStateResetExit,
        PBR0,
        PBR0_UNIT_ID,
        PBR0_NUM_UNIT_IDs
      },
    },
  },
//------------------------------ GPP WRAPPER END -------------------------------------
//------------------------------ DDI WRAPPER START----------------------------------
  //Ddi1
  {
    {
      DESCRIPTOR_DDI_ENGINE | DESCRIPTOR_VIRTUAL,
      offsetof (KB_COMPLEX_CONFIG, Ddi1) - offsetof (KB_COMPLEX_CONFIG, DdiWrapper),
      offsetof (KB_COMPLEX_CONFIG, Ddi2) - offsetof (KB_COMPLEX_CONFIG, Ddi1),
      0
    },
    {PcieDdiEngine},
    0,                                              //Initialization Status
    0xFF                                            //Scratch
  },
  //Ddi2
  {
    {
      DESCRIPTOR_DDI_ENGINE | DESCRIPTOR_VIRTUAL,
      offsetof (KB_COMPLEX_CONFIG, Ddi2) - offsetof (KB_COMPLEX_CONFIG, DdiWrapper),
      offsetof (KB_COMPLEX_CONFIG, Vga) - offsetof (KB_COMPLEX_CONFIG, Ddi2),
      0
    },
    {PcieDdiEngine},
    0,                                              //Initialization Status
    0xFF                                            //Scratch
  },
  //Vga
  {
    {
      DESCRIPTOR_DDI_ENGINE | DESCRIPTOR_VIRTUAL | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY,
      offsetof (KB_COMPLEX_CONFIG, Vga) - offsetof (KB_COMPLEX_CONFIG, DdiWrapper),
      0,
      0
    },
    {PcieDdiEngine},
    0,                                              //Initialization Status
    0xFF                                            //Scratch
  },
  {
    {0, 0, 0, 0, 0}
  }
};

//
// PCIe lane allocation  GPP
//
CONST UINT8 ROMDATA GppPortLaneConfigurationTableKB [] = {
  UNUSED_LANE_ID, UNUSED_LANE_ID, UNUSED_LANE_ID, UNUSED_LANE_ID, UNUSED_LANE_ID, UNUSED_LANE_ID, 4,              7,              0,            3,
  UNUSED_LANE_ID, UNUSED_LANE_ID, UNUSED_LANE_ID, UNUSED_LANE_ID, 6,              7,              4,              5,              0,            3,
  UNUSED_LANE_ID, UNUSED_LANE_ID, 7,              7,              6,              6,              4,              5,              0,            3,
  7,              7,              6,              6,              5,              5,              4,              4,              0,            3
};

//
// DDI lane allocation  DDI
//
CONST UINT8 ROMDATA DdiLaneConfigurationTableKB [] = {
   0,            3,             4,             7,             8,             11
};

//
// PCIe lane allocation  desfriptors
//
CONST PCIe_LANE_ALLOC_DESCRIPTOR ROMDATA PcieLaneAllocConfigurationKB[] = {
  {
    0,
    GPP_WRAP_ID,
    PciePortEngine,
    NUMBER_OF_GPP_PORTS,
    sizeof (GppPortLaneConfigurationTableKB) / (NUMBER_OF_GPP_PORTS * 2),
    &GppPortLaneConfigurationTableKB[0]
  },
  {
    DESCRIPTOR_TERMINATE_LIST,
    DDI_WRAP_ID,
    PcieDdiEngine,
    NUMBER_OF_DDI_DDIS,
    sizeof (DdiLaneConfigurationTableKB) / (NUMBER_OF_DDI_DDIS * 2),
    &DdiLaneConfigurationTableKB[0]
  }
};


/*----------------------------------------------------------------------------------------*/
/**
 * Get length of data block for complex
 *
 *
 *
 * @param[in]  SocketId              Socket ID.
 * @param[out] Length                Length of configuration info block
 * @param[out] StdHeader             Standard configuration header
 * @retval     AGESA_SUCCESS         Configuration data length is correct
 */
AGESA_STATUS
PcieGetComplexDataLengthKB (
  IN       UINT8                         SocketId,
     OUT   UINTN                         *Length,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  )
{
  *Length = sizeof (KB_COMPLEX_CONFIG);
  return  AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Build configuration
 *
 *
 *
 * @param[in]  SocketId         Socket ID.
 * @param[out] Buffer           Pointer to buffer to build internal complex data structure
 * @param[out] StdHeader        Standard configuration header.
 * @retval     AGESA_SUCCESS    Configuration data build successfully
 */
AGESA_STATUS
PcieBuildComplexConfigurationKB (
  IN       UINT8                         SocketId,
     OUT   VOID                          *Buffer,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  )
{
  LibAmdMemCopy (Buffer, &ComplexDataKB, sizeof (KB_COMPLEX_CONFIG), StdHeader);
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 *  get native PHY lane bitmap
 *
 *
 * @param[in]  PhyLaneBitmap  Package PHY lane bitmap
 * @param[in]  Engine         Standard configuration header.
 * @retval     Native PHY lane bitmap
 */
UINT32
PcieGetNativePhyLaneBitmapKB (
  IN       UINT32                        PhyLaneBitmap,
  IN       PCIe_ENGINE_CONFIG            *Engine
  )
{

  return PhyLaneBitmap;
}
