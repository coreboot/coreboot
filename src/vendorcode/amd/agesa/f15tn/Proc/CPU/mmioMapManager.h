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
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
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

