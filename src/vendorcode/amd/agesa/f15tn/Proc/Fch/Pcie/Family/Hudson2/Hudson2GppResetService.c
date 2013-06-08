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

