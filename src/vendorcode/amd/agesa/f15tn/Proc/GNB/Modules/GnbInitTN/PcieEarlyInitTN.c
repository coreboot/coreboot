/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe late post initialization.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 64781 $   @e \$Date: 2012-01-30 21:19:50 -0600 (Mon, 30 Jan 2012) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#include  "amdlib.h"
#include  "Ids.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieTrainingV1.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbPcieInitLibV4.h"
#include  "GnbNbInitLibV4.h"
#include  "PcieLibTN.h"
#include  "PcieComplexDataTN.h"
#include  "GnbRegistersTN.h"
#include  "GnbRegisterAccTN.h"
#include  "cpuFamilyTranslation.h"
#include  "cpuFamRegisters.h"
#include  "F15TnPackageType.h"
#include  "GeneralServices.h"
#include  "Filecode.h"

#define FILECODE PROC_GNB_MODULES_GNBINITTN_PCIEEARLYINITTN_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern BUILD_OPT_CFG UserOptions;
extern CONST PCIE_HOST_REGISTER_TABLE_HEADER ROMDATA CoreInitTableTN;
extern CONST PCIE_HOST_REGISTER_TABLE_HEADER ROMDATA PcieInitEarlyTableTN;
extern CONST PCIE_PORT_REGISTER_TABLE_HEADER ROMDATA PortInitEarlyTableTN;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
PcieEarlyInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * PHY lane parameter Init
 *
 *
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @param[in]  Buffer              Pointer to buffer
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
STATIC AGESA_STATUS
PciePhyLaneInitInitCallbackTN (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      VOID                  *Buffer,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8   Phy;
  UINT8   PhyLaneIndex;
  UINT8   Lane;
  UINT32  LaneBitmap;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePhyLaneInitInitCallbackTN Enter\n");
  LaneBitmap = PcieUtilGetWrapperLaneBitMap (LANE_TYPE_PCIE_PHY_NATIVE, 0, Wrapper);
  for (Lane = 0; Lane < Wrapper->NumberOfLanes; ++Lane) {
    if ((LaneBitmap & (1 << Lane)) != 0) {
      Phy = Lane / MAX_NUM_LANE_PER_PHY;
      PhyLaneIndex = Lane - Phy * MAX_NUM_LANE_PER_PHY;
      PcieRegisterRMW (
        Wrapper,
        PHY_SPACE (Wrapper->WrapId, Phy, D0F0xE4_PHY_400A_ADDRESS + PhyLaneIndex * 0x80),
        D0F0xE4_PHY_400A_BiasDisInLs2_MASK | D0F0xE4_PHY_400A_Ls2ExitTime_MASK,
        (1 << D0F0xE4_PHY_400A_BiasDisInLs2_OFFSET) | (1 << D0F0xE4_PHY_400A_Ls2ExitTime_OFFSET),
        FALSE,
        Pcie
        );
      PcieRegisterRMW (
        Wrapper,
        PHY_SPACE (Wrapper->WrapId, Phy, D0F0xE4_PHY_4002_ADDRESS + PhyLaneIndex * 0x80),
        D0F0xE4_PHY_4002_LfcMax_MASK,
        (8 << D0F0xE4_PHY_4002_LfcMax_OFFSET),
        FALSE,
        Pcie
        );
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePhyLaneInitInitCallbackTN Exit\n");
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Satic init for various registers.
 *
 *
 *
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */
VOID
STATIC
PcieEarlyStaticInitTN (
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  UINTN Index;

  for (Index = 0; Index < PcieInitEarlyTableTN.Length; Index++) {
    GnbLibPciIndirectRMW (
      MAKE_SBDFO (0,0,0,0, D0F0xE0_ADDRESS),
      PcieInitEarlyTableTN.Table[Index].Reg,
      AccessWidth32,
      (UINT32)~PcieInitEarlyTableTN.Table[Index].Mask,
      PcieInitEarlyTableTN.Table[Index].Data,
      GnbLibGetHeader (Pcie)
      );
  }

}

/*----------------------------------------------------------------------------------------*/
/**
 * Init core registers.
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper configuration descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */
VOID
STATIC
PcieEarlyCoreInitTN (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  UINT8   CoreId;
  UINTN   Index;
  if (PcieLibIsPcieWrapper (Wrapper)) {
    IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyCoreInitTN Enter\n");
    for (CoreId = Wrapper->StartPcieCoreId; CoreId <= Wrapper->EndPcieCoreId; CoreId++) {
      for (Index = 0; Index < CoreInitTableTN.Length; Index++) {
        UINT32 Value;
        Value = PcieRegisterRead (
                  Wrapper,
                  CORE_SPACE (CoreId, CoreInitTableTN.Table[Index].Reg),
                  Pcie
                  );
        Value &= (~CoreInitTableTN.Table[Index].Mask);
        Value |= CoreInitTableTN.Table[Index].Data;
        PcieRegisterWrite (
          Wrapper,
          CORE_SPACE (CoreId, CoreInitTableTN.Table[Index].Reg),
          Value,
          FALSE,
          Pcie
          );
      }
    }
    IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyCoreInitTN Exit\n");
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set Dll Cap based on fuses
 *
 *
 *
 * @param[in]  Wrapper             Pointer to Wrapper configuration data area
 * @param[in]  Pcie                Pointer to PCIe configuration data area
 */
STATIC VOID
PcieSetDllCapTN (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  D18F3x1FC_STRUCT           D18F3x1FC;
  D0F0xE4_PHY_500F_STRUCT    D0F0xE4_PHY_500F;
  D0F0xE4_PHY_4010_STRUCT    D0F0xE4_PHY_4010;
  D0F0xE4_PHY_4011_STRUCT    D0F0xE4_PHY_4011;
  UINT32                     Gen1Index;
  UINT32                     Gen2Index;
  CPU_LOGICAL_ID             LogicalId;
  GNB_HANDLE                 *GnbHandle;


  IDS_HDT_CONSOLE (GNB_TRACE, "PcieSetDllCapTN Enter\n");

  D0F0xE4_PHY_500F.Value = 0;
  GnbHandle = GnbGetHandle (GnbLibGetHeader (Pcie));
  ASSERT (GnbHandle != NULL);
  GetLogicalIdOfSocket (GnbGetSocketId (GnbHandle), &LogicalId, GnbLibGetHeader (Pcie));

  //Read SWDllCapTableEn
  GnbRegisterReadTN (D18F3x1FC_TYPE, D18F3x1FC_ADDRESS, &D18F3x1FC, 0, GnbLibGetHeader (Pcie));
  IDS_HDT_CONSOLE (GNB_TRACE, "Read D18F3x1FC value %x\n", D18F3x1FC.Value);

  if ((D18F3x1FC.Field.SWDllCapTableEn != 0) || ((LogicalId.Revision & 0x0000000000000100ull ) != 0x0000000000000100ull  )) {
    IDS_HDT_CONSOLE (GNB_TRACE, "Executing DLL configuration\n");
    // Read D0F0xE4_x0[2:1]2[1:0]_[5:4][7:6,3:0][9,1]0 Phy Receiver Functional Fuse Control (FuseFuncDllProcessCompCtl[1:0])

    IDS_HDT_CONSOLE (GNB_TRACE, "Reading 0x4010 from PHY_SPACE %x\n", PHY_SPACE (Wrapper->WrapId, 0, D0F0xE4_PHY_4010_ADDRESS));
    D0F0xE4_PHY_4010.Value = PcieRegisterRead (Wrapper, PHY_SPACE (Wrapper->WrapId, 0, D0F0xE4_PHY_4010_ADDRESS), Pcie);
    IDS_HDT_CONSOLE (GNB_TRACE, "Read 4010 value = %x\n", D0F0xE4_PHY_4010.Value);
    // Read D0F0xE4_x0[2:1]2[1:0]_[5:4][7:6,3:0][9,1]1 Phy Receiver Process Fuse Control (FuseProcDllProcessComp[2:0])
    IDS_HDT_CONSOLE (GNB_TRACE, "Reading 0x4011 from PHY_SPACE %x\n", PHY_SPACE (Wrapper->WrapId, 0, D0F0xE4_PHY_4011_ADDRESS));
    D0F0xE4_PHY_4011.Value = PcieRegisterRead (Wrapper, PHY_SPACE (Wrapper->WrapId, 0, D0F0xE4_PHY_4011_ADDRESS), Pcie);
    IDS_HDT_CONSOLE (GNB_TRACE, "Read 4011 value = %x\n", D0F0xE4_PHY_4011.Value);

    // If FuseProcDllProcessCompCtl[1:0] == 2'b11 Then Gen1Index[3:0] = FuseProcDllProcessComp[2:0], 0
    // Else...
    // If FuseProcDllProcessComp[2:0] == 3'b000 Then Gen1Index[3:0] =4'b1101 //Typical
    // If FuseProcDllProcessComp[2:0] == 3'b001 Then Gen1Index[3:0] =4'b1111 //Fast
    // If FuseProcDllProcessComp[2:0] == 3'b010 Then Gen1Index[3:0] =4'b1010 //Slow
    IDS_HDT_CONSOLE (GNB_TRACE, "FuseFuncDllProcessCompCtl %x\n", D0F0xE4_PHY_4010.Field.FuseFuncDllProcessCompCtl);
    if (D0F0xE4_PHY_4010.Field.FuseFuncDllProcessCompCtl == 3) {
      IDS_HDT_CONSOLE (GNB_TRACE, "Setting Gen1Index from FuseFuncDllProcessComp %x\n", D0F0xE4_PHY_4011.Field.FuseProcDllProcessComp);
      Gen1Index = D0F0xE4_PHY_4011.Field.FuseProcDllProcessComp << 1;
    } else {
      IDS_HDT_CONSOLE (GNB_TRACE, "Setting Gen1Index from switch case...");
      switch (D0F0xE4_PHY_4011.Field.FuseProcDllProcessComp) {
      case 0:
        IDS_HDT_CONSOLE (GNB_TRACE, "case 0 - using 0xd\n");
        Gen1Index = 0xd;
        break;
      case 1:
        IDS_HDT_CONSOLE (GNB_TRACE, "case 1 - using 0xf\n");
        Gen1Index = 0xf;
        break;
      case 2:
        IDS_HDT_CONSOLE (GNB_TRACE, "case 2 - using 0xa\n");
        Gen1Index = 0xa;
        break;
      default:
        IDS_HDT_CONSOLE (GNB_TRACE, "default - using 0xd\n");
        Gen1Index = 0xd; //Use typical for default case
        break;
      }
    }
    D0F0xE4_PHY_500F.Field.DllProcessFreqCtlIndex1 = Gen1Index;
    IDS_HDT_CONSOLE (GNB_TRACE, "Set Gen1Index to %x\n", Gen1Index);
    // Bits  3:0 = Gen1Index[3:0]
    // Bits 10:7 = DllProcessFreqCtlIndex2Rate50[3:0]
    if (D18F3x1FC.Field.SWDllCapTableEn != 0) {
      IDS_HDT_CONSOLE (GNB_TRACE, "Gen2Index - using DllProcFreqCtlIndex2Rate50 = %x\n", D18F3x1FC.Field.DllProcFreqCtlIndex2Rate50);
      Gen2Index = D18F3x1FC.Field.DllProcFreqCtlIndex2Rate50;
    } else {
      Gen2Index = 0x03; // Hard coded default
    }
    D0F0xE4_PHY_500F.Field.DllProcessFreqCtlIndex2 = Gen2Index;
    IDS_HDT_CONSOLE (GNB_TRACE, "Set Gen2Index to %x\n", Gen2Index);
    PcieRegisterWrite (
      Wrapper,
      PHY_SPACE (Wrapper->WrapId, 0, D0F0xE4_PHY_500F_ADDRESS),
      D0F0xE4_PHY_500F.Value,
      FALSE,
      Pcie
      );
    // Set DllProcessFreqCtlOverride on second write
    D0F0xE4_PHY_500F.Field.DllProcessFreqCtlOverride = 1;
    PcieRegisterWrite (
      Wrapper,
      PHY_SPACE (Wrapper->WrapId, 0, D0F0xE4_PHY_500F_ADDRESS),
      D0F0xE4_PHY_500F.Value,
      FALSE,
      Pcie
      );
    if (Wrapper->WrapId == 1) {
      // For Wrapper 1, configure PHY0 and PHY1
      D0F0xE4_PHY_500F.Field.DllProcessFreqCtlOverride = 0;
      PcieRegisterWrite (
        Wrapper,
        PHY_SPACE (Wrapper->WrapId, 1, D0F0xE4_PHY_500F_ADDRESS),
        D0F0xE4_PHY_500F.Value,
        FALSE,
        Pcie
        );
      // Set DllProcessFreqCtlOverride on second write
      D0F0xE4_PHY_500F.Field.DllProcessFreqCtlOverride = 1;
      PcieRegisterWrite (
        Wrapper,
        PHY_SPACE (Wrapper->WrapId, 1, D0F0xE4_PHY_500F_ADDRESS),
        D0F0xE4_PHY_500F.Value,
        FALSE,
        Pcie
        );
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieSetDllCapTN Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * TN FP2 PCIE allocation x8 check
 *
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper configuration descriptor
 * @param[in]  Buffer          Pointer buffer
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */


STATIC AGESA_STATUS
PcieFP2x8CheckCallbackTN (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  UINT32                    LaneBitmap;
  AGESA_STATUS              Status;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieFP2x8CheckCallbackTN Enter\n");

  Status = AGESA_SUCCESS;
  if (Wrapper->WrapId == GFX_WRAP_ID) {

    LaneBitmap = PcieUtilGetWrapperLaneBitMap (LANE_TYPE_PCIE_PHY_NATIVE | LANE_TYPE_DDI_PHY_NATIVE, 0, Wrapper);
    IDS_HDT_CONSOLE (GNB_TRACE, "FP2 GFX Wrpper phy LaneBitmap = %x\n", LaneBitmap);

    if (((LaneBitmap & 0xFF) != 0) && ((LaneBitmap & 0xFF00) != 0)) {
      IDS_HDT_CONSOLE (GNB_TRACE, "Error!! FP2 GFX Wrpper cannot use both phy#\n");
      Status = AGESA_ERROR;
      PcieConfigDisableAllEngines (PciePortEngine | PcieDdiEngine, Wrapper);
      PutEventLog (
        AGESA_ERROR,
        GNB_EVENT_INVALID_LANES_CONFIGURATION,
        (LibAmdBitScanForward (LaneBitmap) + Wrapper->StartPhyLane),
        (LibAmdBitScanReverse (LaneBitmap) + Wrapper->StartPhyLane),
        0,
        0,
        GnbLibGetHeader (Pcie)
        );

      ASSERT (FALSE);
    }
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieFP2x8CheckCallbackTN Exit\n");
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * TN FP2 PCIE critera check
 *
 *
 *
 * @param[in]  Pcie        Pointer to PICe configuration data area
 */


STATIC AGESA_STATUS
PcieFP2CriteriaTN (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS              Status;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieFP2CriteriaTN Enter\n");

  // PACKAGE_TYPE_FP2    1
  // PACKAGE_TYPE_FS1r2  2
  // PACKAGE_TYPE_FM2    4
  if (LibAmdGetPackageType (GnbLibGetHeader (Pcie)) != PACKAGE_TYPE_FP2) {
    return AGESA_SUCCESS;
  }
  // FP2 force gen1
  Pcie->PsppPolicy = PsppPowerSaving;
  // FP2 only use x8 on the same PHY
  Status = PcieConfigRunProcForAllWrappers (DESCRIPTOR_ALL_WRAPPERS, PcieFP2x8CheckCallbackTN, NULL, Pcie);

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieFP2CriteriaTN Exit\n");
  return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * RX offset cancellation enablement
 *
 *
 *
 * @param[in]  Wrapper             Pointer to Wrapper configuration data area
 * @param[in]  Pcie                Pointer to PCIe configuration data area
 */
STATIC VOID
PcieOffsetCancelCalibration (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32                LaneBitmap;
  D0F0xBC_x1F39C_STRUCT D0F0xBC_x1F39C;

  LaneBitmap = PcieUtilGetWrapperLaneBitMap (LANE_TYPE_PHY_NATIVE_ALL, LANE_TYPE_PCIE_SB_CORE_CONFIG, Wrapper);
  if ((Wrapper->WrapId != GFX_WRAP_ID) && (Wrapper->WrapId != GPP_WRAP_ID)) {
    return;
  }

  if (LaneBitmap != 0) {
    D0F0xBC_x1F39C.Value = 0;
    D0F0xBC_x1F39C.Field.Tx = 1;
    D0F0xBC_x1F39C.Field.Rx = 1;
    D0F0xBC_x1F39C.Field.UpperLaneID = LibAmdBitScanReverse (LaneBitmap) + Wrapper->StartPhyLane;
    D0F0xBC_x1F39C.Field.LowerLaneID = LibAmdBitScanForward (LaneBitmap) + Wrapper->StartPhyLane;
    GnbRegisterWriteTN (D0F0xBC_x1F39C_TYPE,  D0F0xBC_x1F39C_ADDRESS, &D0F0xBC_x1F39C.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Pcie));
    GnbSmuServiceRequestV4 (
      PcieConfigGetParentSilicon (Wrapper)->Address,
      SMC_MSG_PHY_LN_OFF,
      GNB_REG_ACC_FLAG_S3SAVE,
      GnbLibGetHeader (Pcie)
      );
    GnbSmuServiceRequestV4 (
      PcieConfigGetParentSilicon (Wrapper)->Address,
      SMC_MSG_PHY_LN_ON,
      GNB_REG_ACC_FLAG_S3SAVE,
      GnbLibGetHeader (Pcie)
      );
  }
  PcieTopologyLaneControl (
    EnableLanes,
    PcieUtilGetWrapperLaneBitMap (LANE_TYPE_ALL, 0, Wrapper),
    Wrapper,
    Pcie
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Per wrapper Pcie Init SRBM reset prior Aaccess to wrapper registers.
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper configuration descriptor
 * @param[in]  Buffer          Pointer buffer
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */
STATIC AGESA_STATUS
PcieInitSrbmCallbackTN (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  PcieTopologyInitSrbmReset (TRUE, Wrapper, Pcie);
  PcieOffsetCancelCalibration (Wrapper, Pcie);
  PciePifApplyGanging (Wrapper, Pcie);
  PciePhyApplyGanging (Wrapper, Pcie);
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * PHY Pll Personality Init Callback
 *
 *
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @param[in]  Buffer              Pointer to buffer
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
STATIC AGESA_STATUS
PciePhyLetPllPersonalityInitCallbackTN (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      VOID                  *Buffer,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePhyLetPllPersonalityInitCallbackTN Enter\n");
  PciePifPllPowerControl (PowerDownPifs, Wrapper, Pcie);
  PciePifSetPllRampTime (NormalRampup, Wrapper, Pcie);
  PciePollPifForCompeletion (Wrapper, Pcie);
  PcieTopologyLaneControl (
    DisableLanes,
    PcieUtilGetWrapperLaneBitMap (LANE_TYPE_CORE_ALL, LANE_TYPE_PCIE_SB_CORE_CONFIG, Wrapper),
    Wrapper,
    Pcie
    );
  PciePollPifForCompeletion (Wrapper, Pcie);
  PcieSetPhyPersonalityTN (Wrapper, Pcie);
  PcieWrapSetTxS1CtrlForLaneMux (Wrapper, Pcie);
  PciePollPifForCompeletion (Wrapper, Pcie);
  PcieTopologyLaneControl (
    EnableLanes,
    PcieUtilGetWrapperLaneBitMap (LANE_TYPE_CORE_ALL, 0, Wrapper),
    Wrapper,
    Pcie
    );
  PcieWrapSetTxOffCtrlForLaneMux (Wrapper, Pcie);
  PciePollPifForCompeletion (Wrapper, Pcie);
  PciePifPllPowerControl (PowerUpPifs, Wrapper, Pcie);
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePhyLetPllPersonalityInitCallbackTN Exit\n");
  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Per wrapper Pcie Init prior training.
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper configuration descriptor
 * @param[in]  Buffer          Pointer buffer
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */
AGESA_STATUS
STATIC
PcieEarlyInitCallbackTN (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  AGESA_STATUS  Status;
  BOOLEAN       CoreConfigChanged;
  BOOLEAN       PllConfigChanged;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyInitCallbackTN Enter\n");
  CoreConfigChanged = FALSE;
  PllConfigChanged = FALSE;
  PcieTopologyPrepareForReconfig (Wrapper, Pcie);
  Status = PcieTopologySetCoreConfig (Wrapper, &CoreConfigChanged, Pcie);
  ASSERT (Status == AGESA_SUCCESS);
  PcieTopologyApplyLaneMux (Wrapper, Pcie);
  PciePifSetRxDetectPowerMode (Wrapper, Pcie);
  PciePifSetLs2ExitTime (Wrapper, Pcie);
  PcieTopologySelectMasterPll (Wrapper, &PllConfigChanged, Pcie);
  if (CoreConfigChanged || PllConfigChanged) {
    PcieTopologyExecuteReconfigV4 (Wrapper, Pcie);
  }
  PcieTopologyCleanUpReconfig (Wrapper, Pcie);
  PcieTopologySetLinkReversalV4 (Wrapper, Pcie);

  if (Wrapper->Features.PowerOffUnusedPlls != 0) {
    PciePifPllPowerDown (
      PcieUtilGetWrapperLaneBitMap (LANE_TYPE_CORE_ALL, LANE_TYPE_PCIE_CORE_ALLOC | LANE_TYPE_DDI_PHY_NATIVE, Wrapper),
      Wrapper,
      Pcie
      );
    PciePifPllInitForDdi (Wrapper, Pcie);
    PciePwrPowerDownDdiPllsV4 (Wrapper, Pcie);
  }
  PcieTopologyLaneControl (
    DisableLanes,
    PcieUtilGetWrapperLaneBitMap (LANE_TYPE_CORE_ALL, LANE_TYPE_PCIE_CORE_ALLOC, Wrapper),
    Wrapper,
    Pcie
    );
  PcieSetDdiOwnPhyV4 (Wrapper, Pcie);
  PciePollPifForCompeletion (Wrapper, Pcie);
  PciePhyAvertClockPickers (Wrapper, Pcie);
  PcieEarlyCoreInitTN (Wrapper, Pcie);
  PcieSetSsidV4 (UserOptions.CfgGnbPcieSSID, Wrapper, Pcie);
  if (PcieConfigIsPcieWrapper (Wrapper)) {
    PcieSetDllCapTN (Wrapper, Pcie);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyInitCallbackTN Exit [%x]\n", Status);
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Pcie Init
 *
 *
 *
 * @param[in]  Pcie                Pointer to global PCIe configuration
 * @retval     AGESA_SUCCESS       Topology successfully mapped
 * @retval     AGESA_ERROR         Topology can not be mapped
 */

AGESA_STATUS
STATIC
PcieEarlyInitTN (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS        Status;
  AGESA_STATUS        AgesaStatus;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyInitTN Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Status = PcieFP2CriteriaTN (Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  Status = PcieConfigRunProcForAllWrappers (DESCRIPTOR_ALL_WRAPPERS, PcieInitSrbmCallbackTN, NULL, Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  PcieEarlyStaticInitTN (Pcie);
  Status = PcieConfigRunProcForAllWrappers (DESCRIPTOR_ALL_WRAPPERS, PciePhyLetPllPersonalityInitCallbackTN, NULL, Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  PcieOscInitTN (Pcie);
  Status = PcieConfigRunProcForAllWrappers (DESCRIPTOR_ALL_WRAPPERS, PciePhyLaneInitInitCallbackTN, NULL, Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  Status = PcieConfigRunProcForAllWrappers (DESCRIPTOR_ALL_WRAPPERS, PcieEarlyInitCallbackTN, NULL, Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  PcieSetVoltageTN (PcieGen1, Pcie);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyInitTN Exit [%x]\n", AgesaStatus);
  return  AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to init various features on all active ports
 *
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  Buffer          Not used
 * @param[in]       Pcie            Pointer to global PCIe configuration
 *
 */

VOID
STATIC
PcieEarlyPortInitCallbackTN (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyPortInitCallbackTN Enter\n");
  ASSERT (Engine->EngineData.EngineType == PciePortEngine);
  PciePortProgramRegisterTable (PortInitEarlyTableTN.Table, PortInitEarlyTableTN.Length, Engine, FALSE, Pcie);
  PcieSetLinkSpeedCapV4 (PcieGen1, Engine, Pcie);
  PcieSetLinkWidthCap (Engine, Pcie);
  PcieCompletionTimeout (Engine, Pcie);
  PcieLinkSetSlotCap (Engine, Pcie);
  PcieLinkInitHotplug (Engine, Pcie);
  //PciePhyChannelCharacteristic (Engine, Pcie);
  if (Engine->Type.Port.PortData.PortPresent == PortDisabled ||
      (Engine->Type.Port.PortData.EndpointStatus == EndpointNotPresent &&
       Engine->Type.Port.PortData.LinkHotplug != HotplugEnhanced &&
       Engine->Type.Port.PortData.LinkHotplug != HotplugServer)) {
    ASSERT (!PcieConfigIsSbPcieEngine (Engine));
    //
    //  Pass endpoint tstaus in scratch
    //
    PciePortRegisterRMW (
      Engine,
      DxF0xE4_x01_ADDRESS,
      0x1,
      0x1,
      FALSE,
      Pcie
      );
    PcieTrainingSetPortState (Engine, LinkStateDeviceNotPresent, FALSE, Pcie);
  }
  if (Engine->Type.Port.PortData.MiscControls.LinkComplianceMode == 0x1) {
    PcieTrainingSetPortState (Engine, LinkStateTrainingCompleted, FALSE, Pcie);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyPortInitCallbackTN Exit\n");
}


/*----------------------------------------------------------------------------------------*/
/**
 * Master procedure to init various features on all active ports
 *
 *
 *
 *
 * @param[in]   Pcie            Pointer to global PCIe configuration
 * @retval      AGESA_STATUS
 *
 */

AGESA_STATUS
STATIC
PcieEarlyPortInitTN (
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS  Status;
  Status = AGESA_SUCCESS;
  // Leave all device in Presence Detect Presence state for distributed training will be completed at PciePortPostEarlyInit
  if (Pcie->TrainingAlgorithm == PcieTrainingDistributed) {
    Pcie->TrainingExitState = LinkStateResetExit;
  }
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PcieEarlyPortInitCallbackTN,
    NULL,
    Pcie
    );
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * PCIe Early Post Init
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
PcieEarlyInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          Status;
  AGESA_STATUS          AgesaStatus;
  PCIe_PLATFORM_CONFIG  *Pcie;
  AgesaStatus = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyInterfaceTN Enter\n");
  Status = PcieLocateConfigurationData (StdHeader, &Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    PciePortsVisibilityControlTN (UnhidePorts, Pcie);

    Status = PcieEarlyInitTN (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    Status = PcieEarlyPortInitTN (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    Status = PcieTraining (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    IDS_OPTION_CALLOUT (IDS_CALLOUT_GNB_PCIE_PHY_CONFIG, Pcie, StdHeader);
    PciePortsVisibilityControlTN (HidePorts, Pcie);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEarlyInterfaceTN Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}

