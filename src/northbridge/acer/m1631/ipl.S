/* wow, the acer way to to this hurts. So we do it our way:
 * 32-bit test not needed. 
 */
/* the MCR is 32-bits. You set it, it programs SDRAM. 
 * first check: get Column address size (CAS)
 * Start out assuming that it is 8 bits, then grow. 
 * nicely, if we put the 32-bit MCR value in %ecs, we can 
 * ror it 16 bits, and as we grow the CAS, we just inc cs, and that will
 * set the right value. 
 */

	movl	$INIT_MCR,  %ecx
	mov	$0x6c, %al
	CALL_SP PCI_WRITE($0x6c)
	rorl	$16, %ecx
	/* test 8 bit CAS */
	clrb	0
	movb	$1, 0x800
	tstb	0
	jnz	sizeram
	inc	%cl
	movb	$1, 0x1000
	tstb	0
	jnz	sizeram
	inc	%cl
	movb	$1, 0x2000
	tstb	0
	jnz	sizeram
	inc	%cl
	
	
	rorl	$16, %ecx
	/* clear the 'no multi page' bit. */
	andw	$0xefff, %cx
	mov	$0x6c, %al
	CALL_SP PCI_WRITE($0x6c)
/* size is now in cx[19:16] */
/* now size the dram */
/* you had best have at least 32M; that's as small as we go */
/* rorr 20 the ecx value, to get row size into lsb */
	clrb	0
	ror	$20, %ecx
	movl	$0x400000, %esi
1: 
	movb	$1, [esi]
	inc	%cl
	jz	1b
/* size is now in esi */
/* %ecx has setting for register */
	rol	$20, %ecx
/* set 4 banks. */
	orb	$1, %cl
	mov	$0x6c, %al
	CALL_SP PCI_WRITE($0x6c)
/* bank detect */
/* you don't need to even care how large CAS is. 
 * Just set 4-bank mode, and set non-zero numbers into a few places. 
 * if the byte at 0 changes, you have two banks. Trivial.
 */
	clrb	0
	movb	$1, 0x1000
	movb	$1, 0x2000
	movb	$1, 0x4000
	tstb	0
	jz	1f /* only one bank */
	orl	$80000, %ecx
1: 
	/* clear 4 banks */
	andb	$0xfe, %cl
	mov	$0x6c, %al
	CALL_SP PCI_WRITE($0x6c)
	/* at this point, dram slot 0 is up.  we hope. */
	
