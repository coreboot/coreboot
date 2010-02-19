/* -*- asm -*-
 * $ $
 *
 */

/* 
 * Copyright (C) 1996-2002 Markus Franz Xaver Johannes Oberhumer
 *
 * This file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Originally this code was part of ucl the data compression library
 * for upx the ``Ultimate Packer of eXecutables''.
 *
 * - Converted to gas assembly, and refitted to work with etherboot.
 *   Eric Biederman 20 Aug 2002
 * - Merged the nrv2b decompressor into crt0.base of coreboot
 *   Eric Biederman 26 Sept 2002
 */


#include <arch/asm.h>
#include <arch/intel.h>
#include <console/loglevel.h>	

/*
 * This is the entry code the code in .reset section
 * jumps to this address.
 *
 */
.section ".rom.data", "a", @progbits
.section ".rom.text", "ax", @progbits

	intel_chip_post_macro(0x01)             /* delay for chipsets */

