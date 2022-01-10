/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <amdblocks/lpc.h>
#include <device/pci_ops.h>
#include <soc/espi.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include <types.h>

void espi_disable_lpc_ldrq(void)
{
	/* Beware that the bit definitions for LPC_LDRQ0_PU_EN and LPC_LDRQ0_PD_EN are swapped
	   on Picasso and older compared to Renoir/Cezanne and newer */
	uint32_t dword = pci_read_config32(SOC_LPC_DEV, LPC_MISC_CONTROL_BITS);
	dword &= ~(LPC_LDRQ0_PU_EN | LPC_LDRQ1_EN | LPC_LDRQ0_EN);
	dword |= LPC_LDRQ0_PD_EN;
	pci_write_config32(SOC_LPC_DEV, LPC_MISC_CONTROL_BITS, dword);
}

void espi_switch_to_spi2_pads(void)
{
	/* Use SPI2 pins for eSPI */
	uint32_t dword = pm_read32(PM_SPI_PAD_PU_PD);
	dword |= PM_ESPI_CS_USE_DATA2;
	pm_write32(PM_SPI_PAD_PU_PD, dword);

	/* Switch the pads that can be used as either LPC or secondary eSPI to 1.8V mode */
	dword = pm_read32(PM_ACPI_CONF);
	dword |= PM_ACPI_S5_LPC_PIN_MODE | PM_ACPI_S5_LPC_PIN_MODE_SEL;
	pm_write32(PM_ACPI_CONF, dword);
}
