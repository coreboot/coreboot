/*
This software and ancillary information (herein called SOFTWARE )
called LinuxBIOS          is made available under the terms described
here.  The SOFTWARE has been approved for release with associated
LA-CC Number 00-34   .  Unless otherwise indicated, this SOFTWARE has
been authored by an employee or employees of the University of
California, operator of the Los Alamos National Laboratory under
Contract No. W-7405-ENG-36 with the U.S. Department of Energy.  The
U.S. Government has rights to use, reproduce, and distribute this
SOFTWARE.  The public may copy, distribute, prepare derivative works
and publicly display this SOFTWARE without charge, provided that this
Notice and any statement of authorship are reproduced on all copies.
Neither the Government nor the University makes any warranty, express
or implied, or assumes any liability or responsibility for the use of
this SOFTWARE.  If SOFTWARE is modified to produce derivative works,
such modified SOFTWARE should be clearly marked, so as not to confuse
it with the version available from LANL.
 */
/* Copyright 2000, Ron Minnich, Advanced Computing Lab, LANL
 * rminnich@lanl.gov
 */
/* this set of steps is REQUIRED before you do anything to DRAM */

	/* Reg name        Power-on dflt*/
	CS_WRITE(SIS_PCMC_REG04,$0x07)
	CS_WRITE(SIS_PCMC_REG05,$0x00)
	CS_WRITE(SIS_PCMC_REG06,$0x00)
	CS_WRITE(SIS_PCMC_REG07,$0x02)
	CS_WRITE(SIS_PCMC_REG0C,$0x00)
	CS_WRITE(SIS_PCMC_REG0D,$0x20)
	CS_WRITE(SIS_PCMC_REG0E,$0x00)
	CS_WRITE(SIS_PCMC_REG0F,$0x00)
	CS_WRITE(SIS_PCMC_REG50,$0x09C)
	CS_WRITE(SIS_PCMC_REG51,$0x000)
	CS_WRITE(SIS_PCMC_REG52,$0x0C5)
	CS_WRITE(SIS_PCMC_REG53,$0x000)
	CS_WRITE(SIS_PCMC_REG54,$0x000)		/** 00h (66/100), 08h (133)*/
	CS_WRITE(SIS_PCMC_REG55,$0x029)		/** 29h (100), 1Dh (133)*/
	CS_WRITE(SIS_PCMC_REG56,$0x080)		/** 00h (66), 80h (100/133) */
	/*CS_WRITE(SIS_PCMC_REG57,$0x000)  */		/* 000h (100||VCM), 001h (133)*/
	/* temporary change ... set it to 1, since the bst1b does that too */

#ifndef USE_DOC_MIL
	CS_WRITE(SIS_PCMC_REG57,$0x001)		/** 000h (100||VCM), 001h (133)*/
#endif
	CS_WRITE(SIS_PCMC_REG58,$0x000)
	CS_WRITE(SIS_PCMC_REG59,$0x035)
	CS_WRITE(SIS_PCMC_REG5A,$0x051)
	CS_WRITE(SIS_PCMC_REG5B,$0x000)

#ifndef USE_DOC_MIL
	CS_WRITE(SIS_PCMC_REG63,$0x000)
#endif

	/* This is needed for SiSfb. Shared memory is in dimm 0. */
	CS_WRITE(SIS_PCMC_REG65,$0x000)
	CS_WRITE(SIS_PCMC_REG6A,$0x0C0)
	CS_WRITE(SIS_PCMC_REG6B,$0x001)		/** 000h (66/133), 001h (100)*/
	CS_WRITE(SIS_PCMC_REG6C,$0x020)		/** 02Eh (66), 020h (100), 02Ch (133)*/
	CS_WRITE(SIS_PCMC_REG70,$0x000)		/* Shadow register*/
	CS_WRITE(SIS_PCMC_REG71,$0x080)		/* Shadow register*/
	CS_WRITE(SIS_PCMC_REG72,$0x000)		/* Shadow register*/
	CS_WRITE(SIS_PCMC_REG73,$0x000)		/* Shadow register*/

