/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pci_devs.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <soc/soc_info.h>
#include <intelblocks/tcss.h>
#include <console/console.h>

uint8_t get_max_usb20_port(void)
{
	uint8_t usb20_port = CONFIG_SOC_INTEL_USB2_DEV_MAX;
	printk(BIOS_DEBUG, "soc_info: max_usb20_port:%d\n", usb20_port);
	return usb20_port;
}

uint8_t get_max_usb30_port(void)
{
	uint8_t usb30_port = CONFIG_SOC_INTEL_USB3_DEV_MAX;
	printk(BIOS_DEBUG, "soc_info: max_usb30_port:%d\n", usb30_port);
	return usb30_port;
}

uint8_t get_max_tcss_port(void)
{
	uint8_t tcss_port = MAX_TYPE_C_PORTS;
	printk(BIOS_DEBUG, "soc_info: tcss_port:%d\n", tcss_port);
	return tcss_port;
}

uint8_t get_max_tbt_pcie_port(void)
{
	uint8_t tbt_pcie_port = CONFIG_MAX_TBT_ROOT_PORTS;
	printk(BIOS_DEBUG, "soc_info: max_tbt_pcie_port:%d\n", tbt_pcie_port);
	return tbt_pcie_port;
}

uint8_t get_max_pcie_port(void)
{
	uint8_t pcie_port = CONFIG_MAX_ROOT_PORTS;
	printk(BIOS_DEBUG, "soc_info: max_pcie_port:%d\n", pcie_port);
	return pcie_port;
}

uint8_t get_max_pcie_clock(void)
{
	uint8_t pcie_clock = CONFIG_MAX_PCIE_CLOCK_SRC;
	printk(BIOS_DEBUG, "soc_info: max_pcie_clock:%d\n", pcie_clock);
	return pcie_clock;
}

uint8_t get_max_uart_port(void)
{
	uint8_t uart_port = CONFIG_SOC_INTEL_UART_DEV_MAX;
	printk(BIOS_DEBUG, "soc_info: max_uart_port:%d\n", uart_port);
	return uart_port;
}

uint8_t get_max_i2c_port(void)
{
	uint8_t i2c_port = CONFIG_SOC_INTEL_I2C_DEV_MAX;
	printk(BIOS_DEBUG, "soc_info: max_i2c_port:%d\n", i2c_port);
	return i2c_port;
}

uint8_t get_max_gspi_port(void)
{
	uint8_t gspi_port = CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX;
	printk(BIOS_DEBUG, "soc_info: max_gspi_port:%d\n", gspi_port);
	return gspi_port;
}
