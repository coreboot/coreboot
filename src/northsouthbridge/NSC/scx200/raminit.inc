/* freebios/src/northsouthbridge/NSC/scx200/raminit.inc

   Copyright (c) 2002 Christer Weinigel <wingel@hack.org>

   Do memory sizing for a National Semiconductor SCx200 CPU.

   FIXME -- this isn't completely stable, it fails to detect
   any DIMMS every now and then.  I ought to check the memory
   timing I suppose.  enable_dimms is a prime suspect.
*/

/**********************************************************************/

#define MC_MEM_CNTRL1		0x8400
#define     SDCLK_START		(1<<17)
#define     TEST_REF		(1<<4)
#define     PROGRAM_SDRAM	(1<<1)

#define MC_MEM_CNTRL2		0x8404
#define     SDCLK_MASK		0x000003c0
#define     SDCLKOUT_MASK	0x00000400

#define MC_BANK_CFG		0x8408
#define     DIMM_PG_SZ		0x00000070
#define     DIMM_SZ		0x00000700
#define     DIMM_COMP_BNK	0x00001000
#define     DIMM_MOD_BNK	0x00004000

#define MC_SYNC_TIM1		0x840c

/**********************************************************************/

#define TEST_DATA1 0x05A5A5A5A
#define TEST_DATA2 0x0DEADBEEF

#define NUM_REFRESH 8

/* Destroys EAX, high word of ECX and low word of EDI */
#define DO_REFRESH \
        movw    $MC_MEM_CNTRL1, %di ;\
	movl	%es:(%edi), %eax ;\
	orb	$TEST_REF, %al ;\
	shll	$16, %ecx /* save CX */ ;\
	movw	$NUM_REFRESH, %cx ;\
1:	movl	%eax, %es:(%edi) ;\
	loopw	1b ;\
	shrl	$16, %ecx /* restore CX */ ;\
	andb	$~TEST_REF, %al ;\
	movl	%eax, %es:(%edi)

/* Destroys high word of ECX */
#define DO_DELAY(count) \
	shll	$16, %ecx /* save CX */ ;\
        movw	count, %cx ;\
1:	outb	%al, $0xed ;\
        loopw	1b ;\
	shrl	$16, %ecx /* restore CX */

/**********************************************************************/

scx200_setup_ram:
	CONSOLE_DEBUG_TX_STRING($str_setup_default_memory)

/* Initialize the GX1 configuration registers */
	movl	$cpu_init_len, %ecx
	leal	cpu_init_table, %esi
cpu_init_loop:
	movw	(%esi), %ax
	addl	$2, %esi
	outb	%al, $0x22
	movb	%ah, %al
	outb	%al, $0x23
	loop	cpu_init_loop

	/* Set up Memory controller, note that all the code below
	   assumes that the high word of %edi contains GX_BASE */
	movl	$GX_BASE, %edi
	
	movl	$mem_init_len, %ecx
	leal	mem_init_table, %esi

mem_init_loop:
	movw	(%esi), %di
	addl	$2, %esi
	movl	(%esi), %eax
	addl	$4, %esi
	movl	%eax, (%edi)
	loop	mem_init_loop

	CALLSP(enable_dimm)		/* Program the DIMM's */

	CONSOLE_DEBUG_TX_STRING($str_sizing_memory)
	
	/* EBP will contain the DIMM configuration after we're done.
           To begin with, it is set up for no DIMMs at all */
	movl	$0x00700070, %ebp

	/* Parameters for DIMM0 */
	movl	$0x00705740, %edx	/* DIMM0 is maxed, no DIMM0 */
	movb	$0, %cl			/* Shift value DIMM0=0, DIMM1=16 */

dimm_loop:
	movl	%edx, %esi
	CONSOLE_INFO_TX_STRING($str_probing)
	movb	%cl, %al
	rorb	$4, %al
	addb	$'0', %al
	CONSOLE_INFO_TX_CHAR(%al)
	CONSOLE_INFO_TX_STRING($str_nl)
	movl	%esi, %edx

	/* First configure the memory with the new settings */
        movw    $MC_BANK_CFG, %di
        movl    %edx, (%edi)
	DO_REFRESH

	/* Write something to location 0 and read it back */
	movl	$TEST_DATA1, 0		/* Write data to location 0 */
	movl	$0, 0x100		/* Clear the bus */

	movl	0, %eax			/* Read data from location 0 */
	movl	$0, 0			/* Clear location 0 */

	/* If we got the same value back, there is memory in the bank */
	cmpl	$TEST_DATA1, %eax
	jne	next_dimm

	movl	$TEST_DATA2, 0		/* Write data to location 0 */
	movl	$0, 0x100		/* Clear the bus */
	movl	0, %eax			/* Read data from location 0 */
	movl	$0, 0			/* Clear location 0 */

	cmpl	$TEST_DATA2, %eax
	jne	next_dimm

#ifdef DEBUG
	movl	%edx, %esi
	CONSOLE_DEBUG_TX_STRING($str_found)
	movb	%cl, %al
	rorb	$4, %al
	addb	$'0', %al
	CONSOLE_DEBUG_TX_CHAR(%al)
	CONSOLE_DEBUG_TX_STRING($str_nl)
	movl	%esi, %edx
#endif

/* Do page size detection, read the NatSemi reference drivers for an
   explanation of how it works */

	movl	$0x0040, %ebx		/* MC_BANK_CFG for 16kB page size */ 
	movl	$0x2000, %esi		/* Offset: 16kB/2 */

page_size_loop:
	movl	$TEST_DATA1, 0		/* Write data to location 0 */
	movl	$TEST_DATA2, (%esi)	/* Write to page size/2 */
	movl	$0, 0x100		/* Clear the bus */
	movl	0, %eax			/* Read data from location 0 */
	movl	$0, 0			/* Clear location 0 */

	cmpl	$TEST_DATA1, %eax	/* If no aliasing, we have found */
	je	page_size_found		/*     out the page size  */

	/* We had aliasing, so go to the next smaller setting */
	movw	$MC_BANK_CFG, %di
	movl	(%edi), %edx		/* Get current MC_BANK_CFG settings */
	movl    $~DIMM_PG_SZ, %eax	/* Mask for the page size */
	roll	%cl, %eax		/* Shift for the right DIMM */
	andl	%eax, %edx		/* Do the masking */
	subl	$0x10,%ebx		/* Select the next smaller page size */
	jc	mem_sizing_failed	/* Nothing matched, we've failed */
	shll	%cl, %ebx		/* Shift for the right DIMM */
	orl	%ebx, %edx		/* And merge it into the settings */
	shrl	%cl, %ebx		/* And shift it back again */
	shrl	$1, %esi		/* Divide test address by 2 */

	movl	%edx, (%edi)		/* Use the new MC_BANK_CFG settings */
	DO_REFRESH
	jmp	page_size_loop

page_size_found:
	/* Save the page size we found in EBP */
	movl	$~DIMM_PG_SZ, %eax
	roll	%cl, %eax
	andl	%eax, %ebp
	movl	%ebx, %eax
	shll	%cl, %eax
	or	%eax, %ebp

#ifdef DEBUG
	shll	$1, %esi
	movw	%si, %di
	movl	%edx, %esi
	CONSOLE_DEBUG_TX_STRING($str_page_size)
	xorl	%eax, %eax
	movw	%di, %ax
	CONSOLE_DEBUG_TX_HEX32(%eax)
	CONSOLE_DEBUG_TX_STRING($str_nl)
	movl	%esi, %edx
#endif

