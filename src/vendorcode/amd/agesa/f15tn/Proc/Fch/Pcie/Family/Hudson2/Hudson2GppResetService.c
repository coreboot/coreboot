/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Hudson2 Pcie controller
 *
 * Init GPP (pcie Controller) features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
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
****************************************************************************
*/
#include "FchPlatform.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_PCIE_FAMILY_HUDSON2_HUDSON2GPPRESETSERVICE_FILECODE


/**
 * ProgramFchGppInitReset - Config Gpp at PowerOnReset
 *
 *
 * @param[in] FchGpp      Pointer to Fch GPP configuration structure
 * @param[in] StdHeader   Pointer to AMD_CONFIG_PARAMS
 *
 */
VOID
ProgramFchGppInitReset (
  IN       FCH_GPP             *FchGpp,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  //
  // Toggle GEVENT4 to reset all GPP devices
  //
  ProgramGppTogglePcieReset (FchGpp->GppToggleReset, StdHeader);
  if (FchGpp->SerialDebugBusEnable) {
    RwAlink (FCH_ABCFG_REGC0, (UINT32) (ABCFG << 29), (UINT32)~BIT12, 0x00);
  }
}

/**
 * FchResetPcie - Toggle GEVENT4 to assert/deassert GPP device
 * reset
 *
 *
 * @param[in] ResetBlock - PCIE reset for FCH GPP or NB PCIE
 * @param[in] ResetOp    - Assert or deassert PCIE reset
 * @param[in] StdHeader
 *
 */
VOID
FchResetPcie (
  IN       RESET_BLOCK         ResetBlock,
  IN       RESET_OP            ResetOp,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8    Or8;
  UINT8    Mask8;

  if (ResetBlock == NbBlock) {
    if (ResetOp == AssertReset) {
      Or8 = BIT4;
      Mask8 = 0;
      LibAmdMemRMW (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGC4), &Or8, &Mask8, StdHeader);
    } else if (ResetOp == DeassertReset) {
      Or8 = 0;
      Mask8 = BIT4;
      LibAmdMemRMW (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGC4), &Or8, &Mask8, StdHeader);
    }
  } else if (ResetBlock == FchBlock) {
    Or8 = BIT1;
    Mask8 = BIT1 + BIT0;
    LibAmdMemRMW (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + IOMUX_BASE + FCH_GEVENT_REG04), &Or8, &Mask8, StdHeader);
    if (ResetOp == AssertReset) {
      Or8 = 0;
      Mask8 = BIT5;
      LibAmdMemRMW (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + GPIO_BASE + FCH_GEVENT_REG04), &Or8, &Mask8, StdHeader);
      Or8 = BIT4;
      Mask8 = 0;
      LibAmdMemRMW (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGBF), &Or8, &Mask8, StdHeader);
    } else if (ResetOp == DeassertReset) {
      Or8 = 0;
      Mask8 = BIT4;
      LibAmdMemRMW (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGBF), &Or8, &Mask8, StdHeader);
      Or8 = BIT5;
      Mask8 = 0;
      LibAmdMemRMW (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + GPIO_BASE + FCH_GEVENT_REG04), &Or8, &Mask8, StdHeader);
    }
  }
}

