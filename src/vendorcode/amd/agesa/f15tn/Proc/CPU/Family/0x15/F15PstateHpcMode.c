/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 P-state HPC mode Initialization
 *
 * Enables High performance Computing mode.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "heapManager.h"
#include "cpuF15PowerMgmt.h"
#include "CommonReturns.h"
#include "cpuPstateHpcMode.h"
#include "cpuPstateTables.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_F15PSTATEHPCMODE_FILECODE

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

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * entry point for enabling High Performance Computing.
 *
 * This function must be run after P-states initialization and before enabling low power P-states
 *
 * @param[in]  PstateHpcModeServices   The current CPU's family services.
 * @param[in]  PlatformConfig          Contains the runtime modifiable feature input data.
 * @param[in]  StdHeader               Config handle for library and services.
 *
 * @retval     AGESA_SUCCESS           Always succeeds.
 *
 */
AGESA_STATUS
STATIC
F15InitializePstateHpcMode (
  IN       PSTATE_HPC_MODE_FAMILY_SERVICES  *PstateHpcModeServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT8   OriginalPstate;
  UINT8   X;
  UINT32  Socket;
  UINT32  Module;
  UINT32  Core;
  UINT32  SocketCount;
  UINT32  i;
  UINT64  MsrData;
  PCI_ADDR  PciAddr;
  AGESA_STATUS IgnoredSts;
  AGESA_STATUS Flag;
  F15_CPB_CTRL_REGISTER CpbCtrl;
  CLK_PWR_TIMING_CTRL2_REGISTER CPTC2;
  HTC_REGISTER Htc;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  LOCATE_HEAP_PTR        LocateHeapParams;
  PSTATE_LEVELING        *PStateLevelingBuffer;
  PSTATE_LEVELING        *PStateLevelingBufferTemp;

  Flag = AGESA_SUCCESS;
  // Locate P-State data buffer
  LocateHeapParams.BufferHandle = AMD_PSTATE_DATA_BUFFER_HANDLE;
  if (HeapLocateBuffer (&LocateHeapParams, StdHeader) != AGESA_SUCCESS) {
    Flag = AGESA_ERROR;
    PStateLevelingBuffer = NULL;
    SocketCount = 1;
  } else {
    PStateLevelingBuffer = ((S_CPU_AMD_PSTATE *) (LocateHeapParams.BufferPtr))->PStateLevelingStruc;
    SocketCount = ((S_CPU_AMD_PSTATE *) (LocateHeapParams.BufferPtr))->TotalSocketInSystem;
  }

  // Step1. Read MSRC001_0063[CurPstate] and store the value in OriginalPstate.
  LibAmdMsrRead (MSR_PSTATE_STS, &MsrData, StdHeader);
  OriginalPstate = (UINT8) (((PSTATE_STS_MSR *) &MsrData)->CurPstate);
  // Step2. Write 0 to MSRC001_0062[PstateCmd].
  // Step3. Wait for MSRC001_0063[CurPstate] == 0.
  GetCpuServicesOfCurrentCore (&FamilySpecificServices, StdHeader);
  FamilySpecificServices->TransitionPstate (FamilySpecificServices, (UINT8) 0, (BOOLEAN) TRUE, StdHeader);
  // Step4. If D18F4x15C[NumBoostStates] != D18F3xDC[PstateMaxVal], execute the following sequence
  //   4.A  Set X = D18F4x15C[NumBoostStates].
  //   4.B  If X+1 == D18F3xDC[PstateMaxVal], go to step 5.
  //   4.C  Copy MSRC001_00[6B:64] indexed by P-state X to MSRC001_00[6B:64] indexed by P-state X+1.
  //   4.D  Write 0b to PstateEn from MSRC001_00[6B:64] indexed by P-state X+1.
  //   4.E  Set X = X+1 and go to step B.
  IdentifyCore (StdHeader, &Socket, &Module, &Core, &IgnoredSts);
  GetPciAddress (StdHeader, Socket, Module, &PciAddr, &IgnoredSts);
  PciAddr.Address.Function = FUNC_4;
  PciAddr.Address.Register = CPB_CTRL_REG;
  LibAmdPciRead (AccessWidth32, PciAddr, &CpbCtrl, StdHeader);  // F4x15C

  PciAddr.Address.Function = FUNC_3;
  PciAddr.Address.Register = CPTC2_REG;
  LibAmdPciRead (AccessWidth32, PciAddr, &CPTC2, StdHeader);    // F3xDC

  // In case that F3xDC[PstateMaxVal] was increased by Low Power Pstate function during the first time of running that function.
  // Get the real PstateMaxVal by checking C001_00[6B:64][PsEnable]
  while (CPTC2.PstateMaxVal != 0) {
    LibAmdMsrRead ((PS_REG_BASE + CPTC2.PstateMaxVal), &MsrData, StdHeader);
    if ((MsrData & BIT63) == BIT63) {
      break;
    }
    CPTC2.PstateMaxVal--;
  }

  if (CpbCtrl.NumBoostStates != CPTC2.PstateMaxVal) {
    X = (UINT8) CpbCtrl.NumBoostStates;
    while ((X + 1) < (UINT8) CPTC2.PstateMaxVal) {
      LibAmdMsrRead ((PS_REG_BASE + X), &MsrData, StdHeader);
      MsrData &= ~BIT63;
      LibAmdMsrWrite ((PS_REG_BASE + X + 1), &MsrData, StdHeader);
      // Make sure Agesa doesn't declared the P-states modified by these algorithms to the OS
      if (PStateLevelingBuffer != NULL) {
        PStateLevelingBufferTemp = PStateLevelingBuffer;
        for (i = 0; i < SocketCount; i++) {
          PStateLevelingBufferTemp->PStateCoreStruct[0].PStateStruct[X + 1].PStateEnable = 0;
          //Calculate next node buffer address
          PStateLevelingBufferTemp = (PSTATE_LEVELING *) ((UINT8 *) PStateLevelingBufferTemp + (UINTN) sizeof (PSTATE_LEVELING) + (UINTN) (PStateLevelingBufferTemp->PStateCoreStruct[0].PStateMaxValue * sizeof (S_PSTATE_VALUES)));
        }
      }
      X++;
    }
  }
  // Step5. Write OriginalPstate to MSRC001_0062[PstateCmd].
  // Step6. Wait for MSRC001_0063[CurPstate] == OriginalPstate.
  FamilySpecificServices->TransitionPstate (FamilySpecificServices, OriginalPstate, (BOOLEAN) TRUE, StdHeader);
  // Step7. Write D18F3x64[HtcPstateLimit] with the value from D18F3xDC[PstateMaxVal]
  PciAddr.Address.Register = HTC_REG;
  LibAmdPciRead (AccessWidth32, PciAddr, &Htc, StdHeader);      // F3x64
  Htc.HtcPstateLimit = CPTC2.PstateMaxVal;
  LibAmdPciWrite (AccessWidth32, PciAddr, &Htc, StdHeader);     // F3x64

  return Flag;
}



CONST PSTATE_HPC_MODE_FAMILY_SERVICES ROMDATA F15PstateHpcSupport =
{
  0,
  F15InitializePstateHpcMode
};
