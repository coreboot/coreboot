/**
 * @file
 *
 * HT Initialization
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

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
#define LINK_BUFFERS_IFCM  ((1 << 31) + (1 << 25) + (8 << 20) + (1 << 18) + (1 << 8) + (6 << 5) + 0xF)
#define LINK_BUFFERS_NFCM  ((1 << 31) + (1 << 25) + (8 << 20) + (1 << 18) + (1 << 8) + (6 << 5) + 0x11)
#define SUBLINK_BUFFERS_IFCM  ((1 << 31) + (1 << 25) + (8 << 20) + (1 << 18) + (1 << 8) + (6 << 5) + 0xF)
#define SUBLINK_BUFFERS_NFCM  ((1 << 31) + (1 << 25) + (8 << 20) + (1 << 18) + (1 << 8) + (6 << 5) + 0x11)

typedef VOID DUMMY_CALL (VOID);

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
NbInitRasParityMacro (
  IN      AMD_NB_CONFIG *NbConfigPtr
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

UINT8 IBIASCodeTable[] = {
//0.2G        0.5G        0.6G
  0x44, 0x00, 0x44, 0x00, 0xb6,
//0.8G  1.0G  1.2G  1.4G  1.6G
  0x44, 0x96, 0xb6, 0x23, 0x44,
//1.8G  2.0G  2.2G  2.4G  2.6G
  0x64, 0x96, 0xA6, 0xb6, 0xc6
};



/*----------------------------------------------------------------------------------------*/
/**
 * NB Init at early post.
 *
 *
 *
 * @param[in] pConfig   Northbridge configuration structure pointer.
 *
 */

AGESA_STATUS
HtLibEarlyInit (
  IN OUT   AMD_NB_CONFIG *pConfig
  )
{
  AGESA_STATUS            Status;
  UINT32                  Value;
  PCI_ADDR                ClkPciAddress;
  PCI_ADDR                CpuPciAddress;
  PCI_ADDR                LinkPciAddress;
  HT_CONFIG               *pHtConfig;
  UINT8                   CpuHtSpeed;
  UINT8                   NbHtSpeed;
  HT_INACTIVE_LANE_STATE  InLnSt;
  BOOLEAN                 IsIfcmEnabled;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBHT_TRACE), "[NBHT]HtLibEarlyInit Enter\n"));
  pHtConfig = GET_HT_CONFIG_PTR (pConfig);
  Status = AGESA_SUCCESS;
  ClkPciAddress = pConfig->NbPciAddress;
  ClkPciAddress.Address.Device = 1;
  CpuPciAddress.AddressValue = MAKE_SBDFO (0, 0, pConfig->NbHtPath.NodeID + 0x18, 0,  0);
  LinkPciAddress.AddressValue = MAKE_SBDFO (0, 0, pConfig->NbHtPath.NodeID + 0x18, ((pConfig->NbHtPath.LinkID & 0xF0) > 0x10)?4:0,  0);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBHT_TRACE), "    Node %d Link %d PciAddress %x\n", pConfig->NbHtPath.NodeID, pConfig->NbHtPath.LinkID, LinkPciAddress.AddressValue));
  LibNbEnableClkConfig (pConfig);
//Get Ht Speed  Info
  LibNbPciRead (LinkPciAddress.AddressValue | (HT_PATH_LINK_ID (pConfig->NbHtPath) * 32 + 0x89), AccessWidth8, &CpuHtSpeed, pConfig);
  LibNbPciRead (pConfig->NbPciAddress.AddressValue | NB_PCI_REGD1 , AccessWidth8, &NbHtSpeed, pConfig);
  NbHtSpeed &= 0xf;
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBHT_TRACE), "    Ht speed Cpu %x Nb %x\n", CpuHtSpeed, NbHtSpeed));
//Set IBIAS code
  LibNbPciRMW (ClkPciAddress.AddressValue | NB_CLK_REGD8, AccessWidth16, (UINT32)~(0x3ff), ((UINT8*)FIX_PTR_ADDR (&IBIASCodeTable[0], NULL))[(pHtConfig->HtReferenceClock / 200)*CpuHtSpeed], pConfig);
  if (CpuHtSpeed > HT_FREQUENCY_1000M) {
    UINT8 T0Time;
    UINT8 ForceFullT0;
//Enable Protocol checker
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_HTIU_INDEX, NB_HTIU_REG1E, AccessWidth32, 0, 0x7FFFFFFC, pConfig);
//Set NB Transmitter Deemphasis
    if ((pHtConfig->NbTransmitterDeemphasis & 0x80) == 0) {
      LibHtSetNbTransmitterDeemphasis (pHtConfig->NbTransmitterDeemphasis, pConfig);
    }
    LibNbPciRead (CpuPciAddress.AddressValue | 0x16C, AccessWidth32, &Value, pConfig);
    T0Time = (UINT8) (Value & 0x3F);
    ForceFullT0 = (UINT8) ((Value >> 13) & 0x7);
//Enable LS State and set T0Time
    //T0Time = 0x14; //2us
    if (pHtConfig->LSx <  HtLinkStateSkipInit) {
      if (pHtConfig->LSx == HtLinkStateSameAsCpu) {
        LibNbPciRead (
          CpuPciAddress.AddressValue | (HT_PATH_LINK_ID (pConfig->NbHtPath) * 4 + HT_PATH_SUBLINK_ID (pConfig->NbHtPath) * 0x10 + 0x170),
          AccessWidth32,
          &Value,
          pConfig
          );
        if ((Value & BIT8) != 0) {
          pHtConfig->LSx = HtLinkStateLS2;
        } else {
          pHtConfig->LSx = HtLinkStateLS1;
        }
      } else {
        if (pHtConfig->LSx >=  HtLinkStateLS2) {
          T0Time = 0x26; //12us
          ForceFullT0 = 0x6;
        } else {
          T0Time = 0x14; //2us
          ForceFullT0 = 0x0;
        }
      }
      if (CpuHtSpeed == NbHtSpeed) {
        LibHtEnableLxState (pHtConfig->LSx, pConfig);
      }
    }
//Set up InLnSt
    //Match CPU InLnSt except for HT3 LS1
    LibNbPciRead (
      CpuPciAddress.AddressValue | 0x16C,
      AccessWidth32,
      &Value,
      pConfig
      );
    InLnSt = (Value >> 6) & 0x3;

    // Do not enable HT3 LS1 with InLnSt == 0x1 (PHY_OFF) as per errata 9
    if (pHtConfig->LSx == HtLinkStateLS1) {
      if (InLnSt == InactiveLaneStateSameAsPhyOff) {
        InLnSt = InactiveLaneStateCadCtrlDrivelToLogic0;
      }
    }
    LibNbPciRMW (
      pConfig->NbPciAddress.AddressValue | NB_PCI_REGA0,
      AccessWidth8,
      0x00,
      T0Time | (InLnSt << 6),
      pConfig
      );
    LibNbPciRMW (
      CpuPciAddress.AddressValue | 0x16C,
      AccessWidth16,
      (UINT32)(~((0x7 << 13) + 0x3f)),
      T0Time | (ForceFullT0 << 13),
      pConfig
      );
  // Disable command throtling
    LibNbPciRMW (pConfig->NbPciAddress.AddressValue | (NB_PCI_REGAC + 1), AccessWidth8, (UINT32)~BIT6, BIT6, pConfig);
    LibNbPciRMW (CpuPciAddress.AddressValue | 0x168, AccessWidth16, (UINT32)~BIT10, BIT10, pConfig);
  // Enables strict TM4 detection
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_HTIU_INDEX, NB_HTIU_REG15, AccessWidth32, (UINT32)~BIT22, BIT22, pConfig);
  } else {
  //Set Link Tristate
    if (pHtConfig->HtLinkTriState < HtLinkTriStateSkip) {
      if (pHtConfig->HtLinkTriState == HtLinkTriStateSameAsCpu) {
        LibNbPciRead (LinkPciAddress.AddressValue | (HT_PATH_LINK_ID (pConfig->NbHtPath) * 32 + 0x84), AccessWidth16, &Value, pConfig);
        if ((Value & BIT13) != 0) {
          pHtConfig->HtLinkTriState = HtLinkTriStateCadCtl;
          LibNbPciRead (
            CpuPciAddress.AddressValue | (HT_PATH_LINK_ID (pConfig->NbHtPath) * 4 + HT_PATH_SUBLINK_ID (pConfig->NbHtPath) * 0x10 + 0x170),
            AccessWidth16,
            &Value,
            pConfig
            );
          if ((Value & BIT8) != 0) {
            pHtConfig->HtLinkTriState = HtLinkTriStateCadCtlClk;
          }
        }
      }
      if (pHtConfig->HtLinkTriState >= HtLinkTriStateCadCtl) {
        UINT16 TriStateValue;
        TriStateValue = 0;
        LibNbPciRMW (pConfig->NbPciAddress.AddressValue | NB_PCI_REGC8, AccessWidth16 , (UINT32)~BIT13, BIT13, pConfig);
        LibNbPciRMW (LinkPciAddress.AddressValue | (HT_PATH_LINK_ID (pConfig->NbHtPath) * 32 + 0x84), AccessWidth16, (UINT32)~(BIT13), BIT13, pConfig);
        if (pHtConfig->HtLinkTriState == HtLinkTriStateCadCtlClk) {
          TriStateValue = BIT8;
        }
        LibNbPciRMW (pConfig->NbPciAddress.AddressValue | NB_PCI_REGAC, AccessWidth16, (UINT32)~BIT8, TriStateValue, pConfig);
        if (LibNbGetCpuFamily () != 0x0) {
          LibNbPciRMW (
            CpuPciAddress.AddressValue | (HT_PATH_LINK_ID (pConfig->NbHtPath) * 4 + HT_PATH_SUBLINK_ID (pConfig->NbHtPath) * 0x10 + 0x170),
            AccessWidth16,
            (UINT32)~(BIT8),
            TriStateValue,
            pConfig
            );
        }
      }
    }
    LibNbPciRMW (
      pConfig->NbPciAddress.AddressValue | NB_PCI_REGA0,
      AccessWidth8,
      0x3f,
      InactiveLaneStateSameAsPhyOff << 6,
      pConfig
      );
  }
  //Enable 64bit address mode

  if ((pHtConfig->HtExtendedAddressSupport & 0x80) == 0) {
    LibNbPciRead (LinkPciAddress.AddressValue | (HT_PATH_LINK_ID (pConfig->NbHtPath) * 32 + 0x85), AccessWidth8, &Value, pConfig);
    if (pHtConfig->HtExtendedAddressSupport == HtExtAddressingSameAsCpu) {
      Value &= BIT7;
    } else {
      Value = (pHtConfig->HtExtendedAddressSupport == HtExtAddressingEnable)? BIT7 : 0;
    }
    LibNbPciRMW (((pConfig->NbPciAddress.AddressValue) | (NB_PCI_REGC8 + 1)), AccessWidth8, 0x7C, Value, pConfig);
    LibNbPciRMW (LinkPciAddress.AddressValue | (HT_PATH_LINK_ID (pConfig->NbHtPath) * 32 + 0x85), AccessWidth8, 0x7F, Value, pConfig);
  }

  // Check if IFCM enabled in CPU
  LibNbPciRead (LinkPciAddress.AddressValue | (HT_PATH_LINK_ID (pConfig->NbHtPath) * 32 + 0x85), AccessWidth8, &Value, pConfig);
  IsIfcmEnabled = (Value & BIT4) ? TRUE:FALSE;
  if (IsIfcmEnabled) {
    // Enable Isoc in chipset
    LibNbPciRMW (((pConfig->NbPciAddress.AddressValue) | (NB_PCI_REGC8 + 1)), AccessWidth8, 0xFC, BIT4, pConfig);
  }

  if (pHtConfig->LinkBufferOptimization == ON) {
    BOOLEAN IsConnectedToSublink;
    IsConnectedToSublink = (pConfig->NbHtPath.LinkID & 0xF0) > 0 ? TRUE:FALSE;
    if (IsConnectedToSublink) {
      Value = IsIfcmEnabled ? SUBLINK_BUFFERS_IFCM : SUBLINK_BUFFERS_NFCM;
    } else {
      Value = IsIfcmEnabled ? LINK_BUFFERS_IFCM : LINK_BUFFERS_NFCM;
    }
    LibNbPciRMW (
      LinkPciAddress.AddressValue | (HT_PATH_LINK_ID (pConfig->NbHtPath) * 32 + 0x90),
      AccessWidth32,
      0x0,
      Value,
      pConfig
      );
  }
  NbInitRasParityMacro (pConfig);
  LibNbDisableClkConfig (pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBHT_TRACE), "[NBHT]HtLibEarlyInit Exit [0x%x]\n", Status));
  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set NB transmitter deemphasis level.
 *
 *
 * @param[in] NbDeemphasisLevel  NB Deemphasis level See HT_CONFIG::NbTransmitterDeemphasis
 * @param[in] pConfig           Northbridge configuration structure pointer.
 *
 */
