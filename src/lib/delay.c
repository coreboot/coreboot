#include <delay.h>
#include <arch/io.h>

/* Ports for the 8254 timer chip */
#define	TIMER2_PORT	0x42
#define	TIMER_MODE_PORT	0x43

/* Meaning of the mode bits */
#define	TIMER0_SEL	0x00
#define	TIMER1_SEL	0x40
#define	TIMER2_SEL	0x80
#define	READBACK_SEL	0xC0

#define	LATCH_COUNT	0x00
#define	LOBYTE_ACCESS	0x10
#define	HIBYTE_ACCESS	0x20
#define	WORD_ACCESS	0x30

#define	MODE0		0x00
#define	MODE1		0x02
#define	MODE2		0x04
#define	MODE3		0x06
#define	MODE4		0x08
#define	MODE5		0x0A

#define	BINARY_COUNT	0x00
#define	BCD_COUNT	0x01

/* Timers tick over at this rate */
#define	TICKS_PER_MS	1193

/* Parallel Peripheral Controller Port B */
#define	PPC_PORTB	0x61

/* Meaning of the port bits */
#define	PPCB_T2OUT	0x20	/* Bit 5 */
#define	PPCB_SPKR	0x02	/* Bit 1 */
#define	PPCB_T2GATE	0x01	/* Bit 0 */


static void load_timer2(unsigned int ticks)
{
	/* Set up the timer gate, turn off the speaker */
	outb((inb(PPC_PORTB) & ~PPCB_SPKR) | PPCB_T2GATE, PPC_PORTB);
	outb(TIMER2_SEL|WORD_ACCESS|MODE0|BINARY_COUNT, TIMER_MODE_PORT);
	outb(ticks & 0xFF, TIMER2_PORT);
	outb(ticks >> 8, TIMER2_PORT);
}


void udelay(int usecs)
{
	load_timer2((usecs*TICKS_PER_MS)/1000);
	while ((inb(PPC_PORTB) & PPCB_T2OUT) == 0)
		;
}
void mdelay(int msecs)
{
	int i;
	for(i = 0; i < msecs; i++) {
		udelay(1000);
	}
}