#ifdef NOTUSED
	CS_WRITE(SIS_PCMC_REG74,$0x000)		/* Shadow register*/
	CS_WRITE(SIS_PCMC_REG75,$0x010)		/* Shadow register*/
#endif

	CS_WRITE(SIS_PCMC_REG80,$0x021)
	CS_WRITE(SIS_PCMC_REG81,$0x0FF)
	CS_WRITE(SIS_PCMC_REG82,$0x07F)
	CS_WRITE(SIS_PCMC_REG83,$0x01E)
	CS_WRITE(SIS_PCMC_REG84,$0x060)
	CS_WRITE(SIS_PCMC_REG85,$0x000)
	CS_WRITE(SIS_PCMC_REG86,$0x003)
	CS_WRITE(SIS_PCMC_REG87,$0x040)
	CS_WRITE(SIS_PCMC_REG88,$0x000)
	CS_WRITE(SIS_PCMC_REG89,$0x008)

#ifndef USE_DOC_MIL
	CS_WRITE(SIS_PCMC_REG8C,$0x066)
	CS_WRITE(SIS_PCMC_REG8D,$0x066)
	CS_WRITE(SIS_PCMC_REG8E,$0x003)		/** default=03h for 100/133, recommend=00h*/
	CS_WRITE(SIS_PCMC_REG8F,$0x055)
#endif

	CS_WRITE(SIS_PCMC_REG90,$0x000)
	CS_WRITE(SIS_PCMC_REG91,$0x000)
	CS_WRITE(SIS_PCMC_REG92,$0x000)
	CS_WRITE(SIS_PCMC_REG93,$0x000)
	CS_WRITE(SIS_PCMC_REG94,$0x040)
	CS_WRITE(SIS_PCMC_REG97,$0x001)
	CS_WRITE(SIS_PCMC_REG98,$0x000)
	CS_WRITE(SIS_PCMC_REG9C,$0x002)
	CS_WRITE(SIS_PCMC_REGA0,$0x000)
	CS_WRITE(SIS_PCMC_REGA1,$0x000)
	CS_WRITE(SIS_PCMC_REGA2,$0x003)
	CS_WRITE(SIS_PCMC_REGA3,$0x001)
	CS_WRITE(SIS_PCMC_REGA5,$0x000)
	CS_WRITE(SIS_PCMC_REGA6,$0x000)
	CS_WRITE(SIS_PCMC_REGA7,$0x000)
	CS_WRITE(SIS_PCMC_REGA8,$0x000)
	CS_WRITE(SIS_PCMC_REGA9,$0x000)
	CS_WRITE(SIS_PCMC_REGAA,$0x000)
	CS_WRITE(SIS_PCMC_REGAB,$0x000)
	CS_WRITE(SIS_PCMC_REGAC,$0x000)
	CS_WRITE(SIS_PCMC_REGC8,$0x004)
	CS_WRITE(SIS_PCMC_REGC9,$0x000)
	CS_WRITE(SIS_PCMC_REGCA,$0x000)
	CS_WRITE(SIS_PCMC_REGCB,$0x000)

#ifdef SIS630S
	CS_WRITE(SIS_PCMC_REG50, $0x7e)
	CS_WRITE(SIS_PCMC_REG51, $0x13)
	CS_WRITE(SIS_PCMC_REG53, $0x0b)
	CS_WRITE(SIS_PCMC_REG55, $0x05)
	CS_WRITE(SIS_PCMC_REG59, $0x05)
	CS_WRITE(SIS_PCMC_REG6A, $0xc6)
	CS_WRITE(SIS_PCMC_REG6B, $0x00)
	CS_WRITE(SIS_PCMC_REG6C, $0x2f)
	CS_WRITE(SIS_PCMC_REG71, $0x00)
	CS_WRITE(SIS_PCMC_REG80, $0x23)
	CS_WRITE(SIS_PCMC_REG83, $0x1f)
	CS_WRITE(SIS_PCMC_REG84, $0xf0)
	CS_WRITE(SIS_PCMC_REG86, $0x01)
	CS_WRITE(SIS_PCMC_REG9C, $0x80)
#endif /* SIS630S */
	
