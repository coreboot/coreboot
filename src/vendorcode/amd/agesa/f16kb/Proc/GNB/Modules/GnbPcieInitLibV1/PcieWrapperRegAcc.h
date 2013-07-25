/* $NoKeywords:$ */
/**
 * @file
 *
 * Supporting services to access PCIe wrapper/core/PIF/PHY indirect register spaces
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
* ***************************************************************************
*
*/

#ifndef _PCIEWRAPPERREGACC_H_
#define _PCIEWRAPPERREGACC_H_

//#define WRAP_SPACE(w, x)   (0x01300000ul | (w << 16) | (x))
//#define CORE_SPACE(c, x)   (0x00010000ul | (c << 24) | (x))
//#define PHY_SPACE(w, p, x) (0x00200000ul | ((p + 1) << 24) | (w << 16) | (x))
//#define PIF_SPACE(w, p, x) (0x00100000ul | ((p + 1) << 24) | (w << 16) | (x))
#define IMP_SPACE(x) (0x01080000ul | (x))

UINT32
PcieRegisterRead (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PcieRegisterWrite (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

UINT32
PcieRegisterReadField (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      UINT8                 FieldOffset,
  IN      UINT8                 FieldWidth,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PcieRegisterWriteField (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      UINT8                 FieldOffset,
  IN      UINT8                 FieldWidth,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PcieRegisterRMW (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      UINT32                AndMask,
  IN      UINT32                OrMask,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

UINT32
PcieSiliconRegisterRead (
  IN      PCIe_SILICON_CONFIG   *Silicon,
  IN      UINT32                Address,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PcieSiliconRegisterWrite (
  IN      PCIe_SILICON_CONFIG   *Silicon,
  IN      UINT32                Address,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PcieSiliconRegisterRMW (
  IN      PCIe_SILICON_CONFIG   *Silicon,
  IN      UINT32                Address,
  IN      UINT32                AndMask,
  IN      UINT32                OrMask,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

#endif
