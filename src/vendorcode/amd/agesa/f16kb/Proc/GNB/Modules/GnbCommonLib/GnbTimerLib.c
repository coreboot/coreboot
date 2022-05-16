/* $NoKeywords:$ */
/**
 * @file
 *
 * Various Timer services.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 86079 $   @e \$Date: 2013-01-16 00:59:04 -0600 (Wed, 16 Jan 2013) $
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


#include  "AGESA.h"
#include  "amdlib.h"
#include  "S3SaveState.h"
#include  "Gnb.h"
#include  "GnbLib.h"
#include  "GnbTimerLib.h"
#include  "GnbFamServices.h"
#include  "GnbPcieConfig.h"
#include  "GnbCommonLib.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBCOMMONLIB_GNBTIMERLIB_FILECODE




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

  TimeStampStart = GnbFmTimeStamp (StdHeader);
  do {
    TimeStampCurrent = GnbFmTimeStamp (StdHeader);
    TimeStampDelta = ((TimeStampCurrent > TimeStampStart) ? (TimeStampCurrent - TimeStampStart) : (0xffffffffull - TimeStampStart + TimeStampCurrent));
  } while (TimeStampDelta < Microsecond);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Stall S3 script
 *
 *
 *
 * @param[in]  StdHeader      Standard configuration header
 * @param[in]  ContextLength  Context Length (not used)
 * @param[in]  Context        pointer to UINT32 number of us
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
/**
 * Family specific time stamp function
 *
 *
 * @param[in]  StdHeader       Standard configuration header
 * @retval                     Count
 */
UINT32
GnbFmTimeStamp (
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  AGESA_STATUS               Status;
  GNB_FAM_TS_SERVICES       *GnbFamTsFunc;
  GNB_HANDLE                *GnbHandle;

  GnbHandle = GnbGetHandle (StdHeader);

  Status = GnbLibLocateService (GnbFamTsService, GnbGetSocketId (GnbHandle), (CONST VOID **)&GnbFamTsFunc, StdHeader);
  ASSERT (Status == AGESA_SUCCESS);
  if (Status == AGESA_SUCCESS) {
    return GnbFamTsFunc->GnbFmTimeStamp (StdHeader);
  }
  return 0;
}
