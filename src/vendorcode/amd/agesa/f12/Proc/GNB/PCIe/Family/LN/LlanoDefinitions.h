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
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
* ***************************************************************************
*
*/
#ifndef _LLANODEFINITIONS_H_
#define _LLANODEFINITIONS_H_

#define SOCKET_ID           0

#define MAX_NUM_PHYs          2
#define MAX_NUM_LANE_PER_PHY  8

#define NUMBER_OF_PORTS     8
#define NUMBER_OF_GPP_PORTS 5
#define NUMBER_OF_GFX_PORTS 2
#define NUMBER_OF_GFX_DDIS  4
#define NUMBER_OF_DDIS      2
#define NUMBER_OF_WRAPPERS  3
#define NUMBER_OF_SILICONS  1

#define GFX_WRAP_ID         1
#define GFX_NUMBER_OF_PIFs  2
#define GFX_START_PHY_LANE  8
#define GFX_END_PHY_LANE    23
#define GFX_CORE_ID         2

#define GFX_CORE_x16        ((16 << 8) | 0)
#define GFX_CORE_x8x8       ((8 << 8) | 8)

#define GPP_WRAP_ID         0
#define GPP_NUMBER_OF_PIFs  1
#define GPP_START_PHY_LANE  0
#define GPP_END_PHY_LANE    7
#define GPP_CORE_ID         1

#define GPP_CORE_x4x1x1x1x1   ((1ull << 32) | (1ull << 24) | (1ull << 16) | (1ull << 8) | (4ull << 0))
#define GPP_CORE_x4x2x1x1     ((2ull << 32) | (1ull << 24) | (1ull << 16) | (0ull << 8) | (4ull << 0))
#define GPP_CORE_x4x2x1x1_ST  ((2ull << 32) | (0ull << 24) | (1ull << 16) | (1ull << 8) | (4ull << 0))
#define GPP_CORE_x4x2x2       ((2ull << 32) | (2ull << 24) | (0ull << 16) | (0ull << 8) | (4ull << 0))
#define GPP_CORE_x4x2x2_ST    ((2ull << 32) | (0ull << 24) | (2ull << 16) | (0ull << 8) | (4ull << 0))
#define GPP_CORE_x4x4         ((4ull << 32) | (0ull << 24) | (0ull << 16) | (0ull << 8) | (4ull << 0))

#define DDI_WRAP_ID         2
#define DDI_NUMBER_OF_PIFs  1
#define DDI_START_PHY_LANE  24
#define DDI_END_PHY_LANE    31

///Gen2 capability
typedef enum {
  OscFuses,                   ///< Not capable
  OscRO,                      ///< Gen2 with RO
  OscLC,                      ///< Gen2 with LC
  OscDefault,                 ///< Skip initialization of OSC
} OSC_MODE;

///Family specific silicon configuration
typedef struct {
  OSC_MODE  OscMode;          ///<OSC mode
} F12_PCIe_SILICON_CONFIG;


/// Complex Configuration
typedef struct {
  PCIe_SILICON_CONFIG     Silicon;                ///< Silicon
  PCIe_WRAPPER_CONFIG     GfxWrapper;             ///< Graphics Wrapper
  PCIe_WRAPPER_CONFIG     GppWrapper;             ///< General Purpose Port
  PCIe_WRAPPER_CONFIG     DdiWrapper;             ///< DDI
  PCIe_ENGINE_CONFIG      Port2;                  ///< Port 2
  PCIe_ENGINE_CONFIG      Port3;                  ///< Port 3
  PCIe_ENGINE_CONFIG      Dp1;                    ///< DP1
  PCIe_ENGINE_CONFIG      Dp2;                    ///< DP2
  PCIe_ENGINE_CONFIG      Dp3;                    ///< DP3
  PCIe_ENGINE_CONFIG      Dp4;                    ///< DP4
  PCIe_ENGINE_CONFIG      Port4;                  ///< Port 4
  PCIe_ENGINE_CONFIG      Port5;                  ///< Port 5
  PCIe_ENGINE_CONFIG      Port6;                  ///< Port 6
  PCIe_ENGINE_CONFIG      Port7;                  ///< Port 7
  PCIe_ENGINE_CONFIG      Port8;                  ///< Port 8
  PCIe_ENGINE_CONFIG      Dpa;                    ///< DPA
  PCIe_ENGINE_CONFIG      Dpb;                    ///< DPB
  F12_PCIe_SILICON_CONFIG FmSilicon;              ///< Fm Silicon
} F12_COMPLEX_CONFIG;

VOID
PcieFmOscInitPhyForGen2 (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

#endif
