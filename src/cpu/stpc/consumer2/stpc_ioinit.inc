/*
 * originally on ST web site
 *
 * converted to ATT syntax
 * by
 * Steve M. Gehlbach
 *
 */

//   Initialize basic I/O devices

	
	movb	$0x16, %al
	outb	%al, $0x80
stpc_io_init:

        movl     $IoInitTable, %ebx
IoInitLoop:
        movzbw   %cs:0(%ebx), %dx	// get the address
        cmpb     $0x0ff, %dl
        je      IoInitEnd
        incw     %bx
        movb     %cs:0(%ebx), %al	// get the value
        incw     %bx
        outb     %al, %dx		// put the value in the port
        jmp     IoInitLoop
IoInitEnd:
		jmp EnableGateA20	// keep going

IoInitTable:                            // I/O initialization table
        // Slave PIC
        .byte      0xA0, 0b00010001         // ICW1: ICW4 needed
//        .byte      0xA1, 0x070              // ICW2: interrupt vector
// from linux 2.2, the st settings dont work for linux
        .byte      0xA1, 0x028              // ICW2: interrupt vector
        .byte      0xA1, 2                 // ICW3: slave cascade
        .byte      0xA1, 0b00000001         // ICW4: 8086 mode
        .byte      0xA1, 0b11111111         // mask: all masked

        // Master PIC
        .byte      0x20, 0b00010001         // ICW1: ICW4 needed
//        .byte      0x21, 0x008              // ICW2: interrupt vector
// from linux 2.2, the st settings dont work for linux
        .byte      0x21, 0x020              // ICW2: interrupt vector
        .byte      0x21, 1 << 2           // ICW3: master cascade
        .byte      0x21, 0b00000001         // ICW4: 8086 mode
        .byte      0x21, ~ (1 << 2)     // mask: all masked except IRQ2


        // PIT
        .byte      0x43, 0b00110110         // Timer 0 (system time): mode 3
        .byte      0x40, 0x0FF              // 18.2Hz (1.19318MHz/65535)
        .byte      0x40, 0x0FF
        .byte      0x43, 0b01010100         // Timer 1 (ISA refresh): mode 2
        .byte      0x41, 18                // 64KHz (1.19318MHz/18)

        // Slave DMA
        .byte      0x00, 0                 // clear base address 0
        .byte      0x00, 0
        .byte      0x01, 0                 // clear count 0
        .byte      0x01, 0
        .byte      0x02, 0                 // clear base address 1
        .byte      0x02, 0
        .byte      0x03, 0                 // clear count 1
        .byte      0x03, 0
        .byte      0x04, 0                 // clear base address 2
        .byte      0x04, 0
        .byte      0x05, 0                 // clear count 2
        .byte      0x05, 0
        .byte      0x06, 0                 // clear base address 3
        .byte      0x06, 0
        .byte      0x07, 0                 // clear count 3
        .byte      0x07, 0
        .byte      0x0B, 0b01000000         // set channel 0 to single mode, verify transfer
        .byte      0x0B, 0b01000001         // set channel 1 to single mode, verify transfer
        .byte      0x0B, 0b01000010         // set channel 2 to single mode, verify transfer
        .byte      0x0B, 0b01000011         // set channel 3 to single mode, verify transfer
        .byte      0x08, 0                 // enable controller

        // Master DMA
        .byte      0xC0, 0                 // clear base address 0
        .byte      0xC0, 0
        .byte      0xC2, 0                 // clear count 0
        .byte      0xC2, 0
        .byte      0xC4, 0                 // clear base address 1
        .byte      0xC4, 0
        .byte      0xC6, 0                 // clear count 1
        .byte      0xC6, 0
        .byte      0xC8, 0                 // clear base address 2
        .byte      0xC8, 0
        .byte      0xCA, 0                 // clear count 2
        .byte      0xCA, 0
        .byte      0xCC, 0                 // clear base address 3
        .byte      0xCC, 0
        .byte      0xCE, 0                 // clear count 3
        .byte      0xCE, 0
        .byte      0xD6, 0b11000000         // set channel 0 to cascade mode
        .byte      0xD6, 0b01000001         // set channel 1 to single mode, verify transfer
        .byte      0xD6, 0b01000010         // set channel 2 to single mode, verify transfer
        .byte      0xD6, 0b01000011         // set channel 3 to single mode, verify transfer
        .byte      0xD0, 0                 // enable controller

        .byte      0x0E, 0                 // enable DMA0 channels
        .byte      0xD4, 0                 // clear chain 4 mask

        .byte      0xFF                    // end of table


//----------------------------------------------------------------------------
// Keyboard controller definitions

#define KBDC_CONTROL	0x064
#define KBDC_STATUS		0x064
#define KBDC_DATA		0x060

//----------------------------------------------------------------------------
// Gate A20 definitions

#define GATEA20_ENABLE	1
#define GATEA20_DISABLE	0

//----------------------------------------------------------------------------
// KbdcWaitIbe
//       Wait for the keyboard controller ready to receive a command or a data.
// Inputs:
//       none
// Outputs:
//       none
// Uses:
//       AL

#define KbdcWaitIbe					\
									\
0:									\
        inb		$KBDC_STATUS, %al	;\
        testb	$0b00000010, %al		;\
        jnz		0b					;\


//----------------------------------------------------------------------------
// EnableGateA20
//
// Requirements:
//       Keyboard emulation enabled (reg STPC_MISC0/bit3=0).


//		.globl EnableGateA20

EnableGateA20:
        KbdcWaitIbe
        movb	$0xd1, %al
        outb	%al, $KBDC_CONTROL

        KbdcWaitIbe
        movb    $1, %al
        shl		$1, %al                   // gate A20 is bit 1
        orb		$0xdd, %al
        outb	%al, $KBDC_DATA
