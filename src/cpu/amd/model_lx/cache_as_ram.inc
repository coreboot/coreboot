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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef ASM_CONSOLE_LOGLEVEL
#define ASM_CONSOLE_LOGLEVEL CONFIG_MAXIMUM_CONSOLE_LOGLEVEL
#endif

#define	LX_STACK_BASE		CONFIG_DCACHE_RAM_BASE		/* this is where the DCache will be mapped and be used as stack, It would be cool if it was the same base as coreboot normal stack */
#define	LX_STACK_END		LX_STACK_BASE+(CONFIG_DCACHE_RAM_SIZE-1)

#define	LX_NUM_CACHELINES	0x080	/* there are 128lines per way */
#define	LX_CACHELINE_SIZE	0x020	/* there are 32bytes per line */
#define	LX_CACHEWAY_SIZE	(LX_NUM_CACHELINES * LX_CACHELINE_SIZE)
#define	CR0_CD				0x40000000	/* bit 30 = Cache Disable */
#define	CR0_NW				0x20000000	/* bit 29 = Not Write Through */
#include <cpu/amd/lxdef.h>
/***************************************************************************
/**
/**	DCacheSetup
/**
/**	Setup data cache for  use as RAM for a stack.
/**
/***************************************************************************/
DCacheSetup:

	invd
	/* set cache properties */
	movl	$CPU_RCONF_DEFAULT, %ecx
	rdmsr
	movl	$0x010010000, %eax		/*1MB system memory in write back 1|00100|00 */
	wrmsr

	/* in LX DCDIS is set after POR which disables the cache..., clear this bit */
	movl	CPU_DM_CONFIG0,%ecx
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
	movb	$0xC5, %al
	outb	%al, $0x80
DCacheSetupBad:
	hlt		/* issues */
	jmp DCacheSetupBad
DCacheSetupGood:

	/* Go do early init and memory setup */
	call	cache_as_ram_main
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

	/* clear boot_complete flag */
	xorl	%ebp, %ebp
__main:

	/*
	 *	Copy data into RAM and clear the BSS. Since these segments
	 *	isn\'t really that big we just copy/clear using bytes, not
	 *	double words.
	 */
	post_code(0x11)		/* post 11 */

	cld				/* clear direction flag */

	/* copy coreboot from it's initial load location to
	 * the location it is compiled to run at.
	 * Normally this is copying from FLASH ROM to RAM.
	 */
	movl	%ebp, %esi
	pushl	%esi
	pushl $str_coreboot_ram_name
	call cbfs_and_run_core

.Lhlt:
	post_code(0xee)	/* post fail ee */
	hlt
	jmp	.Lhlt

#ifdef __CRT_CONSOLE_TX_STRING
	/* Uses esp, ebx, ax, dx  */
crt_console_tx_string:
	mov	(%ebx), %al
	inc	%ebx
	cmp	$0, %al
	jne	9f
	RETSP
9:
/* Base Address */
#ifndef CONFIG_TTYS0_BASE
#define CONFIG_TTYS0_BASE	0x3f8
#endif
/* Data */
#define TTYS0_RBR (CONFIG_TTYS0_BASE+0x00)

/* Control */
#define TTYS0_TBR TTYS0_RBR
#define TTYS0_IER (CONFIG_TTYS0_BASE+0x01)
#define TTYS0_IIR (CONFIG_TTYS0_BASE+0x02)
#define TTYS0_FCR TTYS0_IIR
#define TTYS0_LCR (CONFIG_TTYS0_BASE+0x03)
#define TTYS0_MCR (CONFIG_TTYS0_BASE+0x04)
#define TTYS0_DLL TTYS0_RBR
#define TTYS0_DLM TTYS0_IER

/* Status */
#define TTYS0_LSR (CONFIG_TTYS0_BASE+0x05)
#define TTYS0_MSR (CONFIG_TTYS0_BASE+0x06)
#define TTYS0_SCR (CONFIG_TTYS0_BASE+0x07)

	mov	%al, %ah
10:	mov	$TTYS0_LSR, %dx
	inb	%dx, %al
	test	$0x20, %al
	je	10b
	mov	$TTYS0_TBR, %dx
	mov	%ah, %al
	outb	%al, %dx

	jmp crt_console_tx_string
#endif /* __CRT_CONSOLE_TX_STRING */

#if defined(CONSOLE_DEBUG_TX_STRING) && (ASM_CONSOLE_LOGLEVEL > BIOS_DEBUG)
.section ".rom.data"
#if CONFIG_COMPRESS
str_copying_to_ram:  .string "Uncompressing coreboot to ram.\r\n"
#else
str_copying_to_ram:  .string "Copying coreboot to ram.\r\n"
#endif
str_pre_main:        .string "Jumping to coreboot.\r\n"
.previous

#endif /* ASM_CONSOLE_LOGLEVEL > BIOS_DEBUG */
str_coreboot_ram_name:	.ascii CONFIG_CBFS_PREFIX
			.string "/coreboot_ram"
