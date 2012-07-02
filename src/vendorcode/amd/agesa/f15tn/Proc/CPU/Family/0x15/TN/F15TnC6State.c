/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Trinity C6 C-state feature support functions.
 *
 * Provides the functions necessary to initialize the C6 feature.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F15/TN
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
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuFeatures.h"
#include "cpuC6State.h"
#include "cpuApicUtilities.h"
#include "cpuF15PowerMgmt.h"
#include "cpuF15TnPowerMgmt.h"
#include "cpuEarlyInit.h"
#include "cpuServices.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_TN_F15TNC6STATE_FILECODE

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
F15TnReloadMicrocodePatchAfterMemInit (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 *  Is C6 supported on this CPU
 *
 * @param[in]    C6Services         Pointer to this CPU's C6 family services.
 * @param[in]    Socket             This core's zero-based socket number.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               C6 state is supported.
 * @retval       FALSE              C6 state is not supported.
 *
 */
BOOLEAN
STATIC
F15TnIsC6Supported (
  IN       C6_FAMILY_SERVICES      *C6Services,
  IN       UINT32                  Socket,
  IN       PLATFORM_CONFIGURATION  *PlatformConfig,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  ASSERT (IsFeatureEnabled (CacheFlushOnHalt, PlatformConfig, StdHeader) == TRUE);
  // Assuming CFOH is always enabled.
  return (IsFeatureEnabled (IoCstate, PlatformConfig, StdHeader));
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Enable C6 on a family 15h CPU.
 *
 * @param[in]    C6Services         Pointer to this CPU's C6 family services.
 * @param[in]    EntryPoint         Timepoint designator.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @return       AGESA_SUCCESS      Always succeeds.
 *
 */
AGESA_STATUS
STATIC
F15TnInitializeC6 (
  IN       C6_FAMILY_SERVICES *C6Services,
  IN       UINT64 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  PCI_ADDR                PciAddress;
  CSTATE_CTRL1_REGISTER   CstateCtrl1;
  POPUP_PSTATE_REGISTER   PopDownPstate;
  CLK_PWR_TIMING_CTRL2_REGISTER   ClkPwrTimingCtrl2;

  if ((EntryPoint & CPU_FEAT_AFTER_PM_INIT) != 0) {
    // Initialize F4x118
    PciAddress.AddressValue = CSTATE_CTRL1_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &CstateCtrl1, StdHeader);
    // Set C-state Action Field 0
    CstateCtrl1.PwrGateEnCstAct0 = 1;
    CstateCtrl1.PwrOffEnCstAct0 = 1;
    CstateCtrl1.NbPwrGate0 = 1;
    CstateCtrl1.NbClkGate0 = 1;
    CstateCtrl1.SelfRefr0 = 1;
    CstateCtrl1.CpuPrbEnCstAct0 = 1;
    // Set C-state Action Field 1
    CstateCtrl1.PwrGateEnCstAct1 = 1;
    CstateCtrl1.PwrOffEnCstAct1 = 1;
    CstateCtrl1.NbPwrGate1 = 1;
    CstateCtrl1.NbClkGate1 = 1;
    CstateCtrl1.SelfRefr1 = 1;
    CstateCtrl1.CpuPrbEnCstAct1 = 1;
    LibAmdPciWrite (AccessWidth32, PciAddress, &CstateCtrl1, StdHeader);

    // Initialize F3xA8[PopDownPstate] = F3xDC[PstateMaxVal]
    PciAddress.AddressValue = CPTC2_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &ClkPwrTimingCtrl2, StdHeader);
    PciAddress.AddressValue = POPUP_PSTATE_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &PopDownPstate, StdHeader);
    PopDownPstate.PopDownPstate = ClkPwrTimingCtrl2.PstateMaxVal;
    LibAmdPciWrite (AccessWidth32, PciAddress, &PopDownPstate, StdHeader);
  }

  return AGESA_SUCCESS;
}

/**
 *  Reload microcode patch after memory is initialized.
 *
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 */
VOID
F15TnReloadMicrocodePatchAfterMemInit (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  LoadMicrocodePatch (StdHeader);
}

CONST C6_FAMILY_SERVICES ROMDATA F15TnC6Support =
{
  0,
  F15TnIsC6Supported,
  F15TnInitializeC6,
  F15TnReloadMicrocodePatchAfterMemInit
};
