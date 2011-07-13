/**
 * @file
 *
 * AMD Family_10 RevD Message-Based C1e feature support functions.
 *
 * Provides the functions necessary to initialize the message-based C1e feature.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F10
 * @e \$Revision: 46667 $   @e \$Date: 2011-02-08 14:06:57 -0700 (Tue, 08 Feb 2011) $
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
 *
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "cpuFeatures.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuMsgBasedC1e.h"
#include "cpuApicUtilities.h"
#include "cpuF10PowerMgmt.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FAMILY_0X10_REVD_F10REVDMSGBASEDC1E_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

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
F10InitializeMsgBasedC1eOnCore (
  IN       VOID              *BmStsAddress,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

BOOLEAN
STATIC
IsDramScrubberEnabled (
  IN       PCI_ADDR PciAddress,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 *  Should message-based C1e be enabled
 *
 * @param[in]    MsgBasedC1eServices Pointer to this CPU's HW C1e family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 * @retval       TRUE               HW C1e is supported.
 *
 */
BOOLEAN
STATIC
F10IsMsgBasedC1eSupported (
  IN       MSG_BASED_C1E_FAMILY_SERVICES *MsgBasedC1eServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  CPU_LOGICAL_ID LogicalId;

  GetLogicalIdOfSocket (Socket, &LogicalId, StdHeader);
  return ((BOOLEAN) ((LogicalId.Revision & AMD_F10_GT_D0) != 0));
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Core 0 task to enable message-based C1e on a family 10h CPU.
 *
 * @param[in]    MsgBasedC1eServices Pointer to this CPU's HW C1e family services.
 * @param[in]    EntryPoint          Timepoint designator.
 * @param[in]    PlatformConfig      Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 * @return       AGESA_SUCCESS      Always succeeds.
 *
 */
AGESA_STATUS
STATIC
F10InitializeMsgBasedC1e (
  IN       MSG_BASED_C1E_FAMILY_SERVICES *MsgBasedC1eServices,
  IN       UINT64 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32       AndMask;
  UINT32       Core;
  UINT32       Module;
  UINT32       OrMask;
  UINT32       PciRegister;
  UINT32       Socket;
  AP_TASK      TaskPtr;
  PCI_ADDR     PciAddress;
  AGESA_STATUS IgnoredSts;

  if ((EntryPoint & (CPU_FEAT_AFTER_POST_MTRR_SYNC | CPU_FEAT_AFTER_RESUME_MTRR_SYNC)) != 0) {
    // Note that this core 0 does NOT have the ability to launch
    // any of its cores.  Attempting to do so could lead to a system
    // hang.

    // Set F3xA0[IdleExitEn] = 1
    PciAddress.Address.Function = FUNC_3;
    PciAddress.Address.Register = PW_CTL_MISC_REG;
    AndMask = 0xFFFFFFFF;
    OrMask = 0;
    ((POWER_CTRL_MISC_REGISTER *) &OrMask)->IdleExitEn = 1;
    ModifyCurrentSocketPci (&PciAddress, AndMask, OrMask, StdHeader); // F3xA0

    // BIOS should set F3x1B8[5]
    PciAddress.Address.Register = 0x1B8;
    OrMask = 0x00000020;
    ModifyCurrentSocketPci (&PciAddress, AndMask, OrMask, StdHeader); // F3x1B8

    // Set F3x188[EnStpGntOnFlushMaskWakeup] = 1
    PciAddress.Address.Register = NB_EXT_CFG_LO_REG;
    OrMask = 0;
    ((NB_EXT_CFG_LO_REGISTER *) &OrMask)->EnStpGntOnFlushMaskWakeup = 1;
    ModifyCurrentSocketPci (&PciAddress, AndMask, OrMask, StdHeader); // F3x188

    // Set F3xD4[MTC1eEn] = 1, F3xD4[CacheFlushImmOnAllHalt] = 1
    // Set F3xD4[StutterScrubEn] = 1 if scrubbing is enabled
    ((CLK_PWR_TIMING_CTRL_REGISTER *) &AndMask)->StutterScrubEn = 0;
    OrMask = 0;
    ((CLK_PWR_TIMING_CTRL_REGISTER *) &OrMask)->MTC1eEn = 1;
    ((CLK_PWR_TIMING_CTRL_REGISTER *) &OrMask)->CacheFlushImmOnAllHalt = 1;

    IdentifyCore (StdHeader, &Socket, &Module, &Core, &IgnoredSts);

    for (Module = 0; Module < (UINT8)GetPlatformNumberOfModules (); Module++) {
      if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredSts)) {
        PciAddress.Address.Function = FUNC_3;
        PciAddress.Address.Register = CPTC0_REG;
        if (IsDramScrubberEnabled (PciAddress, StdHeader)) {
          ((CLK_PWR_TIMING_CTRL_REGISTER *) &OrMask)->StutterScrubEn = 1;
        } else {
          ((CLK_PWR_TIMING_CTRL_REGISTER *) &OrMask)->StutterScrubEn = 0;
        }
        LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
        PciRegister &= AndMask;
        PciRegister |= OrMask;
        LibAmdPciWrite (AccessWidth32, PciAddress, &PciRegister, StdHeader);
      }
    }

  } else if (EntryPoint == CPU_FEAT_AFTER_PM_INIT) {
    // At early, this core 0 can launch its subordinate cores.
    TaskPtr.FuncAddress.PfApTaskI = F10InitializeMsgBasedC1eOnCore;
    TaskPtr.DataTransfer.DataSizeInDwords = 1;
    TaskPtr.DataTransfer.DataPtr = &PlatformConfig->C1ePlatformData;
    TaskPtr.DataTransfer.DataTransferFlags = 0;
    TaskPtr.ExeFlags = WAIT_FOR_CORE;
    ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, NULL);
  }

  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Enable message-based C1e on a family 10h core.
 *
 * @param[in]    BmStsAddress       System I/O address of the bus master status bit.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 */
