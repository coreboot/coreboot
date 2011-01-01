/*
 *****************************************************************************
 *
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 * ***************************************************************************
 *
 */


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


