/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/helpers.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <fsp/soc_binding.h>
#include <gpio.h>
#include <soc/bootblock.h>
#include <soc/uart.h>

static uint16_t legacy_uart_ioadr_tab[] = {0x3F8, 0x2F8, 0x3E8, 0x2E8};

#define ELEM_OF_UART_TAB ARRAY_SIZE(legacy_uart_ioadr_tab)

static void pci_early_hsuart_device_probe(u8 bus, u8 dev, u8 func,
					  u32 mmio_base)
{
	register uint16_t reg16;
	pci_devfn_t uart_dev = PCI_DEV(bus, dev, func);

	/* We're using MMIO for HSUARTs. This section is needed for logging
	*  from FSP only
	*/
	/* Decode IOBASE at IOBA (BAR0). */
	reg16 = pci_read_config16(uart_dev, PCI_BASE_ADDRESS_0) | mmio_base;
	pci_write_config16(uart_dev, PCI_BASE_ADDRESS_0, reg16);

#if (CONFIG(NON_LEGACY_UART_MODE))
	/* Decode MMIO at MEMBA (BAR1) */
	pci_write_config32(uart_dev, PCI_BASE_ADDRESS_1,
			   CONFIG_CONSOLE_UART_BASE_ADDRESS +
				   SIZE_OF_HSUART_RES * func);
#endif

	/* Enable memory/io space and allow to initiate
	 * a transaction as a master
	 */
	pci_write_config16(uart_dev, PCI_COMMAND,
			   pci_read_config16(uart_dev, PCI_COMMAND) |
#if (CONFIG(NON_LEGACY_UART_MODE))
			   PCI_COMMAND_MEMORY |
#endif
				   PCI_COMMAND_MASTER | PCI_COMMAND_IO);

#if (CONFIG(CONSOLE_SERIAL_230400))
	/* Change the highest speed to 230400 */
	uint32_t *psr_reg =
		(uint32_t *)(CONFIG_CONSOLE_UART_BASE_ADDRESS +
			     SIZE_OF_HSUART_RES * func + PSR_OFFSET);
	*psr_reg >>= 1;
#endif
}

static void early_config_gpio(void)
{
	uint32_t reg32;

	// HSUART0:
	// UART0_RXD
	reg32 = read32(
		(void *)PCH_PCR_ADDRESS(PID_GPIOCOM1, R_PAD_CFG_DW0_UART0_RXD));
	if (((reg32 & B_PCH_GPIO_PAD_MODE) >> N_PCH_GPIO_PAD_MODE) !=
	    V_PCH_GPIO_PAD_MODE_NAT_1) {
		reg32 &= ~B_PCH_GPIO_PAD_MODE;
		reg32 |= (UINT32)(V_PCH_GPIO_PAD_MODE_NAT_1
				  << N_PCH_GPIO_PAD_MODE);
		write32((void *)PCH_PCR_ADDRESS(PID_GPIOCOM1,
						R_PAD_CFG_DW0_UART0_RXD),
			reg32);
	}
	// UART0_TXD
	reg32 = read32(
		(void *)PCH_PCR_ADDRESS(PID_GPIOCOM1, R_PAD_CFG_DW0_UART0_TXD));
	if (((reg32 & B_PCH_GPIO_PAD_MODE) >> N_PCH_GPIO_PAD_MODE) !=
	    V_PCH_GPIO_PAD_MODE_NAT_1) {
		reg32 &= ~B_PCH_GPIO_PAD_MODE;
		reg32 |= (UINT32)(V_PCH_GPIO_PAD_MODE_NAT_1
				  << N_PCH_GPIO_PAD_MODE);
		write32((void *)PCH_PCR_ADDRESS(PID_GPIOCOM1,
						R_PAD_CFG_DW0_UART0_TXD),
			reg32);
	}
	// UART0_CTS
	reg32 = read32((void *)PCH_PCR_ADDRESS(PID_GPIOCOM1,
					       R_PAD_CFG_DW0_SMB3_CLTT_CLK));
	if (((reg32 & B_PCH_GPIO_PAD_MODE) >> N_PCH_GPIO_PAD_MODE) !=
	    V_PCH_GPIO_PAD_MODE_NAT_2) {
		reg32 &= ~B_PCH_GPIO_PAD_MODE;
		reg32 |= (UINT32)(V_PCH_GPIO_PAD_MODE_NAT_2
				  << N_PCH_GPIO_PAD_MODE);
		write32((void *)PCH_PCR_ADDRESS(PID_GPIOCOM1,
						R_PAD_CFG_DW0_SMB3_CLTT_CLK),
			reg32);
	}
	// UART0_RTS
	reg32 = read32((void *)PCH_PCR_ADDRESS(PID_GPIOCOM1,
					       R_PAD_CFG_DW0_PCIE_CLKREQ5_N));
	if (((reg32 & B_PCH_GPIO_PAD_MODE) >> N_PCH_GPIO_PAD_MODE) !=
	    V_PCH_GPIO_PAD_MODE_NAT_3) {
		reg32 &= ~B_PCH_GPIO_PAD_MODE;
		reg32 |= (UINT32)(V_PCH_GPIO_PAD_MODE_NAT_3
				  << N_PCH_GPIO_PAD_MODE);
		write32((void *)PCH_PCR_ADDRESS(PID_GPIOCOM1,
						R_PAD_CFG_DW0_PCIE_CLKREQ5_N),
			reg32);
	}

	// HSUART1:
	// UART1_RXD
	reg32 = read32(
		(void *)PCH_PCR_ADDRESS(PID_GPIOCOM1, R_PAD_CFG_DW0_UART1_RXD));
	if (((reg32 & B_PCH_GPIO_PAD_MODE) >> N_PCH_GPIO_PAD_MODE) !=
	    V_PCH_GPIO_PAD_MODE_NAT_1) {
		reg32 &= ~B_PCH_GPIO_PAD_MODE;
		reg32 |= (UINT32)(V_PCH_GPIO_PAD_MODE_NAT_1
				  << N_PCH_GPIO_PAD_MODE);
		write32((void *)PCH_PCR_ADDRESS(PID_GPIOCOM1,
						R_PAD_CFG_DW0_UART1_RXD),
			reg32);
	}
	// UART1_TXD
	reg32 = read32(
		(void *)PCH_PCR_ADDRESS(PID_GPIOCOM1, R_PAD_CFG_DW0_UART1_TXD));
	if (((reg32 & B_PCH_GPIO_PAD_MODE) >> N_PCH_GPIO_PAD_MODE) !=
	    V_PCH_GPIO_PAD_MODE_NAT_1) {
		reg32 &= ~B_PCH_GPIO_PAD_MODE;
		reg32 |= (UINT32)(V_PCH_GPIO_PAD_MODE_NAT_1
				  << N_PCH_GPIO_PAD_MODE);
		write32((void *)PCH_PCR_ADDRESS(PID_GPIOCOM1,
						R_PAD_CFG_DW0_UART1_TXD),
			reg32);
	}
	// UART1_CTS
	reg32 = read32((void *)PCH_PCR_ADDRESS(PID_GPIOCOM1,
					       R_PAD_CFG_DW0_SATA1_SDOUT));
	if (((reg32 & B_PCH_GPIO_PAD_MODE) >> N_PCH_GPIO_PAD_MODE) !=
	    V_PCH_GPIO_PAD_MODE_NAT_1) {
		reg32 &= ~B_PCH_GPIO_PAD_MODE;
		reg32 |= (UINT32)(V_PCH_GPIO_PAD_MODE_NAT_1
				  << N_PCH_GPIO_PAD_MODE);
		write32((void *)PCH_PCR_ADDRESS(PID_GPIOCOM1,
						R_PAD_CFG_DW0_SATA1_SDOUT),
			reg32);
	}
	// UART1_RTS
	reg32 = read32((void *)PCH_PCR_ADDRESS(PID_GPIOCOM1,
					       R_PAD_CFG_DW0_SATA0_SDOUT));
	if (((reg32 & B_PCH_GPIO_PAD_MODE) >> N_PCH_GPIO_PAD_MODE) !=
	    V_PCH_GPIO_PAD_MODE_NAT_1) {
		reg32 &= ~B_PCH_GPIO_PAD_MODE;
		reg32 |= (UINT32)(V_PCH_GPIO_PAD_MODE_NAT_1
				  << N_PCH_GPIO_PAD_MODE);
		write32((void *)PCH_PCR_ADDRESS(PID_GPIOCOM1,
						R_PAD_CFG_DW0_SATA0_SDOUT),
			reg32);
	}
}

void early_uart_init(void)
{
	register int i;

	/* Check: do we have enough elements to init. ? */
	BUILD_BUG_ON(DENVERTON_UARTS_TO_INI > ELEM_OF_UART_TAB);

	/* HSUART(B0:D26:0-1) GPIO init. */
	early_config_gpio();

	for (i = DENVERTON_UARTS_TO_INI - 1; i >= 0; --i) {
		pci_early_hsuart_device_probe(0, CONFIG_HSUART_DEV, i,
					      legacy_uart_ioadr_tab[i]);
	}
}
