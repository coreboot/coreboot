/* Very early on the K7 needs to setup it's multiprocessor state */

	/* Extended BIU Control
	 * 0x44
	 * [23:20] P1 Speculative Read Data Movement Enable 
	 *         0000 == Function Disabled
	 *         0001 == One Clock
	 *         0010 == Two Clocks
	 *         ....
	 *         1111 == Fifteen Clocks
	 *
	 * [19:16] P0 Speculate Read Data Movement Enable
	 *         0000 == Function Disabled
	 *         0001 == One Clock
	 *         0010 == Two Clocks
	 *         ....
	 *         1111 == Fifteen Clocks
	 *
 	 * [13:11] P1 Write Data Delay (Status Register)
 	 * [10: 8] P0 Write Data Delay (Status Register)
 	 * [ 7: 7] 1 == Defer Write Data Movement
	 * [ 4: 4] P1 2 Bit Times Per Frame Enable
	 *         Use 0 for alpha 1 for athlon
	 * [ 3: 3] P0 2 Bit Times Per Frame Enable
	 *         Use 0 for alpha 1 for athlon
	 *
	 */

	/* BIU[01] Status/Control
	 * 0x60, 0x68
	 * [31:31] Probe_Enable
	 *         1 == Probes are sent to this processor
	 *         0 == Probes are not sent to this processor
	 * [27:25] Xca_Probe_Count
	 *         Must be non-zero
	 *         Recommended value 0x2
	 * [24:22] Xca_Read_Count
	 *         Must be non-zero
	 *         Recommended value 0x6
	 * [21:19] Xca_Write_Count
	 *         Must be non-zero
	 *         Recommended value 0x6
	 * [18:18] Halt_Disconnect_Enable
	 *         0 == No AMD Athlon system bus disconnect following HALT
	 *         1 == AMD Athlon system bus discconnect following HALT
	 * [17:17] Stop_Grant_Disconnect_Enable
	 *         0 == No AMD Athlon system bus disconnect following Stop/Grant
	 *         1 == AMD Athlon system bus disconnect following Stop/Grant
	 *              special cycle
	 * [16:14] Probe_Limit
	 *         000 = 1 probe
	 *         001 = 2 probes
	 *         ...
	 *         111 = 8 probes
	 * [13:10] Ack_Limit (Read only)
	 *         0000 = 1 unacknowledged command
	 *         0001 = 2 unacknowledged commands
	 *         ...
	 *         1111 = 16 unacknowledged commands
	 * [09:09] Bypass_Enable
	 *         0 == No don't bypass certain memory pipe stages
	 *         1 == bypass some memory stages for optimal performace
	 *              - Requires single CPU system
	 *              - Requires clock multipler >= 4
	 * [08:07] SysDC_Out_Enable
	 *         Initialized by pinstrapping during reset
	 *         00 == Reserved
	 *         01 == 1 clock
	 *         10 == 2 clocks
	 *         11 == 3 clocks
	 * [06:03] SysDC_In_Enable
	 *         Initialized during pinstrapping
	 *         0000 == 1 clock
	 *         0001 == 2 clocks
	 *         1111 == 16 clocks
	 * [02:02] WR2_Read
	 *         Initialized during pinstrapping
	 * [01:00] WR2_Write
	 *         Initialized during pinstrapping
	 */

#include <cpu/k7/mtrr.h>
#include <cpu/p6/apic.h>

#define BIU_CONTROL_MASK 0x00003dff
#define BIU_CONTROL ((1 << 31)|(0x2 << 25)|(0x6 << 22)|(0x6 << 19)|(0 << 18)\
	|(1 << 17)|(7<<14)|(0<<9))
#define EBIU_MASK   0xffffe4ff
#define EBIU_VALUE  ((0<<20)|(0<<16)|(1<<4)|(1<<3))

