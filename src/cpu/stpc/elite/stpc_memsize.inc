/*
 * ram size detection for the STPC Elite
 *
 * Modified by Peter Fox:
 * moved the DetectMemorySize symbol
 * Support SPD timing for the Elite (Set option SPD_MEM_TIMING, uses default if fails)
 * Support faster default memory timing (Set option SLOW_MEM for original)
 * Note that have to initialise non-existant graphics memory for SDRAM to startup ok
 * Added GPL header
 *
 * This software comes with ABSOLUTELY NO WARRANTY; This is free software, and
 * you are welcome to redistribute it under certain conditions; you can
 * redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation version 2 of the
 * License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the  GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 * Ave, Cambridge, MA 02139, USA. "
 *
 * From the STPC Consumer II version
 *
 * by Steve M. Gehlbach
 *
 * follows code from st web site
 *
 * set ts=4 to edit
 *
 */

#define STPC_MEM_GRAPHIC        0x36
#define STPC_GBASE 0x08000000
#define STPC_MEM_REG0 STPC_GBASE + 0x004C6000
#define STPC_MEM_REG1 STPC_GBASE + 0x004C6004
#define STPC_MEM_REG2 STPC_GBASE + 0x004C6008

#define SetStpc(Index,Value)			 \
	mov		Index, %al					;\
	outb    %al, $STPC_CONF_REG_INDEX	;\
	mov		Value, %al					;\
	outb    %al, $STPC_CONF_REG_DATA

/*----------------------------------------------------------------------------
  DetectMemorySize
 	Determine the memory configuration.
  Inputs:
 	DS: 4GB access
 	SDRAM only:
 	    BL: bits 0-3: read clock delay
 		bit 4: '1' to enable CRTC latch
 		bit 5: '1' to enable GE latch
 		bit 6: '1' to enable host latch
 		bit 7: '1' for registered DIMM
    Typical setting: 0x00- no latches, no registered, rd delay=0
  Outputs:
 	EAX: top memory address
 	CY: if bank 0 is empty
  Uses:
 	EAX, CX, EDI, ESI, BX
	Sets frame buffer = 0
	Craps on memory in various places.

  Requirements:
	Flat 4GB 
 	Refresh running
 	Gate A20 enabled
-----------------------------------------------------------------------------*/

//    Set SDRAM default configuration

	.global DetectMemorySize

DetectMemorySize:
	movb	$0x17, %al
	outb	%al, $0x80

#if (SPD_MEM_TIMING==1)
	CALL_LABEL(stpc_read_spd_timing)
	jnz	1f
#endif
	xor	%ebx, %ebx // all params zero

	movzbl	%bl, %eax
	and		$0b10000000, %al 		// set registered DIMM per %bl
	shlw	$2, %ax
	movb	%bl, %al
	andb	$0b01110000, %al		// set host, GE, CRTC latch per %bl
	shll	$18, %eax
1:
#if (SLOW_MEM==1)
		//    RCT=8   BL=7           RRW=3     PRA=3       CASlat=3
		//    MRSR=1      RCASLCY=3
	orl		$8 + (0b111 << 4) + (3 << 8) + (3 << 15) + (3 << 19) \
			+ (1 << 26) + (3 << 28), %eax
			// TRC=8, TRCD=3, TRP=3, [4M*4]*16 (1024), CL=3,
			// deassert RAS, update SDRAM, Read CL=CL
#else
		//    RCT=7   BL=7           RRW=2     PRA=2       CASlat=2
		//    MRSR=1      RCASLCY=2
	orl		$7 + (0b111 << 4) + (2 << 8) + (2 << 15) + (2 << 19) \
			+ (1 << 26) + (2 << 28), %eax
			// TRC=7, TRCD=2, TRP=2, [4M*4]*16 (1024), CL=2,
			// deassert RAS, update SDRAM, Read CL=CL
#endif
	movl		$STPC_MEM_REG0, %esi
	movl		%eax, %ds:(%esi) 
	movzx	%bl, %eax
	andb	0b00001111, %al		// set read clock delay to minimum
	orb		0b00011000, %al		// enable 16mA
	addw	$4, %si 			// select STPC_MEM_REG1
	movl	%eax, %ds:(%esi)


#if 1
// Elite doesn't have graphic memory, but seems to need this to work

//    Clear frame buffer to test memory

	movb	$STPC_MEM_GRAPHIC, %ah
	stpc_conf_read_macro 
	andb	$0b11000000, %al		// frame buffer=0
	stpc_conf_write_macro
#endif

//   DS:ESI = bank base address
//   DS:EDI = bank end address
//   CH = bank under test

	movl	$0, %esi			// begin at address 0
	movb	$0x30, %ch	// bank 0

.NextBank:
	movl	$(2 << 20), %edi		// begin size detection at 2MB
					// (smallest size in 64 bits)

//   Set size in all bank registers

	mov		%esi, %eax
	add		%edi, %eax

	shr		$(20-8), %eax	// AH = top address in MB
	dec		%ah				// AH = bank size value
	cmp		$128, %ah		// size > 128MB?
	jae		.End			// yes, end the detection
	mov		%ch, %cl		// start from current bank

.BankLoop1:
	SetStpc(%cl, %ah)

	incb	%cl			// next bank register
	cmp		$0x33, %cl	// last bank register?
	jbe		.BankLoop1		// no, continue