/* Do component banks detection */
	movl	$0x0800, %esi		/* Smallest page = 1KB * 2 banks */

	shll	$16, %ecx		/* Save DIMM shift  */
	movl	%ebp, %eax		/* Get the DIMM values */
	shrl	%cl, %eax		/*     shifted back */

	movw	%ax, %cx		/* Use the MC_BANK_CFG */
	shrw	$4, %cx			/*     page size setting as */
	andb	$7, %cl			/*     a shift to calculate */
	shll	%cl, %esi		/*     the page size */

	shrl	$16, %ecx		/* Restore DIMM shift */

	movl	$TEST_DATA1, 0		/* Write data to location 0 */
	movl	$TEST_DATA2, (%esi)	/* Write to next page */
	movl	$0, 0x100		/* Clear the bus */
	movl	0, %eax			/* Read data from location 0 */

	cmpl	$TEST_DATA1, %eax	/* If no aliasing, */
	je	component_banks_4	/*     there are 4 banks */

	/* Reconfigure the memory controller for 2 banks */
	movw	$MC_BANK_CFG, %di
	movl	(%edi), %edx
	movl	$~DIMM_COMP_BNK, %eax	/* Mask for component banks */
	roll	%cl, %eax		/*     shifted */
	andl	%eax, %ebp		/*     clear the bit of EBP */
	andl	%eax, %edx		/*     clear the bit of EDX */
	movl	%edx, (%edi)		/* Use new settings */
	DO_REFRESH
#ifdef DEBUG
	movw	$'2',%di
#endif
	jmp	component_banks_done

component_banks_4:
        movl    $DIMM_COMP_BNK, %eax	/* Set the bit of EBP */
        roll    %cl, %eax		/*    shifted */
        orl     %eax, %ebp
#ifdef DEBUG
	movw	$'4',%di
#endif
component_banks_done:
#ifdef DEBUG
	movl	%edx, %esi
	CONSOLE_DEBUG_TX_STRING($str_component_banks)
	movw	%di, %ax
	CONSOLE_DEBUG_TX_CHAR(%al)
	CONSOLE_DEBUG_TX_STRING($str_nl)
	movl	%esi, %edx
#endif

/* Do module banks detection */
	movl	$0x0800, %esi		/* Smallest page = 1KB * 2 banks */

	shll	$16, %ecx		/* Save DIMM shift  */
	movl	%ebp, %eax		/* Get the DIMM values */
	shrl	%cl, %eax		/*     shifted back */

	movw	%ax, %cx		/* Use the MC_BANK_CFG */
	shrw	$4, %cx			/*     page size setting as */
	andb	$7, %cl			/*     a shift to calculate */
	shll	%cl, %esi		/*     the page size */

	movw	%ax, %cx		/* Use the MC_BANK_CFG */
	shrw	$12, %cx		/*     component bank setting */
	andb	$1, %cl			/*     a shift to calculate */
	shll	%cl, %esi		/*     the total bank size */

	shrl	$16, %ecx		/* Restore DIMM shift */

	movl	$TEST_DATA1, 0		/* Write data to location 0 */
	movl	$TEST_DATA2, (%esi)	/* Write to module bank 2 */
	movl	$0, 0x100		/* Clear the bus */
	movl	(%esi), %eax		/* Read from module bank 2 */

	cmpl	$TEST_DATA2, %eax	/* If good data, */
	je	module_banks_2		/*     there are 2 module banks */

	/* Reconfigure the memory controller for 1 module bank */
	movw	$MC_BANK_CFG, %di
	movl	(%edi), %edx
	movl	$~DIMM_MOD_BNK, %eax	/* Mask for module banks */
	roll	%cl, %eax		/*     shifted */
	andl	%eax, %ebp		/*     clear the bit of EBP */
	andl	%eax, %edx		/*     clear the bit of EDX */
	movl	%edx, (%edi)		/* Use new settings */
	DO_REFRESH
#ifdef DEBUG
	movw	$'1',%di
#endif
	jmp	module_banks_done

module_banks_2:
        movl    $DIMM_MOD_BNK, %eax	/* Set the bit of EBP */
        roll    %cl, %eax		/*    shifted */
        orl     %eax, %ebp
#ifdef DEBUG
	movw	$'2',%di
#endif
module_banks_done:
#ifdef DEBUG
	movl	%edx, %esi
	CONSOLE_DEBUG_TX_STRING($str_module_banks)
	movw	%di, %ax
	CONSOLE_DEBUG_TX_CHAR(%al)
	CONSOLE_DEBUG_TX_STRING($str_nl)
	movl	%esi, %edx
#endif

/* Finally, figure out the size of the DIMM */
	movl	$0x0700, %ebx		/* MC_BANK_CFG for 512MB */ 
	movl	$0x10000000, %esi	/* Offset: 256MB */

dimm_size_loop:
	movl	$TEST_DATA1, 0		/* Write data to location 0 */
	movl	$TEST_DATA2, (%esi)	/* Write to memory size/2 */
	movl	$0, 0x100		/* Clear the bus */
	movl	0, %eax			/* Read data from location 0 */
	movl	$0, 0			/* Clear location 0 */

	cmpl	$TEST_DATA1, %eax	/* If no aliasing, we have found */
	je	dimm_size_found		/*     out the memory size  */

	/* We had aliasing, so go to the next smaller setting */
	movw	$MC_BANK_CFG, %di
	movl	(%edi), %edx		/* Get current MC_BANK_CFG settings */
	movl    $~DIMM_SZ, %eax		/* Mask for the DIMM size */
	roll	%cl, %eax		/* Shift for the right DIMM */
	and	%eax, %edx		/* Do the masking */
	decb	%bh			/* Halve the memory size */
	jc	mem_sizing_failed	/* Nothing matched, we've failed */
	shll	%cl, %ebx		/* Shift for the right DIMM */
	orl	%ebx, %edx		/* And merge it into the settings */
	shrl	%cl, %ebx		/* And shift it back again */
	shrl	$1, %esi		/* Divide test address by 2 */

	movl	%edx, (%edi)		/* Use the new MC_BANK_CFG settings */
	DO_REFRESH
	jmp	dimm_size_loop

dimm_size_found:
	/* Save the page size we found in EBP */
	movl    $~DIMM_SZ, %eax
	roll	%cl, %eax
	andl	%eax, %ebp
	movl	%ebx, %eax
	shll	%cl, %eax
	or	%eax, %ebp

	shrl	$16-1, %esi
	movw	%si, %di
	movl	%edx, %esi
	CONSOLE_INFO_TX_STRING($str_dimm_size)
	movw	%di, %ax
	shll	$16, %eax
	CONSOLE_INFO_TX_HEX32(%eax)
	CONSOLE_INFO_TX_STRING($str_nl)
	movl	%esi, %edx

next_dimm:
	movl	$0x12345678, 0		/* Write garbage to location 0 */

	addb	$16, %cl
	movl	$0x57400070, %edx	/* No DIMM0, DIMM1 is maxed */

	/* Try the two shift values 0, and 16 */
	cmpb	$32, %cl
	jne	dimm_loop

	movl	%edx, %esi
	CONSOLE_DEBUG_TX_STRING($str_done)
	CONSOLE_DEBUG_TX_HEX32(%ebp)
	CONSOLE_DEBUG_TX_STRING($str_nl)
	movl	%esi, %edx

	/* Finally use the MC_BANK_CFG settings we've calculated */
	movw	$MC_BANK_CFG, %di
	movl	%ebp, (%edi)

	jmp	scx200_setup_ram_end

mem_sizing_failed:
#ifdef DEBUG
	movl	%edx, %esi
	CONSOLE_DEBUG_TX_STRING($str_failed)
	movl	%esi, %edx
#endif
	movl	$0xffff0000, %eax
	roll	%cl, %eax
	andl	%eax, %ebp
	movl	$0x00000070, %eax
	shll	%cl, %eax
	orl	%eax, %ebp

	jmp	next_dimm

str_setup_default_memory:
	.string	"Setting up default parameters for memory\r\n"
str_sizing_memory:
	.string	"Sizing memory\r\n"
str_sizing_done:
	.string	"Sizing done\r\n"
