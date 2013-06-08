/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch GPP controller
 *
 * Init GPP features.
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
#include "Ids.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_PCIE_GPPHP_FILECODE

VOID
FchGppHotplugSmiCallback (
  IN       VOID           *DataPtr
  );

/**
 * GPP hot plug handler
 *
 *
 * @param[in] FchGpp      Pointer to Fch GPP configuration structure
 * @param[in] HpPort      The hot plug port number
 * @param[in] StdHeader   Pointer to AMD_CONFIG_PARAMS
 *
 */
STATIC VOID
FchGppHotPlugSmiProcess (
  IN       FCH_GPP             *FchGpp,
  IN       UINT32              HpPort,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8                  FailedPort;
  UINT8                  GppS3Data;

  GppS3Data = 0x00;
  ReadMem ( ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x0D, AccessWidth8, &GppS3Data);
  RwAlink (FCH_RCINDXC_REG40, (UINT32)~BIT3, 0, StdHeader);

  //
  // First restore GPP pads if needed
  //
  if (FchGpp->GppDynamicPowerSaving && FchGpp->UmiPhyPllPowerDown && FchGpp->GppPhyPllPowerDown) {
    RwAlink (0xC0  | (UINT32) (ABCFG << 29), ~(UINT32) (1 << (12 + HpPort)), 0, StdHeader);
    RwAlink (FCH_RCINDXC_REG65, ~(UINT32) (0x101 << HpPort), 0, StdHeader);
    FchStall (1000, StdHeader);
  }

  FailedPort = (UINT8) (1 << HpPort);
  if (FchGpp->GppGen2 && FchGpp->GppGen2Strap) {
    GppS3Data &= (UINT8) !(1 << HpPort);
    if (GppPortPollingLtssm (FchGpp, FailedPort, TRUE, StdHeader)) {
      FchGppForceGen1 (FchGpp, FailedPort, StdHeader);
      FailedPort = GppPortPollingLtssm (FchGpp, FailedPort, FALSE, StdHeader);
      GppS3Data |= (UINT8) (1 << HpPort);
    }
  } else {
    FchGppForceGen1 (FchGpp, FailedPort, StdHeader);
    FailedPort = GppPortPollingLtssm (FchGpp, FailedPort, FALSE, StdHeader);
    GppS3Data |= (UINT8) (1 << HpPort);
  }
  GppS3Data |= (UINT8) (1 << (HpPort + 4));
  RwMem (ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x0D, AccessWidth8, 0, GppS3Data);
  GppGen2Workaround (FchGpp, StdHeader);
}


/**
 * GPP hot-unplug handler
 *
 *
 * @param[in] FchGpp      Pointer to Fch GPP configuration structure
 * @param[in] HpPort      The hot plug port number.
 * @param[in] StdHeader   Pointer to AMD_CONFIG_PARAMS
 *
 */
STATIC VOID
FchGppHotUnplugSmiProcess (
  IN       FCH_GPP             *FchGpp,
  IN       UINT32              HpPort,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8                  GppS3Data;

  GppS3Data = 0x00;
  ReadMem ( ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x0D, AccessWidth8, &GppS3Data);
  FchGpp->PortCfg[HpPort].PortDetected = FALSE;
  GppS3Data &= (UINT8) !(1 << (HpPort + 4));

  if (FchGpp->GppGen2 && FchGpp->GppGen2Strap) {
    FchGppForceGen2 (FchGpp, (UINT8) (1 << HpPort), StdHeader);
  }

  if (FchGpp->GppDynamicPowerSaving && FchGpp->UmiPhyPllPowerDown && FchGpp->GppPhyPllPowerDown) {
    RwAlink (FCH_RCINDXP_REGA2 | HpPort << 24, ~(UINT32) (BIT17), BIT17, StdHeader);
    RwAlink (FCH_RCINDXP_REGA2 | HpPort << 24, ~(UINT32) (BIT8), BIT8, StdHeader);
    RwAlink (0xC0  | (UINT32) (ABCFG << 29), ~(UINT32) (1 << (12 + HpPort)), (1 << (12 + HpPort)), StdHeader);
    RwAlink (FCH_RCINDXP_REGA2 | HpPort << 24, ~(UINT32) (BIT17), 0, StdHeader);

    GppGen2Workaround (FchGpp, StdHeader);

    // Finally re-configure GPP pads if needed
    FchGppDynamicPowerSaving (FchGpp, StdHeader);
  }
  RwMem (ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x0D, AccessWidth8, 0, GppS3Data);
}


/**
 * SMI handler for GPP hot-plug
 *
 *
 * @param[in] DataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchGppHotplugSmiCallback (
  IN       VOID           *DataPtr
  )
{
  UINT32             PortNum;
  UINT32             HpPort;
  FCH_DATA_BLOCK     *FchDb;
  UINT8              HpGeventNum;
  UINT8              GpioPinState;

  FchDb = (FCH_DATA_BLOCK*) DataPtr;
  if (!FchDb->Gpp.GppFunctionEnable) {
    return;
  }

  HpPort = 0xff;
  for (PortNum = 0; PortNum < MAX_GPP_PORTS; PortNum++) {
    if (FchDb->Gpp.PortCfg[PortNum].PortHotPlug == TRUE) {
      HpPort = PortNum;
      break;
    }
  }

  if (HpPort == 0xff) {
    return;
  }

  HpGeventNum = FchDb->Gpp.GppHotPlugGeventNum & 31;
  GpioPinState = ACPIMMIO8 (ACPI_MMIO_BASE + GPIO_BASE + FCH_GEVENT_REG00 + HpGeventNum) >> 7;
  if (!GpioPinState) {
    AGESA_TESTPOINT (TpFchGppHotPlugging, FchDb->StdHeader);
    FchGppHotPlugSmiProcess (&FchDb->Gpp, HpPort, FchDb->StdHeader);
  } else {
    AGESA_TESTPOINT (TpFchGppHotUnplugging, FchDb->StdHeader);
    FchGppHotUnplugSmiProcess (&FchDb->Gpp, HpPort, FchDb->StdHeader);
  }

  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98) ^= (1 << HpGeventNum);     // Swap SmiTrig
}
