/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __ARM_ASM_H
#define __ARM_ASM_H

#if defined __arm__
#  define ARM(x...)	x
#  define THUMB(x...)
#  define W(instr)	instr
#elif defined __thumb__
#  define ARM(x...)
#  define THUMB(x...)	x
#  define W(instr)	instr.w
#  if __COREBOOT_ARM_ARCH__ < 7
#    error thumb mode has not been tested with ARM < v7!
#  endif
#else
#  error Not in ARM or thumb mode!
#endif

#define ALIGN .align 0

#define ENDPROC(name) \
	.type name, %function; \
	END(name)

#define ENTRY(name) \
	.globl name; \
	ALIGN; \
	name:

#define END(name) \
	.size name, .-name

#endif	/* __ARM_ASM_H */
