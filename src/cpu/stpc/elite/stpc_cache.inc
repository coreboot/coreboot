/*
 * for STPC ConsumerII
 *
 * by Steve M. Gehlbach
 *
 * mostly borrowed from st web site and converted
 * to ATT ordering for gnu asm
 *
 * took out turning on L1 since this is done later in linuxbios main C
 * code.
 */

	/************************************************
	 *                                              *
	 *  Setup stpc specific cache registers         *
	 *                                              *
	 ************************************************/

// smg note: linux croaks if this section of code is not run early on

	stpc_chip_post_macro(0x19)

#if 0 // this is done later
	// enable L1 cache (below per ST web site prim_bl)
	movl	%cr0, %eax
	andl	$~0x60000000, %eax
	movl	%eax, %cr0
#endif
	// set cache timing for > 75Mhz
 	movb	$0x22, %ah
	stpc_conf_read_macro
	andb	$0b10000000, %al
	orb		$0b00000101, %al
	stpc_conf_write_macro

	// enable L1 cache write back mode
	movb	$0x20, %ah
	stpc_conf_read_macro
	orb		$0b00100000, %al
	stpc_conf_write_macro

#if 0  // this is done later
	// enable CPU WB (cr0/nw=1)
	movl	%cr0, %eax
	bts		$29, %eax
	movl	%eax, %cr0
#endif
	// CC2 configuration control register
	// enable WB interface, burst write cycles, and lock NW
	movb	$0xc2, %ah
	stpc_conf_read_macro
	andb	$~0b00100000, %al
	orb		$ 0b01000110, %al
	stpc_conf_write_macro

// set isa timing and isa buffering
	movb	$0x001, %ah		// ipc config register
	stpc_conf_read_macro
	andb	$0x03, %al		// clear ws bits (0 ws)
	stpc_conf_write_macro

	movb	$0x050, %ah			// misc reg0
	movb	$0b11010000, %al	// r/w post enable, 0 WS, pciclk/4
	stpc_conf_write_macro

	stpc_chip_post_macro(0x20)
