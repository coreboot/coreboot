/* 
 * Modified
 * By Steve M. Gehlbach
 *
 * Heavily borrowed from sample code at ST web site
 *
 */

//
// This is 32-bit code
// We go ahead and jam the whole chip not just the serial; it's just as easy.
//

/* The base address is either 0x3F0 or 0x370, depending on config pin */

#ifndef SMC_BASE
#error SMC_BASE must be defined as 0x3F0 or 0x370
#endif

#if (SMC_BASE == 0x370)
/* #warning SMC_BASE set to 0x370 */
#elif (SMC_BASE == 0x3f0)
/* #warning SMC_BASE set to 0x3f0 */
#else
#error SMC_BASE defined as other than 0x3F0 or 0x370
#endif

#define SMC_INDEX SMC_BASE
#define SMC_DATA  SMC_BASE+1

//#define SIO_ADDR 0x370
#define SIO_ADDR SMC_BASE
#define SIO_GPIO_ADDR 0x0EA
#define SIO_GPIO_DATA 0x0EB

#define UnlockSio \
	movw	$SIO_ADDR, %dx 	;\
	movb	$0x55, %al		;\
	outb	%al, %dx

#define LockSio \
	movw	$SIO_ADDR, %dx	;\
	movb	$0xaa, %al		;\
	outb	%al, %dx

#define GetSio(Index)		\
	movw	$SIO_ADDR, %dx	;\
	movb	Index, %al		;\
	outb	%al, %dx		;\
	incw	%dx				;\
	inb		%dx, %al

#define SetSio(Index, Value)		\
	movw	$SIO_ADDR, %dx			;\
	movb	Index, %al				;\
	outb	%al, %dx				;\
	incw	%dx						;\
	movb	Value, %al				;\
	outb	%al, %dx

#define SelectSioDev(Device)	\
	SetSio(7, Device)


//
//----------------------------------------------------------------------------
//  this sets up the super IO chip

GLOBAL(stpc_board_init)

	mov		$0x0209, %dx		// clear watchdog
	xorb	%al, %al
	outb	%al, %dx

	UnlockSio

	movl	$SioTable, %ebx
0:	SetSio( (%ebx), 1(%ebx) )
	addl	$2, %ebx
	cmpl	$SioTableEnd, %ebx
	jb	0b

	LockSio

	// GPIO 1
	//  bit7: unused
	//  bit6: unused
	//  bit5: DDC line: 0=disabled, 1=enabled
	//  bit4: video YUV: 0=SAA, 1=VIP connector
	//  bit3: unused
	//  bit2; unused
	//  bit1: external 27MHz: 0=enabled, 1=disabled
	//  bit0: unused

	movb	$1, %al
	outb	%al, $SIO_GPIO_ADDR
	movb	$0b00000010, %al
	outb	%al, $SIO_GPIO_DATA

	// return
	jmp stpc_superio0
//----------------------------------------------------------------------------
// Board data

SioTable:
	.byte	0x002,	0x001		// do a soft reset
	.byte	0x003,	0x083		// gpio port enable (EAh/EBh)
	.byte	0x022,	0x039		// power on
	.byte	0x023,	0x000		// intelligent pwr mgnt off (smg: uncommented)
	.byte	0x024,	0x084		// OSC is on, IRQ8 active low

//   FDC
	.byte	0x007,	0x000		// select
	.byte	0x060,	0x003
	.byte	0x061,	0x0F0		// I/O 3F0h
	.byte	0x070,	0x006		// IRQ 6
	.byte	0x074,	0x002		// DMA 2
	.byte	0x0F0,	0x04E		// FDC output push-pull
	.byte	0x0F1,	0x000		// FDD option register
	.byte	0x0F2,	0x0FF		// FDD type register
	.byte	0x0F4,	0x000		// FDD0
	.byte	0x0F5,	0x000		// FDD1
	.byte	0x030,	0x001		// activate

//   LPT
//	.byte	0x007,	0x003		// select
//	.byte	0x060,	0x003
//	.byte	0x061,	0x078		// I/O 378h
//	.byte	0x070,	0x007		// IRQ 7
//	.byte	0x074,	0x004		// no DMA
//	.byte	0x0F0,	0x038		// SPP mode
//	.byte	0x0F1,	0x000		// normal mode
//	.byte	0x030,	0x001		// activate

//   SER1
	.byte	0x007,	0x004		// select
	.byte	0x060,	0x003
	.byte	0x061,	0x0f8		// I/O 3f8h
	.byte	0x070,	0x004		// IRQ 4
	.byte	0x0F0,	0x000		// normal
	.byte	0x030,	0x001		// activate

//   SER2
	.byte	0x007,	0x005		// select
	.byte	0x060,	0x002
	.byte	0x061,	0x0f8		// I/O 2f8h
	.byte	0x070,	0x003		// IRQ 3
	.byte	0x0F0,	0x000		// normal
//	.byte	0x0F1,	0x002		// IR default
//	.byte	0x0F2,	0x003		// IR default
	.byte	0x030,	0x001		// activate

//   RTC
	.byte	0x007,	0x006		// select
	.byte	0x070,	0x008		// IRQ 8
	.byte	0x0F0,	0x030		// bank 0 at 70h
	.byte	0x030,	0x001		// activate

//   KBDC
	.byte	0x007,	0x007		// select
	.byte	0x070,	0x001		// IRQ 1
	.byte	0x072,	0x00C		// IRQ 12 for PS/2 mouse
	.byte	0x0F0,	0x000		// reset & gate A20 default (smg: uncommented)
	.byte	0x030,	0x001		// activate

//   Aux I/O
	.byte	0x007,	0x008		// select
	.byte	0x0C0,	0x05F		// IRQ control
	.byte	0x0D0,	0x008		// enable IRQ 1
	.byte	0x0D1,	0x008		// enable IRQ 3
	.byte	0x0D2,	0x008		// enable IRQ 4
	.byte	0x0D3,	0x008		// enable IRQ 5
	.byte	0x0D4,	0x008		// enable IRQ 6
	.byte	0x0D5,	0x008		// enable IRQ 7
	.byte	0x0D6,	0x008		// enable IRQ 8
	.byte	0x0D7,	0x008		// enable IRQ 10
	.byte	0x0CB,	0x008		// enable IRQ 11
	.byte	0x0CC,	0x008		// enable IRQ 12
	.byte	0x0CA,	0x001		// enable DRVDEN1
	.byte	0x0E1,	0x000		// GP11 output (external 27MHz)
	.byte	0x0E4,	0x000		// GP14 output (video YUV)
	.byte	0x0E5,	0x000		// GP15 output (DDC line)
	.byte	0x0E6,	0x008		// enable MTR1
	.byte	0x0E7,	0x008		// enable DS1
	.byte	0x030,	0x001    	// activate

//   ACPI
	.byte	0x007,	0x00A		// select
	.byte	0x030,  0x000    	// de-activate

SioTableEnd = .

stpc_superio0:
