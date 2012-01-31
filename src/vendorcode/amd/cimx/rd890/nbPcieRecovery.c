/**
 * @file
 *
 * PCIe in recovery support
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
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
PcieRecoveryCoreInit (
  IN OUT   AMD_NB_CONFIG_BLOCK *ConfigPtr
  );

VOID
PcieRecoveryPortTraining (
  IN OUT   AMD_NB_CONFIG_BLOCK *ConfigPtr
  );

VOID
PcieRecoveryCommonPortInit (
  IN       PORT            PortId,
  IN       AMD_NB_CONFIG   *NbConfigPtr
  );

VOID
PcieRecoveryCommonCoreInit (
  IN       CORE            CoreId,
  IN       AMD_NB_CONFIG   *NbConfigPtr
  );

UINT32
PcieRecoveryGetCoreAddress (
  IN       CORE             CoreId,
  IN       AMD_NB_CONFIG    *NbConfigPtr
  );

PCI_ADDR
PcieRecoveryGetPortPciAddress (
  IN       PORT             PortId,
  IN       AMD_NB_CONFIG    *NbConfigPtr
  );

VOID
PcieRecoveryPcieCheckPorts (
  IN       PORT            PortId,
  IN       AMD_NB_CONFIG   *NbConfigPtr
  );

VOID
PcieRecoveryReleaseTraining (
  IN       PORT            PortId,
  IN       AMD_NB_CONFIG   *NbConfigPtr
  );

PORT
PcieRecoveryNativePortId (
  IN       PORT            PortId,
  IN OUT   AMD_NB_CONFIG   *NbConfigPtr
  );

/*----------------------------------------------------------------------------------------*/
/**
 * PCIE Recovery Init. Basic programming / EP training.
 *    After this call EP are fully operational.
 *
 *
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 *
 */
AGESA_STATUS
AmdPcieEarlyInit (
  IN OUT   AMD_NB_CONFIG_BLOCK *ConfigPtr
  )
{
#ifdef PCIE_RECOVERY_SUPPORT
  PcieRecoveryCoreInit (ConfigPtr);
  PcieRecoveryPortTraining (ConfigPtr);
#endif
  return AGESA_SUCCESS;
}


#ifdef  PCIE_RECOVERY_SUPPORT

INDIRECT_REG_ENTRY
STATIC
PcieRecoveryMiscInitTable[] = {
  {
    NB_MISC_REG0C,
    (UINT32)~0x001f00FC,
    0x00000000
  },
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
  }, //[17][16]Sets Electrical Idle Threshold
  {
    NB_MISC_REG28,
    0xffffff00,
    0x0
  },
  {
    NB_MISC_REG27,
    0x3fffffff,
    0x0
  },
  {
    NB_MISC_REG2D,
    (UINT32)~(BIT5),
    0x0
  }
};

//                                             2  3  4  5  6  7  8  9  A  B  C  D
UINT8 PortToCoreMappingTable[] = { 0xff, 0xff, 0, 0, 3, 3, 3, 3, 4, 3, 3, 1, 1, 3 };

/*----------------------------------------------------------------------------------------*/
/**
 * Minimum core initialization
 *
 *
 *
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 *
 */

