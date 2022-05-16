/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH PCI access lib
 *
 *
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
#define FILECODE PROC_FCH_COMMON_PCILIB_FILECODE

VOID
ReadPci (
  IN       UINT32                  Address,
  IN       UINT8                   OpFlag,
  IN       VOID*                   Value,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  PCI_ADDR  PciAddress;

  PciAddress.AddressValue = ((Address >> 4) & ~0xFFF) + (Address & 0xFFF);
  LibAmdPciRead ((ACCESS_WIDTH) OpFlag, PciAddress, Value, StdHeader);
}


VOID
WritePci (
  IN       UINT32                  Address,
  IN       UINT8                   OpFlag,
  CONST IN       VOID                    *Value,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  PCI_ADDR  PciAddress;

  PciAddress.AddressValue = ((Address >> 4) & ~0xFFF) + (Address & 0xFFF);
  LibAmdPciWrite ((ACCESS_WIDTH) OpFlag, PciAddress, Value, StdHeader);
}


VOID
RwPci (
  IN       UINT32                  Address,
  IN       UINT8                   OpFlag,
  IN       UINT32                  Mask,
  IN       UINT32                  Data,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  PCI_ADDR  PciAddress;
  UINT32    rMask;

  PciAddress.AddressValue = ((Address >> 4) & ~0xFFF) + (Address & 0xFFF);
  rMask = ~Mask;
  LibAmdPciRMW ((ACCESS_WIDTH) OpFlag, PciAddress, &Data, &rMask, StdHeader);
}
