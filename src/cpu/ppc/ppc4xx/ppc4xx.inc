#if 0
/*
 *  Copyright (C) 1998	Dan Malek <dmalek@jlc.net>
 *  Copyright (C) 1999	Magnus Damm <kieraypc01.p.y.kie.era.ericsson.se>
 *  Copyright (C) 2000,2001,2002 Wolfgang Denk <wd@denx.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * This source code has been made available to you by IBM on an AS-IS
 * basis. Anyone receiving this source is licensed under IBM
 * copyrights to use it in any way he or she deems fit, including
 * copying it, modifying it, compiling it, and redistributing it either
 * with or without modifications. No license under IBM patents or
 * patent applications is to be implied by the copyright license.
 *
 * Any user of this software should understand that IBM cannot provide 
 * technical support for this software and will not be responsible for
 * any consequences resulting from the use of this software.
 *
 * Any person who transfers this source code or any derivative work
 * must include the IBM copyright notice, this paragraph, and the
 * preceding two paragraphs in the transferred software.
 *
 * COPYRIGHT   I B M   CORPORATION 1995
 * LICENSED MATERIAL  -  PROGRAM PROPERTY OF I B M
 *
 */

/*
 * Startup Code for IBM 4xx PowerPC based Embedded Boards
 *
 *  Base on the U-Boot Startup Code
 *
 *  The processor starts at 0xfffffffc and the code is executed
 *  from flash/rom.
 *  in memory, but as long we don't jump around before relocating.
 *  board_init lies at a quite high address and when the cpu has
 *  jumped there, everything is ok.
 *  This works because the cpu gives the FLASH (CS0) the whole
 *  address space at startup, and board_init lies as a echo of
 *  the flash somewhere up there in the memorymap.
 *
 *  board_init will change CS0 to be positioned at the correct
 *  address and (s)dram will be positioned at address 0
 */
#include <ppc_asm.tmpl>
#include <ppc_defs.h>

	.globl	_start
_start:

/*****************************************************************************/
	addi	r4,r0,0x0000
	mtspr	sgr,r4
	mtspr	dcwr,r4
	mtesr	r4			/* clear Exception Syndrome Reg */
	mttcr	r4			/* clear Timer Control Reg */
	mtxer	r4			/* clear Fixed-Point Exception Reg */
	mtevpr	r4			/* clear Exception Vector Prefix Reg */
	addi	r4,r0,0x1000		/* set ME bit (Machine Exceptions) */
	oris	r4,r4,0x0002		/* set CE bit (Critical Exceptions) */
	mtmsr	r4			/* change MSR */
	addi	r4,r0,(0xFFFF-0x10000)		/* set r4 to 0xFFFFFFFF (status in the */
					/* dbsr is cleared by setting bits to 1) */
	mtdbsr	r4			/* clear/reset the dbsr */

	/*----------------------------------------------------------------------- */
	/* Invalidate I and D caches. Enable I cache for defined memory regions */
	/* to speed things up. Leave the D cache disabled for now. It will be */
	/* enabled/left disabled later based on user selected menu options. */
	/* Be aware that the I cache may be disabled later based on the menu */
	/* options as well. See miscLib/main.c. */
	/*----------------------------------------------------------------------- */
	bl	invalidate_icache
	bl	invalidate_dcache

	/*----------------------------------------------------------------------- */
	/* Enable two 128MB cachable regions. */
	/*----------------------------------------------------------------------- */
	addis	r4,r0,0x8000
	addi	r4,r4,0x0001
	mticcr	r4			/* instruction cache */
	isync

	addis	r4,r0,0x0000
	addi	r4,r4,0x0000
	mtdccr	r4			/* data cache */

#if !(defined(CFG_EBC_PB0AP) && defined(CFG_EBC_PB0CR))
	/*----------------------------------------------------------------------- */
	/* Tune the speed and size for flash CS0  */
	/*----------------------------------------------------------------------- */
	bl	ext_bus_cntlr_init
