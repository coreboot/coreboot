/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 specific utility functions.
 *
 * Provides numerous utility functions specific to family 15h.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 *****************************************************************************
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
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuApicUtilities.h"
#include "cpuFamilyTranslation.h"
#include "cpuCommonF15Utilities.h"
#include "cpuF15PowerMgmt.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X15_CPUCOMMONF15UTILITIES_FILECODE


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
 *  Set warm reset status and count
 *
 *  @CpuServiceMethod{::F_CPU_SET_WARM_RESET_FLAG}.
 *
 *  This function will use bit9, and bit 10 of register F0x6C as a warm reset status and count.
 *
 *  @param[in]  FamilySpecificServices   The current Family Specific Services.
 *  @param[in]  StdHeader                Handle of Header for calling lib functions and services.
 *  @param[in]  Request                  Indicate warm reset status
 *
 */
VOID
F15SetAgesaWarmResetFlag (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       WARM_RESET_REQUEST *Request
  )
{
  PCI_ADDR  PciAddress;
  UINT32    PciData;

  PciAddress.AddressValue = MAKE_SBDFO (0, 0 , PCI_DEV_BASE, FUNC_0, HT_INIT_CTRL);
  LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);

  // bit[5] - indicate a warm reset is or is not required
  PciData &= ~(HT_INIT_BIOS_RST_DET_0);
  PciData = PciData | (Request->RequestBit << 5);

  // bit[10,9] - indicate warm reset status and count
  PciData &= ~(HT_INIT_BIOS_RST_DET_1 | HT_INIT_BIOS_RST_DET_2);
  PciData |= Request->StateBits << 9;

  LibAmdPciWrite (AccessWidth32, PciAddress, &PciData, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Get warm reset status and count
 *
 *  @CpuServiceMethod{::F_CPU_GET_WARM_RESET_FLAG}.
 *
 *  This function will bit9, and bit 10 of register F0x6C as a warm reset status and count.
 *
 *  @param[in]  FamilySpecificServices   The current Family Specific Services.
 *  @param[in]  StdHeader                Config handle for library and services
 *  @param[out] Request                  Indicate warm reset status
 *
 */
VOID
F15GetAgesaWarmResetFlag (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS *StdHeader,
     OUT   WARM_RESET_REQUEST *Request
  )
{
  PCI_ADDR  PciAddress;
  UINT32    PciData;

  PciAddress.AddressValue = MAKE_SBDFO (0, 0 , PCI_DEV_BASE, FUNC_0, HT_INIT_CTRL);
  LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);

  // bit[5] - indicate a warm reset is or is not required
  Request->RequestBit = (UINT8) ((PciData & HT_INIT_BIOS_RST_DET_0) >> 5);
  // bit[10,9] - indicate warm reset status and count
  Request->StateBits = (UINT8) ((PciData & (HT_INIT_BIOS_RST_DET_1 | HT_INIT_BIOS_RST_DET_2)) >> 9);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Return a number zero or one, based on the Core ID position in the initial APIC Id.
 *
 * @CpuServiceMethod{::F_CORE_ID_POSITION_IN_INITIAL_APIC_ID}.
 *
 * @param[in]     FamilySpecificServices  The current Family Specific Services.
 * @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 * @retval        CoreIdPositionZero      Core Id is not low
 * @retval        CoreIdPositionOne       Core Id is low
 */
CORE_ID_POSITION
F15CpuAmdCoreIdPositionInInitialApicId (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT64 InitApicIdCpuIdLo;

  //  Check bit_54 [InitApicIdCpuIdLo] to find core id position.
  LibAmdMsrRead (MSR_NB_CFG, &InitApicIdCpuIdLo, StdHeader);
  InitApicIdCpuIdLo = ((InitApicIdCpuIdLo & BIT54) >> 54);
  return ((InitApicIdCpuIdLo == 0) ? CoreIdPositionZero : CoreIdPositionOne);
}