VOID
STATIC
F10InitializeMsgBasedC1eOnCore (
  IN       VOID              *BmStsAddress,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 MsrRegister;

  // Set MSRC001_0055[SmiOnCmpHalt] = 0, MSRC001_0055[C1eOnCmpHalt] = 0
  LibAmdMsrRead (MSR_INTPEND, &MsrRegister, StdHeader);
  ((INTPEND_MSR *) &MsrRegister)->SmiOnCmpHalt = 0;
  ((INTPEND_MSR *) &MsrRegister)->C1eOnCmpHalt = 0;
  ((INTPEND_MSR *) &MsrRegister)->BmStsClrOnHltEn = 1;
  ((INTPEND_MSR *) &MsrRegister)->IntrPndMsgDis = 0;
  ((INTPEND_MSR *) &MsrRegister)->IntrPndMsg = 0;
  ((INTPEND_MSR *) &MsrRegister)->IoMsgAddr = (UINT64) *((UINT32 *) BmStsAddress);
  LibAmdMsrWrite (MSR_INTPEND, &MsrRegister, StdHeader);

  // Set MSRC001_0015[HltXSpCycEn] = 1
  LibAmdMsrRead (MSR_HWCR, &MsrRegister, StdHeader);
  MsrRegister |= BIT12;
  LibAmdMsrWrite (MSR_HWCR, &MsrRegister, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Check to see if the DRAM background scrubbers are enabled or not.
 *
 * @param[in]    PciAddress         Address of F10 socket/module to check.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               Memory scrubbers are enabled on the current node.
 * @retval       FALSE              Memory scrubbers are disabled on the current node.
 */
BOOLEAN
STATIC
IsDramScrubberEnabled (
  IN       PCI_ADDR PciAddress,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 PciRegister;

  PciAddress.Address.Function = FUNC_3;
  PciAddress.Address.Register = 0x58;
  LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
  return ((BOOLEAN) ((PciRegister & 0x1F) != 0));
}


CONST MSG_BASED_C1E_FAMILY_SERVICES ROMDATA F10MsgBasedC1e =
{
  0,
  F10IsMsgBasedC1eSupported,
  F10InitializeMsgBasedC1e
};
