/**
 * @file
 *
 *  PCIe silicon specific functions library.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-NB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *
 ***************************************************************************/
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "NbPlatform.h"
#include "amdDebugOutLib.h"
#include "amdSbLib.h"

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

UINT32
PcieLibGetCoreConfiguration (
  IN       CORE              CoreId,
  IN       AMD_NB_CONFIG     *pConfig
  );

AGESA_STATUS
PcieLibValidateGfxConfig (
  IN       PORT            PortId,
  IN OUT   AMD_NB_CONFIG   *pConfig
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */



CORE_INFO  CoreInfoTable[] = {
  { // GPP1_CORE_ID = 0       It is GFX Core (GPP1 Core)
    GPP1_CORE,            // Core Selector
    (BIT2 | BIT3),        // Port ID Mask. Defines what ports belongs
    NB_MISC_REG08,
    NB_MISC_REG28,        // De-emphasis register
    NB_MISC_REG26,        // Strap control register
    28,                   // Strap bit offset
    NB_CLK_REGF0,
    NB_MISC_REG35, 26,    // TX drive strength and offset
    NB_MISC_REG35, 18,    // TX half swing
    NB_MISC_REG36, 24,    // TX zero deemphasis
    0,
    28,
    23
  },
  { // GPP2_CORE_ID = 1       It is GFX2 Core (GPP2 Core)
    GPP2_CORE,            // Core Selector
    (BIT11 | BIT12),      // Port ID Mask. Defines what ports belongs
    NB_MISC_REG08,
    NB_MISC_REG27,        // De-emphasis register
    NB_MISC_REG26,        // Strap control register
    29,                   // Strap bit offset
    NB_CLK_REGF0,
    NB_MISC_REG35, 28,    // TX drive strength and offset
    NB_MISC_REG35, 19,    // TX half swing
    NB_MISC_REG36, 26,    // TX zero deemphasis
    1,
    29,
    24
  },
  { // GPP3a_CORE_ID = 2       It is GPP Core (GPP3a Core)
    GPP3a_CORE,           // Core Selector
    (BIT4 | BIT5 | BIT6 | BIT7 | BIT9 | BIT10), // Port ID Mask. Defines what ports belongs
    NB_MISC_REG08,
    NB_MISC_REG28,        // De-emphasis register
    NB_MISC_REG26,        // Strap control register
    30,                   // Strap bit offset
    NB_CLK_REGF4,
    NB_MISC_REG35, 30,    // TX drive strength and offset
    NB_MISC_REG35, 20,    // TX half swing
    NB_MISC_REG36, 28,    // TX zero deemphasis
    2,
    30,
    25
  },
  { // GPP3b_CORE_ID = 3       It is GPP2 Core (GPP3b Core)
    GPP3b_CORE,           // Core Selector
    (BIT13),              // Port ID Mask. Defines what ports belongs
    NB_MISC_REG2A,
    NB_MISC_REG2D,        // De-emphasis register
    NB_MISC_REG2D,        // Strap control register
    21,                   // Strap bit offset
    NB_CLK_REGF4,
    NB_MISC_REG2C, 4,     // TX drive strength and offset
    NB_MISC_REG2C, 2,     // TX half swing
    NB_MISC_REG2B, 10,    // TX zero deemphasis
    3,
    31,
    26
  },
  { // SB_CORE_ID  = 4       It is SB Core
    SB_CORE,              // Core Selector
    (BIT8),               // Port ID Mask. Defines what ports belongs
    NB_MISC_REG08,
    NB_MISC_REG6F,        // De-emphasis register
    0x0,
    0x0,
    0x0,
    NB_MISC_REG68, 8,     // TX drive strength and offset
    NB_MISC_REG67, 27,    // TX half swing
    NB_MISC_REG68, 20,    // TX zero deemphasis
    0xff,
    0xff,
    0xff
  }
};


PORT_INFO pGfxPortFullA = {
  PcieLinkWidth_x16, 0, 0
};

PORT_INFO pGfxPortA = {
  PcieLinkWidth_x8, 0, 96
};

PORT_INFO pGfxPortB = {
  PcieLinkWidth_x8, 8, 96
};

PORT_INFO pGpp420000[] = {
  {PcieLinkWidth_x4, 0, 56},
  {PcieLinkWidth_x2, 4, 28}
};

PORT_INFO pGpp411000[] = {
  {PcieLinkWidth_x4, 0, 56},
  {PcieLinkWidth_x1, 4, 14},
  {PcieLinkWidth_x1, 5, 14}
};

PORT_INFO pGpp222000[] = {
  {PcieLinkWidth_x2, 0, 28},
  {PcieLinkWidth_x2, 2, 28},
  {PcieLinkWidth_x2, 4, 28}
};

PORT_INFO pGpp221100[] = {
  {PcieLinkWidth_x2, 0, 28},
  {PcieLinkWidth_x2, 2, 28},
  {PcieLinkWidth_x1, 4, 14},
  {PcieLinkWidth_x1, 5, 14}
};

PORT_INFO pGpp211110[] = {
  {PcieLinkWidth_x2, 0, 28},
  {PcieLinkWidth_x1, 2, 14},
  {PcieLinkWidth_x1, 3, 14},
  {PcieLinkWidth_x1, 4, 14},
  {PcieLinkWidth_x4, 0, 0 },  //Dummy entry
  {PcieLinkWidth_x1, 5, 14}
};

PORT_INFO pGpp111111[] = {
  {PcieLinkWidth_x1, 0, 14},
  {PcieLinkWidth_x1, 1, 14},
  {PcieLinkWidth_x1, 2, 14},
  {PcieLinkWidth_x1, 3, 14},
  {PcieLinkWidth_x4, 0, 0 },  //Dummy entry
  {PcieLinkWidth_x1, 4, 14},
  {PcieLinkWidth_x1, 5, 14}
};

GPP_CFG_INFO GppCfgInfoTable[] = {
  {pGpp420000, 0xff50fff4},
  {pGpp411000, 0xf650fff4},
  {pGpp222000, 0xff60f5f4},
  {pGpp221100, 0xf760f5f4},
  {pGpp211110, 0xf97065f4},
  {pGpp111111, 0xfA907654}
};

CONST PORT_STATIC_INFO  PortInfoTable[] = {
//Training  Reversal  Deemp    Mapping   Hotplug Offset
  {4 ,      3,        0 ,      4       , 0         },  //2
  {5 ,      4,        1 ,      8       , 8         },  //3
  {21,      7,        2 ,      12      , 0         },  //4
  {22,      8,        3 ,      16      , 8         },  //5
  {23,      9,        4 ,      20      , 16        },  //6
  {24,      10,       5 ,      24      , 0xFF      },  //7
  {20,      0,        1 ,      0xFF    , 0xFF      },  //8
  {25,      11,       6 ,      28      , 0xFF      },  //9
  {26,      12,       7 ,      0       , 0xFF      },  //10
  {6 ,      5,        30,      4       , 16        },  //11
  {7 ,      6,        31,      8       , 24        },  //12
  {4 ,      25,       5 ,      12      , 24        }   //13
};



/*----------------------------------------------------------------------------------------*/
/**
 * Port Training Control
 *
 *
 *
 * @param[in] PortId    PCI Express Port ID
 * @param[in] Operation Release or Hold training
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */

VOID
PcieLibPortTrainingControl (
  IN       PORT               PortId,
  IN       PCIE_LINK_TRAINING Operation,
  IN       AMD_NB_CONFIG      *pConfig
  )
{
  CORE              CoreId;
  CORE_INFO         *pCoreInfo;
  PORT_STATIC_INFO  *pStaticPortInfo;
  CoreId = PcieLibGetCoreId (PortId, pConfig);
  pStaticPortInfo = PcieLibGetStaticPortInfo (PcieLibNativePortId (PortId, pConfig), pConfig);
  pCoreInfo = PcieLibGetCoreInfo (CoreId, pConfig);
  LibNbPciIndexRMW (
    pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX,
    pCoreInfo->TrainingRegister,
    AccessWidth32,
    ~(1 << pStaticPortInfo->TrainingAddress),
    (Operation == PcieLinkTrainingHold)?(1 << pStaticPortInfo->TrainingAddress):0,
    pConfig
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get PCI address of Port.
 * Function return pcie Address based on port mapping and core configuration.
 *
 *
 * @param[in] PortId    PCI Express Port ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */

PCI_ADDR
PcieLibGetPortPciAddress (
  IN       PORT             PortId,
  IN       AMD_NB_CONFIG    *pConfig
  )
{
  PCI_ADDR  Port;
  UINT32  RemapEnable;
  UINT32  RemapValue;
  PORT_STATIC_INFO  *pPortStaticInfo;

  RemapEnable = 0;
  RemapValue = 0;
  pPortStaticInfo = PcieLibGetStaticPortInfo (PcieLibNativePortId (PortId, pConfig), pConfig);
  Port.AddressValue = pConfig->NbPciAddress.AddressValue;

  LibNbPciIndexRead (
    pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX,
    NB_MISC_REG20,
    AccessWidth32,
    &RemapEnable,
    pConfig
    );
  if (pPortStaticInfo->MappingAddress != 0xff && RemapEnable & BIT0) {
    LibNbPciIndexRead (
      pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX,
      (PortId > 9)? NB_MISC_REG21:NB_MISC_REG20,
      AccessWidth32,
      &RemapValue,
      pConfig
      );
    RemapValue = (RemapValue >> pPortStaticInfo->MappingAddress) & 0xf;
  }
  if (RemapValue == 0) {
    RemapValue = PortId;
  }
  Port.Address.Device = RemapValue;
  return  Port;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get Core register selector.
 * Function return selector to access BIFNB register space for selected core
 *
 *
 * @param[in] CoreId    PCI Express Core ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 *
 */
UINT32
PcieLibGetCoreAddress (
  IN       CORE             CoreId,
  IN       AMD_NB_CONFIG    *pConfig
  )
{
  return  PcieLibGetCoreInfo (CoreId, pConfig)->CoreSelector;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get Core Id
 * Function return PCIE core ID base on Port ID
 *
 *
 * @param[in] PortId    PCI Express Port ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 * @retval              Core ID.
 */
CORE
PcieLibGetCoreId (
  IN       PORT           PortId,
  IN       AMD_NB_CONFIG  *pConfig
  )
{
  CORE_INFO *pCoreInfoTable = (CORE_INFO*)FIX_PTR_ADDR (&CoreInfoTable[0], NULL);
  CORE  CoreId;
  for (CoreId = 0; CoreId <= MAX_CORE_ID; CoreId++) {
    if (pCoreInfoTable[CoreId].PortIdBitMap & (1 << PortId)) {
      break;
    }
  }
    return  CoreId;
}

/*
INDIRECT_REG_ENTRY
STATIC
PcieMiscInitTable[] = {
  {
    NB_MISC_REG20,
    (UINT32)~BIT1,
    0x0
  }, //enable static device remapping by default
  {
    NB_MISC_REG22,
    0xffffffff,
    BIT27
  }, //[10]CMGOOD_OVERRIDE for all 5 pcie cores.
  {
    NB_MISC_REG6B,
    0xffffffff,
    (UINT32) (0x1f << 27)
  }, //[13][12]Turn Off Offset Cancellation
  {
    NB_MISC_REG37,
    (UINT32)~(BIT11 + BIT12 + BIT13),
    0x0
  }, //[14][13]Disables Rx Clock gating in CDR
  {
    NB_MISC_REG67,
    (UINT32)~(BIT26 + BIT10 + BIT11),
    BIT11
  }, //[13]Disables Rx Clock gating in CDR
     //[16]Sets Electrical  Idle Threshold
  {
    NB_MISC_REG2C,
    (UINT32)~(BIT10),
    0x0
  }, //[13]Disables Rx Clock gating in CDR
  {
    NB_MISC_REG2A,
    (UINT32)~(BIT17 + BIT16),
    BIT17
  }, //[16]Sets Electrical l Idle Threshold
  {
    NB_MISC_REG32,
    (UINT32)~(0x3F << 20),
    (UINT32) (0x2A << 20)
  }  //[17][16]Sets Electrical Idle Threshold
};
*/



UINT8 GppConfigTable[] = {
  0x0, 0x1, 0x2, 0xC, 0xA, 0x4, 0xB
};
/*----------------------------------------------------------------------------------------*/
/**
 * Set Core Configuration.
 *
 *
 *
 * @param[in] CoreId    PCI Express Core ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
VOID
PcieLibSetCoreConfiguration (
  IN       CORE           CoreId,
  IN       AMD_NB_CONFIG  *pConfig
  )
{
  UINT32      LaneReversalValue;
  PORT        PortId;
  PCIE_CONFIG *pPcieConfig;
  CORE_INFO   *pCoreInfo;
  CORE        CoreAddress;

  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibSetCoreConfiguration CoreId = %d Configuration = 0x%x Enter\n", CoreId, pPcieConfig->CoreConfiguration[CoreId]));
  pCoreInfo = PcieLibGetCoreInfo (CoreId, pConfig);
  CoreAddress = PcieLibGetCoreAddress (CoreId, pConfig);
  LaneReversalValue = 0;
  PcieLibCoreReset (CoreId, PcieCoreResetAllAssert, pConfig);
  PcieLibStrapModeControl (CoreId, PcieCoreStrapConfigStart, pConfig);
  //Setup GFX/GFX2 core configuration
  if (CoreAddress == GPP1_CORE || CoreAddress == GPP2_CORE) {
    if (pPcieConfig->CoreConfiguration[CoreId] == GFX_CONFIG_AABB) {
      LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG08, AccessWidth32, 0xffffffff, (CoreAddress == GPP1_CORE)?BIT8:BIT9, pConfig);
      STALL (GET_BLOCK_CONFIG_PTR (pConfig), 2000, 0);
    }
    if (pPcieConfig->CoreSetting[CoreId].RefClockInput == ON) {
      LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG6C, AccessWidth32, 0xffffffff, (CoreAddress == GPP1_CORE)?BIT9:BIT31, pConfig);
    }
  }
  //Setup GPP core configuration
  if (CoreAddress == GPP3a_CORE) {
    UINT32  Mux;
    UINT8   *pGppConfigTable;

    Mux = 0;
    pGppConfigTable = (UINT8*)FIX_PTR_ADDR (&GppConfigTable[0], NULL);
    LibNbPciIndexRMW (
      pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX,
      NB_MISC_REG67, AccessWidth32,
      0xfffffff0,
      (UINT32)pGppConfigTable[pPcieConfig->CoreConfiguration[CoreId]],
      pConfig
      );
    switch (pPcieConfig->CoreConfiguration[CoreId]) {
    case  GPP_CONFIG_GPP420000:
      Mux = (pPcieConfig->PortConfiguration[6].PortReversed == ON)?0xF05BA00:0x055B000;
      break;
    case  GPP_CONFIG_GPP411000:
      Mux = 0x215B400;
      break;
    case  GPP_CONFIG_GPP222000:
    case  GPP_CONFIG_GPP211110:
      Mux = (pPcieConfig->PortConfiguration[4].PortReversed == ON)?0xFFF0AAA:0xFF0BAA0;
      break;
    case  GPP_CONFIG_GPP221100:
      Mux = 0x215B400;
      break;
    case  GPP_CONFIG_GPP111111:
      Mux = 0x2AA3554;
      break;
    default:
      CIMX_ASSERT (FALSE);
    }
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG26, AccessWidth32,  0xf0000000, Mux, pConfig);
  }
  PcieLibStrapModeControl (CoreId, PcieCoreStrapConfigStop, pConfig);
  PcieLibCoreReset (CoreId, PcieCoreResetAllDeassert, pConfig);
  //Setup lane reversal
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if (PcieLibIsValidPortId (PortId, pConfig)) {
      if (pPcieConfig->PortConfiguration[PortId].PortPresent == ON &&
          pPcieConfig->PortConfiguration[PortId].PortReversed == ON &&
          (pCoreInfo->PortIdBitMap & (1 << PortId)) != 0) {
        PORT_STATIC_INFO *pStaticPortInfo = PcieLibGetStaticPortInfo (PcieLibNativePortId (PortId, pConfig), pConfig);
        LaneReversalValue |= (1 << (pStaticPortInfo->ReversalAddress));
        CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    Port reversed Port Id %d Native Id %d, Reversal Address %d \n", PortId, PcieLibNativePortId (PortId, pConfig), pStaticPortInfo->ReversalAddress));
      }
    }
  }
  LibNbPciIndexRMW (
    pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX,
    (CoreAddress == GPP3b_CORE) ? NB_MISC_REG2D : NB_MISC_REG27,
    AccessWidth32, 0xffffffff,
    LaneReversalValue,
    pConfig
    );
  //Setup performance mode
  if (pPcieConfig->CoreSetting[CoreId].PerformanceMode == ON) {
    UINT32  RegisterAddress;
    switch (CoreAddress) {
    case  GPP1_CORE:
      RegisterAddress = NB_MISC_REG33;
      break;
    case  GPP2_CORE:
      RegisterAddress = NB_MISC_REG22;
      break;
    default:
      RegisterAddress = 0;
      break;
    }
    if (RegisterAddress != 0) {
      LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, RegisterAddress , AccessWidth32, 0xfffffC00, 0xB5, pConfig);
    }
  }
  //Setup Tx Drive Strength
  LibNbPciIndexRMW (
    pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX,
    pCoreInfo->TxDriveStrengthRegister ,
    AccessWidth32,
    ~(0x3 << pCoreInfo->TxDriveStrengthOffset),
    pPcieConfig->CoreSetting[CoreId].TxDriveStrength << pCoreInfo->TxDriveStrengthOffset,
    pConfig
    );
  //Setup Tx half swing
  if (pPcieConfig->CoreSetting[CoreId].TxHalfSwingMode == ON) {
    LibNbPciIndexRMW (
      pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX,
      pCoreInfo->TxHalfSwingRegister,
      AccessWidth32,
      ~(0x1 << pCoreInfo->TxHalfSwingOffset),
      0x0,
      pConfig
    );
    // Setup half swing deemphasis
    LibNbPciIndexRMW (
      pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX,
      pCoreInfo->TxHalfSwingDeepmhasisRegister ,
      AccessWidth32,
      ~(0x3 << pCoreInfo->TxHalfSwingDeepmhasisOffset),
      0x0,
      pConfig
    );
  }
  //Finalize straps for this core
  PcieLibStrapModeControl (CoreId, PcieCoreStrapConfigStart, pConfig);
  PcieLibStrapModeControl (CoreId, PcieCoreStrapConfigStop, pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibSetCoreConfiguration Exit\n"));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get Core Configuration
 * Function return GPPSB/GFX/GFX2 core configuration.
 *
 *
 *
 * @param[in] CoreId    PCI Express Core ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
UINT32
PcieLibGetCoreConfiguration (
  IN       CORE              CoreId,
  IN       AMD_NB_CONFIG     *pConfig
  )
{
  UINT32  CoreConfiguration;
  UINT32  Value;
  CORE    CoreAddress;

  CoreConfiguration = 0,
  CoreAddress = PcieLibGetCoreAddress (CoreId, pConfig);
//  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibGetCoreConfiguration (Core = 0x%x) Enter\n", CoreAddress));
  if (CoreAddress == GPP1_CORE || CoreAddress == GPP2_CORE) {
    LibNbPciIndexRead (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG08, AccessWidth32, &Value, pConfig);
    CoreConfiguration = (Value & ((CoreAddress == GPP1_CORE) ? BIT8:BIT9))? GFX_CONFIG_AABB:GFX_CONFIG_AAAA;
  } else {
    if (CoreAddress == GPP3a_CORE) {
      UINT8 *pGppConfigTable;
      pGppConfigTable = (UINT8*)FIX_PTR_ADDR (&GppConfigTable[0], NULL);
      LibNbPciIndexRead (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG67, AccessWidth32, &Value, pConfig);
      while (pGppConfigTable[CoreConfiguration] != (Value & 0xf)) {
        CoreConfiguration++;
      }
    }
  }
//  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibGetCoreConfiguration (CoreConfiguration = 0x%x) Exit\n", CoreConfiguration));
  return    CoreConfiguration;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Return link misc information (max link width, current link width, lane 0 map)
 *
 *
 *
 *
 * @param[in] PortId    PCI Express Port ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
LINK_INFO
PcieLibGetPortLinkInfo (
  IN       PORT                PortId,
  IN       AMD_NB_CONFIG       *pConfig
  )
{
  UINT32    Value;
  PCI_ADDR  Port;
  PORT_INFO *pPortInfo;
  LINK_INFO LinkInfo = {0, 0, 0};

  Port = PcieLibGetPortPciAddress (PortId, pConfig);
  pPortInfo = PcieLibGetPortInfo (PortId, pConfig);
//Read current link width
  LibNbPciIndexRead (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REGA2, AccessWidth32, &Value, pConfig);
  Value = (Value >> 4) & 0xf;
  LinkInfo.LinkWidth = (UINT8)Value;
  LinkInfo.MaxLinkWidth = pPortInfo->MaxLinkWidth;
  LinkInfo.Line0Offset = pPortInfo->Line0Offset;
//  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    PortId %d LinkWidth 0x%x MaxLinkWidth 0x%x Line0Offset %d\n", PortId, LinkInfo.LinkWidth, LinkInfo.MaxLinkWidth,LinkInfo.Line0Offset));
  return  LinkInfo;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Check if port in lane reversed configuration.
 *
 *
 *
 * @param[in] PortId    PCI Express Port ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
BOOLEAN
PcieLibIsPortReversed (
  IN       PORT              PortId,
  IN       AMD_NB_CONFIG     *pConfig
  )
{
  BOOLEAN Result;
  UINT32  Value;
  PCIE_CONFIG *pPcieConfig;
  PCI_ADDR Port;

  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  Port = PcieLibGetPortPciAddress (PortId, pConfig);
  LibNbPciIndexRead (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REG50, AccessWidth32, &Value, pConfig);
  if (pPcieConfig->PortConfiguration[PortId].PortReversed == ON || (Value & BIT0) != 0) {
    Result = TRUE;
  } else {
    Result = FALSE;
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check if core id valid for current silicon
 *
 *
 *
 * @param[in] CoreId    PCI Express Core ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */

BOOLEAN
PcieLibIsValidCoreId (
  IN       CORE               CoreId,
  IN       AMD_NB_CONFIG      *pConfig
  )
{
  UINT32  CoreAddress;
  NB_INFO NbInfo;

  CoreAddress = PcieLibGetCoreAddress (CoreId, pConfig);
  NbInfo = LibNbGetRevisionInfo (pConfig);
  if (CoreAddress == GPP3b_CORE) {
    if (NbInfo.Type == NB_RD890 || NbInfo.Type == NB_SR5690) {
      return TRUE;
    } else {
      return FALSE;
    }
  }
  if (CoreAddress == GPP2_CORE && (NbInfo.Type == NB_RD780 || NbInfo.Type == NB_RX780 || NbInfo.Type == NB_SR5650 || NbInfo.Type == NB_990X || NbInfo.Type == NB_970)) {
    return FALSE;
  }
  return TRUE;
}



/*----------------------------------------------------------------------------------------*/
/**
 * Check if port Id valid for current core configuration
 *
 *
 *
 * @param[in] PortId    PCI Express Port ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */

BOOLEAN
PcieLibIsValidPortId (
  IN       PORT               PortId,
  IN       AMD_NB_CONFIG      *pConfig
  )
{
  CORE  CoreId;
  NB_INFO NbInfo;

  CoreId = PcieLibGetCoreId (PortId, pConfig);
  NbInfo = LibNbGetRevisionInfo (pConfig);
  if (!PcieLibIsValidCoreId (CoreId, pConfig)) {
    return FALSE;
  }
  if ((PortId == 3 || PortId == 12) && PcieLibGetCoreConfiguration (CoreId, pConfig) != GFX_CONFIG_AABB) {
    return FALSE;
  }
  if (PortId == 3  && NbInfo.Type == NB_970)  {
    return FALSE;
  }
  if (PortId == 12 && NbInfo.Type == NB_SR5670) {
    return FALSE;
  }
  if (PortId == 13 || PortId == 8) {
    return TRUE;
  } else {
    return (PcieLibNativePortId (PortId, pConfig) == 0xf)?FALSE:TRUE;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set Link mode. Gen1/Gen2/Gen2-Advertize
 *
 *
 *
 *
 * @param[in] PortId    PCI Express Port ID
 * @param[in] Operation Link Mode
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */


VOID
PcieLibSetLinkMode (
  IN       PORT               PortId,
  IN       PCIE_LINK_MODE     Operation,
  IN       AMD_NB_CONFIG      *pConfig
  )
{
  PCI_ADDR          Port;
  UINT8             LinkSpeed;
  UINT32            LinkDeemphasisMask;
  UINT32            LinkDeemphasisValue;
  UINT32            RegA4Value;
  UINT32            RegA2Value;
  UINT32            RegC0Value;
  CORE_INFO         *pCoreInfo;
  PORT_STATIC_INFO  *pStaticPortInfo;
  PCIE_CONFIG       *pPcieConfig;

  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibSetLinkMode PortId %d Operation %d Enter\n", PortId, Operation));
  LinkSpeed = 2;
  RegA4Value = BIT29 + BIT0;
  RegA2Value = 0;
  RegC0Value = 0;
  Port = PcieLibGetPortPciAddress (PortId, pConfig);
  pStaticPortInfo = PcieLibGetStaticPortInfo (PcieLibNativePortId (PortId, pConfig), pConfig);
  pCoreInfo = PcieLibGetCoreInfo (PcieLibGetCoreId (PortId, pConfig), pConfig);

  LinkDeemphasisValue = pPcieConfig->ExtPortConfiguration[PortId].PortDeemphasis << pStaticPortInfo->DeemphasisAddress;
  LinkDeemphasisMask = ~(1 << pStaticPortInfo->DeemphasisAddress);

  if (Operation == PcieLinkModeGen1 || Operation == PcieLinkModeGen2AdvertizeOnly) {
    RegC0Value = BIT15;
    RegA2Value = BIT13;
    if (Operation == PcieLinkModeGen1) {
      RegA4Value = 0;
      LinkSpeed = 1;
      LinkDeemphasisValue = 0;
    }
  }

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "     pCoreInfo->DeemphasisRegister %x pStaticPortInfo->DeemphasisAddress %x LinkDeemphasisMask %x, LinkDeemphasisValue  %x\n", pCoreInfo->DeemphasisRegister, pStaticPortInfo->DeemphasisAddress, LinkDeemphasisMask, LinkDeemphasisValue));
  LibNbPciIndexRMW (Port.AddressValue  | NB_BIF_INDEX, NB_BIFNBP_REGA4 , AccessWidth32, (UINT32)~(BIT0 + BIT29), RegA4Value , pConfig);
  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, pCoreInfo->DeemphasisRegister, AccessWidth32, LinkDeemphasisMask, LinkDeemphasisValue , pConfig);
  LibNbPciRMW (Port.AddressValue | NB_PCIP_REG88, AccessWidth8, 0xF0, LinkSpeed, pConfig);
  LibNbPciIndexRMW (Port.AddressValue  | NB_BIF_INDEX, NB_BIFNBP_REGC0 , AccessWidth32, (UINT32)~(BIT15), RegC0Value , pConfig);
  LibNbPciIndexRMW (Port.AddressValue  | NB_BIF_INDEX, NB_BIFNBP_REGA2 , AccessWidth32, (UINT32)~(BIT13), RegA2Value , pConfig);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Request PCIE reset to be executed
 *
 *
 *
 * @param[in] pConfig   Northbridge configuration structure pointer.
 *
 */
AGESA_STATUS
PcieLibRequestPciReset (
  IN       AMD_NB_CONFIG     *pConfig
  )
{
  AGESA_STATUS  Status;
  SCRATCH_1     Scratch;

  Status = AGESA_UNSUPPORTED;
  LibNbPciIndexRead (pConfig->NbPciAddress.AddressValue | NB_HTIU_INDEX, NB_HTIU_REG15, AccessS3SaveWidth32, (UINT32*)&Scratch, pConfig);
  if (Scratch.ResetCount == 0xf) {
    Scratch.ResetCount = 0;
  }
  if (Scratch.ResetCount < 5) {
    ++Scratch.ResetCount;
    LibNbPciIndexWrite (pConfig->NbPciAddress.AddressValue | NB_HTIU_INDEX, NB_HTIU_REG15, AccessS3SaveWidth32, (UINT32*)&Scratch, pConfig);
    if (LibNbCallBack (PHCB_AmdGeneratePciReset, WARM_RESET , pConfig) != AGESA_SUCCESS) {
      LibNbIoRMW (0xCF9, AccessWidth8, 0, 0x6, pConfig);
    }
  }
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Control Core Reset
 *
 *
 *
 * @param[in] CoreId    PCI Express Core ID
 * @param[in] Operation Assert/Deassert/Check core reset
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */

PCI_CORE_RESET
PcieLibCoreReset (
  IN      CORE            CoreId,
  IN      PCI_CORE_RESET  Operation,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  UINT32 Value;
  UINT32 CalibrationReset;
  UINT32 GlobalReset;
  UINT32 RegisterAddress;
  UINT32 CoreAddress;

  RegisterAddress = NB_MISC_REG08;
  CoreAddress = PcieLibGetCoreAddress (CoreId, pConfig);
  switch (CoreAddress) {
  case  GPP3b_CORE:
    RegisterAddress = NB_MISC_REG2A;            // break missing and it is not an error.
  case  GPP1_CORE:
    CalibrationReset = BIT14;
    GlobalReset = BIT15;
    break;
  case  GPP2_CORE:
    CalibrationReset = BIT12;
    GlobalReset = BIT13;
    break;
  case  GPP3a_CORE:
    CalibrationReset = BIT30;
    GlobalReset = BIT31;
    break;
  default:
    return PcieCoreResetAllDeassert;
  }
  switch (Operation) {
  case PcieCoreResetAllDeassert:
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, RegisterAddress, AccessS3SaveWidth32, ~CalibrationReset, 0x0, pConfig);
    STALL (GET_BLOCK_CONFIG_PTR (pConfig), 200, 0);
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, RegisterAddress, AccessS3SaveWidth32, ~GlobalReset, 0x0, pConfig);
    STALL (GET_BLOCK_CONFIG_PTR (pConfig), 2000, 0);
    break;
  case PcieCoreResetAllAssert:
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, RegisterAddress, AccessS3SaveWidth32, 0xffffffff, CalibrationReset | GlobalReset, pConfig);
    break;
  case PcieCoreResetAllCheck:
    LibNbPciIndexRead (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, RegisterAddress, AccessS3SaveWidth32, &Value, pConfig);
    Operation = (Value & (CalibrationReset | GlobalReset))?PcieCoreResetAllAssert:PcieCoreResetAllDeassert;
    break;
  default:
    CIMX_ASSERT (FALSE);
  }
  return Operation;
}

UINT8  GfxLineMapTable[] = {
  0x00, 0x01, 0x01, 0x03, 0x0f, 0x00, 0xFF
};
UINT8  GppLineMapTable[] = {
  0x00, 0x01, 0x03, 0x0F
};
/*----------------------------------------------------------------------------------------*/
/**
 * Power off port lanes.
 *
 *
 *
 * @param[in] PortId    PCI Express Port ID
 * @param[in] Width     Port Link Width.
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */

VOID
PcieLibPowerOffPortLanes (
  IN       PORT              PortId,
  IN       PCIE_LINK_WIDTH   Width,
  IN       AMD_NB_CONFIG     *pConfig
  )
{
  CORE      CoreId;
  LINK_INFO LinkInfo;
  UINT32    PowerOffPads;
  UINT32    CoreAddress;
  UINT8*    pLineMapTable;
  UINT16    MaxLaneBitMap;
  UINT16    LaneBitMap;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibPowerOffPortLanes (PortId = %d, Width = %d) Enter\n", PortId, Width));
  CoreId = PcieLibGetCoreId (PortId, pConfig);
  LinkInfo = PcieLibGetPortLinkInfo (PortId, pConfig);
  CoreAddress = PcieLibGetCoreAddress (CoreId, pConfig);
  if (CoreAddress == GPP1_CORE || CoreAddress == GPP2_CORE) {
    pLineMapTable = &GfxLineMapTable[0];
    LinkInfo.Line0Offset /= 2;
  } else {
    pLineMapTable = &GppLineMapTable[0];
  }
  pLineMapTable = (UINT8*)FIX_PTR_ADDR (pLineMapTable, NULL);
  LaneBitMap = pLineMapTable[Width];
  MaxLaneBitMap = pLineMapTable[LinkInfo.MaxLinkWidth];
  if (PcieLibIsPortReversed (PortId, pConfig)) {
    LaneBitMap = (UINT16)LibNbBitReverse ((UINT32)LaneBitMap, LibAmdBitScanForward (MaxLaneBitMap), LibAmdBitScanReverse (MaxLaneBitMap));
  }
  PowerOffPads = (MaxLaneBitMap ^ LaneBitMap) << LinkInfo.Line0Offset;
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "   Pads %x  Exit\n", PowerOffPads));
  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_BIF_INDEX, NB_BIFNB_REG65 | CoreAddress, AccessWidth32, 0xffffffff, PowerOffPads | (PowerOffPads << 8), pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibPowerOffPortLanes  Exit\n"));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Hide Unused Ports
 *
 *
 *
 * @param[in] pConfig   Northbridge configuration structure pointer.
 *
 */
VOID
PcieLibHidePorts (
  IN       AMD_NB_CONFIG     *pConfig
  )
{
  UINT32      PresentPortMask;
  UINT32      DetectedPortMask;
  UINT32      HotplugPortMask;
  UINT32      Value;
  PORT        PortId;
  PCIE_CONFIG *pPcieConfig;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibHidePorts Enter\n"));
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  PresentPortMask = 0;
  DetectedPortMask = 0;
  HotplugPortMask = 0;
  // Hide SB Port
  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG00, AccessS3SaveWidth32, (UINT32)~BIT6, 0, pConfig);
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if (pPcieConfig->PortConfiguration[PortId].PortPresent == ON && PcieLibIsValidPortId (PortId, pConfig)) {
      PCI_ADDR Port;
      Port = PcieLibGetPortPciAddress (PortId, pConfig);
      if (pPcieConfig->PortConfiguration[PortId].PortDetected == ON ) {
        DetectedPortMask |= 1 << Port.Address.Device;
      }
      if (pPcieConfig->PortConfiguration[PortId].PortHotplug != OFF) {
        HotplugPortMask |= 1 << Port.Address.Device;
      }
      PresentPortMask |= 1 << Port.Address.Device;
    }
  }

  if (pPcieConfig->PcieConfiguration.DisableHideUnusedPorts == ON) {
    Value = PresentPortMask;
  } else {
    Value = DetectedPortMask | HotplugPortMask;
  }
  //CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    Present Port 0x%x Visible Ports 0x%xExit\n",PresentPortMask,VisiblePortMask));
  Value = (~((Value & (0xFC)) + ((Value & 0x3E00) << 7))) & 0x1F00FC;
  // Hide GFX/GFX2/GPP/GPP2 Ports
  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG0C, AccessS3SaveWidth32, 0xffffffff, Value, pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibHidePorts Exit\n"));
}

