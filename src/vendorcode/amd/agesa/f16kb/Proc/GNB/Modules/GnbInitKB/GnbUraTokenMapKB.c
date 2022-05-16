/* $NoKeywords:$ */
/**
 * @file
 *
 * AGESA gnb file
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  GNB
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "amdlib.h"
#include  "Ids.h"
#include  "Gnb.h"
#include  "GnbUra.h"
#include  "GnbUraToken.h"
#include  "GnbRegistersKB.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITKB_GNBURATOKENMAPKB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

VOID
GnbUraLocateRegTblKB (
  IN       DEV_OBJECT    *Device,
  IN OUT   UINT32        *UraTableAddress
  );
/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

CONST REG_FIELD_TABLE_STRUCT  UraTableKB = {
  {0xC2100000, 0x4, 0},
  {0, 1, FIELD_OFFSET(RxSmuIntReq ,BfxSmuIntToggle)},
  {1, 16, FIELD_OFFSET(RxSmuIntReq ,BfxSmuServiceIndex)},
  {0xC2100004, 0x4, 0},
  {0, 1, FIELD_OFFSET(RxSmuIntSts ,BfxSmuIntAck)},
  {1, 1, FIELD_OFFSET(RxSmuIntSts ,BfxSmuIntDone)},
  {0xE0003088, 0x4, 0},
  {0, 1, FIELD_OFFSET(RxSmuAuthSts ,BfxSmuAuthDone)},
  {1, 1, FIELD_OFFSET(RxSmuAuthSts ,BfxSmuAuthPass)},
  {0xE00030A4, 0x4, 0},
  {16, 1, FIELD_OFFSET(RxSmuFwAuth ,BfxSmuProtectedMode)},
  {0xC0000004, 0x4, 0},
  {7, 1, FIELD_OFFSET(REG_FIELD_TABLE_STRUCT_fld11 ,BfxSmuBootSeqDone)},
  {0x3F800, 0x4, 0},
  {0, 1, FIELD_OFFSET(RxSmuFwFlags ,BfxSmuInterruptsEnabled)},
  {0x80000000, 0x4, 0},
  {0, 1, FIELD_OFFSET(RxSmuResetCntl ,BfxSmuRstReg)},
  {0x80000004, 0x4, 0},
  {0, 1, FIELD_OFFSET(RxSmuClkCntl ,BfxSmuCkDisable)},
  {0x80008000, 0x4, 0},
  {D0F0xBC_x20000_ADDRESS, D0F0xBC_x20000_TYPE, 0},
  {D0F0xBC_x0_ADDRESS, D0F0xBC_x0_TYPE, 0},
  {D0F0xBC_xC210003C_ADDRESS, D0F0xBC_xC210003C_TYPE, 0},

};

/*----------------------------------------------------------------------------------------*/
/**
 * Gnb Unified Register Access method to locate register table.
 *
 *
 * @param[in]        Device             Standard configuration header
 * @param[in, out]   UraTableAddress    Ura register table address
 */
VOID
GnbUraLocateRegTblKB (
  IN       DEV_OBJECT    *Device,
  IN OUT   UINT32        *UraTableAddress
  )
{
  *UraTableAddress = (UINT32)((UINTN)(&UraTableKB));
  return;
}
