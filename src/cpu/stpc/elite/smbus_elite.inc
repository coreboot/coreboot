	jmp smbus_elite_init

// Bit bashing spd code for the STPC
//
// Copyright (C) 2004 Aeroflex International Limited
// peter.fox @ aeroflex.com, Longacres House, Six Hills Way, Stevenage, SG1 2AN, UK.
// 
// This software comes with ABSOLUTELY NO WARRANTY; This is free software, and
// you are welcome to redistribute it under certain conditions; you can
// redistribute it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation version 2 of the
// License.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the  GNU General Public License for
// more details.
// 
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 675 Mass
// Ave, Cambridge, MA 02139, USA. "
//

#define DDC_CLOCK_BIT 1
#define DDC_DATA_BIT 2

//
// 66 corresponds to 5us delay at 133MHz CPU clock, with no wait states
// -> 100kHz SPD clock
// In practice this gives ~1ms, due to the slow ISA bus speed.
//
#define DDC_WRITE_DELAY 2

	/*
	 * Macro:	SetDDC
	 * Arguments:	Value to write in al
	 * Results:	none
	 * Trashed:	al, ecx
	 * Effects:	Sets the DDC lines as told, and delays at least 5us
	 */
#define SetDDC\
	movb	%al, %cl;\
	movb	$0x97, %al;\
	outb    %al, $STPC_CONF_REG_INDEX;\
	inb	$STPC_CONF_REG_DATA, %al;\
	movb	%al, %ch;\
	andb	$0xF0, %ch;\
	andb	$DDC_CLOCK_BIT | DDC_DATA_BIT, %cl;\
	orb	%cl, %ch;\
	movb	$0x97, %al;\
	outb    %al, $STPC_CONF_REG_INDEX;\
	mov	%ch, %al;\
	outb    %al, $STPC_CONF_REG_DATA;\
	movl	$DDC_WRITE_DELAY, %ecx;\
0:	nop;\
	loop	0b

	/*
	 * Macro:	ReadSDA
	 * Arguments:	none
	 * Results:	bit value returned in lsb of al
	 * Trashed:	al
	 * Effects:	Read state of the SDA line
	 */
#define ReadSDA\
	mov	$0x97, %al;\
	outb    %al, $STPC_CONF_REG_INDEX;\
	inb	$STPC_CONF_REG_DATA, %al;\
	shrb	$3, %al;\
	andb	$1, %al

	/*
	 * Macro:	DDC_start
	 * Arguments:	none
	 * Results:	none
	 * Trashed:	al, ecx
	 * Effects:	Send spd start condition
	 */
#define DDC_start\
	mov	$DDC_CLOCK_BIT | DDC_DATA_BIT, %al;\
	SetDDC;\
	mov	$DDC_CLOCK_BIT, %al;\
	SetDDC;\
	xor	%al,%al;\
	SetDDC;

	/*
	 * Macro:	DDC_stop
	 * Arguments:	none
	 * Results:	none
	 * Trashed:	al, ecx
	 * Effects:	Send spd stop condition
	 */
#define DDC_stop\
	xor	%al,%al;\
	SetDDC;\
	mov	$DDC_CLOCK_BIT, %al;\
	SetDDC;\
	mov	$DDC_CLOCK_BIT | DDC_DATA_BIT, %al;\
	SetDDC;

	/*
	 * Macro:	DDC_send_8_bits
	 * Arguments:	8 bits in dl
	 * Results:	Ack bit in ah
	 * Trashed:	ax, dx, ecx
	 * Effects:	Send 8 bits, and receive the acknowledge bit
	 */
#define DDC_send_8_bits\
	movb	$8, %dh;\
2:	xor	%ah, %ah;\
	testb	$0x80, %dl;\
	jz	1f;\
	movb	$DDC_DATA_BIT, %ah;\
1:	movb	%ah, %al;\
	SetDDC;\
	movb	%ah, %al;\
	orb	$DDC_CLOCK_BIT, %al;\
	SetDDC;\
	movb	%ah, %al;\
	SetDDC;\
	shlb	$1, %dl;\
	decb	%dh;\
	jnz	2b;\
	mov	$DDC_DATA_BIT, %al;\
	SetDDC;\
	mov	$DDC_CLOCK_BIT | DDC_DATA_BIT, %al;\
	SetDDC;\
	ReadSDA;\
	movb	%al, %ah;\
	mov	$DDC_DATA_BIT, %al;\
	SetDDC;\
	testb	$1, %ah

	/*
	 * Macro:	DDC_receive_8_bits
	 * Arguments:	Ack bit in ah
	 * Results:	Data byte in dl
	 * Trashed:	ax, dx, ecx
	 * Effects:	Receive 8 bits, and send the acknowledge bit
	 */
#define DDC_receive_8_bits\
	movb	$8, %dh;\
	xor	%dl, %dl;\
1:	mov	$DDC_DATA_BIT, %al;\
	SetDDC;\
	mov	$DDC_CLOCK_BIT | DDC_DATA_BIT, %al;\
	SetDDC;\
	ReadSDA;\
	shlb	$1, %dl;\
	orb	%al, %dl;\
	decb	%dh;\
	jnz	1b;\
	testb	$0x01, %ah;\
	jz	2f;\
	movb	$DDC_DATA_BIT, %dh;\
2:	movb	%dh, %al;\
	SetDDC;\
	movb	%dh, %al;\
	orb	$DDC_CLOCK_BIT, %al;\
	SetDDC;\
	movb	%dh, %al;\
	SetDDC

	/*
	 * Routine:	smbus_read_byte
	 * Arguments:	%esp return address
	 *              %bl device on the smbus to read from 
	 *              %bh address on the smbus to read
	 *
	 * Results:	zf clear (non-zero)
	 *              byte read %eax
	 * 	On Error:	
	 *              zf set (zero)
	 *              %eax trashed
	 *
	 * Trashed:	%ecx, %edx, %eax
	 * Effects:	reads a byte off of the smbus
	 */

#define SMBUS_READ_BYTE(device, address) \
	movl $( (device) | ((address) << 8)), %ebx ; \
	CALLSP(smbus_read_byte)

smbus_read_byte:
	/* Do a dummy write to set the device internal read pointer */
	DDC_start
	movb	%bl /* device */, %dl
	shlb	$1, %dl
	DDC_send_8_bits
	jnz	smbus_nodev
	/* Set the address pointer */
	movb %bh /* address */, %dl
	DDC_send_8_bits
	jnz	smbus_nodev
	/* Now do a read from current pointer */
	DDC_start
	movb	%bl /* device */, %dl
	shlb	$1, %dl
	orb	$1, %dl
	DDC_send_8_bits
	jnz	smbus_nodev
	mov	$1, %ah
	DDC_receive_8_bits
	mov	%dl, %al
	orb	$1, %ah /* Non-zero result */
	jmp	3f
smbus_nodev:
	xor	%al, %al /* Zero result */
3:
	RETSP

smbus_elite_init:
	mov	$DDC_CLOCK_BIT | DDC_DATA_BIT, %dl
	SetDDC

