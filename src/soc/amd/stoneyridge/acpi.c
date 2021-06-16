/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * ACPI - create the Fixed ACPI Description Tables (FADT)
 */

#include <console/console.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <device/pci_ops.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <device/pci.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/acpi.h>
#include <amdblocks/ioapic.h>
#include <soc/acpi.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include <soc/northbridge.h>
#include <soc/gpio.h>
#include <version.h>

unsigned long acpi_fill_madt(unsigned long current)
{
	/* create all subtables for processors */
	current = acpi_create_madt_lapics(current);

	/* Write Kern IOAPIC, only one */
	current += acpi_create_madt_ioapic_from_hw((acpi_madt_ioapic_t *)current, IO_APIC_ADDR);

	current += acpi_create_madt_ioapic_from_hw((acpi_madt_ioapic_t *)current, IO_APIC2_ADDR);

	/* PIT is connected to legacy IRQ 0, but IOAPIC GSI 2 */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)current,
			MP_BUS_ISA, 0, 2,
			MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT);
	/* SCI IRQ type override */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)current,
			MP_BUS_ISA, 9, 9,
			MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW);

	/* create all subtables for processors */
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)current,
			ACPI_MADT_LAPIC_NMI_ALL_PROCESSORS,
			MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH,
			1 /* 1: LINT1 connect to NMI */);

	return current;
}

/*
 * Reference section 5.2.9 Fixed ACPI Description Table (FADT)
 * in the ACPI 3.0b specification.
 */
void acpi_fill_fadt(acpi_fadt_t *fadt)
{
	printk(BIOS_DEBUG, "pm_base: 0x%04x\n", ACPI_IO_BASE);

	fadt->sci_int = 9;		/* IRQ 09 - ACPI SCI */

	if (permanent_smi_handler()) {
		fadt->smi_cmd = APM_CNT;
		fadt->acpi_enable = APM_CNT_ACPI_ENABLE;
		fadt->acpi_disable = APM_CNT_ACPI_DISABLE;
	}

	fadt->pm1a_evt_blk = ACPI_PM_EVT_BLK;
	fadt->pm1a_cnt_blk = ACPI_PM1_CNT_BLK;
	fadt->pm_tmr_blk = ACPI_PM_TMR_BLK;
	fadt->gpe0_blk = ACPI_GPE0_BLK;

	fadt->pm1_evt_len = 4;	/* 32 bits */
	fadt->pm1_cnt_len = 2;	/* 16 bits */
	fadt->pm_tmr_len = 4;	/* 32 bits */
	fadt->gpe0_blk_len = 8;	/* 64 bits */

	fill_fadt_extended_pm_regs(fadt);

	fadt->p_lvl2_lat = ACPI_FADT_C2_NOT_SUPPORTED;
	fadt->p_lvl3_lat = ACPI_FADT_C3_NOT_SUPPORTED;
	fadt->duty_offset = 1;	/* CLK_VAL bits 3:1 */
	fadt->duty_width = 3;	/* CLK_VAL bits 3:1 */
	fadt->day_alrm = RTC_DATE_ALARM;
	fadt->mon_alrm = 0;	/* Not supported */
	fadt->iapc_boot_arch = FADT_BOOT_ARCH;	/* See table 5-10 */
	fadt->res2 = 0;		/* reserved, MUST be 0 ACPI 3.0 */
	fadt->flags |= ACPI_FADT_WBINVD | /* See table 5-10 ACPI 3.0a spec */
				ACPI_FADT_C1_SUPPORTED |
				ACPI_FADT_SLEEP_BUTTON |
				ACPI_FADT_S4_RTC_WAKE |
				ACPI_FADT_32BIT_TIMER |
				ACPI_FADT_PCI_EXPRESS_WAKE |
				ACPI_FADT_PLATFORM_CLOCK |
				ACPI_FADT_S4_RTC_VALID |
				ACPI_FADT_REMOTE_POWER_ON;

	fadt->ARM_boot_arch = 0;	/* Must be zero if ACPI Revision <= 5.0 */

	fadt->x_firmware_ctl_l = 0;	/* set to 0 if firmware_ctrl is used */
	fadt->x_firmware_ctl_h = 0;
}

void generate_cpu_entries(const struct device *device)
{
	int cores, cpu;

	/* Stoney Ridge is single node, just report # of cores */
	cores = pci_read_config32(SOC_NB_DEV, NB_CAPABILITIES2) & CMP_CAP_MASK;
	cores++; /* number of cores is CmpCap+1 */

	printk(BIOS_DEBUG, "ACPI \\_SB report %d core(s)\n", cores);

	/* Generate BSP \_SB.P000 */
	acpigen_write_processor(0, ACPI_GPE0_BLK, 6);
	acpigen_pop_len();

	/* Generate AP \_SB.Pxxx */
	for (cpu = 1; cpu < cores; cpu++) {
		acpigen_write_processor(cpu, 0, 0);
		acpigen_pop_len();
	}

	acpigen_write_processor_package("PPKG", 0, cores);
}
