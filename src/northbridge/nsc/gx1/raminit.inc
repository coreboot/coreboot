/*
    freebios/src/northbridge/nsc/gx1/raminit.inc

    Copyright (c) 2002 Christer Weinigel <wingel@hack.org>

    Do memory sizing for a National Semiconductor GX1 CPU.
*/

#include "gx1def.h"

/**********************************************************************/

#define TEST_DATA1 0x05A5A5A5A
#define TEST_DATA2 0x0DEADBEEF

#define NUM_REFRESH 8

/**********************************************************************/

ram_setup:
	CONSOLE_DEBUG_TX_STRING($str_setup_default_memory)

	/* Set up Memory controller, note that all the code below
	   assumes that the high word of %edi contains GX_BASE */
	movl	$GX_BASE, %edi

	movw	$MC_MEM_CNTRL2, %di
	movl	$0x000007d8, (%edi)	/* Disable all SDCLKs, Shift = 3 */

	movw	$MC_MEM_CNTRL1, %di
	movl	$0x92140000, (%edi)	/* MD_DS=2, MA_DS=2, CNTL_DS=2
					   SDCLKRATE=/4 */

	movw	$MC_BANK_CFG, %di
	movl	$0x00700070, (%edi)	/* No DIMMS installed */

	movw	$MC_SYNC_TIM1, %di
	movl	$0x3A733225, (%edi)	/* LTMODE 3, RC 10, RAS 7, RP 3, 
					   RCD 3, RRD 2, DPL 2 */

	movw	$MC_BANK_CFG, %di
	movl	$0x57405740, (%edi)	/* Largest possible DIMMS installed
					       0x4000 -- 2 module banks
					       0x1000 -- 4 component banks
					       0x0700 -- DIMM size 512MB
					       0x0040 -- Page Size 16kB */

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
	CALLSP(do_refresh)

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
	CALLSP(do_refresh)
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
	CALLSP(do_refresh)
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
	CALLSP(do_refresh)
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
	CALLSP(do_refresh)
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

	jmp	ram_setup_end

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
	.string	"    Found DIMM"
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
/* all these functions assume that the upper word of EDI contains
   GX_BASE */

/* Destroys EAX, high word of ECX and low word of EDI */
do_refresh:
	shll	$16, %ecx /* save CX */
        movw    $MC_MEM_CNTRL1, %di
	movl	(%edi), %eax
	orb	$RFSHTST, %al
	movw	$NUM_REFRESH, %cx
1:	movl	%eax, (%edi)
	loopw	1b
	andb	$~RFSHTST, %al
	movl	%eax, (%edi)
	shrl	$16, %ecx /* restore CX */
	RETSP

enable_dimm:
	/* Start the SDCLK's */
	movw	$MC_MEM_CNTRL1, %di
	movl	(%edi), %eax
	andl	$~SDCLKSTRT, %eax
	movl	%eax, (%edi)
	orl	$SDCLKSTRT, %eax
	movl	%eax, (%edi)

	/* Unmask the SDCLK's */
	movw	$MC_MEM_CNTRL2, %di
	movl	(%edi), %eax
	andl	$~(SDCLK_MASK | SDCLKOUT_MASK), %eax
	movl	%eax, (%edi)

        movw	$5000, %cx		/* Wait for clocks to unmask */
1:	outb	%al, $0xed
        loopw	1b

        movw    $MC_MEM_CNTRL1, %di	/* Refresh memory */
	movl	(%edi), %eax
	orb	$RFSHTST, %al
	movw	$NUM_REFRESH, %cx
1:	movl	%eax, (%edi)
	loopw	1b
	andb	$~RFSHTST, %al

	/* Start the SDCLK's */
	andl	$~PROGRAM_SDRAM, %eax
	movl	%eax, (%edi)
	orl	$PROGRAM_SDRAM | 0x00002000, %eax /* Set refresh timing */
	movl	%eax, (%edi)
	andl	$~PROGRAM_SDRAM, %eax
	movl	%eax, (%edi)

	movl	(%edi), %eax	/* Refresh memory again */
	orb	$RFSHTST, %al
	movw	$NUM_REFRESH, %cx
1:	movl	%eax, (%edi)
	loopw	1b

        movw	$2000, %cx
1:	outb	%al, $0xed
        loopw	1b

	RETSP

/**********************************************************************/

ram_setup_end:

/*
    Local variables:
        compile-command: "make -C /export/bios/voyager2"
    End:
*/
