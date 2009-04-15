/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric W. Biederman <ebiederm@xmission.com>
 * Copyright (C) 2003 Linux Networx
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

	/* enable LPC superio on the AMD 768 south bridge */
#if 0
	movl	$(0x80000000 | AMD768_DEV | 0x54), %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfc, %dx
	inb	%dx, %al
	orb	$0x30, %al
	outb	%al, %dx

	/* enable LPC serial port 0x3f8 */
	movl	$(0x80000000 | AMD768_DEV | 0x50), %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfe, %dx
	movb	$0x8, %al
	outb	%al, %dx
#endif
