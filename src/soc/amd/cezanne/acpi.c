/* SPDX-License-Identifier: GPL-2.0-only */

/* ACPI - create the Fixed ACPI Description Tables (FADT) */

#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/ioapic.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <soc/acpi.h>
#include <soc/iomap.h>
#include <types.h>
#include "chip.h"

unsigned long acpi_fill_madt(unsigned long current)
{
	/* create all subtables for processors */
	current = acpi_create_madt_lapics(current);

	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current,
		FCH_IOAPIC_ID, IO_APIC_ADDR, 0);

	/* TODO: Add GNB-IOAPIC */

	current += acpi_create_madt_irqoverride(
		(acpi_madt_irqoverride_t *)current,
		MP_BUS_ISA, 0, 2,
		MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT);
	current += acpi_create_madt_irqoverride(
		(acpi_madt_irqoverride_t *)current,
		MP_BUS_ISA, ACPI_SCI_IRQ, ACPI_SCI_IRQ,
		MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW);

	return current;
}

/*
 * Reference section 5.2.9 Fixed ACPI Description Table (FADT)
 * in the ACPI 3.0b specification.
 */
void acpi_fill_fadt(acpi_fadt_t *fadt)
{
	const struct soc_amd_common_config *cfg = soc_get_common_config();

	printk(BIOS_DEBUG, "pm_base: 0x%04x\n", ACPI_IO_BASE);

	fadt->sci_int = ACPI_SCI_IRQ;

	if (permanent_smi_handler()) {
		fadt->smi_cmd = APM_CNT;
		fadt->acpi_enable = APM_CNT_ACPI_ENABLE;
		fadt->acpi_disable = APM_CNT_ACPI_DISABLE;
	}

	fadt->pstate_cnt = 0;

	fadt->pm1a_evt_blk = ACPI_PM_EVT_BLK;
	fadt->pm1a_cnt_blk = ACPI_PM1_CNT_BLK;
	fadt->pm_tmr_blk = ACPI_PM_TMR_BLK;
	fadt->gpe0_blk = ACPI_GPE0_BLK;

	fadt->pm1_evt_len = 4;	/* 32 bits */
	fadt->pm1_cnt_len = 2;	/* 16 bits */
	fadt->pm_tmr_len = 4;	/* 32 bits */
	fadt->gpe0_blk_len = 8;	/* 64 bits */

	fadt->p_lvl2_lat = ACPI_FADT_C2_NOT_SUPPORTED;
	fadt->p_lvl3_lat = ACPI_FADT_C3_NOT_SUPPORTED;
	fadt->duty_offset = 0;	/* Not supported */
	fadt->duty_width = 0;	/* Not supported */
	fadt->day_alrm = RTC_DATE_ALARM;
	fadt->mon_alrm = 0;
	fadt->century = RTC_ALT_CENTURY;
	fadt->iapc_boot_arch = cfg->fadt_boot_arch; /* legacy free default */
	fadt->flags |=	ACPI_FADT_WBINVD | /* See table 5-34 ACPI 6.3 spec */
			ACPI_FADT_C1_SUPPORTED |
			ACPI_FADT_S4_RTC_WAKE |
			ACPI_FADT_32BIT_TIMER |
			ACPI_FADT_PCI_EXPRESS_WAKE |
			ACPI_FADT_PLATFORM_CLOCK |
			ACPI_FADT_S4_RTC_VALID |
			ACPI_FADT_REMOTE_POWER_ON;
	fadt->flags |= cfg->fadt_flags; /* additional board-specific flags */

	/*
	 * The Cezanne PPR defines the ACPI registers starting at PMx00000500. This translates
	 * to 0x300 + 0x500 = 0x800 which is identical to acpimmio_acpi.
	 */
	fadt->x_pm1a_evt_blk.space_id = ACPI_ADDRESS_SPACE_MEMORY;
	fadt->x_pm1a_evt_blk.bit_width = 32;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;
	fadt->x_pm1a_evt_blk.addrl = (u32)acpimmio_acpi + MMIO_ACPI_PM1_EVT_BLK;
	fadt->x_pm1a_evt_blk.addrh = 0x0;

	fadt->x_pm1a_cnt_blk.space_id = ACPI_ADDRESS_SPACE_MEMORY;
	fadt->x_pm1a_cnt_blk.bit_width = 16;
	fadt->x_pm1a_cnt_blk.bit_offset = 0;
	fadt->x_pm1a_cnt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;
	fadt->x_pm1a_cnt_blk.addrl = (u32)acpimmio_acpi + MMIO_ACPI_PM1_CNT_BLK;
	fadt->x_pm1a_cnt_blk.addrh = 0x0;

	fadt->x_pm_tmr_blk.space_id = ACPI_ADDRESS_SPACE_MEMORY;
	fadt->x_pm_tmr_blk.bit_width = 32;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_pm_tmr_blk.addrl = (u32)acpimmio_acpi + MMIO_ACPI_PM_TMR_BLK;
	fadt->x_pm_tmr_blk.addrh = 0x0;

	fadt->x_gpe0_blk.space_id = ACPI_ADDRESS_SPACE_MEMORY;
	fadt->x_gpe0_blk.bit_width = 64;
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_gpe0_blk.addrl = (u32)acpimmio_acpi + MMIO_ACPI_GPE0_BLK;
	fadt->x_gpe0_blk.addrh = 0x0;
}