#endif

#if defined(CONFIG_405EP)
	/*----------------------------------------------------------------------- */
	/* DMA Status, clear to come up clean */
	/*----------------------------------------------------------------------- */
        addis   r3,r0, 0xFFFF         /* Clear all existing DMA status */
        ori     r3,r3, 0xFFFF
        mtdcr   dmasr, r3

	bl	ppc405ep_init         /* do ppc405ep specific init */
#endif /* CONFIG_405EP */

	/*----------------------------------------------------------------------- */
	/* Setup temporary stack in DCACHE or OCM if needed for SDRAM SPD. */
	/*----------------------------------------------------------------------- */
#ifdef CFG_INIT_DCACHE_CS
	/*----------------------------------------------------------------------- */
	/* Memory Bank x (nothingness) initialization 1GB+64MEG */
	/* used as temporary stack pointer for stage0  */
	/*----------------------------------------------------------------------- */
	li	r4,PBxAP
	mtdcr	ebccfga,r4
	lis	r4,0x0380
	ori	r4,r4,0x0480
	mtdcr	ebccfgd,r4

	addi	r4,0,PBxCR
	mtdcr	ebccfga,r4
	lis	r4,0x400D
	ori	r4,r4,0xa000
	mtdcr	ebccfgd,r4

	/* turn on data chache for this region */
	lis	r4,0x0080
	mtdccr	r4

	/* set stack pointer and clear stack to known value */

	lis	r1,CFG_INIT_RAM_ADDR@h
	ori     r1,r1,CFG_INIT_SP_OFFSET@l

	li	r4,2048			/* we store 2048 words to stack */
	mtctr	r4

	lis	r2,CFG_INIT_RAM_ADDR@h		/* we also clear data area */
	ori	r2,r2,CFG_INIT_RAM_END@l 	/* so cant copy value from r1 */

	lis	r4,0xdead		/* we store 0xdeaddead in the stack */
	ori	r4,r4,0xdead

..stackloop:
	stwu	r4,-4(r2)
	bdnz	..stackloop

	li	r0, 0			/* Make room for stack frame header and */
	stwu	r0, -4(r1)		/* clear final stack frame so that	*/
	stwu	r0, -4(r1)		/* stack backtraces terminate cleanly	*/
	/*
	 * Set up a dummy frame to store reset vector as return address.
	 * this causes stack underflow to reset board.
	 */
	stwu	r1, -8(r1)		/* Save back chain and move SP */
	addis	r0, 0, RESET_VECTOR@h	/* Address of reset vector */
	ori	r0, r0, RESET_VECTOR@l
	stwu	r1, -8(r1)		/* Save back chain and move SP */
	stw	r0, +12(r1)		/* Save return addr (underflow vect) */
#endif /* CFG_INIT_DCACHE_CS */

	/*----------------------------------------------------------------------- */
	/* Initialize SDRAM Controller  */
	/*----------------------------------------------------------------------- */
	bl	sdram_init

	/*
	 * Setup temporary stack pointer only for boards
	 * that do not use SDRAM SPD I2C stuff since it
	 * is already initialized to use DCACHE or OCM
	 * stacks.
	 */
#if !(defined(CFG_INIT_DCACHE_CS) || defined(CFG_TEMP_STACK_OCM))
	lis	r1, CFG_INIT_RAM_ADDR@h
	ori	r1,r1,CFG_INIT_SP_OFFSET /* set up the stack in SDRAM */

	li	r0, 0			/* Make room for stack frame header and */
	stwu	r0, -4(r1)		/* clear final stack frame so that	*/
	stwu	r0, -4(r1)		/* stack backtraces terminate cleanly	*/
	/*
	 * Set up a dummy frame to store reset vector as return address.
	 * this causes stack underflow to reset board.
	 */
	stwu	r1, -8(r1)		/* Save back chain and move SP */
	lis	r0, RESET_VECTOR@h	/* Address of reset vector */
	ori	r0, r0, RESET_VECTOR@l
	stwu	r1, -8(r1)		/* Save back chain and move SP */
	stw	r0, +12(r1)		/* Save return addr (underflow vect) */
