/**
 * @file
 *
 * Config Southbridge GEC controller
 *
 * Init GEC features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-SB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*
 *****************************************************************************
 *
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 * ***************************************************************************
 *
 */

#include "SBPLATFORM.h"

/**
 * gecInitBeforePciEnum - Config GEC controller before PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
gecInitBeforePciEnum (
  IN       AMDSBCFG*   pConfig
  )
{
  UINT8 cimSBGecDebugBus;
  UINT8 cimSBGecPwr;

  cimSBGecDebugBus = (UINT8) pConfig->SBGecDebugBus;
  cimSBGecPwr = (UINT8) pConfig->SBGecPwr;
#if  SB_CIMx_PARAMETER == 0
  cimSBGecDebugBus = cimSBGecDebugBusDefault;
  cimSBGecPwr = cimSBGecPwrDefault;
#endif
  if ( pConfig->GecConfig == 0) {
    // GEC Enabled
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF6, AccWidthUint8, ~BIT0, 0x00);
    RWMEM (ACPI_MMIO_BASE + IOMUX_BASE + SB_GEVENT_REG11, AccWidthUint8, 0, 0x00);
    RWMEM (ACPI_MMIO_BASE + IOMUX_BASE + SB_GEVENT_REG21, AccWidthUint8, 0, 0x01);
    RWMEM (ACPI_MMIO_BASE + IOMUX_BASE + SB_GPIO_REG166, AccWidthUint8, 0, 0x01);
    //RWMEM (ACPI_MMIO_BASE + IOMUX_BASE + SB_GPIO_REG181, AccWidthUint8, 0, 0x01);
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF8, AccWidthUint8, ~(BIT5 + BIT6), (UINT8) ((cimSBGecPwr) << 5));
  } else {
    // GEC Disabled
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF6, AccWidthUint8, ~BIT0, BIT0);
    return;   //return if GEC controller is disabled.
  }
  if (  cimSBGecDebugBus == 1) {
    // GEC Debug Bus Enabled
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF6, AccWidthUint8, ~BIT3, BIT3);
  } else {
    // GEC Debug Bus Disabled
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF6, AccWidthUint8, ~BIT3, 0x00);
  }
}

/**
 * gecInitAfterPciEnum - Config GEC controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
gecInitAfterPciEnum (
  IN       AMDSBCFG* pConfig
  )
{
  VOID* GecRomAddress;
  VOID* GecShadowRomAddress;
  UINT32 ddTemp;
  UINT8  dbVar;
  UINT8  dbTemp;
  if ( pConfig->GecConfig == 0) {
    dbVar = 0;
    ReadPCI ((GEC_BUS_DEV_FUN << 16) + SB_GEC_REG04, AccWidthUint8, &dbVar);
    dbTemp = 0x07;
    WritePCI ((GEC_BUS_DEV_FUN << 16) + SB_GEC_REG04, AccWidthUint8, &dbTemp);
    if ( !pConfig->DYNAMICGECROM.DynamicGecRomAddress_Ptr == NULL ) {
      GecRomAddress = pConfig->DYNAMICGECROM.DynamicGecRomAddress_Ptr;
      GecShadowRomAddress = (VOID*) (UINTN) pConfig->BuildParameters.GecShadowRomBase;
      AmdSbCopyMem (GecShadowRomAddress, GecRomAddress, 0x100);
      ReadPCI ((GEC_BUS_DEV_FUN << 16) + SB_GEC_REG10, AccWidthUint32, &ddTemp);
      ddTemp = ddTemp & 0xFFFFFFF0;
      RWMEM (ddTemp + 0x6804, AccWidthUint32, 0, BIT0 + BIT29);
    }
    WritePCI ((GEC_BUS_DEV_FUN << 16) + SB_GEC_REG04, AccWidthUint8, &dbVar);
  }
}

/**
 * gecInitLatePost - Prepare GEC controller to boot to OS.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
gecInitLatePost (
  IN       AMDSBCFG* pConfig
  )
{
  if ( !pConfig->GecConfig == 0) {
    return;   //return if GEC controller is disabled.
  }
}


