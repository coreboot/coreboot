/**
 * @file
 *
 * GNB init tables
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
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbCommonLib.h"
#include  "GnbTable.h"
#include  "GnbRegistersKB.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           T A B L E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

GNB_TABLE ROMDATA GnbIommuInitTableKB [] = {

  // 1. Program D0F0x64_x0D[PciDev0Fn2RegEn] = 1h
  GNB_ENTRY_RMW (
    D0F0x64_x0D_TYPE,
    D0F0x64_x0D_ADDRESS,
    D0F0x64_x0D_PciDev0Fn2RegEn_MASK,
    (0x1 << D0F0x64_x0D_PciDev0Fn2RegEn_OFFSET)
    ),
  // 2. Program credits for the BIF client as follows
  GNB_ENTRY_RMW (
    0xa,
    ((L1_SEL_BIF << 16) | 0x32),
    0x10000 | 0x3F0 | 0xFC00,
    (0x1 << 16) | (0x8 << 4) | (0x8 << 10)
    ),
  // 3. Program credits for the PPD client as follows
  GNB_ENTRY_RMW (
    0xa,
    ((L1_SEL_PPD << 16) | 0x32),
    0x3F0 | 0xFC00,
    (0x8 << 4) | (0x7 << 10)
    ),
  // 4. Program credits for the INTGEN client as follows
  GNB_ENTRY_RMW (
    0xa,
    ((L1_SEL_INTGEN << 16) | 0x32),
    0x3F0 | 0xFC00,
    (0x4 << 4) | (0x4 << 10)
    ),
  // 5. Program clock gating as follows
  GNB_ENTRY_RMW (
    0xa,
    ((L1_SEL_PPD << 16) | 0x33),
    0x10 | 0x20 |
    0x40 | 0x80 |
    0x100 | 0x200 |
    0x400 | 0x80000000 |
    0x800,
    (0x1 << 4) | (0x1 << 5) |
    (0x1 << 6) | (0x1 << 7) |
    (0x1 << 8) | (0x1 << 9) |
    (0x1 << 10) | (0x1 << 31) |
    (0x1 << 11)
    ),
  GNB_ENTRY_RMW (
    0xa,
    ((L1_SEL_BIF << 16) | 0x33),
    0x10 | 0x20 |
    0x40 | 0x80 |
    0x100 | 0x200 |
    0x400 | 0x80000000 |
    0x800,
    (0x1 << 4) | (0x1 << 5) |
    (0x1 << 6) | (0x1 << 7) |
    (0x1 << 8) | (0x1 << 9) |
    (0x1 << 10) | (0x1 << 31) |
    (0x1 << 11)
    ),
  GNB_ENTRY_RMW (
    0xa,
    ((L1_SEL_INTGEN << 16) | 0x33),
    0x10 | 0x20 |
    0x40 | 0x80 |
    0x100 | 0x200 |
    0x400 | 0x80000000 |
    0x800,
    (0x1 << 4) | (0x1 << 5) |
    (0x1 << 6) | (0x1 << 7) |
    (0x1 << 8) | (0x1 << 9) |
    (0x1 << 10) | (0x1 << 31) |
    (0x1 << 11)
    ),
  // 6. Program D0F0x64_x0D[PciDev0Fn2RegEn] = 0h
  GNB_ENTRY_RMW (
    D0F0x64_x0D_TYPE,
    D0F0x64_x0D_ADDRESS,
    D0F0x64_x0D_PciDev0Fn2RegEn_MASK,
    0
    ),
  GNB_ENTRY_TERMINATE
};
