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

#include "cbtypes.h"

VOID WriteECmsg (IN UINT8  Address, IN UINT8 OpFlag, IN VOID* Value);
VOID WaitForEcLDN9MailboxCmdAck (VOID);
VOID ReadECmsg (IN UINT8  Address, IN UINT8 OpFlag, OUT VOID* Value);

// IMC Message Register Software Interface
#define CPU_MISC_BUS_DEV_FUN  ((0x18 << 3) + 3)

#define MSG_SYS_TO_IMC  0x80
#define Fun_80  0x80
#define Fun_81  0x81
#define Fun_82  0x82
#define Fun_83  0x83
#define Fun_84  0x84
#define Fun_85  0x85
#define Fun_86  0x86
#define Fun_87  0x87
#define Fun_88  0x88
#define Fun_89  0x89
#define Fun_90  0x90
#define MSG_IMC_TO_SYS  0x81
#define MSG_REG0  0x82
#define MSG_REG1  0x83
#define MSG_REG2  0x84
#define MSG_REG3  0x85
#define MSG_REG4  0x86
#define MSG_REG5  0x87
#define MSG_REG6  0x88
#define MSG_REG7  0x89
#define MSG_REG8  0x8A
#define MSG_REG9  0x8B
#define MSG_REGA  0x8C
#define MSG_REGB  0x8D
#define MSG_REGC  0x8E
#define MSG_REGD  0x8F