VOID
PcieRecoveryCoreInit (
  IN OUT   AMD_NB_CONFIG_BLOCK *ConfigPtr
  )
{
  CORE          CoreId;
  PORT          PortId;
  AMD_NB_CONFIG *NbConfigPtr;
  PCIE_CONFIG   *pPcieConfig;

  NbConfigPtr = &ConfigPtr->Northbridges[0];
  pPcieConfig = GET_PCIE_CONFIG_PTR (NbConfigPtr);
//Init Misc registers
  LibNbIndirectTableInit (
    NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX,
    0,
    (INDIRECT_REG_ENTRY*)FIX_PTR_ADDR (&PcieRecoveryMiscInitTable[0],NULL),
    (sizeof (PcieRecoveryMiscInitTable) / sizeof (INDIRECT_REG_ENTRY)),
    NbConfigPtr
    );
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if (pPcieConfig->PortConfiguration[PortId].PortPresent == ON) {
      pPcieConfig->CoreSetting[PortToCoreMappingTable[PortId]].CoreDisabled = OFF;
    }
  }
  for (CoreId = 0; CoreId <= MAX_CORE_ID; CoreId++) {
    if (pPcieConfig->CoreSetting[CoreId].CoreDisabled == OFF) {
      //Init core registers and configuration
      PcieRecoveryCommonCoreInit (CoreId, NbConfigPtr);
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Port link training initialization
 *
 *
 *
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 *
 */

VOID
PcieRecoveryPortTraining (
  IN OUT   AMD_NB_CONFIG_BLOCK *ConfigPtr
  )
{
  PORT          PortId;
  UINT32        PortToHideMap;
  AMD_NB_CONFIG *NbConfigPtr;
  PCIE_CONFIG   *pPcieConfig;

  PortToHideMap = 0;
  NbConfigPtr = &ConfigPtr->Northbridges[0];
  pPcieConfig = GET_PCIE_CONFIG_PTR (NbConfigPtr);
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if (pPcieConfig->PortConfiguration[PortId].PortPresent == ON) {
      PcieRecoveryCommonPortInit (PortId, NbConfigPtr);
      if (LibNbCallBack (PHCB_AmdPortResetDeassert, 1 << PortId, NbConfigPtr) == AGESA_SUCCESS) {
        STALL (GET_BLOCK_CONFIG_PTR (NbConfigPtr), pPcieConfig->ResetToTrainingDelay, 0);
      }
      if (PortId != 8) {
        PcieRecoveryReleaseTraining (PortId, NbConfigPtr);
      }
    }
  }
  STALL (GET_BLOCK_CONFIG_PTR (pConfig), pPcieConfig->ReceiverDetectionPooling, 0);
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    PCI_ADDR Port;
    Port = PcieRecoveryGetPortPciAddress (PortId, NbConfigPtr);
    if (pPcieConfig->PortConfiguration[PortId].PortPresent == ON && PortId != 8) {
      PcieRecoveryPcieCheckPorts (PortId, NbConfigPtr);

      pPcieConfig->PortConfiguration[PortId].PortLinkMode = PcieLinkModeGen1;

      LibNbPciIndexWrite (
        Port.AddressValue | NB_BIF_INDEX,
        NB_BIFNBP_REG01,
        AccessWidth32,
        (UINT32*)&pPcieConfig->PortConfiguration[PortId],
        NbConfigPtr
        );
    }
    if (pPcieConfig->PortConfiguration[PortId].PortDetected == OFF) {
      PortToHideMap |= (1 << PortId);
    }
  }
  LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG0C, AccessWidth32, 0xffffffff, (PortToHideMap & 0xFC) | ((PortToHideMap & 0x3E00) << 7), NbConfigPtr);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Check link training Status
 *
 *
 *
 *
 * @param[in] Config   Northbridges configuration structure pointer.
 *
 */
