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
