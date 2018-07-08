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
#ifndef _BIOSEMU_DEBUG_H_
#define _BIOSEMU_DEBUG_H_

#include <timer.h>
#include <types.h>

#if IS_ENABLED(CONFIG_X86EMU_DEBUG_TIMINGS)
extern struct mono_time zero;
#endif
extern u32 debug_flags;
// from x86emu...needed for debugging
extern void x86emu_dump_xregs(void);

/* printf is not available in coreboot... use printk */
#include <console/console.h>
#include "x86emu/x86emu.h"
#define printf(x...) printk(BIOS_DEBUG, x)

/* PH: empty versions of set/clr_ci
 * TODO: remove! */
static inline void clr_ci(void) {};
static inline void set_ci(void) {};

/* debug_flags is a binary switch that allows you to select the following items
 * to debug. 1=on 0=off. After you decide what you want to debug create the
 * binary value, convert to hex and set the option. These options can be
 * selected in Kconfig.
 *
 * |-DEBUG_JMP - print info about JMP and RETF opcodes from x86emu
 * ||-DEBUG_TRACE_X86EMU - print _all_ opcodes that are executed by x86emu (WARNING: this will produce a LOT of output)
 * |||-Currently unused
 * ||||-Currently unused
 * |||||-Currently unused
 * ||||||-DEBUG_PNP - Print Plug And Play access made by option rom
 * |||||||-DEBUG_DISK - Print Disk I/O related messages, currently unused
 * ||||||||-DEBUG_PMM - Print messages related to POST Memory Manager (PMM)
 * |||||||||-DEBUG_VBE - Print messages related to VESA BIOS Extension (VBE) functions
 * ||||||||||-DEBUG_PRINT_INT10 - let INT10 (i.e. character output) calls print messages to Debug output
 * |||||||||||-DEBUG_INTR - Print messages related to interrupt handling
 * ||||||||||||-DEBUG_CHECK_VMEM_ACCESS - Print messages related to accesse to certain areas of the virtual Memory (e.g. BDA (BIOS Data Area) or Interrupt Vectors)
 * |||||||||||||-DEBUG_MEM - Print memory access made by option ROM (NOTE: this also includes accesses to fetch instructions)
 * ||||||||||||||-DEBUG_IO - Print I/O access made by option rom
 * 11000111111111 - Max Binary Value, Debug All (WARNING: - This could run for hours)
 */

#define DEBUG_IO 0x1
#define DEBUG_MEM 0x2
// set this to print messages for certain virtual memory accesses (Interrupt Vectors, ...)
#define DEBUG_CHECK_VMEM_ACCESS 0x4
#define DEBUG_INTR 0x8
#define DEBUG_PRINT_INT10 0x10	// set to have the INT10 routine print characters
#define DEBUG_VBE 0x20
#define DEBUG_PMM 0x40
#define DEBUG_DISK 0x80
#define DEBUG_PNP 0x100

#define DEBUG_TRACE_X86EMU 0x1000
// set to enable tracing of JMPs in x86emu
#define DEBUG_JMP 0x2000

#if IS_ENABLED(CONFIG_X86EMU_DEBUG)

#define CHECK_DBG(_flag) if (debug_flags & _flag)

#define DEBUG_PRINTF(_x...) printf(_x);
// prints the CS:IP before the printout, NOTE: actually its CS:IP of the _next_ instruction
// to be executed, since the x86emu advances CS:IP _before_ actually executing an instruction

#if IS_ENABLED(CONFIG_X86EMU_DEBUG_TIMINGS)
#define DEBUG_PRINTF_CS_IP(_x...) DEBUG_PRINTF("[%08lx]%x:%x ", (current_time_from(&zero)).microseconds, M.x86.R_CS, M.x86.R_IP); DEBUG_PRINTF(_x);
#else
#define DEBUG_PRINTF_CS_IP(_x...) DEBUG_PRINTF("%x:%x ", M.x86.R_CS, M.x86.R_IP); DEBUG_PRINTF(_x);
#endif

#define DEBUG_PRINTF_IO(_x...) CHECK_DBG(DEBUG_IO) { DEBUG_PRINTF_CS_IP(_x) }
#define DEBUG_PRINTF_MEM(_x...) CHECK_DBG(DEBUG_MEM) { DEBUG_PRINTF_CS_IP(_x) }
#define DEBUG_PRINTF_INTR(_x...) CHECK_DBG(DEBUG_INTR) { DEBUG_PRINTF_CS_IP(_x) }
#define DEBUG_PRINTF_VBE(_x...) CHECK_DBG(DEBUG_VBE) { DEBUG_PRINTF_CS_IP(_x) }
#define DEBUG_PRINTF_PMM(_x...) CHECK_DBG(DEBUG_PMM) { DEBUG_PRINTF_CS_IP(_x) }
#define DEBUG_PRINTF_DISK(_x...) CHECK_DBG(DEBUG_DISK) { DEBUG_PRINTF_CS_IP(_x) }
#define DEBUG_PRINTF_PNP(_x...) CHECK_DBG(DEBUG_PNP) { DEBUG_PRINTF_CS_IP(_x) }

#else

#define CHECK_DBG(_flag) if (0)

#define DEBUG_PRINTF(_x...)
#define DEBUG_PRINTF_CS_IP(_x...)

#define DEBUG_PRINTF_IO(_x...)
#define DEBUG_PRINTF_MEM(_x...)
#define DEBUG_PRINTF_INTR(_x...)
#define DEBUG_PRINTF_VBE(_x...)
#define DEBUG_PRINTF_PMM(_x...)
#define DEBUG_PRINTF_DISK(_x...)
#define DEBUG_PRINTF_PNP(_x...)

#endif				//DEBUG

void dump(u8 *addr, u32 len);

#endif
