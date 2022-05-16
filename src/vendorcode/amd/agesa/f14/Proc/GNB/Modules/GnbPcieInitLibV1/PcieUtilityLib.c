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
 * @e \$Revision: 38931 $   @e \$Date: 2010-10-01 15:50:05 -0700 (Fri, 01 Oct 2010) $
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Ids.h"
#include  "amdlib.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  GNB_MODULE_DEFINITIONS (GnbPcieConfig)
#include  GNB_MODULE_DEFINITIONS (GnbPcieInitLibV1)
#include  "GnbRegistersON.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEINITLIBV1_PCIEUTILITYLIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


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
                            DxF0xE4_xA5_ADDRESS + Index,
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
    DxF0xE4_x50_STRUCT  DxF0xE4_x50;
    DxF0xE4_x50.Value = PciePortRegisterRead (
                          Engine,
                          DxF0xE4_x50_ADDRESS,
                          Pcie
                          );
    LinkReversal ^= DxF0xE4_x50.Field.PortLaneReversal;
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
  DxF0xE4_xA2_STRUCT  DxF0xE4_xA2;
  DxF0xE4_xA2.Value = PciePortRegisterRead (
                        Engine,
                        DxF0xE4_xA2_ADDRESS,
                        Pcie
                        );
  switch (DxF0xE4_xA2.Field.LcLinkWidthRd) {
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
 * Get  bitmap of engine lane of requested type
 *
 *
 * @param[in]   IncludeLaneType     Include Lane type
 * @param[in]   ExcludeLaneType     Exclude Lane type
 * @param[in]   Engine              Pointer to engine config descriptor
 * @param[in]   Pcie                Pointer to global PCIe configuration
 * @retval                          Lane bitmap
 */

UINT32
PcieUtilGetEngineLaneBitMap (
  IN      UINT32                         IncludeLaneType,
  IN      UINT32                         ExcludeLaneType,
  IN      PCIe_ENGINE_CONFIG             *Engine,
  IN      PCIe_PLATFORM_CONFIG           *Pcie
  )
{
  UINT32  LaneBitmap;
  UINT16  LaneOffset;
  LaneBitmap = 0;
  if ((IncludeLaneType & LANE_TYPE_PCIE_LANES) && Engine->EngineData.EngineType == PciePortEngine) {
    if (IncludeLaneType & LANE_TYPE_PCIE_ALL) {
      LaneBitmap |= (((1 << PcieConfigGetNumberOfCoreLane (Engine)) - 1) << Engine->Type.Port.StartCoreLane);
    }
    if (PcieLibIsEngineAllocated (Engine)) {
      if (IncludeLaneType & LANE_TYPE_PCIE_ALLOCATED) {
        LaneBitmap |= (((1 << PcieConfigGetNumberOfPhyLane (Engine)) - 1) << Engine->Type.Port.StartCoreLane);
      }
      if (IncludeLaneType & LANE_TYPE_PCIE_ACTIVE) {
        if (Engine->Type.Port.PortData.LinkHotplug == HotplugEnhanced) {
          LaneBitmap |= PcieUtilGetEngineLaneBitMap (LANE_TYPE_PCIE_ALLOCATED, 0, Engine, Pcie);
        } else if (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_PORT_IN_COMPLIANCE)) {
          LaneBitmap |= PcieUtilGetEngineLaneBitMap (LANE_TYPE_PCIE_ALLOCATED, 0, Engine, Pcie);
        } else if (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS)) {
          UINT8     LinkWidth;
          BOOLEAN   LinkReversed;
          LinkWidth = PcieUtilGetLinkWidth (Engine, Pcie);
          if (LinkWidth > PcieConfigGetNumberOfPhyLane (Engine)) {
            LinkWidth = PcieConfigGetNumberOfPhyLane (Engine);
          }
          LinkReversed = PcieUtilIsLinkReversed (TRUE, Engine, Pcie);
          LaneOffset = LinkReversed ? (Engine->Type.Port.EndCoreLane - LinkWidth + 1) : Engine->Type.Port.StartCoreLane;
          LaneBitmap |= (((1 << LinkWidth) - 1) << LaneOffset);
        }
      }
      if (IncludeLaneType & LANE_TYPE_PCIE_SB) {
        if (Engine->Type.Port.IsSB) {
          LaneBitmap |= PcieUtilGetEngineLaneBitMap (LANE_TYPE_PCIE_ACTIVE, 0, Engine, Pcie);
          IDS_HDT_CONSOLE (GNB_TRACE, "SB Lane Bitmap is 0x%x\n", LaneBitmap);
        }
      }
      if (IncludeLaneType & LANE_TYPE_PCIE_HOTPLUG) {
        if (Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled) {
          LaneBitmap |= PcieUtilGetEngineLaneBitMap (LANE_TYPE_PCIE_ALLOCATED, 0, Engine, Pcie);
        }
      }
    }
  }
  if ((IncludeLaneType & LANE_TYPE_DDI_LANES) && Engine->EngineData.EngineType == PcieDdiEngine) {
    if (PcieLibIsEngineAllocated (Engine)) {
      if (IncludeLaneType & (LANE_TYPE_DDI_ALLOCATED | LANE_TYPE_DDI_ALL)) {
        LaneOffset = PcieLibGetLoPhyLane (Engine) - PcieEngineGetParentWrapper (Engine)->StartPhyLane;
        LaneBitmap |= ((1 << PcieConfigGetNumberOfPhyLane (Engine)) - 1) << LaneOffset;
      }
      if (IncludeLaneType & LANE_TYPE_DDI_ACTIVE) {
        if (Engine->InitStatus & INIT_STATUS_DDI_ACTIVE) {
          LaneBitmap |= PcieUtilGetEngineLaneBitMap (LANE_TYPE_DDI_ALL, 0, Engine, Pcie);
        }
      }
    }
  }
  if (ExcludeLaneType != 0) {
    LaneBitmap &= (~PcieUtilGetEngineLaneBitMap (ExcludeLaneType, 0, Engine, Pcie));
  }
  return LaneBitmap;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get bitmap of Wrapper lane of requested type
 *
 *
 * @param[in]  IncludeLaneType Include Lane type
 * @param[in]  ExcludeLaneType Exclude Lane type
 * @param[in]  Wrapper   Pointer to wrapper config descriptor
 * @param[in]  Pcie      Pointer to PCIe config descriptor
 * @retval               Lane bitmap
 */

