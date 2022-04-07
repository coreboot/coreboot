/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/gspi.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/p2sb.h>
#include <intelblocks/pcr.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/rtc.h>
#include <soc/bootblock.h>
#include <soc/iomap.h>
#include <soc/p2sb.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>

#define PCR_PSF3_TO_SHDW_PMC_REG_BASE	0x0600
#define PCR_PSFX_TO_SHDW_BAR0	0
#define PCR_PSFX_TO_SHDW_BAR1	0x4
#define PCR_PSFX_TO_SHDW_BAR2	0x8
#define PCR_PSFX_TO_SHDW_BAR3	0xC
#define PCR_PSFX_TO_SHDW_BAR4	0x10
#define PCR_PSFX_TO_SHDW_PCIEN_IOEN	0x01
#define PCR_PSFX_T0_SHDW_PCIEN	0x1C

static void soc_config_pwrmbase(void)
{
	/*
	 * Assign Resources to PWRMBASE
	 * Clear BIT 1-2 Command Register
	 */
	pci_and_config16(PCH_DEV_PMC, PCI_COMMAND, ~(PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER));

	/* Program PWRM Base */
	pci_write_config32(PCH_DEV_PMC, PWRMBASE, PCH_PWRM_BASE_ADDRESS);

	/* Enable Bus Master and MMIO Space */
	pci_or_config16(PCH_DEV_PMC, PCI_COMMAND, (PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER));

	/* Enable PWRM in PMC */
	setbits32((void *) PCH_PWRM_BASE_ADDRESS + ACTL, PWRM_EN);
}

void bootblock_pch_early_init(void)
{
	/*
	 * Perform P2SB configuration before any another controller initialization as the
	 * controller might want to perform PCR settings.
	 */
	p2sb_enable_bar();
	p2sb_configure_hpet();

	fast_spi_early_init(SPI_BASE_ADDRESS);
	gspi_early_bar_init();

	/*
	 * Enabling PWRM Base for accessing
	 * Global Reset Cause Register.
	 */
	soc_config_pwrmbase();
}

static void soc_config_acpibase(void)
{
	uint32_t pmc_reg_value;

	pmc_reg_value = pcr_read32(PID_PSF3, PCR_PSF3_TO_SHDW_PMC_REG_BASE +
						PCR_PSFX_TO_SHDW_BAR4);

	if (pmc_reg_value != 0xFFFFFFFF) {
		/* Disable Io Space before changing the address */
		pcr_rmw32(PID_PSF3, PCR_PSF3_TO_SHDW_PMC_REG_BASE +
				PCR_PSFX_T0_SHDW_PCIEN,
				~PCR_PSFX_TO_SHDW_PCIEN_IOEN, 0);
		/* Program ABASE in PSF3 PMC space BAR4*/
		pcr_write32(PID_PSF3, PCR_PSF3_TO_SHDW_PMC_REG_BASE +
				PCR_PSFX_TO_SHDW_BAR4,
				ACPI_BASE_ADDRESS);
		/* Enable IO Space */
		pcr_rmw32(PID_PSF3, PCR_PSF3_TO_SHDW_PMC_REG_BASE +
				PCR_PSFX_T0_SHDW_PCIEN,
				~0, PCR_PSFX_TO_SHDW_PCIEN_IOEN);
	}
}

void pch_early_iorange_init(void)
{
	uint16_t io_enables = LPC_IOE_SUPERIO_2E_2F | LPC_IOE_KBC_60_64 |
		LPC_IOE_EC_62_66 | LPC_IOE_LGE_200;

	/* IO Decode Range */
	if (CONFIG(DRIVERS_UART_8250IO))
		lpc_io_setup_comm_a_b();

	/* IO Decode Enable */
	lpc_enable_fixed_io_ranges(io_enables);

	/* Program generic IO Decode Range */
	pch_enable_lpc();
}

void bootblock_pch_init(void)
{
	/*
	 * Enabling ABASE for accessing PM1_STS, PM1_EN, PM1_CNT,
	 * GPE0_STS, GPE0_EN registers.
	 */
	soc_config_acpibase();

	/* Set up GPE configuration */
	pmc_gpe_init();

	enable_rtc_upper_bank();
}
