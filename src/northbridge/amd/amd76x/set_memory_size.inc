/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2001-2003 Eric W. Biederman <ebiederm@xmission.com>
 * Copyright (C) 2001-2003 Linux Networx
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
 */

jmp set_memory_size_out

#include <cpu/k7/mtrr.h>
set_memory_size:
	/* Read the top of memory */
	movl	$0x9c, %eax
	PCI_READ_CONFIG_DWORD

	/* Set the Athlon Top of memory MSR */
	xorl	%edx, %edx
	movl	$TOP_MEM, %ecx
	wrmsr
	movl	$TOP_MEM2, %ecx
	wrmsr

	/* Clear the IOrr's */
	xorl	%edx, %edx
	xorl	%eax, %eax
	movl	$IORR0_BASE, %ecx
	wrmsr
	movl	$IORR0_MASK, %ecx
	wrmsr
	movl	$IORR1_BASE, %ecx
	wrmsr
	movl	$IORR1_MASK, %ecx
	wrmsr

set_memory_size_end:
	RETSP
set_memory_size_out:
