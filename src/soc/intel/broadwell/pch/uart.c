/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_def.h>
#include <reg_script.h>
#include <stdint.h>
#include <uart8250.h>
#include <soc/iobp.h>
#include <soc/serialio.h>

const struct reg_script uart_init[] = {
	/* Set MMIO BAR */
	REG_PCI_WRITE32(PCI_BASE_ADDRESS_0, CONFIG_TTYS0_BASE),
	/* Enable Memory access and Bus Master */
	REG_PCI_OR32(PCI_COMMAND, PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER),
	/* Initialize LTR */
	REG_MMIO_RMW32(CONFIG_TTYS0_BASE + SIO_REG_PPR_GEN,
		      ~SIO_REG_PPR_GEN_LTR_MODE_MASK, 0),
	REG_MMIO_RMW32(CONFIG_TTYS0_BASE + SIO_REG_PPR_RST,
		      ~(SIO_REG_PPR_RST_ASSERT), 0),
	/* Take UART out of reset */
	REG_MMIO_OR32(CONFIG_TTYS0_BASE + SIO_REG_PPR_RST,
		     SIO_REG_PPR_RST_ASSERT),
	/* Set M and N divisor inputs and enable clock */
	REG_MMIO_WRITE32(CONFIG_TTYS0_BASE + SIO_REG_PPR_CLOCK,
			SIO_REG_PPR_CLOCK_EN | SIO_REG_PPR_CLOCK_UPDATE |
			(SIO_REG_PPR_CLOCK_N_DIV << 16) |
			(SIO_REG_PPR_CLOCK_M_DIV << 1)),
	REG_SCRIPT_END
};

void pch_uart_init(void)
{
	/* Program IOBP CB000154h[12,9:8,4:0] = 1001100011111b */
	u32 gpiodf = 0x131f;
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t dev;
#else
	struct device *dev;
#endif

	/* Put UART in byte access mode for 16550 compatibility */
	switch (CONFIG_INTEL_PCH_UART_CONSOLE_NUMBER) {
	case 0:
		dev = PCH_DEV_UART0;
		gpiodf |= SIO_IOBP_GPIODF_UART0_BYTE_ACCESS;
		break;
	case 1:
		dev = PCH_DEV_UART1;
		gpiodf |= SIO_IOBP_GPIODF_UART1_BYTE_ACCESS;
		break;
	default:
		return;
	}

	/* Program IOBP GPIODF */
	pch_iobp_update(SIO_IOBP_GPIODF, ~gpiodf, gpiodf);

	/* Program IOBP CB000180h[5:0] = 111111b (undefined register) */
	pch_iobp_update(0xcb000180, ~0x0000003f, 0x0000003f);

	/* Initialize chipset uart interface */
	reg_script_run_on_dev(dev, uart_init);

	/*
	 * Perform standard UART initialization
	 * Divisor 1 is 115200 BAUD
	 */
	uart8250_mem_init(CONFIG_TTYS0_BASE, 1);
}
