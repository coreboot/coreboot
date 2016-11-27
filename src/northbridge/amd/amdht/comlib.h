/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef COMLIB_H
#define COMLIB_H

#undef FILECODE
#define FILECODE 0xF001

#include <inttypes.h>
#include <stdlib.h>
#include "porting.h"

#ifdef AMD_DEBUG
    #define ASSERT(x) ((x) ? 0 : ErrorStop(((uint32)FILECODE)*0x10000 + ((__LINE__)%10) + (((__LINE__/10)%10)*0x10) + (((__LINE__/100)%10)*0x100) +(((__LINE__/1000)%10)*0x1000)))
#else
    #define ASSERT(x)
#endif

#ifdef AMD_DEBUG_ERROR_STOP
    /* Macro to aid debugging, causes program to halt and display the line number of the halt in decimal */
    #define STOP_HERE ErrorStop(((uint32)FILECODE)*0x10000 + ((__LINE__)%10) + (((__LINE__/10)%10)*0x10) + (((__LINE__/100)%10)*0x100) +(((__LINE__/1000)%10)*0x1000))
#else
    /* Macro to aid debugging, causes program to halt and display the line number of the halt in decimal */
    /* #define STOP_HERE STOP_HERE_OnlyForDebugUse */
    #define STOP_HERE
#endif

void CALLCONV AmdPCIReadBits(SBDFO loc, uint8 highbit, uint8 lowbit, uint32 *value);
void CALLCONV AmdPCIWriteBits(SBDFO loc, uint8 highbit, uint8 lowbit, uint32 *value);
void CALLCONV AmdPCIFindNextCap(SBDFO *current);

void CALLCONV Amdmemcpy(void *dst, const void *src, uint32 length);
void CALLCONV Amdmemset(void *buf, uint8 val, uint32 length);

uint8 CALLCONV AmdBitScanReverse(uint32 value);
uint32 CALLCONV AmdRotateRight(uint32 value, uint8 size, uint32 count);
uint32 CALLCONV AmdRotateLeft(uint32 value, uint8 size, uint32 count);

#endif
