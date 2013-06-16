/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#define	LX_STACK_BASE		CONFIG_DCACHE_RAM_BASE		/* this is where the DCache will be mapped and be used as stack, It would be cool if it was the same base as coreboot normal stack */
#define	LX_STACK_END		LX_STACK_BASE+(CONFIG_DCACHE_RAM_SIZE-1)

#define	LX_NUM_CACHELINES	0x080	/* there are 128lines per way */
#define	LX_CACHELINE_SIZE	0x020	/* there are 32bytes per line */
#define	LX_CACHEWAY_SIZE	(LX_NUM_CACHELINES * LX_CACHELINE_SIZE)
#define	CR0_CD				0x40000000	/* bit 30 = Cache Disable */
#define	CR0_NW				0x20000000	/* bit 29 = Not Write Through */
#include <cpu/amd/lxdef.h>
#include <cpu/x86/post_code.h>
/***************************************************************************
/**
/**	DCacheSetup
/**
/**	Setup data cache for  use as RAM for a stack.
/**
/***************************************************************************/
DCacheSetup:
	/* Save the BIST result */
	movl    %eax, %ebx

	invd
	/* set cache properties */
	movl	$CPU_RCONF_DEFAULT, %ecx
	rdmsr
	movl	$0x010010000, %eax		/*1MB system memory in write back 1|00100|00 */
	wrmsr

	/* in LX DCDIS is set after POR which disables the cache..., clear this bit */
	movl	$CPU_DM_CONFIG0,%ecx
	rdmsr
	andl	$(~(DM_CONFIG0_LOWER_DCDIS_SET)), %eax	/* TODO: make consistent with i$ init,	either whole reg = 0,  or just this bit... */
	wrmsr

	/* get cache timing params from BIOS config data locations and apply */
	/* fix delay controls for DM and IM arrays */
	/* fix delay controls for DM and IM arrays */
	movl	$CPU_BC_MSS_ARRAY_CTL0, %ecx
	xorl	%edx, %edx
	movl	$0x2814D352, %eax
	wrmsr

	movl	$CPU_BC_MSS_ARRAY_CTL1, %ecx
	xorl	%edx, %edx
	movl	$0x1068334D, %eax
	wrmsr

	movl	$CPU_BC_MSS_ARRAY_CTL2, %ecx
	movl	$0x00000106, %edx
	movl	$0x83104104, %eax
	wrmsr

	movl	$GLCP_FIFOCTL, %ecx
	rdmsr
	movl	$0x00000005, %edx
	wrmsr

	/* Enable setting */
	movl	$CPU_BC_MSS_ARRAY_CTL_ENA, %ecx
	xorl	%edx, %edx
	movl	$0x01, %eax
	wrmsr

	/* Get cleaned up. */
	xorl	%edi, %edi
	xorl	%esi, %esi
	xorl	%ebp, %ebp

	/* DCache Ways0 through Ways7 will be tagged for LX_STACK_BASE + CONFIG_DCACHE_RAM_SIZE for holding stack */
	/* remember,  there is NO stack yet... */

	/* Tell cache we want to fill WAY 0 starting at the top */
	xorl	%edx, %edx
	xorl	%eax, %eax
	movl	$CPU_DC_INDEX, %ecx
	wrmsr

	/* startaddress for tag of Way0: ebp will hold the incrementing address. dont destroy! */
	movl	$LX_STACK_BASE, %ebp	/* init to start address */
	orl		$1, %ebp				/* set valid bit and tag for this Way (B[31:12] : Cache tag value for line/way curr. selected by CPU_DC_INDEX */

	/* start tag Ways 0 with 128 lines with 32bytes each: edi will hold the line counter. dont destroy! */
	movl	$LX_NUM_CACHELINES, %edi
DCacheSetupFillWay:

	/* fill with dummy data: zero it so we can tell it from PCI memory space (returns FFs). */
	/* We will now store a line (32 bytes = 4 x 8bytes = 4 quadWords) */
	movw	$0x04, %si
	xorl	%edx, %edx
	xorl	%eax, %eax
	movl	$CPU_DC_DATA, %ecx
DCacheSetup_quadWordLoop:
	wrmsr
	decw	%si
	jnz	DCacheSetup_quadWordLoop

	/* Set the tag for this line,  need to do this for every new cache line to validate it! */
	/* accessing CPU_DC_TAG_I makes the LINE field in CPU_DC_INDEX increment and thus cont. in the next cache line... */
	xorl	%edx, %edx
	movl	%ebp, %eax
	movl	$CPU_DC_TAG, %ecx
	wrmsr

	/* switch to next line */
	/* lines are in Bits10:4 */
	/* when index is crossing 0x7F -> 0x80	writing a RSVD bit as 0x80 is not a valid CL anymore! */
	movl	$CPU_DC_INDEX, %ecx
	rdmsr
	addl	$0x010, %eax /* TODO: prob. would be more elegant to calc. this from counter var edi... */
	wrmsr

	decl	%edi
	jnz	DCacheSetupFillWay

	/* 1 Way has been filled,  forward start address for next Way,	terminate if we have reached end of desired address range */
	addl	$LX_CACHEWAY_SIZE, %ebp
	cmpl	$LX_STACK_END, %ebp
	jge	leave_DCacheSetup
	movl	$LX_NUM_CACHELINES, %edi

	/* switch to next way */
	movl	$CPU_DC_INDEX, %ecx
	rdmsr
	addl	$0x01, %eax
	andl	$0xFFFFF80F, %eax /* lets be sure: reset line index Bits10:4 */
	wrmsr

	jmp	DCacheSetupFillWay

leave_DCacheSetup:
	xorl	%edi, %edi
	xorl	%esi, %esi
	xorl	%ebp, %ebp

	/* Disable the cache,  but ... DO NOT INVALIDATE the tags. */
	/* Memory reads and writes will all hit in the cache. */
	/* Cache updates and memory write-backs will not occur ! */
	movl	%cr0, %eax
	orl		$(CR0_CD + CR0_NW), %eax	/* set the CD and NW bits */
	movl	%eax, %cr0

	/* Now point sp to the cached stack. */
	/* The stack will be fully functional at this location. No system memory is required at all ! */
	/* set up the stack pointer */
	movl	$LX_STACK_END, %eax
	movl	%eax, %esp

	/* test the stack*/
	movl	$0x0F0F05A5A, %edx
	pushl	%edx
	popl	%ecx
	cmpl	%ecx, %edx
	je	DCacheSetupGood

	post_code(0xc5)
DCacheSetupBad:
	hlt		/* issues */
	jmp DCacheSetupBad
DCacheSetupGood:
	/* Go do early init and memory setup */

	/* Restore the BIST result */
	movl	%ebx, %eax
	movl	%esp, %ebp
	pushl	%eax

	post_code(0x23)

	/* Call romstage.c main function */
	call	main
done_cache_as_ram_main:

        /* We now run over the stack-in-cache, copying it back to itself to invalidate the cache */

        push   %edi
        mov    $(CONFIG_DCACHE_RAM_SIZE/4),%ecx
        push   %esi
        mov    $(CONFIG_DCACHE_RAM_BASE),%edi
        mov    %edi,%esi
        cld
        rep movsl %ds:(%esi),%es:(%edi)
        pop    %esi
        pop    %edi

	/* Clear the cache out to ram */
	wbinvd
        /* re-enable the cache */
        movl    %cr0, %eax
        xorl             $(CR0_CD + CR0_NW), %eax        /* clear  the CD and NW bits */
        movl    %eax, %cr0

__main:
	post_code(POST_PREPARE_RAMSTAGE)

	/* TODO For suspend/resume the cache will have to live between
	 * CONFIG_RAMBASE and CONFIG_RAMTOP
	 */

	cld				/* clear direction flag */

	/* copy coreboot from it's initial load location to
	 * the location it is compiled to run at.
	 * Normally this is copying from FLASH ROM to RAM.
	 */
	call copy_and_run

.Lhlt:
	post_code(POST_DEAD_CODE)
	hlt
	jmp	.Lhlt

