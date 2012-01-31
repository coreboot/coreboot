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
#include "HotplugFirmware.h"
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define MCU_CLEAR_BLOCK_LENGTH  16

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

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
    BIT27 | (0x8 << 12) | (0x8 << 16) | (0x8 << 20)
  },
  {
    NB_MISC_REG2B,
    0xffffffff,
    (0x8 << 12)
  },
  {
    NB_MISC_REG6C,
    0xffffffff,
    (0x8 << 16)
  },
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

/*----------------------------------------------------------------------------------------*/
/**
 * Misc Initialization prior port training.
 *
 *
 *
 * @param[in] pConfig   Northbridge configuration structure pointer.
 *
 */

VOID
PcieLibPreTrainingInit (
  IN      AMD_NB_CONFIG     *pConfig
  )
{
  UINT32      Value;
  PCIE_CONFIG *pPcieConfig;
  UINT32      ServerHotplugMask;
  BOOLEAN     SmuWa;
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibPreTrainingInit Enter\n"));
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  ServerHotplugMask = 0;
//Init Misc registers
  LibNbIndirectTableInit (
    pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX,
    0,
    (INDIRECT_REG_ENTRY*)FIX_PTR_ADDR (&PcieMiscInitTable[0],NULL),
    (sizeof (PcieMiscInitTable) / sizeof (INDIRECT_REG_ENTRY)),
    pConfig
    );
//Setup peer-to-peer
  if (pPcieConfig->PcieConfiguration.Peer2Peer == ON) {
    if (pPcieConfig->CoreConfiguration[PcieLibGetCoreId (3, pConfig)] == GFX_CONFIG_AABB) {
      Value = 0x08080404;
    } else {
      Value = 0x08080008;
    }
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG49, AccessWidth32, 0, Value, pConfig);
    if (pPcieConfig->CoreConfiguration[PcieLibGetCoreId (12, pConfig)] == GFX_CONFIG_AABB) {
      Value = 0xFFFF0404;
    } else {
      Value = 0xFFFF0008;
    }
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG2F, AccessWidth32, 0, Value, pConfig);
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG48, AccessWidth32, (UINT32)~(BIT8), 0xffff0000, pConfig);
  }

 //Remap device number
#ifndef DEVICE_REMAP_DISABLE
  if (PciePortRemapInit (pConfig) != AGESA_SUCCESS ) {
    REPORT_EVENT (AGESA_ERROR, PCIE_ERROR_DEVICE_REMAP, 0, 0, 0, 0, pConfig);
  }
#endif

#ifndef HOTPLUG_SUPPORT_DISABLED
  ServerHotplugMask = PcieInitHotplug (pConfig);
#endif

  LibNbPciIndexRead (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG4A, AccessWidth32, &Value, pConfig);
  SmuWa = ((Value & BIT21) != 0) ? TRUE : FALSE;

  if (SmuWa || ServerHotplugMask != 0) {
    UINT32  BlockIndex;
    UINT32  SmuWaData;
    UINT16  Address;
    UINT32  Data[MCU_CLEAR_BLOCK_LENGTH];

    LibNbMcuControl (AssertReset, pConfig);
    // clear SMU RAM
    LibAmdMemFill (&Data[0], 0, sizeof (Data), (AMD_CONFIG_PARAMS *)&(pConfig->sHeader));
    for (Address = 0; Address < (16 * 1024); Address = Address + 4 * MCU_CLEAR_BLOCK_LENGTH) {
      LibNbLoadMcuFirmwareBlock (Address, MCU_CLEAR_BLOCK_LENGTH, &Data[0], pConfig);
    }
    //Load SMU firmware
    for (BlockIndex = 0; BlockIndex < Fm.NumberOfBlock; BlockIndex++) {
      LibNbLoadMcuFirmwareBlock (Fm.BlockArray[BlockIndex].Address, Fm.BlockArray[BlockIndex].Length, Fm.BlockArray[BlockIndex].Data, pConfig);
    }
    if (SmuWa) {
      SmuWaData = LibHtGetSmuWaData (pConfig);
      LibNbLoadMcuFirmwareBlock (0xFE70, 0x1, &SmuWaData, pConfig);
    }
    SmuWaData = ((SmuWa == TRUE) ? 0x100 : 0x100) | ((ServerHotplugMask != 0) ?  0x1 : 0);
    LibNbLoadMcuFirmwareBlock (0xFE74, 0x1, &SmuWaData, pConfig);

    LibNbMcuControl (DeAssertReset, pConfig);
  }

#ifndef HOTPLUG_SUPPORT_DISABLED
  PcieCheckHotplug (ServerHotplugMask, pConfig);
#endif

}

