/*
 * $Id$
 * $Source$
 *
 * turn on ram at memory hole
 * This is needed to put irq route table in 0xf0000
 * area by writing to this area later.
 * Also move the gdt to low ram.  See note below.
 *
 * by
 * Steve M. Gehlbach (steve@kesa.com)
 *
 */

	movb	$0x13, %al
	outb	%al, $0x80

// turn on write to ram at 0xf0000
	movb	$0x28, %ah // cache cntrl reg 3
	stpc_conf_read_macro
	orb		$0b00000001, %al  
	stpc_conf_write_macro

////////////////////////////////////////////////////////////////
//
//  We are going to move and reload the gdt, since we are 
//  executing from high mem.  The current gdt is located 
//  above 1M and linux will hang unless the gdt is located <1M.   
//  So we move  the gdt to ram in <1M area. Just under 0x90000
//  is (apparently) a safe spot.
//
////////////////////////////////////////////////////////////////
#define NEW_GDT_PTR_ADDR   0x0008ff00

        movl    $new_gdt_ptr,%esi // source 
        movl    $NEW_GDT_PTR_ADDR,%edi  // find some ram
        movl    $(new_gdt_end-new_gdt_ptr), %ecx   // get size
		shrl	$2, %ecx	// divide by 4 and add 1
		incl	%ecx
        rep
        movsl

// reset the gdt addr to new spot
        movl    $(NEW_GDT_PTR_ADDR+6), (NEW_GDT_PTR_ADDR+2)

.align 4
// now load the new gdt
        lgdt    %cs:NEW_GDT_PTR_ADDR
        ljmp    $0x10, $new_gdt_loaded

new_gdt_loaded:
	movb	$0x14, %al
	outb	%al, $0x80

//
// turn on read cycles to ram in memory hole (0xf000)
//
	movb	$0x28, %ah // cache cntrl reg 3
	stpc_conf_read_macro
	orb		$0b01000011, %al
	stpc_conf_write_macro

	movb	$0x15, %al
	outb	%al, $0x80
	jmp		new_gdt_end

new_gdt_ptr:
.word   0x8000		// gdt limit=2048,
					//  256 GDT entries
.word   0, 0		// gdt base (filled in later)

new_gdt:
.word   0, 0, 0, 0	// dummy
.word   0, 0, 0, 0	// unused

.word   0xFFFF		// 4Gb - (0x100000*0x1000 = 4Gb)
.word   0			// base address = 0
.word   0x9A00		// code read/exec
.word   0x00CF		// granularity = 4096, 386
					//  (+5th nibble of limit)
.word   0xFFFF		// 4Gb - (0x100000*0x1000 = 4Gb)
.word   0			// base address = 0
.word   0x9200		// data read/write
.word   0x00CF		// granularity = 4096, 386
					//  (+5th nibble of limit)
new_gdt_end:
