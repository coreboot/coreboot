#include <stdint.h>
#include <delay.h>
#include <cpu/p6/msr.h>
#include <cpu/p6/apic.h>

void init_timer(void)
{
	/* Set the apic timer to no interrupts and periodic mode */
	apic_write(APIC_LVTT, (1 << 17)|(1<< 16)|(0 << 12)|(0 << 0));
	/* Set the divider to 1, no divider */
	apic_write(APIC_TDCR, APIC_TDR_DIV_1);
	/* Set the initial counter to 0xffffffff */
	apic_write(APIC_TMICT, 0xffffffff);
}

void udelay(unsigned usecs)
{
	uint32_t start, value, ticks;
	/* Calculate the number of ticks to run, our FSB runs a 200Mhz */
	ticks = usecs * 200;
	start = apic_read(APIC_TMCCT);
	do {
		value = apic_read(APIC_TMCCT);
	} while((start - value) < ticks);
	
}