#ifndef USE_DOC_MIL
/* this sequence gets the SiS 630 ram up and running, we hope. */
/* for now, let's stick to bank 0, and jam the size to 64M, 2 side (128M) */
/* the sizing is not hard, but we need an opinion from SiS on whether 
 * they would prefer to write it. They can do this better than we can.
 */
	/* post 0x01 to 0x80 port for debugging */ 
	intel_chip_post_macro(0x01) 

	movb $0xBF, %al		/* Bit 7 controls NMI*/
	outb %al, $0x70		/* Disable NMI interrupts */

	movw $0x0, %si
	movw $0x0, %di

OtherBank:

	/* registers we use:	 eax, cl, and dx */
	/* parameters:	 at some point, cl will be the bank # */
	/* returns:	 none */

	/* note:	 the macros trash cl, eax and dx. Nothing else gets
	 * hurt. So to subroutine-ify them you can call with things in
	 * ebx and ch. So you could have the bank in ch and return in ebx
	 * For now, this is in-line code until we think we know what we're
	 * doing.
	 */
	/* for now, we don't try to autosize
	 * We know the size -- 64M X 2
	 */

	/* turn on dimm 0 */
	movw %si, %cx
	movw $0x1, %ax
	shlw %cl, %ax
	movw %ax, %bx
	CS_WRITE(SIS_PCMC_REG63, %bl)
	movw %cx, %si

	/* make it the max size configuration */
	movw %si, %bx
	addw SIS_PCMC_REG60, %si
	CS_WRITE(%si, DRAM_MAXSIZE)
	movw %bx, %si

	/* disable refresh */
	REGBIC(SIS_PCMC_REG52, REFRESH_ENABLE)
	/* mode register set */
	REGBIS(SIS_PCMC_REG57, MRS)
	/* precharge */
	REGBIS(SIS_PCMC_REG57, PRECHARGE)
	/* refresh command */
	/* the question is, will these cause trouble back-to-back */
	/* not really, because this is a macro, so we have lots of insns
	 * between sets
	 * macros sometimes win
	 * 16 refresh commands
	 */
	REGBIS(SIS_PCMC_REG57, REFRESH)
	REGBIS(SIS_PCMC_REG57, REFRESH)
	REGBIS(SIS_PCMC_REG57, REFRESH)
	REGBIS(SIS_PCMC_REG57, REFRESH)
	REGBIS(SIS_PCMC_REG57, REFRESH)
	REGBIS(SIS_PCMC_REG57, REFRESH)
	REGBIS(SIS_PCMC_REG57, REFRESH)
	REGBIS(SIS_PCMC_REG57, REFRESH)
	REGBIS(SIS_PCMC_REG57, REFRESH)
	REGBIS(SIS_PCMC_REG57, REFRESH)
	REGBIS(SIS_PCMC_REG57, REFRESH)
	REGBIS(SIS_PCMC_REG57, REFRESH)
	REGBIS(SIS_PCMC_REG57, REFRESH)
	REGBIS(SIS_PCMC_REG57, REFRESH)
	REGBIS(SIS_PCMC_REG57, REFRESH)
	REGBIS(SIS_PCMC_REG57, REFRESH)

	/* now turn refresh back on */
	REGBIS(SIS_PCMC_REG52, REFRESH_ENABLE)

	/* now write some data to the sdram. This is required by the 
	 * pc100 spec. The specific patterns vary by mainboard ...
	 * the jmp's are cheap delays, back-to-back writes at this point
	 * may be a problem.
	 */
	movl MPATTERN0, %eax
	movl %eax, 0
	movl SDPATTERN1, %eax
	jmp 1f
1:	movl %eax, 0x8
	jmp 1f
1:	movl %eax, 0x8
	jmp 1f
1:	movl %eax, 0x8
	jmp 1f
1:	movl %eax, 0x8
	jmp 1f
1:	movl %eax, 0x8
	jmp 1f
1:	movl %eax, 0x8
	jmp 1f
1:	movl %eax, 0x8
	jmp 1f
1:	movl %eax, 0x8
	jmp 1f
1:	
	movl MPATTERN0, %eax
	cmpl 0, %eax
	jmp 1f
