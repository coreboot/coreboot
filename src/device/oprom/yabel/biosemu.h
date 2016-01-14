/******************************************************************************
 * Copyright (c) 2004, 2008 IBM Corporation
 * Copyright (c) 2009 Pattrick Hueper <phueper@hueper.net>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *     IBM Corporation - initial implementation
 *****************************************************************************/

#ifndef _BIOSEMU_BIOSEMU_H_
#define _BIOSEMU_BIOSEMU_H_

#define MIN_REQUIRED_VMEM_SIZE 0x100000	// 1MB

//define default segments for different components
#define STACK_SEGMENT 0x1000	//1000:xxxx
#define STACK_START_OFFSET 0xfffe

#define DATA_SEGMENT 0x2000
#define VBE_SEGMENT 0x3000

#define PMM_CONV_SEGMENT 0x4000	// 4000:xxxx is PMM conventional memory area, extended memory area
				// will be anything beyond MIN_REQUIRED_MEMORY_SIZE
#define PNP_DATA_SEGMENT 0x5000

#define OPTION_ROM_CODE_SEGMENT 0xc000

#define BIOS_DATA_SEGMENT 0xF000
// both EBDA values are _initial_ values, they may (and will be) changed at runtime by option ROMs!!
#define INITIAL_EBDA_SEGMENT 0xF600	// segment of the Extended BIOS Data Area
#define INITIAL_EBDA_SIZE 0x400	// size of the EBDA (at least 1KB!! since size is stored in KB!)

#define PMM_INT_NUM 0xFC	// we misuse INT FC for PMM functionality, at the PMM Entry Point
				// Address, there will only be a call to this INT and a RETF
#define PNP_INT_NUM 0xFD

/* array of function pointers to override generic interrupt handlers
 * a YABEL caller can add functions to this array before calling YABEL
 * if a interrupt occurs, YABEL checks whether a function is set in
 * this array and only runs the generic interrupt handler code, if
 * the function pointer is NULL */
typedef int (* yabel_handleIntFunc)(void);
extern yabel_handleIntFunc yabel_intFuncArray[256];
void mainboard_interrupt_handlers(int, yabel_handleIntFunc);

struct device;

u32 biosemu(u8 *biosmem, u32 biosmem_size, struct device *dev, unsigned long rom_addr);
#endif
