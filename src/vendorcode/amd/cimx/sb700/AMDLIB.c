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



#include "Platform.h"

VOID
ReadIO (
  IN       UINT16 Address,
  IN       UINT8 OpFlag,
  IN       VOID* Value
  )
{
  OpFlag = OpFlag & 0x7f;
  switch ( OpFlag ) {
  case AccWidthUint8:
    *(UINT8*)Value = ReadIo8 (Address);
    break;
  case AccWidthUint16:
    *(UINT16*)Value = ReadIo16 (Address);
    break;
  case AccWidthUint32:
    *(UINT32*)Value = ReadIo32 (Address);
    break;
  default:
    break;
  }
}

VOID
WriteIO (
  IN       UINT16 Address,
  IN       UINT8 OpFlag,
  IN       VOID* Value
  )
{
  OpFlag = OpFlag & 0x7f;
  switch ( OpFlag ) {
  case AccWidthUint8:
    WriteIo8 (Address, *(UINT8*)Value);
    break;
  case AccWidthUint16:
    WriteIo16 (Address, *(UINT16*)Value);
    break;
  case AccWidthUint32:
    WriteIo32 (Address, *(UINT32*)Value);
    break;
  default:
    break;
  }
}

VOID
RWIO (
  IN       UINT16 Address,
  IN       UINT8 OpFlag,
  IN       UINT32 Mask,
  IN       UINT32 Data
  )
{
  UINT32 Result;
  ReadIO (Address, OpFlag, &Result);
  Result = (Result & Mask) | Data;
  WriteIO (Address, OpFlag, &Result);
}


VOID
ReadPCI (
  IN       UINT32 Address,
  IN       UINT8 OpFlag,
  IN       VOID* Value
  )
{
  OpFlag = OpFlag & 0x7f;

  if ( (UINT16)Address < 0xff ) {
    //Normal Config Access
    UINT32 AddrCf8;
    AddrCf8 = (1 << 31) + ((Address >> 8) & 0x0FFFF00) + (Address & 0xFC);
    WriteIO (0xCf8, AccWidthUint32, &AddrCf8);
    ReadIO ((UINT16) (0xCfC + (Address & 0x3)), OpFlag, Value);
  }
}

VOID
WritePCI (
  IN       UINT32 Address,
  IN       UINT8 OpFlag,
  IN       VOID* Value
  )
{
  OpFlag = OpFlag & 0x7f;
  if ( (UINT16)Address < 0xff ) {
    //Normal Config Access
    UINT32 AddrCf8;
    AddrCf8 = (1 << 31) + ((Address >> 8)&0x0FFFF00) + (Address & 0xFC);
    WriteIO (0xCf8, AccWidthUint32, &AddrCf8);
    WriteIO ((UINT16) (0xCfC + (Address & 0x3)), OpFlag, Value);
  }
}

VOID
RWPCI (
  IN       UINT32 Address,
  IN       UINT8 OpFlag,
  IN       UINT32 Mask,
  IN       UINT32 Data
  )
{
  UINT32 Result;
  Result = 0;
  OpFlag = OpFlag & 0x7f;
  ReadPCI (Address, OpFlag, &Result);
  Result = (Result & Mask) | Data;
  WritePCI (Address, OpFlag, &Result);
}

void
ReadIndexPCI32	(
UINT32	PciAddress,
UINT32	IndexAddress,
void*	Value
)
{
	WritePCI(PciAddress,AccWidthUint32,&IndexAddress);
	ReadPCI(PciAddress+4,AccWidthUint32,Value);
}

void
WriteIndexPCI32	(
UINT32	PciAddress,
UINT32	IndexAddress,
UINT8	OpFlag,
void*	Value
)
{

	WritePCI(PciAddress,AccWidthUint32 | (OpFlag & 0x80),&IndexAddress);
	WritePCI(PciAddress+4,AccWidthUint32 | (OpFlag & 0x80) ,Value);
}

void
RWIndexPCI32	(
UINT32	PciAddress,
UINT32	IndexAddress,
UINT8	OpFlag,
UINT32	Mask,
UINT32	Data
)
{
	UINT32 Result;
	ReadIndexPCI32(PciAddress,IndexAddress,&Result);
	Result = (Result & Mask)| Data;
	WriteIndexPCI32(PciAddress,IndexAddress,(OpFlag & 0x80),&Result);

}

void
ReadMEM	(
UINT32	Address,
UINT8	OpFlag,
void*	Value
)
{
	OpFlag = OpFlag & 0x7f;
	switch	(OpFlag){
		case	AccWidthUint8 : *((UINT8*)Value)=*((UINT8*)Address);break;
		case	AccWidthUint16: *((UINT16*)Value)=*((UINT16*)Address);break;
		case	AccWidthUint32: *((UINT32*)Value)=*((UINT32*)Address);break;
	}
}

void
WriteMEM	(
UINT32	Address,
UINT8	OpFlag,
void*	Value
)
{
	OpFlag = OpFlag & 0x7f;
	switch	(OpFlag){
		case	AccWidthUint8 : *((UINT8*)Address)=*((UINT8*)Value);break;
		case	AccWidthUint16: *((UINT16*)Address)=*((UINT16*)Value);break;
		case	AccWidthUint32: *((UINT32*)Address)=*((UINT32*)Value);break;
	}
}

void
RWMEM	(
UINT32	Address,
UINT8	OpFlag,
UINT32	Mask,
UINT32	Data
)
{
	UINT32 Result;
	ReadMEM(Address,OpFlag,&Result);
	Result = (Result & Mask)| Data;
	WriteMEM(Address,OpFlag,&Result);
}


