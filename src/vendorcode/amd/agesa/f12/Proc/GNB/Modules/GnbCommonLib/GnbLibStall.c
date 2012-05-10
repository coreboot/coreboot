/* $NoKeywords:$ */
/**
 * @file
 *
 * Various PCI service routines.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 01:16:51 -0800 (Wed, 22 Dec 2010) $
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


#include  "AGESA.h"
#include  "amdlib.h"
#include  "S3SaveState.h"
#include  "Gnb.h"
#include  "GnbLib.h"
#include  "GnbLibStall.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBCOMMONLIB_GNBLIBSTALL_FILECODE




/*----------------------------------------------------------------------------------------*/
/*
 * Stall and save to script table
 *
 *
 *
 * @param[in] Microsecond     Stall time
 * @param[in] StdHeader       Standard configuration header
 */

VOID
GnbLibStallS3Save (
  IN      UINT32              Microsecond,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  S3_SAVE_DISPATCH (StdHeader, GnbLibStallS3Script_ID, sizeof (Microsecond), &Microsecond);
  GnbLibStall (Microsecond, StdHeader);
}


/*----------------------------------------------------------------------------------------*/
/*
 * Stall
 *
 *
 *
 * @param[in] Microsecond     Stall time
 * @param[in] StdHeader       Standard configuration header
 */

VOID
GnbLibStall (
  IN      UINT32              Microsecond,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32 TimeStampStart;
  UINT32 TimeStampDelta;
  UINT32 TimeStampCurrent;

  TimeStampStart = GnbLibTimeStamp (StdHeader);
  do {
    TimeStampCurrent = GnbLibTimeStamp (StdHeader);
    TimeStampDelta = ((TimeStampCurrent > TimeStampStart) ? (TimeStampCurrent - TimeStampStart) : (0xffffffffull - TimeStampStart + TimeStampCurrent));
  } while (TimeStampDelta < Microsecond);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Stall S3 scrept
 *
 *
 *
 * @param[in]  StdHeader      Standard configuration header
 * @param[in]  ContextLength  Context Length (not used)
 * @param[in]  Context        Context pointer (not used)
 */
VOID
GnbLibStallS3Script (
  IN      AMD_CONFIG_PARAMS    *StdHeader,
  IN      UINT16               ContextLength,
  IN      VOID*                Context
  )
{
  GnbLibStall (* ((UINT32*) Context), StdHeader);
}
/*----------------------------------------------------------------------------------------*/
/*
 * Time stamp in us
 *
 *
 *
 * @param[in] StdHeader       Standard configuration header
 * @retval    TRUE            Device is a bridge
 * @retval    FALSE           Device is not a bridge
 */

UINT32
GnbLibTimeStamp (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32  TimeStamp;
  GnbLibPciIndirectRead (
    MAKE_SBDFO (0, 0, 0, 0, 0xE0),
    0x13080F0,
    AccessWidth32,
    &TimeStamp,
    StdHeader
    );
  return TimeStamp;
}