#endif /* !(CFG_INIT_DCACHE_CS  || !CFG_TEM_STACK_OCM) */

	GET_GOT			/* initialize GOT access			*/

       	bl	cpu_init_f	/* run low-level CPU init code     (from Flash)	*/

	/* NEVER RETURNS! */
	bl	board_init_f	/* run first part of init code (from Flash)	*/



/* Cache functions.
*/
invalidate_icache:
	iccci	r0,r0			/* for 405, iccci invalidates the */
	blr				/*   entire I cache */

invalidate_dcache:
	addi	r6,0,0x0000		/* clear GPR 6 */
	/* Do loop for # of dcache congruence classes. */
	addi	r7,r0, (CFG_DCACHE_SIZE / CFG_CACHELINE_SIZE / 2)
					/* NOTE: dccci invalidates both */
	mtctr	r7			/* ways in the D cache */
..dcloop:
	dccci	0,r6			/* invalidate line */
	addi	r6,r6, CFG_CACHELINE_SIZE /* bump to next line */
	bdnz	..dcloop
	blr

flush_dcache:
	addis	r9,r0,0x0002		/* set mask for EE and CE msr bits */
	ori	r9,r9,0x8000
	mfmsr	r12			/* save msr */
	andc	r9,r12,r9
	mtmsr	r9			/* disable EE and CE */
	addi	r10,r0,0x0001		/* enable data cache for unused memory */
	mfdccr	r9			/* region 0xF8000000-0xFFFFFFFF via */
	or	r10,r10,r9		/* bit 31 in dccr */
	mtdccr	r10

	/* do loop for # of congruence classes. */
	addi	r10,r0,(CFG_DCACHE_SIZE / CFG_CACHELINE_SIZE / 2)
	addi	r11,r0,(CFG_DCACHE_SIZE / 2) /* D cache set size - 2 way sets */
	mtctr	r10
	addi	r10,r0,(0xE000-0x10000)	/* start at 0xFFFFE000 */
	add	r11,r10,r11		/* add to get to other side of cache line */
..flush_dcache_loop:
	lwz	r3,0(r10)		/* least recently used side */
	lwz	r3,0(r11)		/* the other side */
	dccci	r0,r11			/* invalidate both sides */
	addi	r10,r10,CFG_CACHELINE_SIZE /* bump to next line */
	addi	r11,r11,CFG_CACHELINE_SIZE /* bump to next line */
	bdnz	..flush_dcache_loop
	sync				/* allow memory access to complete */
	mtdccr	r9			/* restore dccr */
	mtmsr	r12			/* restore msr */
	blr

	.globl	icache_enable
icache_enable:
	mflr	r8
	bl	invalidate_icache
	mtlr	r8
	isync
	addis	r3,r0, 0x8000	      /* set bit 0 */
	mticcr	r3
	blr

	.globl	icache_disable
icache_disable:
	addis	r3,r0, 0x0000	      /* clear bit 0 */
	mticcr	r3
	isync
	blr

	.globl	icache_status
icache_status:
	mficcr	r3
	srwi	r3, r3, 31	/* >>31 => select bit 0 */
	blr

	.globl	dcache_enable
dcache_enable:
	mflr	r8
	bl	invalidate_dcache
	mtlr	r8
	isync
	addis	r3,r0, 0x8000	      /* set bit 0 */
	mtdccr	r3
	blr

	.globl	dcache_disable
dcache_disable:
	mflr	r8
	bl	flush_dcache
	mtlr	r8
	addis	r3,r0, 0x0000	      /* clear bit 0 */
	mtdccr	r3
	blr

	.globl	dcache_status
