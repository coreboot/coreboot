/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <northbridge/amd/agesa/state_machine.h>
#include "gpio_ftns.h"
#include <SB800.h>
#include <sb_cimx.h>

static void early_lpc_init(void)
{
	u32 mmio_base;

	/* PC Engines requires system boot when power is applied. This feature is
	 * controlled in PM_REG 5Bh register. "Always Power On" works by writing a
	 * value of 05h.
	 */
	u8 bdata = pm_read8(SB_PMIOA_REG5B);
	bdata &= 0xf8; //clear bits 0-2
	bdata |= 0x05; //set bits 0,2
	pm_write8(SB_PMIOA_REG5B, bdata);

	/* Multi-function pins switch to GPIO0-35, these pins are shared with PCI pins */
	bdata = pm_read8(SB_PMIOA_REGEA);
	bdata &= 0xfe; //clear bit 0
	bdata |= 0x01; //set bit 0
	pm_write8(SB_PMIOA_REGEA, bdata);

	//configure required GPIOs
	mmio_base = find_gpio_base();
	configure_gpio(mmio_base, GPIO_10,  GPIO_FTN_1, GPIO_OUTPUT | GPIO_DATA_HIGH);
	configure_gpio(mmio_base, GPIO_11,  GPIO_FTN_1, GPIO_OUTPUT | GPIO_DATA_HIGH);
	configure_gpio(mmio_base, GPIO_15,  GPIO_FTN_1, GPIO_INPUT);
	configure_gpio(mmio_base, GPIO_16,  GPIO_FTN_1, GPIO_INPUT);
	configure_gpio(mmio_base, GPIO_17,  GPIO_FTN_1, GPIO_INPUT);
	configure_gpio(mmio_base, GPIO_18,  GPIO_FTN_1, GPIO_INPUT);
	configure_gpio(mmio_base, GPIO_187, GPIO_FTN_1, GPIO_INPUT);
	configure_gpio(mmio_base, GPIO_189, GPIO_FTN_1, GPIO_OUTPUT | GPIO_DATA_LOW);
	configure_gpio(mmio_base, GPIO_190, GPIO_FTN_1, GPIO_OUTPUT | GPIO_DATA_LOW);
	configure_gpio(mmio_base, GPIO_191, GPIO_FTN_1, GPIO_OUTPUT | GPIO_DATA_LOW);
}

void board_BeforeAgesa(struct sysinfo *cb)
{
	early_lpc_init();
}