VOID
LibHtSetNbTransmitterDeemphasis (
  IN      UINT8         NbDeemphasisLevel,
  IN      AMD_NB_CONFIG *pConfig
  )
{
  LibNbPciRMW (pConfig->NbPciAddress.AddressValue | NB_PCI_REGA7, AccessWidth8, (UINT32)~0x07, NbDeemphasisLevel + BIT7, pConfig);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Enable LSx state
 *
 *
 *
 * @param[in] LSx     LS State to enable. See HT_CONFIG::LSx
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
VOID
LibHtEnableLxState (
  IN      UINT8         LSx,
  IN      AMD_NB_CONFIG *pConfig
  )
{
  UINT32  Value;
  UINT32  NbLSx;
  UINT32  CpuLSx;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBHT_TRACE), "    Enable HT LS%d\n", LSx));
  switch (LSx) {
  case 0:
    Value = LS0;
    break;
  case 1:
    Value = LS1;
    break;
  case 2:
    Value = LS2;
    break;
  case 3:
    Value = LS3;
    break;
  default:
    Value = 0;
    CIMX_ASSERT (FALSE);
    return;
  }
  NbLSx = (Value << 7);
  CpuLSx = (LSx >= 2)?BIT8:0;
  LibNbPciRMW (pConfig->NbPciAddress.AddressValue | NB_PCI_REGAC, AccessWidth16, (UINT32)~(BIT7 + BIT8), NbLSx, pConfig);
  LibNbPciRMW (
    MAKE_SBDFO (0, 0, pConfig->NbHtPath.NodeID + 0x18, 0, HT_PATH_LINK_ID (pConfig->NbHtPath) * 4 + HT_PATH_SUBLINK_ID (pConfig->NbHtPath) * 0x10 + 0x170),
    AccessWidth32,
    (UINT32)~BIT8,
    CpuLSx,
    pConfig
    );
}


