/* The algorithm is as follows:
 */
#include <pc80/mc146818rtc.h>
#include <part/fallback_boot.h>


#define PM_DEVFN ((0x1f <<3) + 0)

	.section ".rom.data"

pci_clk_stop_beg:
	.byte	0x08, 0x47, 0x7f, 0xc7, 0x38
pci_clk_stop_end:

pci_clk_start_beg:
	.byte	0x08, 0x47, 0x7f, 0xc7, 0x3f
pci_clk_start_end:

pci_clk_restart_msg:
	.asciz "Restarting pci clocks\r\n"
pci_clk_power_msg:
	.asciz "Power failed\r\n"

	.previous

	/* The PC64H2 has a known errata where there is a race
	 * between getting a good level, and getting a clock signal.  
	 * Nasting things normally boot failure while readding the pci
	 * bus happen when this errata is hit.
	 * 
	 * Thw work around is to stop and restart the 66Mhz reference clock
 	 * the PC64H2.  And to do this I need to talk to the reference clock
	 * generator in this case a cypress CY28329.
	 *
 	 * The I2C interface on the Cyrpess chip is fragile.  If you talk
	 * to it wrong or if you talk to it long enough after getting power,
	 * it will not talk to you.  So I have to be very careful.  What is
	 * especially bad is that sometimes a failed attempt to talk
	 * to the Cypress chip will lock the i2c/smbus controller on
	 * the ICH3.  So I cannot talk to i2c EEPROM on memory dimms
	 * to detect how much memory I have.
	 *
	 * Placing the motherboard in soft power off is not enough to get the
	 * cypress chip talking again.  So I only attempt the PC64H2
	 * errata work around when I boot up immediate after the
	 * motherboard has lost power.
	 *
	 * This procedure appears to navigate the cascading mines of
	 * the hardware bugs.
	 */
__reboot_ck:
	/* See if power is failed, making it is safe to talk to the
	 * cypress chip. 
	 */ 
	movl	$((PM_DEVFN << 8) + 0xA4), %eax
	PCI_READ_CONFIG_DWORD

	/* If power has not failed get, skip the errata work around */
	testl	$(1 << 1), %eax
	jz	__continue_booting

	/* Clear the power failed bit, so I don't apply this
	 * work around when it is not safe.
	 */
	movl	$((PM_DEVFN << 8) + 0xA4), %eax
	movl	$(1 << 1), %ecx
	PCI_WRITE_CONFIG_DWORD

	/* Report that I have detected a power failure */
	CONSOLE_INFO_TX_STRING($pci_clk_power_msg)

	/* Attempt to write to the cypress chip and bail out if this
	 * fails.
	 */
	/* stop the clocks */
	movb	$0x69, %bl
	movb	$0x00, %bh
	movb	$(pci_clk_stop_end - pci_clk_stop_beg), %cl
	movl	$(pci_clk_stop_beg), %esi
	/* stop the pci clock */
	CALLSP(smbus_write_block)
	jc	__continue_booting

	/* wait for about 100us */
	movl	$0x2c, %ecx
	rdmsr
	andl	$0xff000000,%eax
	shrl	$10, %eax      /* this gives the ticks for 160usec */
	movl	%eax, %ebx
	rdtsc
	addl	%ebx, %eax
	adcl	$0, %edx
	movl	%eax, %ebx
	movl	%edx, %ecx
1:	rdtsc
	subl	%ebx, %eax
	sbbl	%ecx, %edx
	jb	1b

	/* Add an extra delay to display this message */
	CONSOLE_INFO_TX_STRING($pci_clk_restart_msg)

	/* restart the clocks */
	movb	$0x69, %bl
	movb	$0x00, %bh
	movb	$(pci_clk_start_end - pci_clk_start_beg), %cl
	movl	$(pci_clk_start_beg), %esi
	/* start the pci clock */
	CALLSP(smbus_write_block)

	/* Set boot_option to last_boot, so I don't fallback by accident */
        movb    $RTC_BOOT_BYTE, %al
        outb    %al, $0x70
        inb     $0x71, %al
	movb	%al, %bl
	shrb	$1, %bl
	andb	$1, %bl
        andb    $0xfe, %al
	orb	%bl, %al
        outb    %al, $0x71

	/* reboot the motherboard */
	movw	$0x0cf9, %dx
	inb	%dx, %al
	andb	$~((1 <<3)|(1<<2)), %al
	orb	$(1<<1), %al
	outb	%al, %dx
	orb	$(1<<2), %al
	outb	%al, %dx
2:	hlt
	jmp	2b

__continue_booting:
