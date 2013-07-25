/* $NoKeywords:$ */
/**
 * @file
 *
 * Family specific PCIe definitions
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
#ifndef _PCIECOMPLEXDATAKB_H_
#define _PCIECOMPLEXDATAKB_H_

#define MAX_NUM_PHYs            2
#define MAX_NUM_LANE_PER_PHY    8

#define NUMBER_OF_GPP_PORTS       5
#define NUMBER_OF_DDI_DDIS        3

#define NON_INITIALIZED_PCI_ADDRESS  0

#define GPP_WRAP_ID               0
#define GPP_START_PHY_LANE        0
#define GPP_END_PHY_LANE          7
#define GPP_CORE_ID               0
#define GPP_NUMBER_OF_PIFs        1

#define DDI_WRAP_ID               3
#define DDI_START_PHY_LANE        8
#define DDI_END_PHY_LANE          19
#define DDI_NUMBER_OF_PIFs        1

// PBR0
#define PBR0                      0
#define PBR0_NATIVE_PCI_DEV       2
#define PBR0_NATIVE_PCI_FUN       1
#define PBR0_CORE_ID              GPP_CORE_ID
#define PBR0_PORT_ID              0
#define PBR0_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR0_UNIT_ID              0x4
#define PBR0_NUM_UNIT_IDs         0x1

// PBR1
#define PBR1                      1
#define PBR1_NATIVE_PCI_DEV       2
#define PBR1_NATIVE_PCI_FUN       2
#define PBR1_CORE_ID              GPP_CORE_ID
#define PBR1_PORT_ID              1
#define PBR1_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR1_UNIT_ID              0x5
#define PBR1_NUM_UNIT_IDs         0x1

// PBR2
#define PBR2                      2
#define PBR2_NATIVE_PCI_DEV       2
#define PBR2_NATIVE_PCI_FUN       3
#define PBR2_CORE_ID              GPP_CORE_ID
#define PBR2_PORT_ID              2
#define PBR2_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR2_UNIT_ID              0x6
#define PBR2_NUM_UNIT_IDs         0x1

// PBR3
#define PBR3                      3
#define PBR3_NATIVE_PCI_DEV       2
#define PBR3_NATIVE_PCI_FUN       4
#define PBR3_CORE_ID              GPP_CORE_ID
#define PBR3_PORT_ID              3
#define PBR3_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR3_UNIT_ID              0x7
#define PBR3_NUM_UNIT_IDs         0x1

// PBR4
#define PBR4                      4
#define PBR4_NATIVE_PCI_DEV       2
#define PBR4_NATIVE_PCI_FUN       5
#define PBR4_CORE_ID              GPP_CORE_ID
#define PBR4_PORT_ID              4
#define PBR4_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR4_UNIT_ID              0x8
#define PBR4_NUM_UNIT_IDs         0x1

#define MaxDevNum                 4
#define MaxDevFunc                5

#define GPP_CORE_x4x4             ((4ull << 8) | (4ull << 0))
#define GPP_CORE_x4x2x2           ((2ull << 16) | (2ull << 8) | (4ull << 0))
#define GPP_CORE_x4x2x1x1         ((1ull << 24) | (1ull << 16) | (2ull << 8) | (4ull << 0))
#define GPP_CORE_x4x1x1x1x1       ((1ull << 32) | (1ull << 24) | (1ull << 16) | (1ull << 8) | (4ull << 0))

///Family specific silicon configuration
typedef struct {
  UINT8     PortDevMap [5];                      ///< Device number that has beed allocated already
} KB_PCIe_SILICON_CONFIG;


/// Complex Configuration for silicon module
typedef struct {
  PCIe_SILICON_CONFIG     Silicon;                ///< Silicon

  PCIe_WRAPPER_CONFIG     GppWrapper;            ///< GPP Wrapper
  PCIe_WRAPPER_CONFIG     DdiWrapper;            ///< DDI   Wrapper

  // GPP
  PCIe_ENGINE_CONFIG      PortPBR4;               ///< Port PBR6
  PCIe_ENGINE_CONFIG      PortPBR3;               ///< Port PBR5
  PCIe_ENGINE_CONFIG      PortPBR2;               ///< Port PBR4
  PCIe_ENGINE_CONFIG      PortPBR1;               ///< Port PBR3
  PCIe_ENGINE_CONFIG      PortPBR0;               ///< Port PBR2

  // DDI
  PCIe_ENGINE_CONFIG      Ddi1;                   ///< Ddi1
  PCIe_ENGINE_CONFIG      Ddi2;                   ///< Ddi2
  PCIe_ENGINE_CONFIG      Vga;                    ///< Vga
  KB_PCIe_SILICON_CONFIG  FmSilicon;              ///< Fm silicon config
} KB_COMPLEX_CONFIG;


VOID
PcieSetPortPciAddressMapKB (
  IN      PCIe_SILICON_CONFIG     *Silicon
  );
#endif
