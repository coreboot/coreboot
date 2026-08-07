/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/psp.h>
#include <amdblocks/smi.h>
#include <amdblocks/smm.h>
#include <amdblocks/spi.h>
#include <arch/hlt.h>
#include <console/cbmem_console.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/smm.h>
#include <elog.h>
#include <psp_verstage/psp_transfer.h>
#include <soc/smi.h>
#include <soc/smu.h>
#include <soc/southbridge.h>
#include <types.h>

void fch_slp_typ_handler(void)
{
	uint32_t pci_ctrl, reg32;
	uint16_t pm1cnt, reg16;
	uint8_t slp_typ, rst_ctrl;

	/* Figure out SLP_TYP */
	pm1cnt = acpi_read16(MMIO_ACPI_PM1_CNT_BLK);
	printk(BIOS_SPEW, "SMI#: SLP = 0x%04x\n", pm1cnt);
	slp_typ = acpi_sleep_from_pm1(pm1cnt);

	/* Do any mainboard sleep handling */
	mainboard_smi_sleep(slp_typ);

	switch (slp_typ) {
	case ACPI_S0:
		printk(BIOS_DEBUG, "SMI#: Entering S0 (On)\n");
		break;
	case ACPI_S3:
		printk(BIOS_DEBUG, "SMI#: Entering S3 (Suspend-To-RAM)\n");
		break;
	case ACPI_S4:
		printk(BIOS_DEBUG, "SMI#: Entering S4 (Suspend-To-Disk)\n");
		break;
	case ACPI_S5:
		printk(BIOS_DEBUG, "SMI#: Entering S5 (Soft Power off)\n");
		break;
	default:
		printk(BIOS_DEBUG, "SMI#: ERROR: SLP_TYP reserved\n");
		break;
	}

	if (slp_typ >= ACPI_S3) {
		/* Sleep Type Elog S3, S4, and S5 entry */
		elog_gsmi_add_event_byte(ELOG_TYPE_ACPI_ENTER, slp_typ);

		wbinvd();

		clear_all_smi_status();

		/* Do not send SMI before AcpiPm1CntBlkx00[SlpTyp] */
		pci_ctrl = pm_read32(PM_PCI_CTRL);
		pci_ctrl &= ~FORCE_SLPSTATE_RETRY;
		if (CONFIG(SOC_AMD_COMMON_BLOCK_SMI_STPCLK_RETRY))
			pci_ctrl |= FORCE_STPCLK_RETRY;
		pm_write32(PM_PCI_CTRL, pci_ctrl);

		/* Enable SlpTyp */
		rst_ctrl = pm_read8(PM_RST_CTRL1);
		rst_ctrl |= SLPTYPE_CONTROL_EN;
		pm_write8(PM_RST_CTRL1, rst_ctrl);

		/*
		 * Before the final command, check if there's pending wake
		 * event. Read enable first, so that reading the actual status
		 * is as close as possible to entering S3. The idea is to
		 * minimize the opportunity for a wake event to happen before
		 * actually entering S3. If there's a pending wake event, log
		 * it and continue normal path. S3 will fail and the wake event
		 * becomes a SCI.
		 */
		if (CONFIG(ELOG_GSMI)) {
			reg16 = acpi_read16(MMIO_ACPI_PM1_EN);
			reg16 &= acpi_read16(MMIO_ACPI_PM1_STS);
			if (reg16)
				elog_add_extended_event(
						ELOG_SLEEP_PENDING_PM1_WAKE,
						(u32)reg16);

			reg32 = acpi_read32(MMIO_ACPI_GPE0_EN);
			reg32 &= acpi_read32(MMIO_ACPI_GPE0_STS);
			if (reg32)
				elog_add_extended_event(
						ELOG_SLEEP_PENDING_GPE0_WAKE,
						reg32);
		}

		/*
		 * smu_sx_entry() uses SMN and clobbers the SMN index register.
		 * psp_notify_sx_info() uses the PSP MMIO mailbox and does not.
		 * Since this is the last thing run before entering S3, the SMN
		 * clobber does not interfere with other indirect SMN accesses.
		 */
		if (slp_typ == ACPI_S3) {
			if (CONFIG(SOC_AMD_COMMON_BLOCK_SMI_PSP_SX_NOTIFY))
				psp_notify_sx_info(ACPI_S3);

			if (CONFIG(SOC_AMD_COMMON_BLOCK_SMI_USB_S3_ENTRY))
				soc_smi_usb_s3_entry();
		}

		smu_sx_entry(); /* Leave SlpTypeEn clear, SMU will set */
		printk(BIOS_ERR, "System did not go to sleep\n");
		hlt();
	}
}

/*
 * Table of functions supported in the SMI handler.  Note that SMI source setup
 * in fch.c is unrelated to this list.
 */
static const struct smi_sources_t smi_sources[] = {
	{ .type = SMITYPE_SMI_CMD_PORT, .handler = fch_apmc_smi_handler },
	{ .type = SMITYPE_SLP_TYP, .handler = fch_slp_typ_handler },
	{ .type = SMITYPE_PWRBUTTON_UP, .handler = fch_power_button_handler },
	{ .type = SMITYPE_PSP, .handler = psp_smi_handler },
};

void *get_smi_source_handler(int source)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(smi_sources); i++)
		if (smi_sources[i].type == source)
			return smi_sources[i].handler;

	return NULL;
}

void smm_soc_early_init(void)
{
	/*
	 * Replay the transfer buffer console to the SMM console buffer, so
	 * that SMM console output isn't attributed to the verstage that ran
	 * on the PSP.
	 */
	if (CONFIG(SOC_AMD_COMMON_BLOCK_SMM_CBMEMC_REPLAY) &&
	    CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK) && __CBMEM_CONSOLE_ENABLE__)
		replay_transfer_buffer_cbmemc();

	if (CONFIG(SPI_FLASH_SMM))
		fch_spi_backup_registers();
}

void smm_soc_exit(void)
{
	if (CONFIG(SPI_FLASH_SMM))
		fch_spi_restore_registers();
}
