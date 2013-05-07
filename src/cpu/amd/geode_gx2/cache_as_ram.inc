/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Nils Jacobs
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

#define	GX2_STACK_BASE		CONFIG_DCACHE_RAM_BASE		/* this is where the DCache will be mapped and be used as stack, It would be cool if it was the same base as coreboot normal stack */
#define	GX2_STACK_END		GX2_STACK_BASE+(CONFIG_DCACHE_RAM_SIZE-1)

#define	GX2_NUM_CACHELINES	0x080	/* there are 128lines per way */
#define	GX2_CACHELINE_SIZE	0x020	/* there are 32bytes per line */
#define	GX2_CACHEWAY_SIZE	(GX2_NUM_CACHELINES * GX2_CACHELINE_SIZE)
#define	CR0_CD				0x40000000	/* bit 30 = Cache Disable */
#define	CR0_NW				0x20000000	/* bit 29 = Not Write Through */
#include <cpu/amd/gx2def.h>
#include <cpu/x86/post_code.h>
/***************************************************************************
/**
/**	DCacheSetup
/**
/**	Setup data cache for  use as RAM for a stack.
/**
/**	Max. size data cache =0x4000 (16KB)
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

	/* in GX2 DCDIS is set after POR which disables the cache..., clear this bit */
	movl	$CPU_DM_CONFIG0, %ecx
	rdmsr
	andl	$(~(DM_CONFIG0_LOWER_DCDIS_SET)), %eax	/* TODO: make consistent with i$ init,	either whole reg = 0,  or just this bit... */
	wrmsr

	/* Get cleaned up. */
	xorl	%edi, %edi
	xorl	%esi, %esi
	xorl	%ebp, %ebp

	/* DCache Ways0 through Ways3 will be tagged for GX2_STACK_BASE + CONFIG_DCACHE_RAM_SIZE for holding stack */
	/* remember,  there is NO stack yet... */

	/* Tell cache we want to fill WAY 0 starting at the top */
	xorl	%edx, %edx
	xorl	%eax, %eax
	movl	$CPU_DC_INDEX, %ecx
	wrmsr

	/* startaddress for tag of Way0: ebp will hold the incrementing address. dont destroy! */
	movl	$GX2_STACK_BASE, %ebp	/* init to start address */
	orl	$1, %ebp	/* set valid bit and tag for this Way (B[31:12] : Cache tag value for line/way curr. selected by CPU_DC_INDEX */

	/* start tag Ways 0 with 128 lines with 32bytes each: edi will hold the line counter. dont destroy! */
	movl	$GX2_NUM_CACHELINES, %edi
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

	/* Set the tag for this line,need to do this for every new cache line to validate it! */
	/* accessing CPU_DC_TAG_I makes the LINE field in CPU_DC_INDEX increment and thus cont. in the next cache line... */
	xorl	%edx, %edx
	movl	%ebp, %eax
	movl	$CPU_DC_TAG, %ecx
	wrmsr

	/* switch to next line */
	/* lines are in Bits8:2 */
	/* when index is crossing 0x7F -> 0x80	writing a RSVD bit as 0x80 is not a valid CL anymore! */
	movl	$CPU_DC_INDEX, %ecx
	rdmsr
	addl	$0x04, %eax /* inc DC_LINE. TODO: prob. would be more elegant to calc. this from counter var edi... */
	wrmsr

	decl	%edi
	jnz	DCacheSetupFillWay

	/* 1 Way has been filled,  forward start address for next Way,	terminate if we have reached end of desired address range */
	addl	$GX2_CACHEWAY_SIZE, %ebp
	cmpl	$GX2_STACK_END, %ebp
	jge	leave_DCacheSetup
	movl	$GX2_NUM_CACHELINES, %edi

	/* switch to next way */
	movl	$CPU_DC_INDEX, %ecx
	rdmsr
	addl	$0x01, %eax
	andl	$0xFFFFFE03, %eax /* lets be sure: reset line index Bits8:2 */
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
	movl	$GX2_STACK_END, %eax
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