/*----------------------------------------------------------------------------------------*/
/**
 * NB validate HY Input parameters
 *
 *
 *
 * @param[in] pConfig           Northbridge configuration structure pointer.
 *
 */

AGESA_STATUS
HtLibInitValidateInput (
  IN OUT   AMD_NB_CONFIG *pConfig
  )
{
  AGESA_STATUS  Status;
  HT_CONFIG     *pHtConfig;
  NB_INFO       NbInfo;

  Status = AGESA_SUCCESS;
  pHtConfig = GET_HT_CONFIG_PTR (pConfig);
  NbInfo = LibNbGetRevisionInfo (pConfig);
  if (pHtConfig == NULL || NbInfo.Type == NB_UNKNOWN) {
    return  AGESA_FATAL;
  }
  if (pHtConfig->sHeader.InitializerID != INITIALIZED_BY_INITIALIZER) {
    Status = HtLibInitializer (pConfig);
  }
  return  Status;
}

UINT8 SmuWaBasePeriod[] = {
  0x1F,   //HT 200Mhz
  0x00,
  0x1F,   //HT 400Mhz
  0x00,
  0x17,   //HT 600Mhz
  0x1F,   //HT 800Mhz
  0x27,   //HT 1000Mhz
  0x2E,   //HT 1200Mhz
  0x36,   //HT 1400Mhz
  0x3E,   //HT 1600Mhz
  0x46,   //HT 1800Mhz
  0x4E,   //HT 2000Mhz
  0x55,   //HT 2200Mhz
  0x5D,   //HT 2400Mhz
  0x65    //HT 2600Mhz
};

UINT8 SmuWaBaseDelay[] = {
  0x3,   //HT 200Mhz
  0x0,
  0x3,   //HT 400Mhz
  0x0,
  0x2,   //HT 600Mhz
  0x3,   //HT 800Mhz
  0x3,   //HT 1000Mhz
  0x4,   //HT 1200Mhz
  0x5,   //HT 1400Mhz
  0x6,   //HT 1600Mhz
  0x7,   //HT 1800Mhz
  0x7,   //HT 2000Mhz
  0x8,   //HT 2200Mhz
  0x9,   //HT 2400Mhz
  0xA    //HT 2600Mhz
};

