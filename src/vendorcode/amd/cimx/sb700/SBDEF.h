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

#ifndef _AMD_SBDEF_H_
#define _AMD_SBDEF_H_

//AMD Library Routines

UINT64
MsrRead (
  IN       UINT32 MsrAddress
  );

VOID
MsrWrite (
  IN       UINT32 MsrAddress,
  IN			 UINT64 Value
  );

void	ReadIO(UINT16	Address, UINT8	OpFlag, void *Value);
void	WriteIO(UINT16	Address, UINT8	OpFlag, void *Value);
void	ReadPCI(UINT32	Address, UINT8	OpFlag, void *Value);
void	WritePCI(UINT32 Address,UINT8	OpFlag, void *Value);
void	RWPCI(UINT32 Address,UINT8	OpFlag,UINT32 Mask,UINT32	Data);
void	ReadIndexPCI32(UINT32	PciAddress,UINT32 IndexAddress,void* Value);
void	WriteIndexPCI32(UINT32	PciAddress,UINT32 IndexAddress,UINT8 OpFlag,void* Value);
void	RWIndexPCI32(UINT32	PciAddress,UINT32 IndexAddress,UINT8 OpFlag,UINT32	Mask,UINT32	Data);
void	RWIO (UINT16	Address, UINT8	OpFlag, UINT32	Mask, UINT32 Data);
void	ReadMEM(UINT32	Address,UINT8 OpFlag, void*	Value);
void	WriteMEM(UINT32	Address,UINT8 OpFlag, void*	Value);
void    RWMEM(UINT32	Address,UINT8	OpFlag,UINT32	Mask,UINT32 Data);
UINT32	IsFamily10(void);
UINT64	ReadMSR(UINT32	Address);
void	WriteMSR(UINT32	Address,UINT64	Value);
void	RWMSR(UINT32 Address, UINT64 Mask, UINT64 Value);
void*	LocateImage(UINT32 Signature);
void*	CheckImage(	UINT32	Signature, void*	ImagePtr);
void	Stall(UINT32	uSec);
void	Reset(void);
CIM_STATUS	RWSMBUSBlock(UINT8 Controller, UINT8 Address, UINT8 Offset, UINT8  BufferSize, UINT8* BufferPrt);
void	InitSerialOut(void);
void	ReadPMIO(UINT8 Address, UINT8	OpFlag, void* Value);
void	WritePMIO(UINT8 Address, UINT8	OpFlag, void* Value);
void	RWPMIO(UINT8 Address, UINT8	OpFlag, UINT32 AndMask, UINT32 OrMask);
void	ReadPMIO2(UINT8 Address, UINT8	OpFlag, void* Value);
void	WritePMIO2(UINT8 Address, UINT8	OpFlag, void* Value);
void	RWPMIO2(UINT8 Address, UINT8	OpFlag, UINT32 AndMask, UINT32 OrMask);
void	outPort80(UINT32 pcode);
UINT8	GetNumberOfCpuCores(void);
UINT8	ReadNumberOfCpuCores(void);
UINT8	GetByteSum(void* pData, UINT32	Length);
UINT32	readAlink(UINT32 Index);
void	writeAlink(UINT32 Index,UINT32	Data);

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void	azaliaInitAfterPciEnum (AMDSBCFG* pConfig);

void	SendBytePort(UINT8 Data);
void	SendStringPort(char* pstr);
void	ItoA(UINT32 Value,int Radix,char* pstr);
AMDSBCFG* getConfigPointer(void);
void	saveConfigPointer(AMDSBCFG* pConfig);


UINT32	GetFixUp(void);