/*----------------------------------------------------------------------------------------*/
/**
 * UnHide all PCIE Ports
 *
 *
 *
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */

VOID
PcieLibUnHidePorts (
  IN      AMD_NB_CONFIG    *pConfig
  )
{
  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG0C, AccessS3SaveWidth32, (UINT32)~0x1F00FCul, 0, pConfig);
  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG00, AccessS3SaveWidth32, (UINT32)~BIT6, BIT6, pConfig);
}



PCIE_DEFAULT_CONFIG PcieDefaultConfig = {
  {0, 1, 0, 0},
  {
    {0, 1, 1, 1, 1, 1, 1, 0, PcieTxDriveStrangth22mA, 0, 0, PcieMediumChannel, 1, 1, 1},   //GPP1
    {0, 1, 1, 1, 1, 1, 1, 0, PcieTxDriveStrangth22mA, 0, 0, PcieMediumChannel, 1, 1, 1},   //GPP2
    {0, 1, 1, 1, 1, 1, 1, 0, PcieTxDriveStrangth22mA, 0, 0, PcieMediumChannel, 1, 1, 1},   //GPP3a
    {0, 1, 1, 1, 1, 1, 0, 0, PcieTxDriveStrangth22mA, 0, 0, PcieMediumChannel, 1, 1, 1},   //GPP3b
    {0, 1, 0, 1, 1, 1, 0, 0, PcieTxDriveStrangth22mA, 0, 0, PcieMediumChannel, 0, 0, 0}    //SB
  },
  (BIT2 + BIT4 + BIT5 + BIT6 + BIT7 + BIT8 + BIT9 + BIT10 + BIT11 + BIT13) | (BIT3 + BIT12),
  0,
  2,
  0,
  60,
};

