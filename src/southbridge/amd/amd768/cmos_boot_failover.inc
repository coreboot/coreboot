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
	
	/* See if the CMOS error condition has been flagged */
	movb	$RTC_REG_D, %al
	outb	%al, $0x70
	inb	$0x71, %al
	andb	$RTC_VRT, %al
	testb	%al, %al
	jz	__rtc_failed

	/* Compute the cmos checksum, and see if it is valid */
	movb	$PC_CKS_RANGE_START, %bl
	xorl	%ecx, %ecx
1:
	movb	%bl, %al
	outb	%al, $0x70
	xorl	%eax, %eax
	inb	$0x71, %al
	addl	%eax, %ecx
	incb	%bl
	cmpb	$PC_CKS_RANGE_END, %bl
	jbe	1b

	/* Compute the cmos checksum, and see if it is valid */
	/* Read the stored checksum */
	movb	$(PC_CKS_LOC), %al
	outb	%al,$0x70
	inb	$0x71, %al
	movb	%al, %bh
	movb	$(PC_CKS_LOC+1), %al
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
	outb	%al, $0x71
	jmp	__rtc_ok

	/* The byte is o.k. see where to go */
__rtc_ok:
	movb	$RTC_BOOT_BYTE, %al
	outb	%al, $0x70
	inb	$0x71, %al

	/* Transfer the boot bit from bit 0 to bit 1. 
	 * And clear bit 0 so that unless we say it works we 
	 * fallback to the other bios image immediately.
	 */
	movb	%al, %bl
	andb	$0xfc, %al
	andb	$1, %bl
	shlb	%bl
	orb	%bl, %al
	outb	%al, $0x71

	testb	$(1<<1), %al
	jz	__failover_boot

	/* Enable 4MB rom access at 0xFFC00000 - 0xFFFFFFFF */
	movl	$(0x80000000 | AMD766_DEV | 0x40), %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$(0xcfc + 3), %dx
	inb	%dx, %al
	orb	$0x80, %al
	outb	%al, %dx

	jmp	__normal_image
__failover_boot:
