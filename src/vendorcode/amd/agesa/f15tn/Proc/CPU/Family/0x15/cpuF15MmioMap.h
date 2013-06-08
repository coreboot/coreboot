/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 MMIO map manager
 *
 * manage MMIO base/limit registers.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

#ifndef _CPU_F15_MMIO_MAP_H_
#define _CPU_F15_MMIO_MAP_H_


/*---------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */
#define MMIO_REG_PAIR_NUM     12

#define CONF_MAP_RANGE_0      0xE0
#define CONF_MAP_RANGE_1      0xE4
#define CONF_MAP_RANGE_2      0xE8
#define CONF_MAP_RANGE_3      0xEC
#define CONF_MAP_NUM          4
/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */
/// MMIO base low
typedef struct {
  UINT32 RE:1;          ///< Read enable
  UINT32 WE:1;          ///< Write enable
  UINT32 CpuDis:1;      ///< CPU Disable
  UINT32 Lock:1;        ///< Lock
  UINT32 :4;            ///< Reserved
  UINT32 MmioBase:24;   ///< MMIO base address register bits[39:16]
} MMIO_BASE_LOW;

/// MMIO limit low
typedef struct {
  UINT32 DstNode:3;     ///< Destination node ID bits
  UINT32 :1;            ///< Reserved
  UINT32 DstLink:2;     ///< Destination link ID
  UINT32 DstSubLink:1;  ///< Destination sublink
  UINT32 NP:1;          ///< Non-posted
  UINT32 MmioLimit:24;  ///< MMIO limit address register bits[39:16]
} MMIO_LIMIT_LOW;

/// MMIO base/limit high
typedef struct {
  UINT32 MmioBase:8;    ///< MMIO base address register bits[47:40]
  UINT32 :8;            ///< Reserved
  UINT32 MmioLimit:8;   ///< MMIO limit address register bits[47:40]
  UINT32 :8;            ///< Reserved
} MMIO_BASE_LIMIT_HI;

/// MMIO base/limit high
typedef struct {
  UINT32 RE:1;          ///< Read enable
  UINT32 WE:1;          ///< Write enable
  UINT32 DevCmpEn:1;    ///< Device number compare mode enable
  UINT32 :1;            ///< Reserved
  UINT32 DstNode:3;     ///< Destination node ID bits
  UINT32 :1;            ///< Reserved
  UINT32 DstLink:2;     ///< Destination link ID
  UINT32 DstSubLink:1;  ///< Destination sublink
  UINT32 :5;            ///< Reserved
  UINT32 BusNumBase:8;  ///< Bus number base bits
  UINT32 BusNumLimit:8; ///< Bus number limit bits
} CONFIGURATION_MAP;

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */

#endif  // _CPU_F15_MMIO_MAP_H_