1:	jne	noram

	movl MPATTERN1, %eax
	movl %eax, 0x40000000-0x10
	movl SDPATTERN1, %eax
	movl %eax, 0x40000000-0x8
	movl MPATTERN1, %eax
	cmpl %eax, 0x40000000-0x10  
	jne 1f

	/* save drame type of this slot
	 * in bit 15-31 of %ecx */
	movb $0x20, %cl
	shll $0x10, %ecx
	jmp doubleside
1:	
	mov $0x00, %cl
	shll $0x10,%ecx

doubleside:
	movw %di, %bx
	movw $0x1, %di
	movw %si, %cx
	shlw %cl, %di
	orw  %bx, %di

	/* FIXME: why do we have this two ?? */
	movl SDPATTERN1, %eax
	cmpl 8, %eax

	CALLSP(sizeram630)
	shrl $0x10, %ecx
	movb %al, %ch
	orb  %cl, %ch

	movw %si, %bx
	addw SIS_PCMC_REG60, %si
	CS_WRITE(%si, %ch)
	movw %bx, %si
	incw %si

	/* this appears to be trouble, so we'll skip it for now. RGM */
#if 1
	cmpw $0x03, %si
	je  done
	jmp OtherBank
#else
	jmp done
#endif

#define CMPBRANCH(address, target) cmp %eax, address;  jne target
#define CMPEQRETURN(address, value) cmp %eax, address;  jne 1f; mov value, %eax; RETSP; 1:
#define CMPNERETURN(address, value) cmp %eax, address;  je 1f; mov value, %eax; RETSP; 1:
#define RETURN(value) mov value, %eax; RETSP

sizeram630:
	movl MPATTERN0, %eax
	movl %eax, 0
	CMPBRANCH(0x1000, SMA2)
	CMPBRANCH(0x800000, SMA_1_1x_0910)
	CMPEQRETURN(0x4000000, SMA_1_11_08)
	RETURN(SMA_1_13_08)
SMA_1_1x_0910:
	CMPBRANCH(0x1000000, SMA_1_xx_10)
	CMPEQRETURN(0x4000000, SMA_1_11_09)
	RETURN(SMA_1_13_09)
SMA_1_xx_10:
	CMPNERETURN(0x8000000, SMA_1_13_11)
	CMPEQRETURN(0x4000000, SMA_1_11_10)
	RETURN(SMA_1_13_10)
SMA2:	CMPBRANCH(0x800000, SMA_2_xx_091011)
	CMPNERETURN(0x10000000, SMA_2_13_08)
	CMPEQRETURN(0x4000000, SMA_2_11_08)
	RETURN(SMA_2_12_08)
SMA_2_xx_091011:
	CMPBRANCH(0x1000000, SMA_2_xx_1011)
	CMPEQRETURN(0x10000000, SMA_2_12_09)
	RETURN(SMA_2_13_09)
SMA_2_xx_1011:
	CMPBRANCH(0x8000000, SMA_2_xx_11)
	CMPEQRETURN(0x10000000, SMA_2_12_10)
	RETURN(SMA_2_13_10)
SMA_2_xx_11:
	CMPEQRETURN(0x10000000, SMA_2_12_11)
	movl MPATTERN1, %eax
	movl %eax, 0x20000000
	CMPEQRETURN(0x20000000, SMA_2_13_11)

noram:
	incw %si
	cmpw $0x03,%si
	je   done
	jmp  OtherBank

done:
	movl $0x80000060, %eax
	movw $0x0cf8, %dx
	outl %eax, %dx
	movw $0x0cff, %dx
	movw %di, %ax
	outb %al, %dx

#ifdef SIS630
#ifdef SIS_FIXUP_FOR_FB
	// make shared memory between the agp and the processor
	movl $0x80000060, %eax
	movw $0x0cf8, %dx
	outl %eax, %dx	
	movw $0x0cff, %dx
	inb  %dx, %al
	orb  $0x90, %al
	outb %al, %dx
#endif  /* SIS_FIXUP_FOR_FB */
#endif  /* SIS630 */

#endif /* USE_DOC_MIL */
