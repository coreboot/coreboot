/*;********************************************************************************
;
; Copyright (C) 2012 Advanced Micro Devices, Inc.
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Redistributions in binary form must reproduce the above copyright
;       notice, this list of conditions and the following disclaimer in the
;       documentation and/or other materials provided with the distribution.
;     * Neither the name of Advanced Micro Devices, Inc. nor the names of
;       its contributors may be used to endorse or promote products derived
;       from this software without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
;*********************************************************************************/

#ifndef _AMD_ACPILIB_H_
#define _AMD_ACPILIB_H_

typedef struct _RSDP{
	UINT64	Signature;
	UINT8	Checksum;
	UINT8	OEMID[6];
	UINT8	Revision;
	UINT32	RsdtAddress;
	UINT32	Length;
	UINT64	XsdtAddress;
	UINT8	ExtendedChecksum;
	UINT8	Reserved[3];
}RSDP;

typedef struct _DESCRIPTION_HEADER{
	UINT32	Signature;
	UINT32	Length;
	UINT8	Revision;
	UINT8	Checksum;
	UINT8	OEMID[6];
	UINT8	OEMTableID[8];
	UINT32	OEMRevision;
	UINT32	CreatorID;
	UINT32	CreatorRevision;
}DESCRIPTION_HEADER;

void* ACPI_LocateTable(UINT32 Signature);
void  ACPI_SetTableChecksum(void* TablePtr);
UINT8 ACPI_GetTableChecksum(void* TablePtr);

#endif	//ifndef _AMD_ACPILIB_H_