void
RWMSR(
UINT32	Address,
UINT64	Mask,
UINT64	Value
)
{
	MsrWrite(Address,(MsrRead(Address)& Mask)|Value);
}

UINT32
IsFamily10()
{
	CPUID_DATA Cpuid;
	CpuidRead(0x1,(CPUID_DATA *)&Cpuid);

	return Cpuid.REG_EAX & 0xff00000;
}


UINT8 GetNumberOfCpuCores(void)
{
	UINT8 Result=1;
        Result=ReadNumberOfCpuCores();
	return Result;
}


void
Stall(
UINT32	uSec
)
{
	UINT16	timerAddr;
	UINT32	startTime, elapsedTime;
	ReadPMIO(SB_PMIO_REG24, AccWidthUint16, &timerAddr);

	if (timerAddr ==0){
		uSec = uSec/2;
		while	(uSec!=0){
		ReadIO(0x80,AccWidthUint8,(UINT8 *)(&startTime));
		uSec--;
		}
	}
	else{
		ReadIO(timerAddr, AccWidthUint32,&startTime);
		while (1){
			ReadIO(timerAddr, AccWidthUint32,&elapsedTime);
			if (elapsedTime < startTime)
				elapsedTime = elapsedTime+0xFFFFFFFF-startTime;
			else
				elapsedTime = elapsedTime-startTime;
			if ((elapsedTime*28/100)>uSec)
				break;
		}
	}
}


void
Reset(
)
{
	RWIO(0xcf9,AccWidthUint8,0x0,0x06);
}


CIM_STATUS
RWSMBUSBlock(
UINT8 Controller,
UINT8 Address,
UINT8 Offset,
UINT8  BufferSize,
UINT8* BufferPrt
)
{
	UINT16 SmbusPort;
	UINT8  i;
	UINT8  Status;
	ReadPCI(PCI_ADDRESS(0,0x14,0,Controller?0x58:0x10),AccWidthUint16,&SmbusPort);
	SmbusPort &= 0xfffe;
	RWIO(SmbusPort + 0,AccWidthUint8,0x0,0xff);
	RWIO(SmbusPort + 4,AccWidthUint8,0x0,Address);
	RWIO(SmbusPort + 3,AccWidthUint8,0x0,Offset);
	RWIO(SmbusPort + 2,AccWidthUint8,0x0,0x14);
	RWIO(SmbusPort + 5,AccWidthUint8,0x0,BufferSize);
	if(!(Address & 0x1)){
		for (i = 0 ;i < BufferSize;i++){
			WriteIO(SmbusPort + 7,AccWidthUint8,&BufferPrt[i]);
		}
	}
	RWIO(SmbusPort + 2,AccWidthUint8,0x0,0x54);
	do{
		ReadIO(SmbusPort + 0,AccWidthUint8,&Status);
		if (Status & 0x1C) return CIM_ERROR;
		if (Status & 0x02) break;
	}while(!(Status & 0x1));

	do{
		ReadIO(SmbusPort + 0,AccWidthUint8,&Status);
	}while(Status & 0x1);

	if(Address & 0x1){
		for (i = 0 ;i < BufferSize;i++){
			ReadIO(SmbusPort + 7,AccWidthUint8,&BufferPrt[i]);
		}
	}
	return CIM_SUCCESS;
}



void outPort80(UINT32 pcode)
{
	WriteIO(0x80, AccWidthUint8, &pcode);
	return;
}


UINT8
GetByteSum(
	void*   pData,
	UINT32	Length
)
{
	UINT32	i;
	UINT8 Checksum = 0;
	for (i = 0;i < Length;i++){
		Checksum += *((UINT8*)pData+i);
	}
	return Checksum;
}


UINT32
readAlink(
	UINT32	Index
){
	UINT32	Data;
	WriteIO(ALINK_ACCESS_INDEX, AccWidthUint32, &Index);
	ReadIO(ALINK_ACCESS_DATA, AccWidthUint32, &Data);
	//Clear Index
	Index=0;
	WriteIO(ALINK_ACCESS_INDEX, AccWidthUint32, &Index);
	return Data;
}


void
writeAlink(
	UINT32	Index,
	UINT32	Data
){
	WriteIO(ALINK_ACCESS_INDEX, AccWidthUint32, &Index);
	WriteIO(ALINK_ACCESS_DATA, AccWidthUint32, &Data);
	//Clear Index
	Index=0;
	WriteIO(ALINK_ACCESS_INDEX, AccWidthUint32, &Index);

}


/**
 *
 * IsServer - Determine if southbridge type is SP5100 (server) or SB7x0 (non-server)
 *
 * A SP5100 is determined when both following two items are true:
 *    1) Revision >= A14;
 *    2) A server north bridge chipset is detected;
 *
 * A list of server north bridge chipset:
 *
 *      Family    DeviceID
 *     ----------------------
 *      SR5690     0x5A10
 *      SR5670     0x5A12
 *      SR5650     0x5A13
 *
 */
UINT8
IsServer (void){
  UINT16     DevID;

  if (getRevisionID () < SB700_A14) {
    return 0;
  }
  ReadPCI ((NB_BDF << 16) + 2, AccWidthUint16, &DevID);
  return ((DevID == 0x5a10) || (DevID == 0x5a12) || (DevID == 0x5a13))? 1: 0;
}

/**
 *
 * IsLS2Mode - Determine if LS2 mode is enabled or not in northbridge.
 *
 */
UINT8
IsLs2Mode (void)
{
  UINT32     HT3LinkTraining0;

  ReadPCI ((NB_BDF << 16) + 0xAC, AccWidthUint32, &HT3LinkTraining0);
  return ( HT3LinkTraining0 & 0x100 )? 1: 0;
}