UINT32
PcieUtilGetWrapperLaneBitMap (
  IN      UINT32                         IncludeLaneType,
  IN      UINT32                         ExcludeLaneType,
  IN      PCIe_WRAPPER_CONFIG            *Wrapper,
  IN      PCIe_PLATFORM_CONFIG           *Pcie
  )
{
  PCIe_ENGINE_CONFIG        *EngineList;
  UINT32                    LaneBitmap;
  EngineList = PcieWrapperGetEngineList (Wrapper);
  LaneBitmap = 0;
  if ((IncludeLaneType | ExcludeLaneType) != 0) {
    if ((IncludeLaneType & LANE_TYPE_ALL) == LANE_TYPE_ALL) {
      LaneBitmap = (1 << (Wrapper->EndPhyLane - Wrapper->StartPhyLane + 1)) - 1;
      if (ExcludeLaneType != 0) {
        LaneBitmap &= (~PcieUtilGetWrapperLaneBitMap (ExcludeLaneType, 0, Wrapper, Pcie));
      }
    } else {
      while (EngineList != NULL) {
        LaneBitmap |= PcieUtilGetEngineLaneBitMap (IncludeLaneType, ExcludeLaneType, EngineList, Pcie);
        EngineList = PcieLibGetNextDescriptor (EngineList);
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
        CORE_SPACE (CoreId, 0x10),
        0,
        1,
        0x1,
        TRUE,
        Pcie
        );
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieLockRegisters Exit\n");
}
