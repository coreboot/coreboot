#include <stdint.h>
#include "../../../northbridge/ibm/cpc710/cpc710.h"

/*
 * Bus clock jumper settings on SIOR0 27:28
 */
static uint32_t BusClockSpeed[] = {
	66000000,	/* 00 */
	83000000,	/* 01 */
	100000000,	/* 10 */
	133000000	/* 11 */
};

/*
 * Timer frequency is 1/4 of the bus clock frequency.
 *
 * For the briQ, bits 27:28 of SIOR0 encode bus clock frequency.
 */
unsigned long 
get_timer_freq(void)
{
	uint32_t sior0 = getCPC710(CPC710_SDRAM0_SIOR0);

	return BusClockSpeed[(sior0 >> 3) & 0x2] / 4;
}

/*
 * Frequency of PCI bus.
 * 
 * For the briQ, bit 29 of SIOR0 is 66MHz enable (active low).
 */
unsigned long 
get_pci_bus_freq(void)
{
	uint32_t sior0 = getCPC710(CPC710_SDRAM0_SIOR0);

	if (sior0 & 0x4 == 0x4)
		return 33000000;

	return 66000000;
}

