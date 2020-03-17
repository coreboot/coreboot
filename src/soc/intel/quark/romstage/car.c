/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <fsp/util.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <soc/reg_access.h>

static const struct reg_script legacy_gpio_init[] = {
	/* Temporarily enable the legacy GPIO controller */
	REG_PCI_WRITE32(R_QNC_LPC_GBA_BASE, IO_ADDRESS_VALID
		| LEGACY_GPIO_BASE_ADDRESS),
	/* Temporarily enable the GPE controller */
	REG_PCI_WRITE32(R_QNC_LPC_GPE0BLK, IO_ADDRESS_VALID
		| GPE0_BASE_ADDRESS),
	REG_PCI_OR8(PCI_COMMAND, PCI_COMMAND_IO),
	REG_SCRIPT_END
};

static const struct reg_script i2c_gpio_controller_init[] = {
	/* Temporarily enable the GPIO controller */
	REG_PCI_WRITE32(PCI_BASE_ADDRESS_0, I2C_BASE_ADDRESS),
	REG_PCI_WRITE32(PCI_BASE_ADDRESS_1, GPIO_BASE_ADDRESS),
	REG_PCI_OR8(PCI_COMMAND, PCI_COMMAND_MEMORY),
	REG_SCRIPT_END
};

static const struct reg_script hsuart_init[] = {
	/* Enable the HSUART */
	REG_PCI_WRITE32(PCI_BASE_ADDRESS_0, UART_BASE_ADDRESS),
	REG_PCI_OR8(PCI_COMMAND, PCI_COMMAND_MEMORY),
	REG_SCRIPT_END
};

void car_soc_pre_console_init(void)
{
	/* Initialize the controllers */
	reg_script_run_on_dev(I2CGPIO_BDF, i2c_gpio_controller_init);
	reg_script_run_on_dev(LPC_BDF, legacy_gpio_init);

	/* Enable the HSUART */
	if (CONFIG(ENABLE_BUILTIN_HSUART0))
		reg_script_run_on_dev(HSUART0_BDF, hsuart_init);
	if (CONFIG(ENABLE_BUILTIN_HSUART1))
		reg_script_run_on_dev(HSUART1_BDF, hsuart_init);
}

void car_soc_post_console_init(void)
{
	report_platform_info();
};