INDIRECT_REG_ENTRY  PcieCoreInitTable[] = {
  {
    NB_BIFNB_REG10,
    (UINT32)~(BIT10 + BIT11 + BIT12),
    BIT12
  },
  {
    NB_BIFNB_REG20,
    (UINT32)~(BIT8 + BIT9),
    BIT9
  },
  {
    NB_BIFNB_REG02,
    (UINT32)~(BIT0),
    BIT0
  },
  {
    NB_BIFNB_REG40,
    (UINT32)~(BIT14 + BIT15),
    BIT15
  },
  {
    NB_BIFNB_REGC2,
    (UINT32)~(BIT25),
    BIT25
  },
  {
    NB_BIFNB_REGC1,
    (UINT32)~(BIT0),
    (BIT0 + BIT1 + BIT2)
  },
  {
    NB_BIFNB_REG1C,
    0x0,
    (4 << 6) + (4 << 1) + 1
  }
};

INDIRECT_REG_ENTRY  PcieRd790CoreInitTable[] = {
  {
    NB_BIFNB_REGC2,
    (UINT32)~(BIT14),
    (BIT14)
  },
  {
    NB_BIFNB_REGC1,
    (UINT32)~(BIT2),
    0x0
  },
};

/*----------------------------------------------------------------------------------------*/
/**
 * Init Core registers
 *
 *
 *
 * @param[in] CoreId    PCI Express Core ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
VOID
PcieLibCommonCoreInit (
  IN       CORE              CoreId,
  IN       AMD_NB_CONFIG     *pConfig
  )
{
  UINT32  CoreAddress;
  NB_INFO NbInfo;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibCommonCoreInit (CoreId = %d) Enter\n", CoreId));
  CoreAddress = PcieLibGetCoreAddress (CoreId, pConfig);
  NbInfo = LibNbGetRevisionInfo (pConfig);

  LibNbIndirectTableInit (
    pConfig->NbPciAddress.AddressValue | NB_BIF_INDEX,
    CoreAddress,
    (INDIRECT_REG_ENTRY*)FIX_PTR_ADDR (&PcieCoreInitTable[0],NULL),
    (sizeof (PcieCoreInitTable) / sizeof (INDIRECT_REG_ENTRY)),
    pConfig
    );

  if (CoreAddress == SB_CORE) {
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_BIF_INDEX, NB_BIFNB_REG10 | CoreAddress, AccessWidth32, (UINT32)~BIT9, BIT9, pConfig);
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_HTIU_INDEX, NB_HTIU_REG06, AccessWidth32, (UINT32)~BIT26, BIT26 + BIT1, pConfig);
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_BIF_INDEX, NB_BIFNB_REG1C | CoreAddress, AccessWidth32, (UINT32)~BIT0, 0x0, pConfig);
  }
  if ( NbInfo.Type < NB_SR5690 ) {
    LibNbIndirectTableInit (
      pConfig->NbPciAddress.AddressValue | NB_BIF_INDEX,
      CoreAddress,
      (INDIRECT_REG_ENTRY*)FIX_PTR_ADDR (&PcieRd790CoreInitTable[0], NULL),
      (sizeof (PcieRd790CoreInitTable) / sizeof (INDIRECT_REG_ENTRY)),
      pConfig
      );
  }
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibCommonCoreInit Exit\n"));
};


/*----------------------------------------------------------------------------------------*/
/**
 * Init Core after training is completed
 *
 *
 *
 * @param[in] CoreId    PCI Express Core ID
 * @param[in] pConfig   Northbridge configuration structure pointer.  *
 */


VOID
PcieLibCoreAfterTrainingInit (
  IN       CORE              CoreId,
  IN       AMD_NB_CONFIG     *pConfig
  )
{
  UINT32      CoreAddress;
  PCIE_CONFIG *pPcieConfig;
  PCI_ADDR    ClkPciAddress;

  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  CoreAddress = PcieLibGetCoreAddress (CoreId, pConfig);
  ClkPciAddress = pConfig->NbPciAddress;
  ClkPciAddress.Address.Function = 1;
  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_BIF_INDEX, NB_BIFNB_REG20 | CoreAddress, AccessWidth32, (UINT32)~(BIT9), 0, pConfig);
//Save core setting in scratch register
  LibNbPciIndexWrite (
    pConfig->NbPciAddress.AddressValue | NB_BIF_INDEX,
    NB_BIFNB_REG01 | CoreAddress,
    AccessWidth32,
    (UINT32*)&pPcieConfig->CoreSetting[CoreId],
    pConfig
    );
//Save general setting in scratch
  LibNbEnableClkConfig (pConfig);
  LibNbPciWrite (ClkPciAddress.AddressValue | NB_CLK_REG78, AccessWidth32, &pPcieConfig->PcieConfiguration, pConfig);
  LibNbDisableClkConfig (pConfig);

}


INDIRECT_REG_ENTRY  PciePortInitTable[] = {
  {
    NB_BIFNBP_REG02,
    (UINT32)~(BIT15),
    BIT15
  },
  {
    NB_BIFNBP_REGA1,
    (UINT32)~(BIT24 + BIT26),
    BIT11
  },
  {
    NB_BIFNBP_REGB1,
    0xffffffff,
    BIT28 + BIT23 + BIT19 + BIT20
  },
  {
    NB_BIFNBP_REGA4,
    (UINT32)~(BIT0),
    0x0
  },
  {
    NB_BIFNBP_REGA2,
    (UINT32)~(BIT13),
    BIT13
  },
  {
    NB_BIFNBP_REGA3,
    (UINT32)~(BIT9),
    BIT9
  },
  {
    NB_BIFNBP_REGA0,
    0xffff000f,
    0x6830
  },
  {
    NB_BIFNBP_REGC1,
    0xfffffff0,
    0xC
  },
  {
    NB_BIFNBP_REG70,
    (UINT32)~(BIT16 + BIT17 + BIT18),
    BIT16 + BIT18
  }
};
/*----------------------------------------------------------------------------------------*/
/**
 * Init port registers
 *
 *
 *
 * @param[in] PortId    PCI Express Port ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 *
 */
VOID
PcieLibCommonPortInit (
  IN       PORT              PortId,
  IN       AMD_NB_CONFIG     *pConfig
  )
{
  PCI_ADDR    Port;
  PCIE_CONFIG *pPcieConfig;
  UINT32      PcieSlotCapability;
  UINT32      CoreAddress;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibCommonPortInit PortId %d Enter\n", PortId));
  Port = PcieLibGetPortPciAddress (PortId, pConfig);
  CoreAddress = PcieLibGetCoreAddress (PcieLibGetCoreId (PortId, pConfig), pConfig);
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);

  LibNbIndirectTableInit (
    Port.AddressValue | NB_BIF_INDEX,
    0x0,
    (INDIRECT_REG_ENTRY*)FIX_PTR_ADDR (&PciePortInitTable[0],NULL),
    (sizeof (PciePortInitTable) / sizeof (INDIRECT_REG_ENTRY)),
    pConfig
    );
  if (CoreAddress == GPP3a_CORE || CoreAddress == SB_CORE || CoreAddress == GPP3b_CORE) {
    LibNbPciIndexRMW (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REG70, AccessWidth32, (UINT32)~(BIT16 + BIT17 + BIT18), (BIT17 + BIT18), pConfig);
    if (CoreAddress == GPP3a_CORE) {
      LibNbPciIndexRMW (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REGB1, AccessWidth32, (UINT32)~(BIT22), BIT22, pConfig);
    }
  }
// Set completion timeout
  LibNbPciRMW (Port.AddressValue | NB_PCIP_REG80, AccessS3SaveWidth8, 0xF0, 0x6, pConfig);
  //if (CoreAddress != SB_CORE) {
  //  LibNbPciIndexRMW (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REG10, AccessWidth32, (UINT32)~BIT0, 0x0, pConfig);
  //}
//For hotplug ports
  //if (pPcieConfig->PortConfiguration[PortId].PortHotplug != OFF ||
  //    pPcieConfig->PcieConfiguration.DisableHideUnusedPorts == ON ||
  //    LibNbGetRevisionInfo (pConfig).Revision == NB_REV_A11) {
  LibNbPciIndexRMW (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REG20, AccessWidth32, (UINT32)~BIT19, 0x0, pConfig);
  //}

// Enable Immediate ACK
  if (pPcieConfig->ExtPortConfiguration[PortId].PortL1ImmediateACK == ON) {
    LibNbPciIndexRMW (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REGA0, AccessWidth32, (UINT32)~BIT23, BIT23, pConfig);
  }
//Set up slot capability
  PcieSlotCapability = (pPcieConfig->ExtPortConfiguration[PortId].PortPowerLimit << 7) |
                       ((Port.Address.Device | Port.Address.Bus << 5 ) << 19);
  LibNbPciRMW (Port.AddressValue | NB_PCIP_REG6C, AccessS3SaveWidth32, (UINT32)~((0x3ff << 7) | (0x1fff << 19)), PcieSlotCapability, pConfig);
  LibNbPciRMW (Port.AddressValue | NB_PCIP_REG5A, AccessS3SaveWidth16, (UINT32)~BIT8, BIT8, pConfig);
//Set interrupt pin info
  LibNbPciRMW (Port.AddressValue | NB_PCIP_REG3D, AccessS3SaveWidth8, 0x0, 0x1, pConfig);

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibCommonPortInit Exit\n"));
};