VOID
PcieRecoveryPcieCheckPorts (
  IN      PORT            PortId,
  IN      AMD_NB_CONFIG   *NbConfigPtr
  )
{
  PCIE_CONFIG *pPcieConfig;
  PCI_ADDR    Port;
  UINT32      LinkState;
  UINT32      LinkStatePooling;
  UINT32      Value;

  pPcieConfig = GET_PCIE_CONFIG_PTR (NbConfigPtr);
  Port = PcieRecoveryGetPortPciAddress (PortId, NbConfigPtr);
  LinkStatePooling = pPcieConfig->ReceiverDetectionPooling;
  do {
    LibNbPciIndexRead (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REGA5, AccessWidth32, &LinkState, NbConfigPtr);
    CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieRecoveryCoreInit PortId = %d LinkState 0x%x\n", PortId,  LinkState));
    LinkState &= 0x3F;
    if (LinkState == 0x10) {
      UINT16  VcoStatus;
      BOOLEAN VcoNotCompleted;
      UINT32  VcoPooling;
      VcoNotCompleted = TRUE;
      VcoPooling = 6000;
      do {
        LibNbPciRead (Port.AddressValue | NB_PCIP_REG12A, AccessWidth16, &VcoStatus, NbConfigPtr);
        if (VcoStatus & BIT1) {
          LibNbPciIndexRead (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REGA2, AccessWidth32, &Value, NbConfigPtr);
          Value = (Value & 0xfffffe80) | ((Value & 0x70) >> 4) | BIT8;
          LibNbPciIndexWrite (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REGA2, AccessWidth32, &Value, NbConfigPtr);
          STALL (GET_BLOCK_CONFIG_PTR (NbConfigPtr), 5000, 0);
        } else {
          VcoNotCompleted = FALSE;
        }
      } while (VcoNotCompleted || --VcoPooling != 0);
      if (!VcoNotCompleted) {
        pPcieConfig->PortConfiguration[PortId].PortDetected = ON;
      }
    } else {
      STALL (GET_BLOCK_CONFIG_PTR (NbConfigPtr), 1000, 0);
    }
  } while (LinkState != 0x10 && --LinkStatePooling != 0);
}


UINT8 PortTrainingOffset[] = {
  4,  5, 21, 22, 23, 24, 20, 25, 26, 6, 7 , 4
};


/*----------------------------------------------------------------------------------------*/
/**
 * Check link training Status
 *
 *
 *
 *
 * @param[in] Config   Northbridges configuration structure pointer.
 *
 */
VOID
PcieRecoveryReleaseTraining (
  IN        PORT            PortId,
  IN        AMD_NB_CONFIG   *NbConfigPtr
  )
{
  PORT NativePortId;

  NativePortId  = PcieRecoveryNativePortId (PortId, NbConfigPtr);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieRecoveryReleaseTraining PortId = %d NativeId %d BitOfset %d\n",
    PortId, NativePortId, ((UINT8*)FIX_PTR_ADDR (&PortTrainingOffset[0], NULL))[NativePortId - MIN_PORT_ID]));
  LibNbPciIndexRMW (
    NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX,
    (PortId == 13)? NB_MISC_REG2A:NB_MISC_REG08,
    AccessWidth32,
    ~(1 << ((UINT8*)FIX_PTR_ADDR (&PortTrainingOffset[0], NULL))[NativePortId - MIN_PORT_ID]),
    0,
    NbConfigPtr
    );
}

INDIRECT_REG_ENTRY  PcieRecoveryPortInitTable[] = {
  {
    NB_BIFNBP_REG02,
    (UINT32)~(BIT15),
    BIT15
  },
  {
    NB_BIFNBP_REGA1,
    (UINT32)~(BIT24),
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
    0xffff00ff,
    0x6130
  },
  {
    NB_BIFNBP_REG70,
    (UINT32)~(BIT16 + BIT17 + BIT18),
    BIT16 + BIT18
  },
  // Set Link for Gen1
  {
    NB_BIFNBP_REGC0,
    (UINT32)~(BIT15),
    BIT15
  },
  {
    NB_BIFNBP_REGA2,
    (UINT32)~(BIT13),
    BIT13
  },
  {
    NB_BIFNBP_REGA4,
    (UINT32)~(BIT0 + BIT29),
    0x0
  }
};

/*----------------------------------------------------------------------------------------*/
/**
 * Port basic register init
 *
 *
 *
 *
 * @param[in] Config   Northbridges configuration structure pointer.
 *
 */

VOID
PcieRecoveryCommonPortInit (
  IN        PORT            PortId,
  IN        AMD_NB_CONFIG   *NbConfigPtr
  )
{
  PCI_ADDR Port;

  Port = PcieRecoveryGetPortPciAddress (PortId, NbConfigPtr);
  LibNbIndirectTableInit (
    Port.AddressValue | NB_BIF_INDEX,
    0x0,
    (INDIRECT_REG_ENTRY*)FIX_PTR_ADDR (&PcieRecoveryPortInitTable[0],NULL),
    (sizeof (PcieRecoveryPortInitTable) / sizeof (INDIRECT_REG_ENTRY)),
    NbConfigPtr
    );
  LibNbPciRMW (Port.AddressValue | NB_PCIP_REG80, AccessWidth8, 0xF0, 0x6, NbConfigPtr);
  LibNbPciRMW (Port.AddressValue | NB_PCIP_REG88, AccessWidth8, 0xF0, 0x0, NbConfigPtr);
}


UINT8 GppConfigTable[] = {
  0x0, 0x1, 0x2, 0xC, 0xA, 0x4, 0xB
};

INDIRECT_REG_ENTRY  PcieRecoveryCoreInitTable[] = {
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
/*----------------------------------------------------------------------------------------*/
/**
 * Core basic register init
 *
 *
 *
 *
 * @param[in] Config   Northbridges configuration structure pointer.
 *
 */

VOID
PcieRecoveryCommonCoreInit (
  IN        CORE            CoreId,
  IN        AMD_NB_CONFIG   *NbConfigPtr
  )
{
  UINT32      CoreAddress;
  PCIE_CONFIG *pPcieConfig;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieRecoveryCommonCoreInit CoreID = %d Enter\n", CoreId));
  CoreAddress = PcieRecoveryGetCoreAddress (CoreId, NbConfigPtr);
  pPcieConfig = GET_PCIE_CONFIG_PTR (NbConfigPtr);
  //Setup GPP1 core configuration
  if (CoreAddress == GPP1_CORE && (pPcieConfig->CoreConfiguration[0] == GFX_CONFIG_AABB || NbConfigPtr->pPcieConfig->PortConfiguration[3].PortPresent == ON)) {
    LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG08 , AccessWidth32, 0xffffffff, BIT15, NbConfigPtr);
    LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG26, AccessWidth32, (UINT32)~BIT28, BIT28, NbConfigPtr);
    LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG08, AccessWidth32, 0xffffffff, BIT8, NbConfigPtr);
    STALL (GET_BLOCK_CONFIG_PTR (NbConfigPtr), 2000, 0);
    LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG26, AccessWidth32, (UINT32)~BIT28, 0x0, NbConfigPtr);
  }
    LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG08 , AccessWidth32, (UINT32)~BIT15, 0x0, NbConfigPtr);
  //Setup GPP2 core configuration
  if (CoreAddress == GPP2_CORE && (pPcieConfig->CoreConfiguration[1] == GFX_CONFIG_AABB || NbConfigPtr->pPcieConfig->PortConfiguration[12].PortPresent == ON)) {
    LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG08 , AccessWidth32, 0xffffffff, BIT13, NbConfigPtr);
    LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG26, AccessWidth32, (UINT32)~BIT28, BIT29, NbConfigPtr);
    LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG08, AccessWidth32, 0xffffffff, BIT9, NbConfigPtr);
    STALL (GET_BLOCK_CONFIG_PTR (NbConfigPtr), 2000, 0);
    LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG26, AccessWidth32, (UINT32)~BIT29, 0x0, NbConfigPtr);
  }
    LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG08 , AccessWidth32, (UINT32)~BIT13, 0x0, NbConfigPtr);
  //Setup GPP core configuration
  if (CoreAddress == GPP3a_CORE) {
    UINT32  Mux;
    UINT8 *pGppConfigTable;
    Mux = 0;
    pGppConfigTable = (UINT8*)FIX_PTR_ADDR (&GppConfigTable[0], NULL);
    LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG08, AccessWidth32, 0xffffffff, BIT31, NbConfigPtr);
    LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG26, AccessWidth32, (UINT32)~BIT30, BIT30, NbConfigPtr);
    LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG67, AccessWidth32,  0xfffffff0, (UINT32)pGppConfigTable[pPcieConfig->CoreConfiguration[CoreId]], NbConfigPtr);
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
    }
    LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG26, AccessWidth32, 0xf0000000, Mux, NbConfigPtr);
    LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG26, AccessWidth32, (UINT32)~BIT30, 0x0, NbConfigPtr);
    LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG08, AccessWidth32, (UINT32)~BIT31, 0x0, NbConfigPtr);
  }
  if (CoreAddress == GPP3b_CORE) {
    LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG2A, AccessWidth32, (UINT32)~BIT15, 0, NbConfigPtr);
  }
  LibNbIndirectTableInit (
    NbConfigPtr->NbPciAddress.AddressValue | NB_BIF_INDEX,
    CoreAddress,
    (INDIRECT_REG_ENTRY*)FIX_PTR_ADDR (&PcieRecoveryCoreInitTable[0],NULL),
    (sizeof (PcieRecoveryCoreInitTable) / sizeof (INDIRECT_REG_ENTRY)),
    NbConfigPtr
    );
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieRecoveryCommonCoreInit Exitr\n"));
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
PcieRecoveryGetCoreAddress (
  IN       CORE             CoreId,
  IN       AMD_NB_CONFIG    *NbConfigPtr
  )
{
  UINT32  CoreAddress;
  CoreAddress = GPP1_CORE;
  switch (CoreId) {
  case 0x0:
    CoreAddress = GPP1_CORE;
    break;
  case 0x1:
    CoreAddress = GPP2_CORE;
    break;
  case 0x2:
    CoreAddress = GPP3a_CORE;
    break;
  case 0x3:
    CoreAddress = GPP3b_CORE;
    break;
  case 0x4:
    CoreAddress = SB_CORE;
    break;
  default:
    CIMX_ASSERT (FALSE);
  }
  return  CoreAddress;
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
PcieRecoveryGetPortPciAddress (
  IN      PORT             PortId,
  IN      AMD_NB_CONFIG    *NbConfigPtr
  )
{
  PCI_ADDR Port;
  Port = NbConfigPtr->NbPciAddress;
  Port.Address.Device = PortId;
  return  Port;
}

UINT32 GppNativeIdTable[] = {
  0xff50fff4,
  0xf650fff4,
  0xff60f5f4,
  0xf760f5f4,
  0xf87065f4,
  0xf9807654
};

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
PcieRecoveryNativePortId (
  IN       PORT            PortId,
  IN OUT   AMD_NB_CONFIG   *NbConfigPtr
  )
{
  UINT32 GppNativeIdMap;
  if (PortId > 3 && PortId < 11) {
    GppNativeIdMap = ((UINT32*)FIX_PTR_ADDR (&GppNativeIdTable[0], NULL))[NbConfigPtr->pPcieConfig->CoreConfiguration[0x2] - 1];
    return (GppNativeIdMap >> ((PortId - 4)*4)) & 0xF;
  } else {
    return PortId;
  }
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
PcieRecoveryInitializer (
  IN OUT   AMD_NB_CONFIG   *NbConfigPtr
  )
{
  PCIE_CONFIG *pPcieConfig;
  CORE        CoreId;
  pPcieConfig = GET_PCIE_CONFIG_PTR (NbConfigPtr);
  if (pPcieConfig == NULL) {
    return  AGESA_FATAL;
  }
  LibAmdMemFill (pPcieConfig, 0, sizeof (PCIE_CONFIG), (AMD_CONFIG_PARAMS *)NbConfigPtr);
  pPcieConfig->ReceiverDetectionPooling = 120;
  pPcieConfig->ResetToTrainingDelay = 4;
  for (CoreId = 0; CoreId <= MAX_CORE_ID; CoreId++) {
    pPcieConfig->CoreSetting[CoreId].CoreDisabled = ON;
  }
  return  AGESA_SUCCESS;
}

#endif
