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
 *
 *     We use 6 bits of CMOS ram.
 *     Bit 0: Initial boot direction
 *            0 - Boot the failsafe image.
 *            1 - Boot the other image.
 *     Bit 1: Reboot direction
 *            0 - Boot the failsafe image.
 *            1 - Boot the other image.
 *     Bits 4-7: Reboot count
 *
 *     On initial boot we read bit0, and write it to bit1, and clear
 *     bit0.  Additionally we clear bit1 if we bit0 was initially set
 *     and we have reached the maximum boot count. 
 *
 *     On a reboot the bits are not touched.
 *     
 *     Then the appropriate image is jumped to.
 *    
 */
#include "82801.h"
#include <pc80/mc146818rtc.h>
#include <part/fallback_boot.h>

#ifndef MAX_REBOOT_CNT
#error "MAX_REBOOT_CNT not defined"
#endif
#if MAX_REBOOT_CNT > 15
#error "MAX_REBOOT_CNT to large"
#endif

	/* Intel systems magically stop the second cpu in hardware */
	testb	%al, %al
	jz	__failover_boot

__failover_reset:
	movb	$RTC_BOOT_BYTE, %al
	outb	%al, $0x70
	inb	$0x71, %al
	testb	$(1<<1), %al
	jnz	__normal_image
	jmp	__cpu_reset
	

__failover_boot:
	/* See if the cmos clear jumper has been set */
	movl	$((RTC_DEVFN << 8) | GEN_PMCON_3), %eax
	PCI_READ_CONFIG_DWORD
	testl	$RTC_FAILED, %eax
	jz	__cs_test

	/* There are no impossible values, no checksums
	 * so just trust whatever value we have in the
	 * cmos.
	 */
__rtc_failed:
	movb	$RTC_BOOT_BYTE, %al
	outb	%al, $0x70
	inb	$0x71, %al
	andb	$0xfc, %al
	outb	%al, $0x71
	jmp	__cs_test

	/* test the checksum */
__cs_test:
	movl	$77,%ecx
	xor	%ebx,%ebx
	movl    $RTC_BOOT_BYTE, %edx
1:
	addl    $1, %edx
	movl	%edx, %eax
        outb    %al, $0x70
        inb     $0x71, %al
	addl	%eax,%ebx
	subl	$1,%ecx
	jnz	1b
	not	%ebx
	addl    $1, %edx
	movl	%edx, %eax
        outb    %al, $0x70
        inb     $0x71, %al
	movb	%al,%ch
        addl    $1, %edx
	movl	%edx, %eax
        outb    %al, $0x70
        inb     $0x71, %al
	movb	%ch,%ah
	cmpw	%ax,%bx
	jz	__rtc_ok
	/* Set to fall back mode */
	movb	$RTC_BOOT_BYTE, %al
	outb	%al, $0x70
	inb	$0x71, %al
	andb	$0xfc, %al
	outb	%al, $0x71	
	
	/* The byte is o.k. see where to go */
__rtc_ok:
	movb	$RTC_BOOT_BYTE, %al
	outb	%al, $0x70
	inb	$0x71, %al

	movb	%al, %bl
	movb	%al, %cl
	andb	$0x0c, %al	/* Remove the boot bits from %al */
	shrb	$4, %cl		/* Isolate the boot count */

	/* If we are in fallback mode set the reboot count to max */
	testb	$1, %bl
	jnz	__reboot_cnt_read
	movb	$MAX_REBOOT_CNT, %cl
__reboot_cnt_read:
	/* Increment the reboot count and see if we have hit the limit */
	incb	%cl
	cmpb	$MAX_REBOOT_CNT, %cl
	jb	__reboot_cnt_ok
__reboot_cnt_bad:
	xorb	%cl, %cl	/* clear the boot count in fallback */
	xorb	%bl, %bl	/* clear the boot bits in fallback */
	jmp	__reboot_cnt_set

__reboot_cnt_ok:
	movb	$((1<<1)|(1<<0)), %bl	/* Set boot_option=1 last_boot=1 */
	/* Save the new boot bits */
__reboot_cnt_set:
	shlb	$4, %cl
	orb	%bl, %al
	orb	%cl, %al
	outb	%al, $0x71

	testb	$(1<<1), %al
	jnz	__normal_image