#define PCI_CONFIG_ADDR 0xFE000000
#define PCI_IO_ADDR	0xFC000000
#define PCI_IACK_ADDR	0xF8000000
#define PCI_SPEC_ADDR	0xF8000000

	/* Read the northbridge whami register
	 */
	movl	$0x80000080, %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfc, %dx
	inl	%dx, %eax

	/* Isolate the processor id */
	movl	%eax, %ebx
	movl	%eax, %esi
	andl	$0xf, %esi

	/* Set the processor id. */
	movl	$HWCR_MSR, %ecx
	rdmsr
	andl	$0x0FFFFFFF, %eax
	shll	$28, %esi
	orl	%esi, %eax
	shrl	$28, %esi
	/* Enable  SSE detection */
	andl	$(~(1<<15)), %eax
	/* Set PFER */
	orl	$(1<<12), %eax
	/* Say the page tables live in cachable memory */
	orl	$(1<< 3), %eax
	wrmsr


	/* Figure out how many unacknowledged acks
	 * the AMD762 northbridge can support 
	 */
	shll	$3, %esi
	movl	$0x80000060, %eax
	orl	%esi, %eax
	shrl	$3, %esi
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfc, %dx
	inl	%dx, %eax
	
	shrl	$10, %eax
	andl	$0xf, %eax
	addl	$1, %eax

	/* Move the count into a safe place. */
	movl	%eax, %ebp

	/* Tell the processor how many unacknoledged probes the AMD762 can
	 * support 
	 */
	movl	$SYSCFG_MSR, %ecx
	rdmsr
	andl	$(~(0x1f<< 0)), %eax
	andl	$0x1f, %ebp
	orl	%ebp, %eax
	wrmsr

	/* Enable the local apic, and map it where we expect it. */
	movl $APIC_BASE_MSR, %ecx
	rdmsr
	orl	$APIC_BASE_MSR_ENABLE, %eax
	andl	$(~APIC_BASE_MSR_ADDR_MASK), %eax
	orl	$APIC_DEFAULT_BASE, %eax
	xorl	%edx, %edx
	wrmsr
	/* Save off the apic address */
	movl	%eax, %edi
	andl	$APIC_BASE_MSR_ADDR_MASK, %edi

	/* Set the local apicid */
	xorl	%eax, %eax
	shll	$24, %esi
	movl	%esi, APIC_ID(%edi)
	shrl	$24, %esi

	/* Set the enabled bit for the local apic
	 * and clear the spurious vector.
	 */
	movl	APIC_SPIV(%edi), %eax
	andl	$0xfffffe0f, %eax
	orl	$APIC_SPIV_ENABLE, %eax
	movl	%eax, APIC_SPIV(%edi)

	/* See if I'm the first processor to initialize. */
	cmpb	%bl, %bh 
	je	bootstrap_cpu

	/* I'm not the bootstrap processor */
	/* clear the BSP bit */
	movl	$APIC_BASE_MSR, %ecx
	rdmsr
	andl	$(~APIC_BASE_MSR_BOOTSTRAP_PROCESSOR), %eax
	wrmsr

	/* Send an APIC INIT to myself */
	movl	%esi, %eax
	shll	$24, %eax
	movl	%eax, APIC_ICR2(%edi)

	movl	$((0<<18)|(1<<15)|(1<<14)|(0<<11)|(5 << 8)|(0<<0)), %eax
	movl	%eax, APIC_ICR(%edi)
	
	/* Wait for the ipi send to finish */
1:	movl	APIC_ICR(%edi), %eax
	testl	$APIC_ICR_BUSY, %eax
	jnz	1b

	/* Deassert the APIC INIT */
	movl	%esi, %eax
	shll	$24, %eax
	movl	%eax, APIC_ICR2(%edi)

	movl	$((0<<18)|(1<<15)|(0<<14)|(0<<11)|(5 << 8)|(0<<0)), %eax
	movl	%eax, APIC_ICR(%edi)

	/* Wait for the ipi send to finish */
1:	movl	APIC_ICR(%edi), %eax
	testl	$APIC_ICR_BUSY, %eax
	jnz	1b

wait_for_startup_ipi:
	hlt
	jmp wait_for_startup_ipi


bootstrap_cpu:
	/* Set the bootstrap processor flag */
	movl	$APIC_BASE_MSR, %ecx
	rdmsr
	orl	$APIC_BASE_MSR_BOOTSTRAP_PROCESSOR, %eax
	wrmsr


init_cpu_bus_all:
	/* Setup Common AMD Athlon system bus */
	movl	$0x80000044, %eax
	mov	$0xcf8, %dx
	outl	%eax, %dx
	mov	$0xcfc, %dx
	inl	%dx, %eax
	andl	$EBIU_MASK, %eax
	orl	$EBIU_VALUE, %eax
	outl	%eax, %dx
	
init_cpu_bus_0:	
	testl	$(1<<16), %ebx
	jz	init_cpu_bus_0_done
	/* Setup the AMD Athlon system bus */
	movl	$0x80000060, %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfc, %dx
	inl	%dx, %eax
	andl	$BIU_CONTROL_MASK, %eax
	orl	$BIU_CONTROL, %eax
	outl	%eax, %dx
init_cpu_bus_0_done:

init_cpu_bus_1:	
	testl	$(2<<16), %ebx
	jz	init_cpu_bus_1_done
	/* Setup the AMD Athlon system bus */
	movl	$0x80000068, %eax
	movw	$0xcf8, %dx
	outl	%eax, %dx
	movw	$0xcfc, %dx
	inl	%dx, %eax
	andl	$BIU_CONTROL_MASK, %eax
	orl	$BIU_CONTROL, %eax
	outl	%eax, %dx
init_cpu_bus_1_done:


/* FIXME enable superbypass memory accesses if only cpu0 is present,
 * and we have a cpu multiplier > 4.
 */
