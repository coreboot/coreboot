/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 Power Plane Initialization
 *
 * Performs the "BIOS Requirements for Power Plane Initialization" as described
 * in the BKDG.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F10
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuF10PowerMgmt.h"
#include "cpuApicUtilities.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuF10Utilities.h"
#include "Table.h"
#include "F10PackageType.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FAMILY_0X10_CPUF10POWERPLANE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

// Register encodings for F3xD4[PowerStepUp/PowerStepDown]
CONST UINT16 ROMDATA PowerStepEncodings[16] =
{
  400,    // 0000b: 400ns
  300,    // 0001b: 300ns
  200,    // 0010b: 200ns
  100,    // 0011b: 100ns
  90,     // 0100b: 90ns
  80,     // 0101b: 80ns
  70,     // 0110b: 70ns
  60,     // 0111b: 60ns
  50,     // 1000b: 50ns
  45,     // 1001b: 45ns
  40,     // 1010b: 40ns
  35,     // 1011b: 35ns
  30,     // 1100b: 30ns
  25,     // 1101b: 25ns
  20,     // 1110b: 20ns
  15      // 1111b: 15ns
};

// Register encodings for F3xDC[AltvidVSSlamTime]
CONST UINT32 ROMDATA AltvidSlamTime[8] =
{
  0,    // 000b: <1us
  10,   // 001b: 10us
  20,   // 010b: 20us
  40,   // 011b: 40us
  50,   // 100b: 50us
  70,   // 101b: 70us
  80,   // 110b: 80us
  90    // 111b: 90us
};

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
STATIC
F10PmPwrPlaneInitPviCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

