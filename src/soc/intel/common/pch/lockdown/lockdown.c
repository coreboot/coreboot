/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <intelblocks/cfg.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <intelpch/lockdown.h>
#include <intelblocks/gpmr.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/soc_chip.h>

/*
 * This function will get lockdown config specific to soc.
 *
 * Return values:
 *  0 = CHIPSET_LOCKDOWN_COREBOOT = Use coreboot to lockdown IPs
 *  1 = CHIPSET_LOCKDOWN_FSP = use FSP's lockdown functionality to lockdown IPs
 */
int get_lockdown_config(void)
{
	const struct soc_intel_common_config *common_config;
	common_config = chip_get_common_soc_structure();

	return common_config->chipset_lockdown;
}

static void gpmr_lockdown_cfg(void)
{
	/*
	 * GCS reg of DMI
	 *
	 * When set, prevents GCS.BBS from being changed
	 * GCS.BBS: (Boot BIOS Strap) This field determines the destination
	 * of accesses to the BIOS memory range.
	 *	Bits Description
	 *	"0b": SPI
	 *	"1b": LPC/eSPI
	 */
	gpmr_or32(GPMR_GCS, GPMR_GCS_BILD);

	/*
	 * Set Secure Register Lock (SRL) bit in DMI control register to lock
	 * DMI configuration.
	 */
	gpmr_or32(GPMR_DMICTL, GPMR_DMICTL_SRLOCK);
}

static void fast_spi_lockdown_cfg(int chipset_lockdown)
{
	if (!CONFIG(SOC_INTEL_COMMON_BLOCK_FAST_SPI))
		return;

	/* Set FAST_SPI opcode menu */
	fast_spi_set_opcode_menu();

	/* Discrete Lock Flash PR registers */
	fast_spi_pr_dlock();

	/* Check if SPI transaction is pending */
	fast_spi_cycle_in_progress();

	/* Clear any outstanding status bits like AEL, FCERR, FDONE, SAF etc. */
	fast_spi_clear_outstanding_status();

	/* Lock FAST_SPIBAR */
	fast_spi_lock_bar();

	/* Set Vendor Component Lock (VCL) */
	fast_spi_vscc0_lock();

	/* Set BIOS Interface Lock, BIOS Lock */
	if (chipset_lockdown == CHIPSET_LOCKDOWN_COREBOOT) {
		/* BIOS Interface Lock */
		fast_spi_set_bios_interface_lock_down();

		/* Only allow writes in SMM */
		if (CONFIG(BOOTMEDIA_SMM_BWP)) {
			fast_spi_set_eiss();
			fast_spi_enable_wp();
		}

		/* BIOS Lock */
		fast_spi_set_lock_enable();

		/* EXT BIOS Lock */
		fast_spi_set_ext_bios_lock_enable();
	}
}

static void lpc_lockdown_config(int chipset_lockdown)
{
	/* Set BIOS Interface Lock, BIOS Lock */
	if (chipset_lockdown == CHIPSET_LOCKDOWN_COREBOOT) {
		/* BIOS Interface Lock */
		lpc_set_bios_interface_lock_down();

		/* Only allow writes in SMM */
		if (CONFIG(BOOTMEDIA_SMM_BWP)) {
			lpc_set_eiss();
			lpc_enable_wp();
		}

		/* BIOS Lock */
		lpc_set_lock_enable();
	}
}

/*
 * platform_lockdown_config has 2 major part.
 * 1. Common SoC lockdown configuration.
 * 2. SoC specific lockdown configuration as per Silicon
 * guideline.
 */
static void platform_lockdown_config(void *unused)
{
	int chipset_lockdown;
	chipset_lockdown = get_lockdown_config();

	/* SPI lock down configuration */
	fast_spi_lockdown_cfg(chipset_lockdown);

	/* LPC/eSPI lock down configuration */
	lpc_lockdown_config(chipset_lockdown);

	/* GPMR lock down configuration */
	gpmr_lockdown_cfg();

	/* SoC lock down configuration */
	soc_lockdown_config(chipset_lockdown);
}

BOOT_STATE_INIT_ENTRY(BS_DEV_RESOURCES, BS_ON_EXIT, platform_lockdown_config,
				NULL);