/*----------------------------------------------------------------------------------------*/
/**
 * AMD structures initializer for all NB.
 *
 *
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 *
 */

AGESA_STATUS
AmdPcieInitializer (
  IN OUT   AMD_NB_CONFIG_BLOCK *ConfigPtr
  )
{
  AGESA_STATUS  Status;
  Status = LibNbApiCall (PcieLibInitializer, ConfigPtr);
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize default PCIE_CONFIG setting
 *
 *
 *
 * @param[in] pConfig   Northbridge configuration structure pointer.
 *
 */
AGESA_STATUS
PcieLibInitializer (
  IN OUT   AMD_NB_CONFIG   *pConfig
  )
{
  CORE                CoreId;
  PCIE_CONFIG         *pPcieConfig;
  PCIE_DEFAULT_CONFIG *pPcieDefaultConfig;
  AMD_NB_CONFIG_BLOCK *ConfigPtr;
  PORT                PortId;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibInitializer Enter\n"));
  pPcieConfig  = GET_PCIE_CONFIG_PTR (pConfig);
  if (pPcieConfig == NULL) {
    return  AGESA_WARNING;
  }
  if (pPcieConfig->sHeader.InitializerID == INITIALIZED_BY_INITIALIZER) {
    return  AGESA_SUCCESS;
  }
  ConfigPtr = GET_BLOCK_CONFIG_PTR (pConfig);
  LibAmdMemFill (pPcieConfig, 0, sizeof (PCIE_CONFIG), (AMD_CONFIG_PARAMS *)&(pPcieConfig->sHeader));
  pPcieConfig->sHeader.InitializerID = INITIALIZED_BY_INITIALIZER;
  pPcieDefaultConfig = (PCIE_DEFAULT_CONFIG*)FIX_PTR_ADDR (&PcieDefaultConfig, NULL);
  for (CoreId = 0; CoreId <= MAX_CORE_ID; CoreId++) {
    pPcieConfig->CoreSetting[CoreId] = pPcieDefaultConfig->CoreSetting[CoreId];
  }
  pPcieConfig->PcieConfiguration = pPcieDefaultConfig->PcieConfiguration;
  if (ConfigPtr->PlatformType == DesktopPlatform) {
    pPcieConfig->PcieConfiguration.NbSbVc1 = ON;
  }
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    pPcieConfig->ExtPortConfiguration[PortId].PortDeemphasis = PcieTxDeemphasis3p5dB;
  }
  pPcieConfig->CoreConfiguration[2] = PcieLibGetCoreConfiguration (2, pConfig);
  pPcieConfig->ReceiverDetectionPooling = pPcieDefaultConfig->ReceiverDetectionPooling;
  pPcieConfig->ResetToTrainingDelay = pPcieDefaultConfig->ResetToTrainingDelay;
  pPcieConfig->ExtPortConfiguration[8].PortL1ImmediateACK = ON;
  pPcieConfig->TrainingToLinkTestDelay = pPcieDefaultConfig->TrainingToLinkTestDelay;
  pPcieConfig->DeviceInitMaskS1 = pPcieDefaultConfig->DeviceInitMaskS1;
  pPcieConfig->DeviceInitMaskS2 = pPcieDefaultConfig->DeviceInitMaskS2;
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibInitializer Exit\n"));
  return  AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/*
 * Validate Gfx Core Configuration
 *
 *
 *
 *
 *
 */
AGESA_STATUS
PcieLibValidateGfxConfig (
  IN       PORT            PortId,
  IN OUT   AMD_NB_CONFIG   *pConfig
  )
{
  CORE        CoreId;
  PCIE_CONFIG *pPcieConfig;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibValidateGfxConfig Enter\n"));
  CoreId = PcieLibGetCoreId (PortId, pConfig);
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    CoreConfiguration[%d] = \n", CoreId));
  if (pPcieConfig->CoreConfiguration[CoreId] == 0x0) {
    pPcieConfig->CoreConfiguration[CoreId] = (pPcieConfig->PortConfiguration[PortId].PortPresent == ON)?GFX_CONFIG_AABB:GFX_CONFIG_AAAA;
  } else {
    if (pPcieConfig->CoreConfiguration[CoreId] != GFX_CONFIG_AABB &&
      pPcieConfig->CoreConfiguration[CoreId] != GFX_CONFIG_AAAA) {
      //We have received request for unknown configuration.
      //pPcieConfig->CoreSetting[CoreId].CoreDisabled = ON;
      pPcieConfig->PortConfiguration[PortId].PortPresent = 0;
      pPcieConfig->PortConfiguration[PortId - 1].PortPresent = 0;
      return AGESA_WARNING;
    }
  }
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    CoreConfiguration[%d] = %x\n", CoreId, pPcieConfig->CoreConfiguration[CoreId]));
  return  AGESA_SUCCESS;
}
/*----------------------------------------------------------------------------------------*/
/**
 * Validate input parameters for early PCIE init.
 *
 *
 *
 * @param[in] pConfig   Northbridge configuration structure pointer.
 *
 */
AGESA_STATUS
PcieLibInitValidateInput (
  IN OUT   AMD_NB_CONFIG   *pConfig
  )
{
  AGESA_STATUS  Status;
  PCIE_CONFIG   *pPcieConfig;
  NB_INFO       NbInfo;
  CORE          CoreId;
  PORT          PortId;

  NbInfo = LibNbGetRevisionInfo (pConfig);
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  if (NbInfo.Type == NB_UNKNOWN || pPcieConfig == NULL) {
    return  AGESA_FATAL;
  }
  Status = AGESA_SUCCESS;
  //Validate GFX configuration
  if (PcieLibValidateGfxConfig (3, pConfig) != AGESA_SUCCESS) {
    REPORT_EVENT (AGESA_WARNING, PCIE_ERROR_CORE_CONFIGURATION, GPP1_CORE , 0, 0, 0, pConfig);
    Status = AGESA_WARNING;
  }
  if (PcieLibValidateGfxConfig (12, pConfig) != AGESA_SUCCESS) {
    REPORT_EVENT (AGESA_WARNING, PCIE_ERROR_CORE_CONFIGURATION, GPP2_CORE , 0, 0, 0, pConfig);
    Status = AGESA_WARNING;
  }
  //Enable SB port on NB - SB chain and disable otherwise
  pPcieConfig->PortConfiguration[8].PortPresent = (pConfig->NbPciAddress.AddressValue == 0)?ON:OFF;

  for (CoreId = 0; CoreId <= MAX_CORE_ID; CoreId++) {
    if (pPcieConfig->PcieConfiguration.DisableHideUnusedPorts == ON) {
      //pPcieConfig->CoreSetting[CoreId].PowerOffUnusedLanes = OFF;
      pPcieConfig->CoreSetting[CoreId].TxClockOff = OFF;
      pPcieConfig->CoreSetting[CoreId].LclkClockOff = OFF;
      pPcieConfig->CoreSetting[CoreId].PowerOffPll = OFF;
    }
    if (pPcieConfig->CoreSetting[CoreId].ChannelType != 0) {
      //Set Trasmitter drive strength based on cahnnel type
      if (pPcieConfig->CoreSetting[CoreId].ChannelType == PcieLongChannel) {
        pPcieConfig->CoreSetting[CoreId].TxDriveStrength = (NbInfo.Revision == NB_REV_A11)? PcieTxDriveStrangth24mA : PcieTxDriveStrangth26mA;
      } else {
        pPcieConfig->CoreSetting[CoreId].TxDriveStrength = PcieTxDriveStrangth22mA;
      }
      // Enable half swing mode
      if (pPcieConfig->CoreSetting[CoreId].ChannelType == PcieShortChannel) {
        pPcieConfig->CoreSetting[CoreId].TxHalfSwingMode = ON;
      } else {
        pPcieConfig->CoreSetting[CoreId].TxHalfSwingMode = OFF;
      }
    }
  }
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    CoreId = PcieLibGetCoreId (PortId, pConfig);
    if (pPcieConfig->ExtPortConfiguration[PortId].PortPowerLimit == 0) {
      pPcieConfig->ExtPortConfiguration[PortId].PortPowerLimit = 75;  //Set 75W by default
    }
    if (pPcieConfig->CoreSetting[CoreId].ChannelType != 0) {
      if (pPcieConfig->CoreSetting[CoreId].ChannelType == PcieLongChannel) {
        pPcieConfig->ExtPortConfiguration[PortId].PortDeemphasis = PcieTxDeemphasis6dB;
      } else {
        pPcieConfig->ExtPortConfiguration[PortId].PortDeemphasis = PcieTxDeemphasis3p5dB;
      }
    }
  }
  return  Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Enable PCIE Extended configuration MMIO.
 *
 *
 *
 * @param[in] PcieMmioBase  MMIO Base Address in 1MB unit.
 * @param[in] PcieMmioSize  MMIO Size in 1MB unit
 * @param[in] pConfig       Northbridge configuration structure pointer.
 *
 */
VOID
PcieLibSetPcieMmioBase (
  IN      UINT16          PcieMmioBase,
  IN      UINT16          PcieMmioSize,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  UINT8 BAR3BusRange;

  BAR3BusRange = LibAmdBitScanReverse ((UINT32)PcieMmioSize);
  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG00, AccessWidth32, (UINT32)~BIT3, 0x0, pConfig);
  LibNbPciRMW (pConfig->NbPciAddress.AddressValue | NB_PCI_REG7F, AccessWidth8, (UINT32)~BIT6, BIT6, pConfig);
  LibNbPciRMW (pConfig->NbPciAddress.AddressValue | (NB_PCI_REG84 + 2), AccessWidth8, (UINT32)~(0x7), (BAR3BusRange > 8)?0:BAR3BusRange, pConfig);
  LibNbPciRMW (pConfig->NbPciAddress.AddressValue | NB_PCI_REG1C, AccessWidth32, 0, (UINT32) (PcieMmioBase << 20), pConfig);
  LibNbPciRMW (pConfig->NbPciAddress.AddressValue | NB_PCI_REG7F, AccessWidth8, (UINT32)~BIT6, 0, pConfig);
  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_HTIU_INDEX, NB_HTIU_REG32, AccessWidth32, 0xffffffff, BIT28, pConfig);
  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG00, AccessWidth32, 0xffffffff, BIT3, pConfig);
  LibNbPciRMW (pConfig->NbPciAddress.AddressValue | NB_PCI_REG04, AccessWidth8, (UINT32)~BIT1, BIT1, pConfig);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Assert/Deassert Strap valid enables programming for misc strap features.
 *
 *
 *
 * @param[in] CoreId    PCI Express Core ID
 * @param[in] Operation Assert or deassert strap valid.
 * @param[in] pConfig   Northbridge configuration structure pointer.
 *
 */
