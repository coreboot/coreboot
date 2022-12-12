/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/uart.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <southbridge/intel/lynxpoint/iobp.h>
#include <southbridge/intel/lynxpoint/lp_gpio.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <types.h>

static pci_devfn_t get_uart_pci_device(void)
{
	switch (CONFIG_UART_FOR_CONSOLE) {
		case 0:  return PCI_DEV(0, 0x15, 5);
		case 1:  return PCI_DEV(0, 0x15, 6);
		default: return dead_code_t(pci_devfn_t);
	}
}

/* TODO: Figure out if all steps are actually necessary */
void uart_bootblock_init(void)
{
	const pci_devfn_t dev = get_uart_pci_device();

	/* Program IOBP GPIODF */
	pch_iobp_update(SIO_IOBP_GPIODF, ~0x131f, 0x131f);

	/* Program IOBP CB000180h[5:0] = 111111b (undefined register) */
	pch_iobp_update(0xcb000180, ~0x0000003f, 0x0000003f);

	/* Set and enable MMIO BAR */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, CONFIG_CONSOLE_UART_BASE_ADDRESS);
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MEMORY);

	void *const bar = (void *)(uintptr_t)CONFIG_CONSOLE_UART_BASE_ADDRESS;

	/* Initialize LTR */
	clrbits32(bar + SIO_REG_PPR_GEN, SIO_REG_PPR_GEN_LTR_MODE_MASK);
	clrbits32(bar + SIO_REG_PPR_RST, SIO_REG_PPR_RST_ASSERT);

	/* Take UART out of reset */
	setbits32(bar + SIO_REG_PPR_RST, SIO_REG_PPR_RST_ASSERT);

	/* Set M and N divisor inputs and enable clock */
	uint32_t ppr_clock = 0;
	ppr_clock |= SIO_REG_PPR_CLOCK_EN;
	ppr_clock |= SIO_REG_PPR_CLOCK_UPDATE;
	ppr_clock |= SIO_REG_PPR_CLOCK_N_DIV << 16;
	ppr_clock |= SIO_REG_PPR_CLOCK_M_DIV << 1;
	write32(bar + SIO_REG_PPR_CLOCK, ppr_clock);
}
