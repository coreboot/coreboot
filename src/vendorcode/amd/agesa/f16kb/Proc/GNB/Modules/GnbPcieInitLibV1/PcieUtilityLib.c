/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe utility. Various supporting functions.
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
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbRegistersCommon.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEINITLIBV1_PCIEUTILITYLIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
/// Lane type
typedef enum {
  LaneTypeCore,         ///< Core Lane
  LaneTypePhy,          ///< Package Phy Lane
  LaneTypeNativePhy     ///< Native Phy Lane
} LANE_TYPE;

/// Lane Property
typedef enum {
  LanePropertyConfig,   ///< Configuration
  LanePropertyActive,   ///< Active
  LanePropertyAllocated ///< Allocated
} LANE_PROPERTY;


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
typedef struct {
  UINT32               Flags;
  PCIE_LINK_SPEED_CAP  LinkSpeedCapability;
} PCIE_GLOBAL_GEN_CAP_WORKSPACE;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Get link state history from HW state machine
 *
 *
 *
 * @param[in]   Engine              Pointer to engine config descriptor
 * @param[out]  History             Buffer to save history
 * @param[in]   Length              Buffer length
 * @param[in]   Pcie                Pointer to global PCIe configuration
 */

VOID
PcieUtilGetLinkHwStateHistory (
  IN       PCIe_ENGINE_CONFIG        *Engine,
     OUT   UINT8                     *History,
  IN       UINT8                     Length,
  IN       PCIe_PLATFORM_CONFIG      *Pcie
  )
{
  UINT8   ReadLength;
  UINT32  LocalHistory [6];
  UINT16  Index;
  ASSERT (Length <= 16);
  ASSERT (Length > 0);
  if (Length > 6*4) {
    Length = 6*4;
  }
  ReadLength = (Length + 3) / 4;
  for (Index = 0; Index < ReadLength; Index++) {
    LocalHistory[Index] = PciePortRegisterRead (
                            Engine,
                            0xa5 + Index,
                            Pcie
                            );
  }
  LibAmdMemCopy (History, LocalHistory, Length, GnbLibGetHeader (Pcie));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Search array for specific pattern
 *
 *
 * @param[in]  Buf1           Pointer to source buffer which will be subject of search
 * @param[in]  Buf1Length     Length of the source buffer
 * @param[in]  Buf2           Pointer to pattern buffer
 * @param[in]  Buf2Length     Length of the pattern buffer
 * @retval     TRUE           Pattern found
 * @retval     TRUE           Pattern not found
 */

BOOLEAN
PcieUtilSearchArray (
  IN      UINT8                         *Buf1,
  IN      UINTN                         Buf1Length,
  CONST IN      UINT8                         *Buf2,
  IN      UINTN                         Buf2Length
  )
{
  UINT8 *CurrentBuf1Ptr;
  CurrentBuf1Ptr = Buf1;
  while (CurrentBuf1Ptr < (Buf1 + Buf1Length - Buf2Length)) {
    UINT8 *SourceBufPtr;
    CONST UINT8 *PatternBufPtr;
    UINTN PatternBufLength;
    SourceBufPtr = CurrentBuf1Ptr;
    PatternBufPtr = Buf2;
    PatternBufLength = Buf2Length;
    while ((*SourceBufPtr++ == *PatternBufPtr++) && (PatternBufLength-- != 0));
    if (PatternBufLength == 0) {
      return TRUE;
    }
    CurrentBuf1Ptr++;
  }
  return FALSE;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Check if link reversed
 *
 *
 * @param[in]  HwLinkState  Check for HW auto link reversal
 * @param[in]  Engine       Pointer to engine config descriptor
 * @param[in]  Pcie         Pointer to PCIe config descriptor
 * @retval                 TRUE if link reversed
 */
BOOLEAN
PcieUtilIsLinkReversed (
  IN      BOOLEAN               HwLinkState,
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32 LinkReversal;

  LinkReversal  = (Engine->EngineData.StartLane > Engine->EngineData.EndLane) ? 1 : 0;
  if (HwLinkState)  {
    UINT32 Value;
    Value = PciePortRegisterRead (
                          Engine,
                          0x50,
                          Pcie
                          );
    LinkReversal ^= (Value & 1);
  }
  return ((LinkReversal & BIT0) != 0) ? TRUE : FALSE;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get link width detected during training
 *
 *
 *
 * @param[in]   Engine              Pointer to engine config descriptor
 * @param[in]   Pcie                Pointer to global PCIe configuration
 * @retval                          Link width
 */
UINT8
PcieUtilGetLinkWidth (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8               LinkWidth;
  UINT32              Value;
  Value = PciePortRegisterRead (
                        Engine,
                        0xA2,
                        Pcie
                        );
  switch ((Value & 7) >> 4) {
  case 0x6:
    LinkWidth = 16;
    break;
  case 0x5:
    LinkWidth = 12;
    break;
  case 0x4:
    LinkWidth = 8;
    break;
  case 0x3:
    LinkWidth = 4;
    break;
  case 0x2:
    LinkWidth = 2;
    break;
  case 0x1:
    LinkWidth = 1;
    break;
  default:
    LinkWidth = 0;
  }
  return LinkWidth;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get bitmap of PCIE engine lane of requested type
 *
 *
 * @param[in]   LaneType            Lane type
 * @param[in]   LaneProperty        Lane Property
 * @param[in]   Engine              Pointer to engine config descriptor
 * @retval                          Lane bitmap
 */

STATIC UINT32
PcieUtilGetPcieEngineLaneBitMap (
  IN      LANE_TYPE                      LaneType,
  IN      LANE_PROPERTY                  LaneProperty,
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  UINT32                LaneBitmap;
  UINT8                 Width;
  UINT16                Offset;
  UINT16                LoPhylane;
  UINT16                HiPhylane;
  PCIe_PLATFORM_CONFIG  *Pcie;

  Width = 0;
  Offset = 0;
  LaneBitmap = 0;
  Pcie = (PCIe_PLATFORM_CONFIG *) PcieConfigGetParent (DESCRIPTOR_PLATFORM, &Engine->Header);

  if (PcieConfigIsPcieEngine (Engine)) {
    if (LaneType == LaneTypeCore && LaneProperty == LanePropertyConfig) {
      Width = PcieConfigGetNumberOfCoreLane (Engine);
      Offset = Engine->Type.Port.StartCoreLane;
      LaneBitmap = ((1 << Width) - 1) << Offset;
    } else if (PcieConfigIsEngineAllocated (Engine)) {
      if (LaneType == LaneTypeNativePhy) {
        LaneBitmap = PcieUtilGetPcieEngineLaneBitMap (LaneTypePhy, LaneProperty, Engine);
        LaneBitmap = PcieFmGetNativePhyLaneBitmap (LaneBitmap, Engine);
      } else {
        if (LaneType == LaneTypeCore) {
          if (LaneProperty == LanePropertyActive) {
            Width = PcieUtilGetLinkWidth (Engine, Pcie);
            Offset = PcieUtilIsLinkReversed (TRUE, Engine, Pcie) ? (Engine->Type.Port.EndCoreLane - Width + 1) : Engine->Type.Port.StartCoreLane;
          } else if (LaneProperty == LanePropertyAllocated) {
            Width = PcieConfigGetNumberOfPhyLane (Engine);
            Offset = PcieUtilIsLinkReversed (FALSE, Engine, Pcie) ? (Engine->Type.Port.EndCoreLane - Width + 1) : Engine->Type.Port.StartCoreLane;
          }
        }
        if (LaneType == LaneTypePhy) {
          LoPhylane = PcieLibGetLoPhyLane (Engine);
          HiPhylane = PcieLibGetHiPhyLane (Engine);
          if (LaneProperty == LanePropertyActive) {
            Width = PcieUtilGetLinkWidth (Engine, Pcie);
            Offset = (PcieUtilIsLinkReversed (TRUE, Engine, Pcie) ? (HiPhylane - Width + 1) : LoPhylane) - PcieConfigGetParentWrapper (Engine)->StartPhyLane;
          } else if (LaneProperty == LanePropertyAllocated) {
            Width = PcieConfigGetNumberOfPhyLane (Engine);
            Offset = LoPhylane - PcieConfigGetParentWrapper (Engine)->StartPhyLane;
          }
        }
        LaneBitmap = ((1 << Width) - 1) << Offset;
      }
    }
  }
  return LaneBitmap;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get bitmap of PCIE engine lane of requested type
 *
 *
 * @param[in]   LaneType            Lane type
 * @param[in]   LaneProperty        Lane Property
 * @param[in]   Engine              Pointer to engine config descriptor
 * @retval                          Lane bitmap
 */

STATIC UINT32
PcieUtilGetDdiEngineLaneBitMap (
  IN      LANE_TYPE                      LaneType,
  IN      LANE_PROPERTY                  LaneProperty,
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  UINT32  LaneBitmap;
  UINT8   Width;
  UINT16  Offset;
  Width = 0;
  Offset = 0;
  LaneBitmap = 0;
  if (PcieConfigIsDdiEngine (Engine)) {
    if (PcieConfigIsEngineAllocated (Engine)) {
      if (LaneType == LaneTypePhy && ((LaneProperty == LanePropertyActive && (Engine->InitStatus & INIT_STATUS_DDI_ACTIVE)) || (LaneProperty == LanePropertyAllocated))) {
        Width = PcieConfigGetNumberOfPhyLane (Engine);
        Offset = PcieLibGetLoPhyLane (Engine) - PcieConfigGetParentWrapper (Engine)->StartPhyLane;
        LaneBitmap = ((1 << Width) - 1) << Offset;
      }
      if (LaneType == LaneTypeNativePhy) {
        LaneBitmap = PcieUtilGetDdiEngineLaneBitMap (LaneTypePhy, LaneProperty, Engine);
        LaneBitmap = PcieFmGetNativePhyLaneBitmap (LaneBitmap, Engine);
      }
    }
  }
  return LaneBitmap;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get  bitmap of engine lane of requested type
 *
 *
 * @param[in]   IncludeLaneType     Include Lane type
 * @param[in]   ExcludeLaneType     Exclude Lane type
 * @param[in]   Engine              Pointer to engine config descriptor
 * @retval                          Lane bitmap
 */

UINT32
PcieUtilGetEngineLaneBitMap (
  IN      UINT32                         IncludeLaneType,
  IN      UINT32                         ExcludeLaneType,
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  UINT32  LaneBitmap;
  LaneBitmap = 0;
  if (IncludeLaneType & LANE_TYPE_PCIE_LANES) {
    if (IncludeLaneType & LANE_TYPE_PCIE_CORE_CONFIG) {
      LaneBitmap |= PcieUtilGetPcieEngineLaneBitMap (LaneTypeCore, LanePropertyConfig, Engine);
    }
    if (IncludeLaneType & LANE_TYPE_PCIE_CORE_ALLOC) {
      LaneBitmap |= PcieUtilGetPcieEngineLaneBitMap (LaneTypeCore, LanePropertyAllocated, Engine);
    }
    if (IncludeLaneType & (LANE_TYPE_PCIE_CORE_ACTIVE | LANE_TYPE_PCIE_CORE_ALLOC_ACTIVE)) {
      if (Engine->Type.Port.PortData.LinkHotplug == HotplugEnhanced || PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_PORT_IN_COMPLIANCE)) {
        LaneBitmap |= PcieUtilGetPcieEngineLaneBitMap (LaneTypeCore, LanePropertyAllocated, Engine);
      } else if (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS)) {
        if (IncludeLaneType & LANE_TYPE_PCIE_CORE_ALLOC_ACTIVE) {
          LaneBitmap |= PcieUtilGetPcieEngineLaneBitMap (LaneTypeCore, LanePropertyAllocated, Engine);
        } else {
          LaneBitmap |= PcieUtilGetPcieEngineLaneBitMap (LaneTypeCore, LanePropertyActive, Engine);
        }
      }
    }
    if ((IncludeLaneType & LANE_TYPE_PCIE_SB_CORE_CONFIG) && PcieConfigIsSbPcieEngine (Engine)) {
      LaneBitmap |= PcieUtilGetPcieEngineLaneBitMap (LaneTypeCore, LanePropertyConfig, Engine);
    }
    if ((IncludeLaneType & LANE_TYPE_PCIE_CORE_HOTPLUG) && (Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled)) {
      LaneBitmap |= PcieUtilGetPcieEngineLaneBitMap (LaneTypeCore, LanePropertyAllocated, Engine);
    }
    if (IncludeLaneType & LANE_TYPE_PCIE_PHY) {
      LaneBitmap |= PcieUtilGetPcieEngineLaneBitMap (LaneTypePhy, LanePropertyAllocated, Engine);
    }
    if (IncludeLaneType & LANE_TYPE_PCIE_PHY_NATIVE) {
      LaneBitmap |= PcieUtilGetPcieEngineLaneBitMap (LaneTypeNativePhy, LanePropertyAllocated, Engine);
    }
    if (IncludeLaneType & (LANE_TYPE_PCIE_PHY_NATIVE_ACTIVE | LANE_TYPE_PCIE_PHY_NATIVE_ALLOC_ACTIVE)) {
      if (Engine->Type.Port.PortData.LinkHotplug == HotplugEnhanced || PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_PORT_IN_COMPLIANCE)) {
        LaneBitmap |= PcieUtilGetPcieEngineLaneBitMap (LaneTypeNativePhy, LanePropertyAllocated, Engine);
      } else if (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS)) {
        if (IncludeLaneType & LANE_TYPE_PCIE_PHY_NATIVE_ALLOC_ACTIVE) {
          LaneBitmap |= PcieUtilGetPcieEngineLaneBitMap (LaneTypeNativePhy, LanePropertyAllocated, Engine);
        } else {
          LaneBitmap |= PcieUtilGetPcieEngineLaneBitMap (LaneTypeNativePhy, LanePropertyActive, Engine);
        }
      }
    }
    if ((IncludeLaneType & LANE_TYPE_PCIE_PHY_NATIVE_HOTPLUG) && (Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled)) {
      LaneBitmap |= PcieUtilGetPcieEngineLaneBitMap (LaneTypeNativePhy, LanePropertyAllocated, Engine);
    }
  }
  if (IncludeLaneType & LANE_TYPE_DDI_LANES) {
    if (IncludeLaneType & LANE_TYPE_DDI_PHY) {
      LaneBitmap |= PcieUtilGetDdiEngineLaneBitMap (LaneTypePhy, LanePropertyAllocated, Engine);
    }
    if (IncludeLaneType & LANE_TYPE_DDI_PHY_NATIVE) {
      LaneBitmap |= PcieUtilGetDdiEngineLaneBitMap (LaneTypeNativePhy, LanePropertyAllocated, Engine);
    }
    if (IncludeLaneType & LANE_TYPE_DDI_PHY_NATIVE_ACTIVE) {
      LaneBitmap |= PcieUtilGetDdiEngineLaneBitMap (LaneTypeNativePhy, LanePropertyActive, Engine);
    }
  }
  if (ExcludeLaneType != 0) {
    LaneBitmap &= (~PcieUtilGetEngineLaneBitMap (ExcludeLaneType, 0, Engine));
  }
  return LaneBitmap;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get bitmap of phy lane confugred for master pll
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper config descriptor
 * @retval     Lane bitmap
 */

STATIC UINT32
PcieUtilGetMasterPllLaneBitMap (
  IN      PCIe_WRAPPER_CONFIG            *Wrapper
  )
{
  if (Wrapper->MasterPll != 0) {
    return 0xf << (Wrapper->MasterPll - 0xA) * 4;
  }
  return 0;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get bitmap of Wrapper lane of requested type
 *
 *
 * @param[in]  IncludeLaneType Include Lane type
 * @param[in]  ExcludeLaneType Exclude Lane type
 * @param[in]  Wrapper         Pointer to wrapper config descriptor
 * @retval     Lane bitmap
 */

UINT32
PcieUtilGetWrapperLaneBitMap (
  IN      UINT32                         IncludeLaneType,
  IN      UINT32                         ExcludeLaneType,
  IN      PCIe_WRAPPER_CONFIG            *Wrapper
  )
{
  PCIe_ENGINE_CONFIG        *EngineList;
  UINT32                    LaneBitmap;
  EngineList = PcieConfigGetChildEngine (Wrapper);
  LaneBitmap = 0;
  if ((IncludeLaneType | ExcludeLaneType) != 0) {
    if ((IncludeLaneType & LANE_TYPE_ALL) == LANE_TYPE_ALL) {
      LaneBitmap = (1 << (Wrapper->NumberOfLanes)) - 1;
      if (ExcludeLaneType != 0) {
        LaneBitmap &= (~PcieUtilGetWrapperLaneBitMap (ExcludeLaneType, 0, Wrapper));
      }
    } else {
      while (EngineList != NULL) {
        LaneBitmap |= PcieUtilGetEngineLaneBitMap (IncludeLaneType, ExcludeLaneType, EngineList);
        EngineList = PcieLibGetNextDescriptor (EngineList);
      }
      if ((IncludeLaneType & LANE_TYPE_PCIE_PHY_NATIVE_MASTER_PLL) != 0) {
        LaneBitmap |= PcieUtilGetMasterPllLaneBitMap (Wrapper);
      }
      if ((ExcludeLaneType & LANE_TYPE_PCIE_PHY_NATIVE_MASTER_PLL) != 0) {
        LaneBitmap &= (~PcieUtilGetMasterPllLaneBitMap (Wrapper));
      }
    }
  }
  return LaneBitmap;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Program port register table
 *
 *
 *
 * @param[in]  Table           Pointer to table
 * @param[in]  Length          number of entries
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  S3Save          Save for S3 flag
 * @param[in]  Pcie            Pointer to global PCIe configuration
 *
 */

VOID
PciePortProgramRegisterTable (
  CONST IN      PCIE_PORT_REGISTER_ENTRY    *Table,
  IN      UINTN                       Length,
  IN      PCIe_ENGINE_CONFIG          *Engine,
  IN      BOOLEAN                     S3Save,
  IN      PCIe_PLATFORM_CONFIG        *Pcie
  )
{
  UINTN   Index;
  UINT32  Value;
  for (Index = 0; Index < Length; Index++) {
    Value = PciePortRegisterRead (
              Engine,
              Table[Index].Reg,
              Pcie
              );
    Value &= (~Table[Index].Mask);
    Value |= Table[Index].Data;
    PciePortRegisterWrite (
      Engine,
      Table[Index].Reg,
      Value,
      S3Save,
      Pcie
      );
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Lock registers
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */

VOID
PcieLockRegisters (
  IN       PCIe_WRAPPER_CONFIG    *Wrapper,
  IN       PCIe_PLATFORM_CONFIG   *Pcie
  )
{
  UINT8   CoreId;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieLockRegisters Enter\n");
  if (PcieLibIsPcieWrapper (Wrapper)) {
    for (CoreId = Wrapper->StartPcieCoreId; CoreId <= Wrapper->EndPcieCoreId; CoreId++) {
      PcieRegisterWriteField (
        Wrapper,
        CORE_SPACE (CoreId, D0F0xE4_CORE_0010_ADDRESS),
        D0F0xE4_CORE_0010_HwInitWrLock_OFFSET,
        D0F0xE4_CORE_0010_HwInitWrLock_WIDTH,
        0x1,
        TRUE,
        Pcie
        );
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieLockRegisters Exit\n");
}


/*----------------------------------------------------------------------------------------*/
/**
 * Training state handling
 *
 *
 *
 * @param[in]       Engine              Pointer to engine config descriptor
 * @param[in, out]  Buffer              Indicate if engine in non final state
 * @param[in]       Pcie                Pointer to global PCIe configuration
 *
 */

VOID
STATIC
PcieUtilGlobalGenCapabilityCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIE_GLOBAL_GEN_CAP_WORKSPACE *GlobalGenCapability;
  PCIE_LINK_SPEED_CAP           LinkSpeedCapability;
  PCIE_HOTPLUG_TYPE             HotPlugType;
  UINT32                        Flags;

  Flags = PCIE_GLOBAL_GEN_CAP_ALL_PORTS;
  GlobalGenCapability = (PCIE_GLOBAL_GEN_CAP_WORKSPACE*) Buffer;
  LinkSpeedCapability = PcieGen1;
  if (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS)) {
    Flags |= PCIE_GLOBAL_GEN_CAP_TRAINED_PORTS;
  }
  HotPlugType = Engine->Type.Port.PortData.LinkHotplug;
  if ((HotPlugType == HotplugBasic) || (HotPlugType == HotplugServer) || (HotPlugType == HotplugEnhanced)) {
    Flags |= PCIE_GLOBAL_GEN_CAP_HOTPLUG_PORTS;
  }
  if ((GlobalGenCapability->Flags & Flags) != 0) {
    ASSERT ((GlobalGenCapability->Flags & (PCIE_PORT_GEN_CAP_MAX | PCIE_PORT_GEN_CAP_BOOT)) != 0);
    LinkSpeedCapability = PcieFmGetLinkSpeedCap (GlobalGenCapability->Flags, Engine);
    if (GlobalGenCapability->LinkSpeedCapability < LinkSpeedCapability) {
      GlobalGenCapability->LinkSpeedCapability = LinkSpeedCapability;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Determine global GEN capability
 *
 *
 * @param[in]  Flags           global GEN capability flags
 * @param[in]  Pcie            Pointer to global PCIe configuration
 *
 */
PCIE_LINK_SPEED_CAP
PcieUtilGlobalGenCapability (
  IN      UINT32                         Flags,
  IN      PCIe_PLATFORM_CONFIG           *Pcie
  )
{
  PCIE_LINK_SPEED_CAP             GlobalCapability;
  PCIE_GLOBAL_GEN_CAP_WORKSPACE   GlobalGenCap;

  GlobalGenCap.LinkSpeedCapability = PcieGen1;
  GlobalGenCap.Flags = Flags;
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PcieUtilGlobalGenCapabilityCallback,
    &GlobalGenCap,
    Pcie
    );

  GlobalCapability = GlobalGenCap.LinkSpeedCapability;

  return GlobalCapability;
}
