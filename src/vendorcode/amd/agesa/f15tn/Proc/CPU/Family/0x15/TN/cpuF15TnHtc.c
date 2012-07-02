/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 TN HTC Initialization
 *
 * Enables Hardware Thermal Control (HTC) feature
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/TN
 * @e \$Revision: 63692 $   @e \$Date: 2012-01-03 22:13:28 -0600 (Tue, 03 Jan 2012) $
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
#include "cpuRegisters.h"
#include "cpuF15PowerMgmt.h"
#include "cpuF15TnPowerMgmt.h"
#include "cpuFeatures.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "CommonReturns.h"
#include "cpuHtc.h"
#include "OptionMultiSocket.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X15_TN_CPUF15TNHTC_FILECODE

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
F15TnHtcInit (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       PCI_ADDR                PciAddress,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern OPTION_MULTISOCKET_CONFIGURATION   OptionMultiSocketConfiguration;

/*---------------------------------------------------------------------------------------*/
/**
 * Entry point for enabling Hardware Thermal Control
 *
 * This function must be run after all P-State routines have been executed
 *
 * @param[in]  HtcServices             The current CPU's family services.
 * @param[in]  EntryPoint              Timepoint designator.
 * @param[in]  PlatformConfig          Platform profile/build option config structure.
 * @param[in]  StdHeader               Config handle for library and services.
 *
 * @retval     AGESA_SUCCESS           Always succeeds.
 *
 */
AGESA_STATUS
STATIC
F15TnInitializeHtc (
  IN       HTC_FAMILY_SERVICES    *HtcServices,
  IN       UINT64                  EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  PCI_ADDR                  PciAddress;
  CPU_SPECIFIC_SERVICES     *FamilySpecificServices;

  if ((EntryPoint & (CPU_FEAT_AFTER_POST_MTRR_SYNC | CPU_FEAT_AFTER_RESUME_MTRR_SYNC)) != 0) {
    GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
    PciAddress.AddressValue = MAKE_SBDFO (0, 0, 24, 0, 0);
    F15TnHtcInit (FamilySpecificServices, PlatformConfig, PciAddress, StdHeader);
  }

  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 *    Main entry point for initializing the Thermal Control
 *    safety net feature.
 *
 *    This must be run by all Family 15h Trinity core 0s in the system.
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  PlatformConfig          Platform profile/build option config structure
 * @param[in]  PciAddress              Segment, bus, device number of the node to transition.
 * @param[in]  StdHeader               Config handle for library and services.
 */
VOID
F15TnHtcInit (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       PCI_ADDR                PciAddress,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32    LocalPciRegister;

  PciAddress.AddressValue = NB_CAPS_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  if (((NB_CAPS_REGISTER *) &LocalPciRegister)->HtcCapable == 1) {
    // Enable HTC
    PciAddress.Address.Register = HTC_REG;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    if (((HTC_REGISTER *) &LocalPciRegister)->HtcTmpLmt != 0) {
      // Enable HTC
      ((HTC_REGISTER *) &LocalPciRegister)->HtcEn = 1;
    } else {
      // Disable HTC
      ((HTC_REGISTER *) &LocalPciRegister)->HtcEn = 0;
    }
    IDS_OPTION_HOOK (IDS_HTC_CTRL, &LocalPciRegister, StdHeader);
    LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  }
}

CONST HTC_FAMILY_SERVICES ROMDATA F15TnHtcSupport =
{
  0,
  (PF_HTC_IS_SUPPORTED) CommonReturnTrue,
  F15TnInitializeHtc
};