void	sataInitAfterPciEnum(AMDSBCFG*	pConfig);
void	sataInitBeforePciEnum(AMDSBCFG*	pConfig);
void	sataInitLatePost(AMDSBCFG*	pConfig);
void	sataDriveDetection(AMDSBCFG* pConfig, UINT32 ddBar5);
void	sataPhyWorkaround(AMDSBCFG* pConfig, UINT32 ddBar5);
void	forceOOB(UINT32	ddBar5);
void	shutdownUnconnectedSataPortClock(AMDSBCFG*	pConfig, UINT32 ddBar5);
void	restrictSataCapabilities(AMDSBCFG* pConfig);


void	commonInitEarlyBoot(AMDSBCFG* pConfig);
void	commonInitEarlyPost(AMDSBCFG* pConfig);
void	setRevisionID(void);
UINT8	getRevisionID(void);
UINT8	IsServer (void);
UINT8	IsLs2Mode (void);
void	abLinkInitBeforePciEnum(AMDSBCFG* pConfig);
void	abcfgTbl(ABTBLENTRY* pABTbl);
void	programSubSystemIDs(AMDSBCFG* pConfig, BUILDPARAM	*pStaticOptions);
void	commonInitLateBoot(AMDSBCFG* pConfig);
void	hpetInit(AMDSBCFG* pConfig, BUILDPARAM	*pStaticOptions);
void	c3PopupSetting(AMDSBCFG* pConfig);

void	sbBeforePciInit (AMDSBCFG* pConfig);
void	sbAfterPciInit(AMDSBCFG* pConfig);
void	sbLatePost(AMDSBCFG* pConfig);
void	sbBeforePciRestoreInit(AMDSBCFG* pConfig);
void	sbAfterPciRestoreInit(AMDSBCFG* pConfig);
void	sbSmmAcpiOn(AMDSBCFG* pConfig);
UINT32	GetPciebase(void);
UINT32	CallBackToOEM(UINT32 Func, UINTN Data,AMDSBCFG* pConfig);
void	sbSmmService(AMDSBCFG* pConfig);
void	softwareSMIservice(void);

void	sbPowerOnInit (AMDSBCFG *pConfig);
void	programPciByteTable(REG8MASK* pPciByteTable, UINT16 dwTableSize);
void	programPmioByteTable(REG8MASK* pPmioByteTable, UINT16 dwTableSize);
UINT8	getClockMode(void);
UINT16	readStrapStatus	(void);

void	usbInitBeforePciEnum(AMDSBCFG* pConfig);
void	usbInitAfterPciInit(AMDSBCFG* pConfig);
void	usbInitMidPost(AMDSBCFG* pConfig);
void	programOhciMmioForEmulation(void);

void	fcInitBeforePciEnum(AMDSBCFG* pConfig);

unsigned char ReadIo8 (IN unsigned short Address);
unsigned short ReadIo16 (IN unsigned short Address);
unsigned int ReadIo32 (IN unsigned short Address);
void WriteIo8 (IN unsigned short Address, IN unsigned char Data);
void WriteIo16 (IN unsigned short Address, IN unsigned short Data);
void WriteIo32 (IN unsigned short Address, IN unsigned int Data);
unsigned long long  ReadTSC (void);
void CpuidRead (IN unsigned int Func, IN OUT CPUID_DATA* Data);

#ifndef	NO_EC_SUPPORT
void	EnterEcConfig(void);
void	ExitEcConfig(void);
void	ReadEC8(UINT8 Address, UINT8* Value);
void	WriteEC8(UINT8 Address, UINT8* Value);
void	RWEC8(UINT8 Address, UINT8 AndMask, UINT8 OrMask);
void	ecPowerOnInit(BUILDPARAM *pBuildOptPtr, AMDSBCFG *pConfig);
void	ecInitBeforePciEnum(AMDSBCFG* pConfig);
void	ecInitLatePost(AMDSBCFG* pConfig);
#endif
UINT8	isEcPresent(void);

void DispatcherEntry(void *pConfig);
AGESA_STATUS AmdSbDispatcher(void *pConfig);
void AMDFamily15CpuLdtStopReq(void);

#endif	//#ifndef _AMD_SBDEF_H_
