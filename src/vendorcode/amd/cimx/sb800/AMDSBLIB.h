/**
 * @file
 *
 * Southbridge IO access common routine define file
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-SB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
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
 * 
 * ***************************************************************************
 *
 */

#ifndef __VENDORCODE_AMD_CIMX_SB800_AMDSBLIB_H__
#define __VENDORCODE_AMD_CIMX_SB800_AMDSBLIB_H__

//AMDSBLIB Routines

/**
 * SbStall - Delay routine
 *
 *
 *
 * @param[in] uSec
 *
 */
void  SbStall (IN unsigned int uSec);

/**
 * SbReset - Generate a reset command
 *
 *
 *
 * @param[in] OpFlag - Dummy
 *
 */
void SbReset (IN unsigned char OpFlag);

/**
 * outPort80 - Send data to PORT 80 (debug port)
 *
 *
 *
 * @param[in] pcode - debug code (32 bits)
 *
 */
void outPort80 (IN unsigned int pcode);

/**
 * getEfuseStatue - Get Efuse status
 *
 *
 * @param[in] Value - Return Chip strap status
 *
 */
void getEfuseStatus (IN void* Value);

/**
 * AmdSbDispatcher - Dispatch Southbridge function
 *
 *
 *
 * @param[in] pConfig   Southbridge configuration structure pointer.
 *
 */
AGESA_STATUS  AmdSbDispatcher (IN void *pConfig);

/**
 * AmdSbCopyMem - Memory copy
 *
 * @param[in] pDest - Destance address point
 * @param[in] pSource - Source Address point
 * @param[in] Length - Data length
 *
 */
void AmdSbCopyMem (IN void* pDest, IN void* pSource, IN unsigned int Length);


/* SB800 CIMx and AGESA V5 can share lib functions */
unsigned char ReadIo8(IN unsigned short Address);
unsigned short ReadIo16(IN unsigned short Address);
unsigned int ReadIo32(IN unsigned short Address);
void WriteIo8(IN unsigned short Address, IN unsigned char Data);
void WriteIo16(IN unsigned short Address, IN unsigned short Data);
void WriteIo32(IN unsigned short Address, IN unsigned int Data);
//void CpuidRead(IN unsigned int CpuidFcnAddress, OUT CPUID_DATA *Value);
void CpuidRead(unsigned int CpuidFcnAddress, CPUID_DATA *Value);
unsigned char ReadNumberOfCpuCores(void);

#endif