dcache_status:
	mfdccr	r3
	srwi	r3, r3, 31	/* >>31 => select bit 0 */
	blr

	.globl get_pvr
get_pvr:
	mfspr	r3, PVR
	blr

#if !defined(CONFIG_440)
	.globl wr_pit
wr_pit:
	mtspr	pit, r3
	blr
#endif

	.globl wr_tcr
wr_tcr:
	mtspr	tcr, r3
	blr

/*------------------------------------------------------------------------------- */
/* Function:	 ppcDcbf */
/* Description:	 Data Cache block flush */
/* Input:	 r3 = effective address */
/* Output:	 none. */
/*------------------------------------------------------------------------------- */
	.globl	ppcDcbf
ppcDcbf:
	dcbf	r0,r3
	blr

/*------------------------------------------------------------------------------- */
/* Function:	 ppcDcbi */
/* Description:	 Data Cache block Invalidate */
/* Input:	 r3 = effective address */
/* Output:	 none. */
/*------------------------------------------------------------------------------- */
	.globl	ppcDcbi
ppcDcbi:
	dcbi	r0,r3
	blr

/*------------------------------------------------------------------------------- */
/* Function:	 ppcSync */
/* Description:	 Processor Synchronize */
/* Input:	 none. */
/* Output:	 none. */
/*------------------------------------------------------------------------------- */
	.globl	ppcSync
ppcSync:
	sync
	blr

/*------------------------------------------------------------------------------*/

/*
 * void relocate_code (addr_sp, gd, addr_moni)
 *
 * This "function" does not return, instead it continues in RAM
 * after relocating the monitor code.
 *
 * r3 = dest
 * r4 = src
 * r5 = length in bytes
 * r6 = cachelinesize
 */
	.globl	relocate_code
relocate_code:
	mr	r1,  r3		/* Set new stack pointer		*/
	mr	r9,  r4		/* Save copy of Init Data pointer	*/
	mr	r10, r5		/* Save copy of Destination Address	*/

	mr	r3,  r5				/* Destination Address	*/
	lis	r4, CFG_MONITOR_BASE@h		/* Source      Address	*/
	ori	r4, r4, CFG_MONITOR_BASE@l
	lwz	r5, GOT(__init_end)
	sub	r5, r5, r4
	li	r6, CFG_CACHELINE_SIZE		/* Cache Line Size	*/

	/*
	 * Fix GOT pointer:
	 *
	 * New GOT-PTR = (old GOT-PTR - CFG_MONITOR_BASE) + Destination Address
	 *
	 * Offset:
	 */
	sub	r15, r10, r4

	/* First our own GOT */
	add	r14, r14, r15
	/* the the one used by the C code */
	add	r30, r30, r15

	/*
	 * Now relocate code
	 */

	cmplw	cr1,r3,r4
	addi	r0,r5,3
	srwi.	r0,r0,2
	beq	cr1,4f		/* In place copy is not necessary	*/
	beq	7f		/* Protect against 0 count		*/
	mtctr	r0
	bge	cr1,2f

	la	r8,-4(r4)
	la	r7,-4(r3)
1:	lwzu	r0,4(r8)
	stwu	r0,4(r7)
	bdnz	1b
	b	4f

2:	slwi	r0,r0,2
	add	r8,r4,r0
	add	r7,r3,r0
3:	lwzu	r0,-4(r8)
	stwu	r0,-4(r7)
	bdnz	3b

/*
 * Now flush the cache: note that we must start from a cache aligned
 * address. Otherwise we might miss one cache line.
 */
4:	cmpwi	r6,0
	add	r5,r3,r5
	beq	7f		/* Always flush prefetch queue in any case */
	subi	r0,r6,1
	andc	r3,r3,r0
	mr	r4,r3
