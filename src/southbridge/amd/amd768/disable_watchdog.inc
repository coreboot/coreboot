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

#define PM_DEV_FN (AMD768_DEV + 0x300)
	/* Disable the watchdog timer */
	movl	$(0x80000000 | PM_DEV_FN | 0x49), %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfd, %dx
	inb	%dx, %al
	orb	$0x04, %al
	outb	%al, %dx
	movl	$(0x80000000 | PM_DEV_FN | 0x41), %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfd, %dx
	inb	%dx, %al
	orb	$0x40, %al
	outb	%al, %dx
	/* added from sourceforge */
        movl    $(0x80000000 | PM_DEV_FN | 0x48), %eax
        movw    $0xcf8, %dx
        outl    %eax, %dx
        movw    $0xcfd, %dx
        inb     %dx, %al
        orb     $0x8, %al
        outb    %al, %dx