UINT8 SmuWaPeriod10us[] = {
  120,
  100,
  90,
  80
};

UINT8 SmuWaDelay1us[] = {
  0x0,   //HT 200Mhz
  0x0,
  0x0,   //HT 400Mhz
  0x0,
  0x0,   //HT 600Mhz
  0x0,   //HT 800Mhz
  0x0,   //HT 1000Mhz
  0x0,   //HT 1200Mhz
  0x2,   //HT 1400Mhz
  0x2,   //HT 1600Mhz
  0x2,   //HT 1800Mhz
  0x3,   //HT 2000Mhz
  0x3,   //HT 2200Mhz
  0x4,   //HT 2400Mhz
  0x4    //HT 2600Mhz
};

/*----------------------------------------------------------------------------------------*/
/**
 * Get SMU wa data
 *
 *
 *
 * @param[in] pConfig           Northbridge configuration structure pointer.
 * @retval                      SMU wa data
 */

UINT32
LibHtGetSmuWaData (
  IN       AMD_NB_CONFIG *pConfig
  )
{
  UINT8                 NorthbridgeId;
  UINT8                 NorthbridgeIndex;
  UINT8                 NbHtSpeed;
  UINT16                SmuWaPeriod;
  UINT16                SmuWaDelay;
  AMD_NB_CONFIG_BLOCK   *ConfigPtr;
  NorthbridgeIndex = 0;
  ConfigPtr = GET_BLOCK_CONFIG_PTR (pConfig);
  for (NorthbridgeId = 0; NorthbridgeId <= ConfigPtr->NumberOfNorthbridges; NorthbridgeId++) {
    AMD_NB_CONFIG *NbConfigPtr = &ConfigPtr->Northbridges[NorthbridgeId];
    if (LibNbIsDevicePresent (NbConfigPtr->NbPciAddress, NbConfigPtr)) {
      if (pConfig == NbConfigPtr) {
        LibNbPciRead (pConfig->NbPciAddress.AddressValue | NB_PCI_REGD1 , AccessWidth8, &NbHtSpeed, pConfig);
        NbHtSpeed &= 0xf;
        SmuWaPeriod = SmuWaPeriod10us [NorthbridgeIndex] * SmuWaBasePeriod [NbHtSpeed];
        SmuWaDelay = SmuWaDelay1us [NbHtSpeed] * SmuWaBaseDelay [NbHtSpeed];
        return ((SmuWaPeriod & 0xFF) << 8) | ((SmuWaPeriod & 0xFF00) >> 8) | ((SmuWaDelay & 0xFF) << 24) | ((SmuWaDelay & 0xFF00) << 8);
      }
      NorthbridgeIndex++;
    }
  }
  return 0;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Init RAS macro
 *
 *
 *
 *
 *  @param[in] NbConfigPtr  Northbridge configuration structure pointer.
 */
VOID
NbInitRasParityMacro (
  IN      AMD_NB_CONFIG *NbConfigPtr
  )
{
  PCI_ADDR    CpuPciAddress;
  UINT32      SaveBase;
  UINT32      SaveLimit;
  UINT32      Value;
  UINT32      Base;
  UINT32      Limit;
  UINT32      Index;
  UINT64      SaveTom;
  UINT64      Value64;
  DUMMY_CALL  *RetAddr;
  UINT8       Node;
  CpuPciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1,  0);
//Set TOM
  LibAmdMsrRead (0xC001001a, &SaveTom, (AMD_CONFIG_PARAMS *)NbConfigPtr);
  Value64 = 0x40000000;
  LibAmdMsrWrite (0xC001001a, &Value64, (AMD_CONFIG_PARAMS *)NbConfigPtr);
//Set mmio
  LibNbPciRead (CpuPciAddress.AddressValue | 0x80, AccessWidth32, &SaveBase, NbConfigPtr);
  LibNbPciRead (CpuPciAddress.AddressValue | 0x84, AccessWidth32, &SaveLimit, NbConfigPtr);
  Limit = ((0x50000000 - 1) >> 8) & (~ 0xFF);
  Limit |= NbConfigPtr->NbHtPath.NodeID | (HT_PATH_LINK_ID (NbConfigPtr->NbHtPath) << 4) | (HT_PATH_SUBLINK_ID (NbConfigPtr->NbHtPath) << 6);
  Base = ((0x40000000 >> 8) & (~ 0xFF)) | 0x3;
  for (Node = 0; Node < 8; Node++) {
    CpuPciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18 + Node, 1,  0);
    if (LibNbIsDevicePresent (CpuPciAddress, NbConfigPtr)) {
      LibNbPciWrite (CpuPciAddress.AddressValue | 0x84, AccessWidth32, &Limit, NbConfigPtr);
      LibNbPciWrite (CpuPciAddress.AddressValue | 0x80, AccessWidth32, &Base, NbConfigPtr);
    } else {
      break;
    }
  }
