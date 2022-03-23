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
 * ***************************************************************************
 *
 */

#include "SBPLATFORM.h"
#include "cbtypes.h"

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
    if ( pConfig->DYNAMICGECROM.DynamicGecRomAddress_Ptr != NULL ) {
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
  /* if ( !pConfig->GecConfig == 0) { */
  /*   return;   //return if GEC controller is disabled. */
  /* } */
}
