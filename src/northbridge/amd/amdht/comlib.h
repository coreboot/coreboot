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

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include "porting.h"

#ifdef AMD_DEBUG_ERROR_STOP
    /* Macro to aid debugging, causes program to halt and display the line number of the halt */
    #define STOP_HERE ASSERT(0)
#else
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