str_done:
	.string "Memory sizing done, MC_BANK_CFG = 0x"
str_probing:
	.string	"Probing for DIMM"
str_found:
	.string	"Found DIMM"
str_failed:
	.string	"    Failed to identify DIMM"
str_page_size:
	.string	"    Page Size:       "
str_component_banks:
	.string	"    Component Banks: "
str_module_banks:
	.string	"    Module Banks:    "
str_dimm_size:
	.string	"    DIMM size:       "
str_nl:
	.string "\r\n"

/**********************************************************************/
/*  all these functions assume that the upper word of EDI contains 
    GX_BASE

*/

enable_dimm:
	/* Start the SDCLK's */
	movw	$MC_MEM_CNTRL1, %di
	movl	(%edi), %eax
	andl	$~SDCLK_START, %eax
	movl	%eax, (%edi)
	orl	$SDCLK_START, %eax
	movl	%eax, (%edi)

	/* Unmask the SDCLK's - may want to unmask only clocks used
	for pwr savings */

	movw	$MC_MEM_CNTRL2, %di
	movl	(%edi), %eax
	andl	$~(SDCLK_MASK | SDCLKOUT_MASK), %eax
	movl	%eax, (%edi)

	DO_DELAY($5000)			/* Wait for clocks to unmask */

	DO_REFRESH

	/* Start the SDCLK's */
	movw	$MC_MEM_CNTRL1, %di
	movl	(%edi), %eax
	andl	$~PROGRAM_SDRAM, %eax
	movl	%eax, (%edi)
	orl	$PROGRAM_SDRAM | 0x00002000, %eax /* Set refresh timing */
	movl	%eax, (%edi)
	andl	$~PROGRAM_SDRAM, %eax
	movl	%eax, (%edi)

	DO_REFRESH

	DO_DELAY($2000)

	RETSP

/**********************************************************************/

	.align	2
cpu_init_table:
	.byte	0xc1, 0x00		/* NO SMIs */
	.byte	0xc3, 0x14		/* Enable CPU config register */
	.byte	0x20, 0x00
	.byte	0xb8, GX_BASE>>30	/* Enable GXBASE address */
	.byte	0xe8, 0x98		/* Sets IORT to minimum setting */
	.byte	0xc3, 0xf8		/* Enable CPU config register */
cpu_init_len = (.-cpu_init_table)/2

/* Memory timing parameters */
CNTRL1 = 0xFE960400			/* Clock divisor 3.5 */

	.align	2
mem_init_table:
	.word	0x8300			/* DC_UNLOCK */
	.long	0x4758
	.word	0x8304			/* DC_GENERAL_CFG */
	.long	0x00000000

	.word	MC_MEM_CNTRL2
	.long	0x00000060		/* Disable SDCLK0, Shift 2, ~FSTRDMSK */
	.word	MC_MEM_CNTRL1
	.long	CNTRL1 & ~(1<<17)	/* ~SDCLKSTRT */
	.word	MC_BANK_CFG
	.long	0x57405740		/* Largest possible memory
					       0x4000 -- 2 module banks
					       0x1000 -- 4 component banks
					       0x0700 -- DIMM size 512MB
					       0x0040 -- Page Size 16kB */
	.word	MC_SYNC_TIM1
	.long	0x3A733225		/* LTMODE 3, RC 10, RAS 7, RP 3, 
					   RCD 3, RRD 2, DPL 2 */

	/* Unmask the SDCLKs */
	.word	MC_MEM_CNTRL2
	.long	0x00003F20 & ~0x7c0	/* Enable all SDCLKs, Shift 2, ~FSTRDMSK */

	/* Start the SDCLKs */
	.word	MC_MEM_CNTRL1
	.long	CNTRL1 | (1<<17)	/* SDCLKSTRT */
mem_init_len = (.-mem_init_table)/6

scx200_setup_ram_end:

/*
    Local variables:
        compile-command: "make -C /export/nano/bios/nano"
    End:
*/
