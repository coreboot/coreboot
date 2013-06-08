/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD MMIO Map Manager APIs, and related functions.
 *
 * Contains code that manage MMIO base/limit registers
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 63522 $   @e \$Date: 2011-12-25 20:25:03 -0600 (Sun, 25 Dec 2011) $
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

#ifndef _MMIO_MAP_MANAGER_H_
#define _MMIO_MAP_MANAGER_H_

/*---------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *---------------------------------------------------------------------------------------
 */
//  Forward declaration needed for multi-structure mutual references
AGESA_FORWARD_DECLARATION (MMIO_MAP_FAMILY_SERVICES);

/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */
/// MMIO attribute
typedef struct _AMD_MMIO_ATTRIBUTE {
  UINT8   MmioReadableRange:1;  ///< Indicator whether the range is readable
  UINT8   MmioWritableRange:1;  ///< Indicator whether the range is writable
  UINT8   MmioPostedRange:1;    ///< Indicator whether the range is posted
  UINT8   MmioSecuredRange:1;   ///< Indicator whether the range is locked
  UINT8   :4;                   ///< Reserved
} AMD_MMIO_ATTRIBUTE;

/// MMIO destination
typedef struct _AMD_MMIO_DST {
  UINT32   DstNode:3;           ///< Destination node ID bits
  UINT32   DstLink:2;           ///< Destination link ID
  UINT32   DstSubLink:1;        ///< Destination sublink
} AMD_MMIO_DST;

/// MMIO range
typedef struct _MMIO_RANGE {
  UINT64  Base;                 ///< Base
  UINT64  Limit;                ///< Limit
  AMD_MMIO_ATTRIBUTE Attribute; ///< Attribute
  AMD_MMIO_DST Destination;     ///< Destination
  UINT8   RangeNum;             ///< Range No.
  BOOLEAN Modified;             ///< if this MMIO base/limit registers need to be updated
} MMIO_RANGE;

/// AMD_ADD_MMIO_PARAMS
typedef struct _AMD_ADD_MMIO_PARAMS {
  AMD_CONFIG_PARAMS StdHeader;  ///< Config Handle for library, services.
  UINT64 BaseAddress;           ///< This is the starting address of the requested MMIO range.
  UINT64 Length;                ///< This is the length of the range to allocate, in bytes.
  PCI_ADDR TargetAddress;       ///< This is the PCIe address of the device for which this range is allocated, and it
                                ///< provides the bus, device, and function of the target device.
  AMD_MMIO_ATTRIBUTE Attributes;///< This indicates the attributes of the requested range.
} AMD_ADD_MMIO_PARAMS;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to MMIO map manager.
 *
 * @param[in]    MmioMapServices    MMIO map manager services.
 * @param[in]    AmdAddMmioParams   Pointer to a data structure containing the parameter information.
 *
 * @return       Family specific error value.
 *
 */
typedef AGESA_STATUS F_MMIO_MAP_ADDING_MAP (
  IN       MMIO_MAP_FAMILY_SERVICES *MmioMapServices,
  IN       AMD_ADD_MMIO_PARAMS      AmdAddMmioParams
  );

/// Reference to a Method.
typedef F_MMIO_MAP_ADDING_MAP *PF_MMIO_MAP_ADDING_MAP;

/**
 * Provide the interface to the MMIO map manager Family Specific Services.
 *
 * Use the methods or data in this struct to adapt the feature code to a specific cpu family or model (or stepping!).
 * Each supported Family must provide an implementation for all methods in this interface, even if the
 * implementation is a CommonReturn().
 */
struct _MMIO_MAP_FAMILY_SERVICES {
  UINT16          Revision;                                             ///< Interface version
  // Public Methods.
  PF_MMIO_MAP_ADDING_MAP  addingMmioMap;                                ///< Method: Family specific call to adding MMIO map.
};

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */

AGESA_STATUS
AmdAddMmioMapping (
  IN       AMD_ADD_MMIO_PARAMS AmdAddMmioParams
  );

#endif // _MMIO_MAP_MANAGER_H_