VOID
PcieLibStrapModeControl (
  IN       CORE            CoreId,
  IN       PCIE_STRAP_MODE Operation,
  IN OUT   AMD_NB_CONFIG   *pConfig
  )
{
  CORE_INFO *pCoreInfo;

  pCoreInfo = PcieLibGetCoreInfo (CoreId, pConfig);
  LibNbPciIndexRMW (
    pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX,
    pCoreInfo->StrapRegister,
    AccessS3SaveWidth32,
    ~(1 << pCoreInfo->StrapAddress),
    (Operation == PcieCoreStrapConfigStart)?(1 << pCoreInfo->StrapAddress):0,
    pConfig
    );
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get Pcie Port Info.
 *
 *
 *
 * @param[in] PortId    PCI Express Port ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 *
 */
PORT_INFO*
PcieLibGetPortInfo (
  IN       PORT            PortId,
  IN OUT   AMD_NB_CONFIG   *pConfig
  )
{
  CORE          CoreId;
  UINT32        CoreConfig;
  PORT_INFO     *pPortInfo;
  PORT          NativePortId;
  GPP_CFG_INFO  *pGppCfgInfoTable;

  CoreId = PcieLibGetCoreId (PortId, pConfig);
  CoreConfig  = PcieLibGetCoreConfiguration (CoreId, pConfig);
  switch (PcieLibGetCoreAddress (CoreId, pConfig)) {
  case  GPP1_CORE:
  case  GPP2_CORE:
    pPortInfo = &pGfxPortFullA;
    if (CoreConfig == GFX_CONFIG_AABB) {
      if (PortId == 3 || PortId == 12) {
        pPortInfo = &pGfxPortB;
      } else {
        pPortInfo = &pGfxPortA;
      }
    }
    break;
  case  SB_CORE:
  case  GPP3b_CORE:
    pPortInfo = &pGpp420000[0];
    break;
  case  GPP3a_CORE:
    pGppCfgInfoTable = (GPP_CFG_INFO*)FIX_PTR_ADDR (&GppCfgInfoTable[CoreConfig - 1], NULL);
    NativePortId = PcieLibNativePortId (PortId, pConfig);
    if (NativePortId == 0xf) {
      return NULL;
    }
    pPortInfo = &pGppCfgInfoTable->PortInfoPtr[NativePortId - 4];
    break;
  default:
    return NULL;
  }
  return (PORT_INFO*)FIX_PTR_ADDR (pPortInfo, NULL);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get Pointer to static port info
 *
 *
 *
 * @param[in] PortId    PCI Express Port ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
PORT_STATIC_INFO*
PcieLibGetStaticPortInfo (
  IN       PORT              PortId,
  IN OUT   AMD_NB_CONFIG     *pConfig
  )
{
  PORT_STATIC_INFO  *pPortStaticInfo;

  pPortStaticInfo = (PORT_STATIC_INFO*)FIX_PTR_ADDR (&PortInfoTable[PortId - MIN_PORT_ID], NULL);
  return pPortStaticInfo ;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get Native Port Id.
 *  Native Port Id can be different from Port ID only on GPPSB core ports.
 *
 *
 * @param[in] PortId    PCI Express Port ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
PORT
PcieLibNativePortId (
  IN       PORT            PortId,
  IN OUT   AMD_NB_CONFIG   *pConfig
  )
{
  CORE          CoreId;
  GPP_CFG_INFO  *pGppCfgInfoTable;

  CoreId = PcieLibGetCoreId (PortId, pConfig);
  if (PcieLibGetCoreAddress (CoreId, pConfig) == GPP3a_CORE) {
    UINT32 CoreConfig;
    CoreConfig = PcieLibGetCoreConfiguration (CoreId, pConfig);
    pGppCfgInfoTable = (GPP_CFG_INFO*)FIX_PTR_ADDR (&GppCfgInfoTable[CoreConfig - 1], NULL);
    return (pGppCfgInfoTable->PortIdMap >> ((PortId - 4) * 4)) & 0xF;
  } else {
    return PortId;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get pointer to Core info structure.
 *
 *
 *
 * @param[in] CoreId    PCI Express Core ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */

CORE_INFO*
PcieLibGetCoreInfo (
  IN      CORE             CoreId,
  IN      AMD_NB_CONFIG    *pConfig
  )
{
  return (CORE_INFO*)FIX_PTR_ADDR (&CoreInfoTable[CoreId], NULL);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Reset Device in slot.
 *    Check if slot has controlled by GPI reset. If support toggle reset for 10us.
 *
 *
 * @param[in] PortId    PCI Express Port ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */

AGESA_STATUS
PcieLibResetSlot (
  IN      PORT             PortId,
  IN      AMD_NB_CONFIG    *pConfig
  )
{
 AGESA_STATUS Status;

  Status = LibNbCallBack (PHCB_AmdPortResetSupported, (UINTN) (1 << PortId), pConfig);
  if (Status == AGESA_SUCCESS) {
    LibNbCallBack (PHCB_AmdPortResetAssert, (UINTN) (1 << PortId), pConfig);
    STALL (GET_BLOCK_CONFIG_PTR (pConfig), 10, 0);
    LibNbCallBack (PHCB_AmdPortResetDeassert, (UINTN) (1 << PortId), pConfig);
  }
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/*
 * Secondary level interface to check if Gen2 disabled.
 *
 *
 *
 * @param[in] PortId    PCI Express Port ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
BOOLEAN
PcieLibCheckGen2Disabled (
  IN       PORT            PortId,
  IN OUT   AMD_NB_CONFIG   *pConfig
  )
{
  SCRATCH_1 Scratch;
  LibNbPciIndexRead (pConfig->NbPciAddress.AddressValue | NB_HTIU_INDEX, NB_HTIU_REG15, AccessS3SaveWidth32, (UINT32*)&Scratch, pConfig);
  if ((Scratch.PortGen2Disable & (1 << (PortId - 2))) != 0) {
    return FALSE;
  } else {
    CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    Force Gen2 Disable\n"));
    return TRUE;
  }
}

/*----------------------------------------------------------------------------------------*/
/*
 * Request Gen 2 disabled on next boot.
 *
 *
 *
 * @param[in] PortId    PCI Express Port ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */

VOID
PcieLibSetGen2Disabled (
  IN       PORT            PortId,
  IN OUT   AMD_NB_CONFIG   *pConfig
  )
{
  SCRATCH_1 Scratch;

  LibNbPciIndexRead (pConfig->NbPciAddress.AddressValue | NB_HTIU_INDEX, NB_HTIU_REG15, AccessS3SaveWidth32, (UINT32*)&Scratch, pConfig);
  Scratch.PortGen2Disable &= (~(1 << (PortId - 2)));
  LibNbPciIndexWrite (pConfig->NbPciAddress.AddressValue | NB_HTIU_INDEX, NB_HTIU_REG15, AccessS3SaveWidth32, (UINT32*)&Scratch, pConfig);
}

/*----------------------------------------------------------------------------------------*/
/*
 * Force link to compliance mode
 *
 *
 *
 * @param[in] PortId    PCI Express Port ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
VOID
PcieLibSetLinkCompliance (
  IN       PORT              PortId,
  IN       AMD_NB_CONFIG     *pConfig
  )
{
  PCI_ADDR    Port;
  PCIE_CONFIG *pPcieConfig;

  Port = PcieLibGetPortPciAddress (PortId, pConfig);
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  if (pPcieConfig->PortConfiguration[PortId].PortLinkMode == PcieLinkModeGen1) {
    LibNbPciIndexRMW (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REGC0, AccessWidth32, (UINT32)~BIT13, BIT13, pConfig);
  } else {
    LibNbPciRMW (Port.AddressValue | NB_PCIP_REG88, AccessWidth8, (UINT32)~BIT4, BIT4, pConfig);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get PCIe device type
 *
 *
 *
 * @param[in] Device    PCI address of device.
 * @param[in] pConfig   Northbridge configuration structure pointer.
 *
 * @retval    PCIe device type (see PCIE_DEVICE_TYPE)
 */
 /*----------------------------------------------------------------------------------------*/

PCIE_DEVICE_TYPE
PcieGetDeviceType (
  IN      PCI_ADDR        Device,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  UINT8       PcieCapPtr;
  UINT8       Value;

  PcieCapPtr = LibNbFindPciCapability (Device.AddressValue, PCIE_CAP_ID, pConfig);
  if (PcieCapPtr != 0) {
    LibNbPciRead (Device.AddressValue | (PcieCapPtr + 0x2) , AccessWidth8, &Value, pConfig);
    return Value >> 4;
  }
  return PcieNotPcieDevice;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get bitmap of cores that have active or potentially active ports
 *
 *
 *
 * @param[in] pConfig   Northbridge configuration structure pointer.
 *
 * @retval    Bitmap of cores
 */
 /*----------------------------------------------------------------------------------------*/

UINT8
PcieLibGetActiveCoreMap (
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  PORT        PortId;
  CORE        CoreId;
  UINT8       ActiveCoreMap;
  PCIE_CONFIG *pPcieConfig;
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  ActiveCoreMap = 0;
  //Check through Ports
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if (pPcieConfig->PortConfiguration[PortId].PortPresent == ON && PcieLibIsValidPortId (PortId, pConfig)) {
      if (pPcieConfig->PortConfiguration[PortId].PortCompliance == ON ||
        pPcieConfig->PortConfiguration[PortId].PortDetected == ON ||
        pPcieConfig->PortConfiguration[PortId].PortHotplug != OFF) {
        CoreId = PcieLibGetCoreId (PortId, pConfig);
        ActiveCoreMap |= (1 << CoreId);
      }
    }
  }
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    Active Core Map  = %x\n", ActiveCoreMap));
  return ActiveCoreMap;
}