//set Scan
  LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_HTIU_INDEX, NB_HTIU_REG06, AccessWidth32, (UINT32)~BIT27, BIT27, NbConfigPtr);
  RetAddr = (DUMMY_CALL* ) (UINTN) 0x40000000;
  *((UINT8*) (UINTN) RetAddr) = 0xC3;
  for (Index = 0; Index < 64; Index++) {
    RetAddr ();
    RetAddr = (DUMMY_CALL*) (UINTN) ((UINT8*) (UINTN) RetAddr + 64);
  }
//Reset scan
  LibNbPciIndexRMW (NbConfigPtr->NbPciAddress.AddressValue | NB_HTIU_INDEX, NB_HTIU_REG06, AccessWidth32, (UINT32)~BIT27, 0x0, NbConfigPtr);
  Value = 0;
// Restore MMIO Map
  for (Node = 0; Node < 8; Node++) {
    CpuPciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18 + Node, 1,  0);
    if (LibNbIsDevicePresent (CpuPciAddress, NbConfigPtr)) {
      LibNbPciWrite (CpuPciAddress.AddressValue | 0x80, AccessWidth32, &Value, NbConfigPtr);
      LibNbPciWrite (CpuPciAddress.AddressValue | 0x84, AccessWidth32, &SaveLimit, NbConfigPtr);
      LibNbPciWrite (CpuPciAddress.AddressValue | 0x80, AccessWidth32, &SaveBase, NbConfigPtr);
    } else {
      break;
    }
  }
// Restore TOM
  LibAmdMsrWrite (0xC001001a, &SaveTom, (AMD_CONFIG_PARAMS *)NbConfigPtr);
}

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
AmdHtInitializer (
  IN OUT   AMD_NB_CONFIG_BLOCK *ConfigPtr
  )
{
  AGESA_STATUS  Status;
  Status = LibNbApiCall (HtLibInitializer, ConfigPtr);
  return Status;
}
/*----------------------------------------------------------------------------------------*/
/**
 * HT config structure initializer
 *
 *
 *
 * @param[in] pConfig           Northbridge configuration structure pointer.
 *
 */

AGESA_STATUS
HtLibInitializer (
  IN OUT   AMD_NB_CONFIG *pConfig
  )
{
  HT_CONFIG *pHtConfig;

  pHtConfig = GET_HT_CONFIG_PTR (pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBHT_TRACE), "[NBHT]HtLibInitializer Enter\n"));
  if (pHtConfig == NULL) {
    return  AGESA_WARNING;
  }
  if (pHtConfig->sHeader.InitializerID == INITIALIZED_BY_INITIALIZER) {
    return  AGESA_SUCCESS;
  }
  LibAmdMemFill (pHtConfig, 0, sizeof (HT_CONFIG), (AMD_CONFIG_PARAMS *)&(pHtConfig->sHeader));
  pHtConfig->sHeader.InitializerID = INITIALIZED_BY_INITIALIZER;
  pHtConfig->HtExtendedAddressSupport = HtExtAddressingSameAsCpu;
  pHtConfig->HtLinkTriState = HtLinkTriStateSameAsCpu;
  pHtConfig->HtReferenceClock = 200;
// Select LS State
  pHtConfig->LSx = HtLinkStateSameAsCpu;
  pHtConfig->LinkBufferOptimization = OFF;
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBHT_TRACE), "[NBHT]HtLibInitializer Exit\n"));
  return AGESA_SUCCESS;
}
