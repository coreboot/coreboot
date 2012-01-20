/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *
 ***************************************************************************/


#include	"Platform.h"

/*++

Routine Description:

  Locate ACPI table

Arguments:

  Signature - table signature

Returns:

  pointer to ACPI table

--*/
void* ACPI_LocateTable(
	UINT32 Signature
)
{
	UINT32	i;
	UINT32* RsdPtr = (UINT32*)0xe0000;
	UINT32* Rsdt = NULL;
	DESCRIPTION_HEADER* CurrentTable;
	do{
//		if (*RsdPtr == ' DSR' && *(RsdPtr+1) == ' RTP'){
		if ((*RsdPtr == Int32FromChar ('R', 'S', 'D', ' ')) && (*(RsdPtr+1) == Int32FromChar ('R', 'T', 'P', ' '))){
			Rsdt = (UINT32*)((RSDP*)RsdPtr)->RsdtAddress;
			break;
		}
		RsdPtr+=4;
	}while (RsdPtr <= (UINT32*)0xffff0);
	if(Rsdt != NULL && ACPI_GetTableChecksum(Rsdt)==0){
		for (i = 0;i < (((DESCRIPTION_HEADER*)Rsdt)->Length - sizeof(DESCRIPTION_HEADER))/4;i++){
			CurrentTable = (DESCRIPTION_HEADER*)*(UINT32*)((UINT8*)Rsdt + sizeof(DESCRIPTION_HEADER) + i*4);
			if (CurrentTable->Signature == Signature) return CurrentTable;
		}
	}
	return NULL;
}

/*++

Routine Description:

  Update table checksum

Arguments:

  TablePtr - table pointer

Returns:

  none

--*/
void  ACPI_SetTableChecksum(
	void* TablePtr
)
{
	UINT8 Checksum = 0;
	((DESCRIPTION_HEADER*)TablePtr)->Checksum = 0;
	Checksum = ACPI_GetTableChecksum(TablePtr);
	((DESCRIPTION_HEADER*)TablePtr)->Checksum = 0x100 - Checksum;
}

/*++

Routine Description:

  Get table checksum

Arguments:

  TablePtr - table pointer

Returns:

  none

--*/
UINT8  ACPI_GetTableChecksum(
	void* TablePtr
)
{
	return GetByteSum(TablePtr,((DESCRIPTION_HEADER*)TablePtr)->Length);
}