UINT32
STATIC
F10CalculateAltvidVSSlamTimeOnCore (
  IN       BOOLEAN PviModeFlag,
  IN       PCI_ADDR *PciAddress,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParams,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
F10PmVrmLowPowerModeEnable (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParams,
  IN       PCI_ADDR              PciAddress,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Family 10h core 0 entry point for performing power plane initialization.
 *
 * The steps are as follows:
 *    1. If single plane, program lower VID code of CpuVid & NbVid for all
 *       enabled P-States.
 *    2. Configure F3xA0[SlamMode] & F3xD8[VsRampTime & VsSlamTime] based on
 *       platform requirements.
 *    3. Configure F3xD4[PowerStepUp & PowerStepDown]
 *    4. Optionally configure F3xA0[PsiVidEn & PsiVid]
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  CpuEarlyParams          Service parameters
 * @param[in]  StdHeader               Config handle for library and services.
 *
 */
VOID
F10CpuAmdPmPwrPlaneInit (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParams,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  BOOLEAN   PviModeFlag;
  PCI_ADDR  PciAddress;
  UINT16    PowerStepTime;
  UINT32    PowerStepEncoded;
  UINT32    LocalPciRegister;
  UINT32    VsSlamTime;
  UINT32    Socket;
  UINT32    Module;
  UINT32    Core;
  UINT32    NumOfCores;
  UINT32    LowCore;
  UINT32    AndMask;
  UINT32    OrMask;
  UINT32    ProcessorPackageType;
  UINT64    LocalMsrRegister;
  AP_TASK   TaskPtr;
  AGESA_STATUS  IgnoredSts;
  PLATFORM_FEATS Features;
  CPU_LOGICAL_ID LogicalId;

  // Initialize the union
  Features.PlatformValue = 0;
  GetPlatformFeatures (&Features, &CpuEarlyParams->PlatformConfig, StdHeader);

  IdentifyCore (StdHeader, &Socket, &Module, &Core, &IgnoredSts);
  GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts);

  ASSERT (Core == 0);

  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);

  // Set SlamVidMode
  PciAddress.Address.Function = FUNC_3;
  PciAddress.Address.Register = PW_CTL_MISC_REG;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  AndMask = 0xFFFFFFFF;
  OrMask = 0x00000000;
  if (((POWER_CTRL_MISC_REGISTER *) &LocalPciRegister)->PviMode == 1) {
    PviModeFlag = TRUE;
    ((POWER_CTRL_MISC_REGISTER *) &AndMask)->SlamVidMode = 0;

    // Have all single plane cores adjust their NB and CPU VID fields
    TaskPtr.FuncAddress.PfApTask = F10PmPwrPlaneInitPviCore;
    TaskPtr.DataTransfer.DataSizeInDwords = 0;
    TaskPtr.ExeFlags = WAIT_FOR_CORE;
    ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, CpuEarlyParams);

  } else {
    PviModeFlag = FALSE;
    ((POWER_CTRL_MISC_REGISTER *) &OrMask)->SlamVidMode = 1;
  }
  ModifyCurrentSocketPci (&PciAddress, AndMask, OrMask, StdHeader);

  F10ProgramVSSlamTimeOnSocket (&PciAddress, CpuEarlyParams, StdHeader);

  // Configure PowerStepUp/PowerStepDown
  PciAddress.Address.Register = CPTC0_REG;
  if ((Features.PlatformFeatures.PlatformSingleLink == 1) ||
      (Features.PlatformFeatures.PlatformUma == 1) ||
      (Features.PlatformFeatures.PlatformUmaIfcm == 1) ||
      (Features.PlatformFeatures.PlatformIfcm == 1) ||
      (Features.PlatformFeatures.PlatformIommu == 1)) {
    PowerStepEncoded = 0x8;
  } else {
    GetGivenModuleCoreRange ((UINT32) Socket,
                             (UINT32) Module,
                             &LowCore,
                             &NumOfCores,
                             StdHeader);
    NumOfCores = ((NumOfCores - LowCore) + 1);
    PowerStepTime = (UINT16) (400 / NumOfCores);
    for (PowerStepEncoded = 0xF; PowerStepEncoded > 0; PowerStepEncoded--) {
      if (PowerStepTime <= PowerStepEncodings[PowerStepEncoded]) {
        break;
      }
    }
  }
  AndMask = 0xFFFFFFFF;
  ((CLK_PWR_TIMING_CTRL_REGISTER *) &AndMask)->PowerStepUp = 0;
  ((CLK_PWR_TIMING_CTRL_REGISTER *) &AndMask)->PowerStepDown = 0;
  OrMask = 0x00000000;
  ((CLK_PWR_TIMING_CTRL_REGISTER *) &OrMask)->PowerStepUp = PowerStepEncoded;
  ((CLK_PWR_TIMING_CTRL_REGISTER *) &OrMask)->PowerStepDown = PowerStepEncoded;
  ModifyCurrentSocketPci (&PciAddress, AndMask, OrMask, StdHeader);

  if ((LogicalId.Revision & AMD_F10_C3) != 0) {
    // Set up Pop up P-state register
    PciAddress.Address.Register = CPTC2_REG;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    AndMask = 0xFFFFFFFF;
    ((POPUP_PSTATE_REGISTER *) &AndMask)->PopupPstate = 0;
    ((POPUP_PSTATE_REGISTER *) &AndMask)->PopupCpuVid = 0;
    ((POPUP_PSTATE_REGISTER *) &AndMask)->PopupCpuFid = 0;
    ((POPUP_PSTATE_REGISTER *) &AndMask)->PopupCpuDid = 0;
    OrMask = 0x00000000;
    ((POPUP_PSTATE_REGISTER *) &OrMask)->PopupEn = 0;
    ((POPUP_PSTATE_REGISTER *) &OrMask)->PopupPstate = ((CLK_PWR_TIMING_CTRL2_REGISTER *) &LocalPciRegister)->PstateMaxVal;
    LibAmdMsrRead ((((CLK_PWR_TIMING_CTRL2_REGISTER *) &LocalPciRegister)->PstateMaxVal + PS_REG_BASE), &LocalMsrRegister, StdHeader);
    ((POPUP_PSTATE_REGISTER *) &OrMask)->PopupCpuVid = (UINT32) ((PSTATE_MSR *) &LocalMsrRegister)->CpuVid;
    ((POPUP_PSTATE_REGISTER *) &OrMask)->PopupCpuFid = (UINT32) ((PSTATE_MSR *) &LocalMsrRegister)->CpuFid;
    ((POPUP_PSTATE_REGISTER *) &OrMask)->PopupCpuDid = (UINT32) ((PSTATE_MSR *) &LocalMsrRegister)->CpuDid;
    PciAddress.Address.Register = POPUP_PSTATE_REG;
    ModifyCurrentSocketPci (&PciAddress, AndMask, OrMask, StdHeader);

    // Set AltVidStart
    PciAddress.Address.Register = CPTC1_REG;
    AndMask = 0xFFFFFFFF;
    ((CLK_PWR_TIMING_CTRL1_REGISTER *) &AndMask)->AltVidStart = 0;
    OrMask = 0x00000000;
    ((CLK_PWR_TIMING_CTRL1_REGISTER *) &OrMask)->AltVidStart = (UINT32) ((PSTATE_MSR *) &LocalMsrRegister)->CpuVid;
    ModifyCurrentSocketPci (&PciAddress, AndMask, OrMask, StdHeader);

    // Set up Altvid slam time
    ProcessorPackageType = LibAmdGetPackageType (StdHeader);
    PciAddress.Address.Register = CPTC2_REG;
    VsSlamTime = F10CalculateAltvidVSSlamTimeOnCore (PviModeFlag, &PciAddress, CpuEarlyParams, StdHeader);
    AndMask = 0xFFFFFFFF;
    ((CLK_PWR_TIMING_CTRL2_REGISTER *) &AndMask)->AltvidVSSlamTime = 0;
    ((CLK_PWR_TIMING_CTRL2_REGISTER *) &AndMask)->SlamTimeMode = 0;
    OrMask = 0x00000000;
    ((CLK_PWR_TIMING_CTRL2_REGISTER *) &OrMask)->AltvidVSSlamTime = VsSlamTime;
    if (ProcessorPackageType == PACKAGE_TYPE_S1G3_S1G4 || ProcessorPackageType == PACKAGE_TYPE_ASB2) {
      // If CPUID Fn8000_0001_EBX[PkgType]=0010b or 0100b, BIOS should program this to 10b;
      // else BIOS should leave this field at 00b.
      ((CLK_PWR_TIMING_CTRL2_REGISTER *) &OrMask)->SlamTimeMode = 2;
    }
    ModifyCurrentSocketPci (&PciAddress, AndMask, OrMask, StdHeader);
  }

  if (IsWarmReset (StdHeader) && !PviModeFlag) {
    // Configure PsiVid
    F10PmVrmLowPowerModeEnable (FamilySpecificServices, CpuEarlyParams, PciAddress, StdHeader);
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 * Support routine for F10CpuAmdPmPwrPlaneInit.
 *
 * This function implements step 1 on each core.
 *
 * @param[in]  StdHeader          Config handle for library and services.
 *
 */
VOID
STATIC
F10PmPwrPlaneInitPviCore (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 MsrAddr;
  UINT32 NbVid;
  UINT32 CpuVid;
  UINT64 LocalMsrRegister;

  for (MsrAddr = PS_REG_BASE; MsrAddr <= PS_MAX_REG; MsrAddr++) {
    LibAmdMsrRead (MsrAddr, &LocalMsrRegister, StdHeader);
    if (((PSTATE_MSR *) &LocalMsrRegister)->PsEnable == (UINT64) 1) {
      NbVid = (UINT32) (((PSTATE_MSR *) &LocalMsrRegister)->NbVid);
      CpuVid = (UINT32) (((PSTATE_MSR *) &LocalMsrRegister)->CpuVid);
      if (NbVid != CpuVid) {
        if (NbVid > CpuVid) {
          NbVid = CpuVid;
        }
        ((PSTATE_MSR *) &LocalMsrRegister)->NbVid = NbVid;
        ((PSTATE_MSR *) &LocalMsrRegister)->CpuVid = NbVid;
        LibAmdMsrWrite (MsrAddr, &LocalMsrRegister, StdHeader);
      }
    }
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 * Returns the encoded altvid voltage stabilization slam time for the executing
 * family 10h core.
 *
 * This function calculates how much time it will take for the voltage to
 * stabilize when transitioning from altvid to Pmin, and returns the necessary
 * encoded value for the amount of time discovered.
 *
 * @param[in]  PviModeFlag        Whether or not the platform uses VRMs that
 *                                employ the parallel VID interface.
 * @param[in]  PciAddress         Full PCI address of the executing core's config space.
 * @param[in]  CpuEarlyParams     Service parameters
 * @param[in]  StdHeader          Config handle for library and services.
 *
 * @retval     Encoded register value.
 *
 */
UINT32
STATIC
F10CalculateAltvidVSSlamTimeOnCore (
  IN       BOOLEAN PviModeFlag,
  IN       PCI_ADDR *PciAddress,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParams,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8     NbVid;
  UINT8     AltVidCode;
  UINT8     PminVidCode;
  UINT32    MsrAddr;
  UINT32    LocalPciRegister;
  UINT64    LocalMsrRegister;
  PCI_ADDR  LocalPciAddress;

  // Calculate Slam Time
  //   VSSlamTime = 0.4us/mV (or 0.2us/mV) * Vpmin - Altvid
  //   In our case, we will scale the values by 100 to avoid
  //   decimals.

  // Get Pmin's index
  LibAmdMsrRead (MSR_PSTATE_CURRENT_LIMIT, &LocalMsrRegister, StdHeader);
  MsrAddr = (UINT32) ((((PSTATE_CURLIM_MSR *) &LocalMsrRegister)->PstateMaxVal) + PS_REG_BASE);

  // Get Pmin's VID
  LibAmdMsrRead (MsrAddr, &LocalMsrRegister, StdHeader);
  PminVidCode = (UINT8) (((PSTATE_MSR *) &LocalMsrRegister)->CpuVid);

  // If SVI, we only care about CPU VID.
  // If PVI, determine the higher voltage b/t NB and CPU
  if (PviModeFlag) {
    NbVid = (UINT8) (((PSTATE_MSR *) &LocalMsrRegister)->NbVid);
    if (PminVidCode > NbVid) {
      PminVidCode = NbVid;
    }
  }

  // Get Alt VID
  LocalPciAddress.AddressValue = PciAddress->AddressValue;
  LocalPciAddress.Address.Function = FUNC_3;
  LocalPciAddress.Address.Register = CPTC2_REG;
  LibAmdPciRead (AccessWidth32, LocalPciAddress, &LocalPciRegister, StdHeader);
  AltVidCode = (UINT8) (((CLK_PWR_TIMING_CTRL2_REGISTER *) &LocalPciRegister)->AltVid);

  return (F10GetSlamTimeEncoding (PminVidCode, AltVidCode, CpuEarlyParams, AltvidSlamTime, StdHeader));
}


/*---------------------------------------------------------------------------------------*/
/**
 * Sets up PSI_L operation.
 *
 * This function implements the LowPowerThreshold parameter.
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  CpuEarlyParams          Contains VrmLowPowerThreshold parameter.
 * @param[in]  PciAddress              PCI address of the executing core's config space.
 * @param[in]  StdHeader               Config handle for library and services.
 *
 */
VOID
STATIC
F10PmVrmLowPowerModeEnable (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParams,
  IN       PCI_ADDR              PciAddress,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32    Pstate;
  UINT32    PstateCurrent;
  UINT32    NextPstateCurrent;
  UINT32    AndMask;
  UINT32    OrMask;
  UINT32    PreviousVID;
  UINT32    PstateVID;
  UINT32    HwPsMaxVal;
  UINT64    PstateMsr;
  BOOLEAN   EnablePsi;

  if (CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].LowPowerThreshold != 0) {
    EnablePsi = FALSE;
    PreviousVID = 0x7F;           // Initialize to invalid zero volt VID code
    PstateVID = 0x7F;
    PciAddress.Address.Function = FUNC_3;
    PciAddress.Address.Register = CPTC2_REG;
    LibAmdPciRead (AccessWidth32, PciAddress, &HwPsMaxVal, StdHeader);

    for (Pstate = 0; Pstate <= (UINT32) ((CLK_PWR_TIMING_CTRL2_REGISTER *) &HwPsMaxVal)->PstateMaxVal; Pstate++) {
      if (FamilySpecificServices->GetProcIddMax (FamilySpecificServices, (UINT8) Pstate, &PstateCurrent, StdHeader)) {
        LibAmdMsrRead ((UINT32) (Pstate + PS_REG_BASE), &PstateMsr, StdHeader);
        PstateVID = (UINT32) (((PSTATE_MSR *) &PstateMsr)->CpuVid);
        if ((Pstate + 1) > (UINT32) ((CLK_PWR_TIMING_CTRL2_REGISTER *) &HwPsMaxVal)->PstateMaxVal) {
          NextPstateCurrent = 0;
        } else if (FamilySpecificServices->GetProcIddMax (FamilySpecificServices, (UINT8) (Pstate + 1), &NextPstateCurrent, StdHeader)) {
          NextPstateCurrent = CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].InrushCurrentLimit + NextPstateCurrent;
        }
        if ((PstateCurrent <= CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].LowPowerThreshold) && (NextPstateCurrent <= CpuEarlyParams->PlatformConfig.VrmProperties[CoreVrm].LowPowerThreshold) && (PstateVID != PreviousVID)) {
          EnablePsi = TRUE;
          break;
        }
        PreviousVID = PstateVID;
      }
    }

    PciAddress.Address.Function = FUNC_3;
    PciAddress.Address.Register = PW_CTL_MISC_REG;
    AndMask = 0xFFFFFFFF;
    OrMask = 0x00000000;
    ((POWER_CTRL_MISC_REGISTER *) &AndMask)->PsiVid = 0;
    if (EnablePsi) {
      ((POWER_CTRL_MISC_REGISTER *) &OrMask)->PsiVid = PstateVID;
      ((POWER_CTRL_MISC_REGISTER *) &OrMask)->PsiVidEn = 1;
    } else {
      ((POWER_CTRL_MISC_REGISTER *) &AndMask)->PsiVidEn = 0;
    }
    ModifyCurrentSocketPci (&PciAddress, AndMask, OrMask, StdHeader);
  }
}