//   Check bank offset 0

	movl	$0x0A55AA55A, %eax		// test pattern
	movl	%eax, %ds:(%esi)			// write pattern
	movl	$0x0ffffffff, %ds:8(%esi)	// precharge bus
	cmpl	%eax, %ds:(%esi)			// does bank hold the pattern?
	jne	.BankEmpty					// no, bank is empty

	not		%eax					// revert pattern
	movl	%eax, %ds:4(%esi)		// write pattern at address+4
	movl	$0x0ffffffff, %ds:12(%esi) // precharge bus
	cmpl	%eax, %ds:4(%esi)		// is bank 64-bit wide?
	je		.FindBankSize			// yes, don't modify setting

//   Bits[63:32] test fails, consider bank empty

	jmp	.BankEmpty


.FindBankSize:

//   Set size in all bank registers

	movl	%esi, %eax
	addl	%edi, %eax
	shrl	$(20-8), %eax		// AH = top address in MB
	decb	%ah				// AH = bank size value
	cmpb	$128, %ah		// size > 128MB?
	jae	.PreviousSize		// yes, return to previous size
	movb	%ch, %cl		// start from current bank
.BankLoop2:
	SetStpc(%cl, %ah)

	incb	%cl					// next bank register
	cmpb	$0x33, %cl	// last bank register?
	jbe		.BankLoop2			// no, continue

//   Tag 2 power N offsets with the offset value

	xorl	%eax, %eax			// start at offset 0
	movl	%eax, %ds:(%esi)	// tag offset 0
	movl	%eax, %ds:4(%esi)	// tag offset 4
	movl	$256, %eax			// start 2 power N offset
.TagLoop:
	movl	%eax, %ds:(%eax,%esi)	// tag offset
	movl	%eax, %ds:4(%eax,%esi)	// tag offset+4
	shll	$1, %eax			// next 2 power N
	cmpl	%edi, %eax			// reach the end of the bank?
	jb		.TagLoop			// no, continue

//   Test the 2 power N offsets with the offset value

	xorl	%eax, %eax			// start at offset 0
	cmpl	%eax, %ds:(%esi)	// does offset hold the value?
	jne		.PreviousSize		// no, return to previous size
	cmpl	%eax, %ds:4(%esi)	// does offset+4 hold the value?
	jne		.PreviousSize		// no, return to previous size
	movl	$256, %eax			// start 2 power N offset
.TestLoop:
	cmpl	%eax, %ds:(%eax, %esi)	// does offset hold the value?
	jne		.PreviousSize		// no, return to previous size
	cmpl	%eax, %ds:4(%eax, %esi)	// does offset+4 hold the value?
	jne		.PreviousSize		// no, return to previous size
	shll	$1, %eax			// next 2 power N
	cmpl	%edi, %eax			// reach the end of the bank?
	jb		.TestLoop			// no, continue

	shll	$1, %edi			// next 2 power N size
	jmp		.FindBankSize

.PreviousSize:
	shrl	$21, %edi			// previous 2 power N size (in MB)
	shll	$20, %edi
	addl	%edi, %esi			// ESI = next bank base address

.BankEmpty:
	movl	%esi, %eax
	shrl	$(20-8), %eax	// AH = top address in MB
	stc						// assume error
	jz		.End			// exit if bank 0 is empty
	dec		%ah				// AH = bank size value

//   Detect the SDRAM configuration (16Mb / 2 banks, 64/128Mb / 2 banks or
//   64/128Mb / 4 banks) by testing for bank 0 the 3 modes and keep the one
//   that gives the highest size.
//   STPC Vega also supports 256Mb / 4 banks configuration.

	cmpb	$0x30, %ch	// bank 0?
	jne		.AlreadyDetected		// no, skip test

	movl	$STPC_MEM_REG1, %edi
	orb		$0b00100000, %ds:(%edi)	// select 16mA for 64/128Mb config

	addw	$4, %di			// select STPC_MEM_REG2
	movb	%ds:(%edi), %al	// AL = current configuration

	cmpb	%bh, %ah		// current size <= previous size?
	jbe		.NextConfig		// yes, skip
	movw	%ax, %bx		// save configuration and size

.NextConfig:
	cmpb	$2, %al			// last configuration?
	jae		.BestConfig		// yes, end detection
	incb	%al				// next configuration
	movb	%al, %ds:(%edi)	// set configuration
	subl	%esi, %esi		// restart at address 0
	movl	$(2 << 20), %edi	// begin size detection at 4MB
	jmp		.FindBankSize

//   Set the best configuration

.BestConfig:
	orb		%bl, %bl			// 16Mb config?
	jnz		.NotConfig16Mb		// no, skip
	andb	$~0b00100000, %ds:-4(%edi)	// clear 16mA for 64/128Mb config
.NotConfig16Mb:
	movb	%bl, %ds:(%edi)
	movb	%bh, %ah
	movzbl	%bh, %esi			// restore base address
	incw	%si
	shll	$20, %esi

.AlreadyDetected:

//   Set current bank size

	SetStpc(%ch, %ah)

	incb	%ch			// next bank
	cmpb	$0x33, %ch	// last bank?
	jbe		.NextBank		// no, continue

.End:

// at this point memsize is in %esi

DectectMemSize0:
