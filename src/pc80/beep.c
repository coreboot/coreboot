#include <cpu/p5/io.h>
#include <delay.h>

static void
beep_on(void)
{
        /* Timer 2, Mode 3, perodic square ware */
        outb(0xb6, 0x43);

        /* frequence = 440 Hz */
        outb(0x97, 0x42);
        outb(0x0a, 0x42);

        /* Enable PC speaker gate */
        outb(0x03, 0x61);
}

static void
beep_off(void)
{
        /* Disable PC speaker gate */
        outb_p(0x00, 0x61);
}

void beep(int ms) {

	beep_on();
// delay
	mdelay(ms);
	beep_off();
}