5:	dcbst	0,r4
	add	r4,r4,r6
	cmplw	r4,r5
	blt	5b
	sync			/* Wait for all dcbst to complete on bus */
	mr	r4,r3
6:	icbi	0,r4
	add	r4,r4,r6
	cmplw	r4,r5
	blt	6b
7:	sync			/* Wait for all icbi to complete on bus */
	isync

/*
 * We are done. Do not return, instead branch to second part of board
 * initialization, now running from RAM.
 */

	addi	r0, r10, in_ram - _start + EXC_OFF_SYS_RESET
	mtlr	r0
	blr				/* NEVER RETURNS! */

in_ram:

	/*
	 * Relocation Function, r14 point to got2+0x8000
	 *
	 * Adjust got2 pointers, no need to check for 0, this code
	 * already puts a few entries in the table.
	 */
	li	r0,__got2_entries@sectoff@l
	la	r3,GOT(_GOT2_TABLE_)
	lwz	r11,GOT(_GOT2_TABLE_)
	mtctr	r0
	sub	r11,r3,r11
	addi	r3,r3,-4
1:	lwzu	r0,4(r3)
	add	r0,r0,r11
	stw	r0,0(r3)
	bdnz	1b

	/*
	 * Now adjust the fixups and the pointers to the fixups
	 * in case we need to move ourselves again.
	 */
2:	li	r0,__fixup_entries@sectoff@l
	lwz	r3,GOT(_FIXUP_TABLE_)
	cmpwi	r0,0
	mtctr	r0
	addi	r3,r3,-4
	beq	4f
3:	lwzu	r4,4(r3)
	lwzux	r0,r4,r11
	add	r0,r0,r11
	stw	r10,0(r3)
	stw	r0,0(r4)
	bdnz	3b
4:
clear_bss:
	/*
	 * Now clear BSS segment
	 */
	lwz	r3,GOT(__bss_start)
	lwz	r4,GOT(_end)

	cmplw	0, r3, r4
	beq	6f

	li	r0, 0
5:
	stw	r0, 0(r3)
	addi	r3, r3, 4
	cmplw	0, r3, r4
	bne	5b
6:

	mr	r3, r9		/* Init Data pointer		*/
	mr	r4, r10		/* Destination Address		*/
	bl	board_init_r

	/*
	 * Copy exception vector code to low memory
	 *
	 * r3: dest_addr
	 * r7: source address, r8: end address, r9: target address
	 */
	.globl	trap_init
trap_init:
	lwz	r7, GOT(_start)
	lwz	r8, GOT(_end_of_vectors)

	li	r9, 0x100		/* reset vector always at 0x100 */

	cmplw	0, r7, r8
	bgelr				/* return if r7>=r8 - just in case */

	mflr	r4			/* save link register		*/
1:
	lwz	r0, 0(r7)
	stw	r0, 0(r9)
	addi	r7, r7, 4
	addi	r9, r9, 4
	cmplw	0, r7, r8
	bne	1b

	/*
	 * relocate `hdlr' and `int_return' entries
	 */
	li	r7, .L_MachineCheck - _start + EXC_OFF_SYS_RESET
	li	r8, Alignment - _start + EXC_OFF_SYS_RESET
2:
	bl	trap_reloc
	addi	r7, r7, 0x100		/* next exception vector	*/
	cmplw	0, r7, r8
	blt	2b

	li	r7, .L_Alignment - _start + EXC_OFF_SYS_RESET
	bl	trap_reloc

	li	r7, .L_ProgramCheck - _start + EXC_OFF_SYS_RESET
	bl	trap_reloc

	li	r7, .L_FPUnavailable - _start + EXC_OFF_SYS_RESET
	li	r8, SystemCall - _start + EXC_OFF_SYS_RESET
3:
	bl	trap_reloc
	addi	r7, r7, 0x100		/* next exception vector	*/
	cmplw	0, r7, r8
	blt	3b

	li	r7, .L_SingleStep - _start + EXC_OFF_SYS_RESET
	li	r8, _end_of_vectors - _start + EXC_OFF_SYS_RESET
4:
	bl	trap_reloc
	addi	r7, r7, 0x100		/* next exception vector	*/
	cmplw	0, r7, r8
	blt	4b

	mtlr	r4			/* restore link register	*/
	blr

	/*
	 * Function: relocate entries for one exception vector
	 */
trap_reloc:
	lwz	r0, 0(r7)		/* hdlr ...			*/
	add	r0, r0, r3		/*  ... += dest_addr		*/
	stw	r0, 0(r7)

	lwz	r0, 4(r7)		/* int_return ...		*/
	add	r0, r0, r3		/*  ... += dest_addr		*/
	stw	r0, 4(r7)

	blr


#ifdef CONFIG_405EP
ppc405ep_init:
        /*
        !-----------------------------------------------------------------------
        ! Check FPGA for PCI internal/external arbitration
        !   If board is set to internal arbitration, update cpc0_pci
        !-----------------------------------------------------------------------
	*/
        addi    r3,0,CPC0_PCI_HOST_CFG_EN
#ifdef CONFIG_BUBINGA405EP
        addis   r5,r0,FPGA_REG1@h      /* set offset for FPGA_REG1 */
        ori     r5,r5,FPGA_REG1@l
        lbz     r5,0x0(r5)              /* read to get PCI arb selection */
        andi.   r6,r5,FPGA_REG1_PCI_INT_ARB  /* using internal arbiter ?*/
        beq     ..pci_cfg_set             /* if not set, then bypass reg write*/
#endif
        ori     r3,r3,CPC0_PCI_ARBIT_EN
..pci_cfg_set:
        mtdcr   CPC0_PCI, r3             /* Enable internal arbiter*/

        /*
        !-----------------------------------------------------------------------
        ! Check to see if chip is in bypass mode.
        ! If so, write stored CPC0_PLLMR0 and CPC0_PLLMR1 values and perform a
        ! CPU reset   Otherwise, skip this step and keep going.
        ! Note:  Running BIOS in bypass mode is not supported since PLB speed
        !        will not be fast enough for the SDRAM (min 66MHz)
        !-----------------------------------------------------------------------
	*/
        mfdcr   r5, CPC0_PLLMR1
        rlwinm  r4,r5,1,0x1            /* get system clock source (SSCS) */
        cmpi    cr0,0,r4,0x1

        beq    pll_done                   /* if SSCS =b'1' then PLL has */
                                          /* already been set */
                                          /* and CPU has been reset */
                                          /* so skip to next section */

#ifdef CONFIG_BUBINGA405EP
	/*
        !-----------------------------------------------------------------------
        ! Read NVRAM to get value to write in PLLMR.
        ! If value has not been correctly saved, write default value
        ! Default config values (assuming on-board 33MHz SYS_CLK) are above.
        ! See CPU_DEFAULT_200 and CPU_DEFAULT_266 above.
        !
        ! WARNING:  This code assumes the first three words in the nvram_t
        !           structure in openbios.h.  Changing the beginning of
        !           the structure will break this code.
        !
        !-----------------------------------------------------------------------
	*/
        addis   r3,0,NVRAM_BASE@h
        addi    r3,r3,NVRAM_BASE@l

        lwz     r4, 0(r3)
        addis   r5,0,NVRVFY1@h
        addi    r5,r5,NVRVFY1@l
        cmp     cr0,0,r4,r5            /* Compare 1st NVRAM Magic number*/
        bne     ..no_pllset
        addi    r3,r3,4
        lwz     r4, 0(r3)
        addis   r5,0,NVRVFY2@h
        addi    r5,r5,NVRVFY2@l
        cmp     cr0,0,r4,r5            /* Compare 2 NVRAM Magic number */
        bne     ..no_pllset
        addi    r3,r3,8                 /* Skip over conf_size */
        lwz     r4, 4(r3)               /* Load PLLMR1 value from NVRAM */
        lwz     r3, 0(r3)               /* Load PLLMR0 value from NVRAM */
        rlwinm  r5,r4,1,0x1             /* get system clock source (SSCS) */
        cmpi     cr0,0,r5,1             /* See if PLL is locked */
        beq     pll_write
..no_pllset:
#endif /* CONFIG_BUBINGA405EP */

        addis   r3,0,PLLMR0_DEFAULT@h       /* PLLMR0 default value */
        ori     r3,r3,PLLMR0_DEFAULT@l     /* */
        addis   r4,0,PLLMR1_DEFAULT@h       /* PLLMR1 default value */
        ori     r4,r4,PLLMR1_DEFAULT@l     /* */

        b       pll_write                 /* Write the CPC0_PLLMR with new value */

pll_done:
        /*
        !-----------------------------------------------------------------------
        ! Clear Soft Reset Register
        ! This is needed to enable PCI if not booting from serial EPROM
        !-----------------------------------------------------------------------
		*/
        addi    r3, 0, 0x0
        mtdcr   CPC0_SRR, r3

        addis    r3,0,0x0010
        mtctr   r3
pci_wait:
        bdnz    pci_wait

	blr				  /* return to main code */

/*
!-----------------------------------------------------------------------------
! Function:     pll_write
! Description:  Updates the value of the CPC0_PLLMR according to CMOS27E documentation
!               That is:
!                         1.  Pll is first disabled (de-activated by putting in bypass mode)
!                         2.  PLL is reset
!                         3.  Clock dividers are set while PLL is held in reset and bypassed
!                         4.  PLL Reset is cleared
!                         5.  Wait 100us for PLL to lock
!                         6.  A core reset is performed
! Input: r3 = Value to write to CPC0_PLLMR0
! Input: r4 = Value to write to CPC0_PLLMR1
! Output r3 = none
!-----------------------------------------------------------------------------
*/
pll_write:
        mfdcr  r5, CPC0_UCR
        andis. r5,r5,0xFFFF
        ori    r5,r5,0x0101              /* Stop the UART clocks */
        mtdcr  CPC0_UCR,r5               /* Before changing PLL */

        mfdcr  r5, CPC0_PLLMR1
        rlwinm r5,r5,0,0x7FFFFFFF        /* Disable PLL */
        mtdcr   CPC0_PLLMR1,r5
        oris   r5,r5,0x4000              /* Set PLL Reset */
        mtdcr   CPC0_PLLMR1,r5

        mtdcr   CPC0_PLLMR0,r3           /* Set clock dividers */
        rlwinm r5,r4,0,0x3FFFFFFF        /* Reset & Bypass new PLL dividers */
        oris   r5,r5,0x4000              /* Set PLL Reset */
        mtdcr   CPC0_PLLMR1,r5           /* Set clock dividers */
        rlwinm r5,r5,0,0xBFFFFFFF        /* Clear PLL Reset */
        mtdcr   CPC0_PLLMR1,r5

		/*
        ! Wait min of 100us for PLL to lock.
        ! See CMOS 27E databook for more info.
        ! At 200MHz, that means waiting 20,000 instructions
		 */
        addi    r3,0,20000              /* 2000 = 0x4e20 */
        mtctr   r3
pll_wait:
        bdnz    pll_wait

        oris   r5,r5,0x8000             /* Enable PLL */
        mtdcr   CPC0_PLLMR1,r5          /* Engage */

        /*
         * Reset CPU to guarantee timings are OK
         * Not sure if this is needed...
 	 */
        addis r3,0,0x1000
        mtspr dbcr0,r3               /* This will cause a CPU core reset, and */
                                     /* execution will continue from the poweron */
                                     /* vector of 0xfffffffc */
#endif /* CONFIG_405EP */
#endif
