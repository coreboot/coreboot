/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch USB3 controller
 *
 * Init USB3 features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 47163 $   @e \$Date: 2011-02-16 07:23:13 +0800 (Wed, 16 Feb 2011) $
 *
 */
/*;********************************************************************************
;
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
;*********************************************************************************/
#include "FchPlatform.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_USB_XHCIENV_FILECODE

extern VOID  FchXhciInitBeforePciInit  (IN FCH_DATA_BLOCK* FchDataPtr);
extern VOID  FchXhciInitIndirectReg    (IN AMD_CONFIG_PARAMS *StdHeader);

//
// Declaration of local functions
//
VOID XhciInitBeforePciInit (IN FCH_DATA_BLOCK* FchDataPtr);
VOID XhciInitIndirectReg (IN AMD_CONFIG_PARAMS *StdHeader);

/**
 * FchInitEnvUsbXhci - Config XHCI controller before PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvUsbXhci (
  IN  VOID     *FchDataPtr
  )
{
  UINT8                 XhciEfuse;
  FCH_DATA_BLOCK        *LocalCfgPtr;
  AMD_CONFIG_PARAMS     *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  if ( LocalCfgPtr->Usb.Xhci1Enable == TRUE ) {
    if ( LocalCfgPtr->Misc.S3Resume == 0 ) {
      XhciInitBeforePciInit (LocalCfgPtr);
    } else {
      XhciInitIndirectReg (StdHeader);
    }
  } else {
    //
    // for power saving.
    //
    // add Efuse checking for Xhci enable/disable
    XhciEfuse = XHCI_EFUSE_LOCATION;
    GetEfuseStatus (&XhciEfuse, StdHeader);
    if ((XhciEfuse & (BIT0 + BIT1)) != (BIT0 + BIT1)) {
      RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, 0xF0FFFBFF, 0x0);
    }
  }
}

/**
 * XhciInitIndirectReg - Config XHCI Indirect Registers
 *
 *
 *
 * @param[in] StdHeader AMD Standard Header
 *
 */
VOID
XhciInitIndirectReg (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  FchXhciInitIndirectReg (StdHeader);
}

/**
 * XhciInitBeforePciInit - Config XHCI controller before PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
XhciInitBeforePciInit (
  IN  FCH_DATA_BLOCK     *FchDataPtr
  )
{
  FchXhciInitBeforePciInit ( FchDataPtr );
}
