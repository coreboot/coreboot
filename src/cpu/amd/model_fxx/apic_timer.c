#include <stdint.h>
#include <delay.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>

void init_timer(void)
{
	/* Set the apic timer to no interrupts and periodic mode */
	lapic_write(LAPIC_LVTT, (1 << 17)|(1<< 16)|(0 << 12)|(0 << 0));

	/* Set the divider to 1, no divider */
	lapic_write(LAPIC_TDCR, LAPIC_TDR_DIV_1);

	/* Set the initial counter to 0xffffffff */
	lapic_write(LAPIC_TMICT, 0xffffffff);

}

void udelay(unsigned usecs)
{
	uint32_t start, value, ticks;
	/* Calculate the number of ticks to run, our FSB runs a 200Mhz */
	ticks = usecs * 200;
	start = lapic_read(LAPIC_TMCCT);
	do {
		value = lapic_read(LAPIC_TMCCT);
	} while((start - value) < ticks);
	
}
