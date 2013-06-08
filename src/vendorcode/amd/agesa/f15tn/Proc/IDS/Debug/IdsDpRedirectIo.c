/**
 * @file
 *
 * AMD Integrated Debug Debug_library Routines
 *
 * Contains all functions related to Redirect IO
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 */
/*****************************************************************************
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

 /*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "Ids.h"
#include "IdsLib.h"
#include "amdlib.h"
#include "AMD.h"
#include "IdsDebugPrint.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_IDS_DEBUG_IDSDPSERIAL_FILECODE

/**
 *  Determine whether IDS console is enabled.
 *
 *
 *  @retval       TRUE    Alway return true
 *
 **/
BOOLEAN
AmdIdsRedirectIoSupport (
  VOID
  )
{

  return TRUE;
}

/**
 *  Get customize Filter
 *
 *  @param[in,out] Filter    Filter do be filled
 *
 *  @retval       FALSE    Alway return FALSE
 *
 **/
BOOLEAN
AmdIdsRedirectIoGetFilter (
  IN OUT   UINT64 *Filter
  )
{
  return FALSE;
}

#define REDIRECT_IO_DATA_BEGIN 0x5f535452ul
#define REDIRECT_IO_DATA_END   0x5f454e44ul

/**
 *  Print formated string with redirect IO
 *
 *  @param[in] Buffer  - Point to input buffer
 *  @param[in] BufferSize  - Buffer size
 *  @param[in] debugPrintPrivate  - Option
 *
**/
VOID
AmdIdsRedirectIoPrint (
  IN      CHAR8   *Buffer,
  IN      UINTN BufferSize,
  IN      IDS_DEBUG_PRINT_PRIVATE_DATA *debugPrintPrivate
  )
{
  UINT32 Value;

  Value = REDIRECT_IO_DATA_BEGIN;
  LibAmdIoWrite (AccessWidth32, IDS_DEBUG_PRINT_IO_PORT, &Value, NULL);

  while (BufferSize--) {
    LibAmdIoWrite (AccessWidth8, IDS_DEBUG_PRINT_IO_PORT, Buffer++, NULL);
  }

  Value = REDIRECT_IO_DATA_END;
  LibAmdIoWrite (AccessWidth32, IDS_DEBUG_PRINT_IO_PORT, &Value, NULL);
}

/**
 *  Init local private data
 *
 *  @param[in] Flag    - filter flag
 *  @param[in] debugPrintPrivate  - Point to debugPrintPrivate
 *
**/
VOID
AmdIdsRedirectIoInitPrivateData (
  IN      UINT64 Flag,
  IN      IDS_DEBUG_PRINT_PRIVATE_DATA *debugPrintPrivate
  )
{

}

CONST IDS_DEBUG_PRINT ROMDATA  IdsDebugPrintRedirectIoInstance =
{
  AmdIdsRedirectIoSupport,
  AmdIdsRedirectIoGetFilter,
  AmdIdsRedirectIoInitPrivateData,
  AmdIdsRedirectIoPrint
};



