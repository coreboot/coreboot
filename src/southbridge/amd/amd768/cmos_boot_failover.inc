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

/* The algorithm is as follows:
 *
 * Step 1:  Test for cpu reset
 *     That is, did I just boot or is this a later boot since power on.
 *     The result of this test in %al
 *     %al == 1 -- We are rebooting
 *     %al == 0 -- This is the initial boot
 *
 * Step 2:  Cpu filter
 *     On an initial boot if we are not the bootstrap CPU go to
 *     sleep.
 *
 * Step 3:  Test for CMOS validity.
 *      On an initial boot if the CMOS checksum or the CMOS error
 *      condition is signaled clear the CMOS ram.
 *
 * Step 4:  Test for which copy of linuxbios to boot.
 *     We use 2 bits of CMOS ram.
 *     Bit 0: Initial boot direction
 *            0 - Boot the failsafe image.
 *            1 - Boot the other image.
 *     Bit 1: Reboot direction
 *            0 - Boot the failsafe image.
 *            1 - Boot the other image.
 *
 *     On initial boot we read bit0, and write it to bit1, and clear
 *     bit0.  
 *
 *     On a reboot the bits are not touched.
 *     
 *     Then the appropriate image is jumped to.
 *
 *    
 */
#include <pc80/mc146818rtc.h>
#include <part/fallback_boot.h>

#if 0
#define MAX_REBOOT_CNT 8
#endif

	/* See if the CMOS error condition has been flagged */
	movb	$RTC_REG_D, %al
	outb	%al, $0x70
	inb	$0x71, %al
	andb	$RTC_VRT, %al
	testb	%al, %al
	jz	__rtc_failed

	/* Compute the cmos checksum, and see if it is valid */
	movb	$LB_CKS_RANGE_START, %bl
	xorl	%ecx, %ecx
1:
	movb	%bl, %al
	outb	%al, $0x70
	xorl	%eax, %eax
	inb	$0x71, %al
	addl	%eax, %ecx
	incb	%bl
	cmpb	$LB_CKS_RANGE_END, %bl
	jbe	1b

	/* Compute the cmos checksum, and see if it is valid */
	/* Read the stored checksum */
	movb	$(LB_CKS_LOC), %al
	outb	%al,$0x70
	inb	$0x71, %al
	movb	%al, %bh
	movb	$(LB_CKS_LOC+1), %al
	outb	%al, $0x70
	inb	$0x71, %al
	movb	%al, %bl

	/* Now finally test the checksum */
	xorw	$0xffff, %cx
	cmpw	%cx, %bx
	je	__rtc_ok

	/* There are no impossible values, no checksums
	 * so just trust whatever value we have in the
	 * cmos, but clear the fallback bit.
	 */
__rtc_failed:
	movb	$RTC_BOOT_BYTE, %al
	outb	%al, $0x70
	inb	$0x71, %al
	andb	$0xfc, %al
	orb	$(MAX_REBOOT_CNT << 4), %al
	outb	%al, $0x71
	jmp	__rtc_ok

	/* The byte is o.k. see where to go */
__rtc_ok:
	movb	$RTC_BOOT_BYTE, %al
	outb	%al, $0x70
	inb	$0x71, %al
	testb	$1, %al		/* are we in normal mode */
	jz	2f
	andb	$0x0f, %al	/* if yes, clear the boot count */
	/* Transfer the boot bit from bit 0 to bit 1. 
	 * And clear bit 0 so that unless we say it works we 
	 * fallback to the other bios image immediately.
	 */
2:	movb	%al, %bl
	shrb	$4, %bl		/* are we already at the max cnt */
	cmpb	$MAX_REBOOT_CNT, %bl
	jz	1f
	addb	$(1<<4), %al	/* no, so add one count */
	movb	%al, %bl
	shrb	$4, %bl		/* now are we at the max count */
	cmpb	$MAX_REBOOT_CNT, %bl
	jnz	1f
	andb	$0xfc, %al	/* yes, so put in fallback mode */
1:	movb	%al, %bl
	testb	$0xe0, %al	/* is this the first boot */
	jnz	2f
	andb	$1, %bl		/* if yes, shift the boot bits */
	shlb	%bl
	andb    $0xfc, %al
	orb	%bl, %al
2:	outb	%al, $0x71	/* save the boot byte */
	movb    %al, %bl
	shrb    $4, %bl         /* are we at the max count */
	cmpb    $MAX_REBOOT_CNT, %bl
	jz	__failover_boot

	/* Enable 4MB rom access at 0xFFC00000 - 0xFFFFFFFF */
	movl	$(0x80000000 | AMD768_DEV | 0x40), %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$(0xcfc + 3), %dx
	inb	%dx, %al
	orb	$0x80, %al
	outb	%al, %dx
	jmp	__normal_image

__failover_boot:
