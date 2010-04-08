/* 
 * Copyright 2002 Eric Biederman
 *
 * This file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */


#include <arch/asm.h>
#include <arch/intel.h>
#include <console/loglevel.h>	

#ifndef ASM_CONSOLE_LOGLEVEL
#define ASM_CONSOLE_LOGLEVEL CONFIG_MAXIMUM_CONSOLE_LOGLEVEL
#endif

/*
 * This is the entry code the code in .reset section
 * jumps to this address.
 *
 */
.section ".rom.data", "a", @progbits
.section ".rom.text", "ax", @progbits

	post_code(0x01)             /* delay for chipsets */

