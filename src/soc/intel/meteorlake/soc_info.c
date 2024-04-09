/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pci_devs.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <soc/soc_info.h>
#include <intelblocks/tcss.h>

uint8_t get_max_usb20_port(void)
{
	return CONFIG_SOC_INTEL_USB2_DEV_MAX;
}

uint8_t get_max_usb30_port(void)
{
	return CONFIG_SOC_INTEL_USB3_DEV_MAX;
}

uint8_t get_max_tcss_port(void)
{
	return MAX_TYPE_C_PORTS;
}

uint8_t get_max_tbt_pcie_port(void)
{
	return CONFIG_MAX_TBT_ROOT_PORTS;
}

uint8_t get_max_pcie_port(void)
{
	return CONFIG_MAX_ROOT_PORTS;
}

uint8_t get_max_pcie_clock(void)
{
	return CONFIG_MAX_PCIE_CLOCK_SRC;
}

uint8_t get_max_uart_port(void)
{
	return CONFIG_SOC_INTEL_UART_DEV_MAX;
}

uint8_t get_max_i2c_port(void)
{
	return CONFIG_SOC_INTEL_I2C_DEV_MAX;
}

uint8_t get_max_gspi_port(void)
{
	return CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX;
}
