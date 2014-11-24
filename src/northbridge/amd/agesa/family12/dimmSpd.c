/*****************************************************************************
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
 *
 ***************************************************************************/

#include <stdlib.h>
#include "Porting.h"
#include "AGESA.h"
#include "amdlib.h"

#include <northbridge/amd/agesa/dimmSpd.h>

typedef struct _DIMM_INFO_SMBUS{
  UINT8   SocketId;
  UINT8   MemChannelId;
  UINT8   DimmId;
  UINT8   SmbusAddress;
} DIMM_INFO_SMBUS;
/*
* SPD address table - porting required
*/
STATIC CONST DIMM_INFO_SMBUS SpdAddrLookup [] =
{
  /* Socket, Channel, Dimm, Smbus */
  {0, 0, 0, 0xA0},
  {0, 1, 0, 0xA2}
};

AGESA_STATUS
AmdMemoryReadSPD (
  IN UINT32 Func,
  IN UINT32 Data,
  IN OUT AGESA_READ_SPD_PARAMS *SpdData
  )
{
   UINT8  SmBusAddress = 0;
   UINTN  Index;
   UINTN  MaxSocket = ARRAY_SIZE(SpdAddrLookup);
   for (Index = 0; Index < MaxSocket; Index ++){
     if ((SpdData->SocketId     == SpdAddrLookup[Index].SocketId)     &&
         (SpdData->MemChannelId == SpdAddrLookup[Index].MemChannelId) &&
         (SpdData->DimmId       == SpdAddrLookup[Index].DimmId)) {
        SmBusAddress = SpdAddrLookup[Index].SmbusAddress;
        break;
      }
   }

	if (SmBusAddress == 0)
		return AGESA_ERROR;

	int err = smbus_readSpd(SmBusAddress, (char *) SpdData->Buffer, 128);
	if (err)
		return AGESA_ERROR;
	return AGESA_SUCCESS;
}
