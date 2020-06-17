/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * ACPI - create the Fixed ACPI Description Tables (FADT)
 */

#include <string.h>
#include <console/console.h>
#include <acpi/acpi.h>
#include <acpi/acpi_gnvs.h>
#include <acpi/acpigen.h>
#include <device/pci_ops.h>
#include <arch/ioapic.h>
#include <cpu/x86/smm.h>
#include <cbmem.h>
#include <device/device.h>
#include <device/pci.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/acpi.h>
#include <soc/acpi.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include <soc/northbridge.h>
#include <soc/nvs.h>
#include <soc/gpio.h>
#include <version.h>

unsigned long acpi_fill_madt(unsigned long current)
{
	/* create all subtables for processors */
	current = acpi_create_madt_lapics(current);

	/* Write Kern IOAPIC, only one */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current,
			CONFIG_MAX_CPUS, IO_APIC_ADDR, 0);

	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current,
			CONFIG_MAX_CPUS+1, IO_APIC2_ADDR, 24);

	/* 0: mean bus 0--->ISA */
	/* 0: PIC 0 */
	/* 2: APIC 2 */
	/* 5 mean: 0101 --> Edge-triggered, Active high */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
						current, 0, 0, 2, 0);
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
						current, 0, 9, 9, 0xf);

	/* create all subtables for processors */
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)current,
			0xff, 5, 1);
	/* 1: LINT1 connect to NMI */

	return current;
}

/*
 * Reference section 5.2.9 Fixed ACPI Description Table (FADT)
 * in the ACPI 3.0b specification.
 */
void acpi_fill_fadt(acpi_fadt_t *fadt)
{
	printk(BIOS_DEBUG, "pm_base: 0x%04x\n", STONEYRIDGE_ACPI_IO_BASE);

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

	fadt->p_lvl2_lat = ACPI_FADT_C2_NOT_SUPPORTED;
	fadt->p_lvl3_lat = ACPI_FADT_C3_NOT_SUPPORTED;
	fadt->duty_offset = 1;	/* CLK_VAL bits 3:1 */
	fadt->duty_width = 3;	/* CLK_VAL bits 3:1 */
	fadt->day_alrm = 0;	/* 0x7d these have to be */
	fadt->mon_alrm = 0;	/* 0x7e added to cmos.layout */
	fadt->century = 0;	/* 0x7f to make rtc alarm work */
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

	fadt->ARM_boot_arch = 0;	/* MUST be 0 ACPI 3.0 */
	fadt->FADT_MinorVersion = 0;	/* MUST be 0 ACPI 3.0 */

	fadt->x_firmware_ctl_l = 0;	/* set to 0 if firmware_ctrl is used */
	fadt->x_firmware_ctl_h = 0;

	fadt->x_pm1a_evt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_evt_blk.bit_width = 32;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;
	fadt->x_pm1a_evt_blk.addrl = ACPI_PM_EVT_BLK;
	fadt->x_pm1a_evt_blk.addrh = 0x0;

	fadt->x_pm1a_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_cnt_blk.bit_width = 16;
	fadt->x_pm1a_cnt_blk.bit_offset = 0;
	fadt->x_pm1a_cnt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;
	fadt->x_pm1a_cnt_blk.addrl = ACPI_PM1_CNT_BLK;
	fadt->x_pm1a_cnt_blk.addrh = 0x0;

	fadt->x_pm_tmr_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm_tmr_blk.bit_width = 32;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_pm_tmr_blk.addrl = ACPI_PM_TMR_BLK;
	fadt->x_pm_tmr_blk.addrh = 0x0;

	fadt->x_gpe0_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_gpe0_blk.bit_width = 64; /* EventStatus + Event Enable */
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_gpe0_blk.addrl = ACPI_GPE0_BLK;
	fadt->x_gpe0_blk.addrh = 0x0;
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
}

unsigned long southbridge_write_acpi_tables(const struct device *device,
		unsigned long current,
		struct acpi_rsdp *rsdp)
{
	return acpi_write_hpet(device, current, rsdp);
}

void acpi_create_gnvs(struct global_nvs *gnvs)
{
	/* Set unknown wake source */
	gnvs->pm1i = ~0ULL;
	gnvs->gpei = ~0ULL;

	/* CPU core count */
	gnvs->pcnt = dev_count_cpu();
}

void southbridge_inject_dsdt(const struct device *device)
{
	struct global_nvs *gnvs;

	gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);

	if (gnvs) {
		acpi_create_gnvs(gnvs);

		/* Add it to DSDT */
		acpigen_write_scope("\\");
		acpigen_write_name_dword("NVSA", (uintptr_t)gnvs);
		acpigen_pop_len();
	}
}

static void acpigen_soc_get_gpio_in_local5(uintptr_t addr)
{
	/*
	 *   Store (\_SB.GPR2 (addr), Local5)
	 * \_SB.GPR2 is used to read control byte 2 from control register.
	 * / It is defined in gpio_lib.asl.
	 */
	acpigen_write_store();
	acpigen_emit_namestring("\\_SB.GPR2");
	acpigen_write_integer(addr);
	acpigen_emit_byte(LOCAL5_OP);
}

static int acpigen_soc_get_gpio_val(unsigned int gpio_num, uint32_t mask)
{
	if (gpio_num >= SOC_GPIO_TOTAL_PINS) {
		printk(BIOS_WARNING, "Warning: Pin %d should be smaller than"
					" %d\n", gpio_num, SOC_GPIO_TOTAL_PINS);
		return -1;
	}
	uintptr_t addr = gpio_get_address(gpio_num);

	acpigen_soc_get_gpio_in_local5(addr);

	/* If (And (Local5, mask)) */
	acpigen_write_if_and(LOCAL5_OP, mask);

	/* Store (One, Local0) */
	acpigen_write_store_ops(ONE_OP, LOCAL0_OP);

	acpigen_pop_len();	/* If */

	/* Else */
	acpigen_write_else();

	/* Store (Zero, Local0) */
	acpigen_write_store_ops(ZERO_OP, LOCAL0_OP);

	acpigen_pop_len();	/* Else */

	return 0;
}

static int acpigen_soc_set_gpio_val(unsigned int gpio_num, uint32_t val)
{
	if (gpio_num >= SOC_GPIO_TOTAL_PINS) {
		printk(BIOS_WARNING, "Warning: Pin %d should be smaller than"
					" %d\n", gpio_num, SOC_GPIO_TOTAL_PINS);
		return -1;
	}
	uintptr_t addr = gpio_get_address(gpio_num);

	/* Store (0x40, Local0) */
	acpigen_write_store();
	acpigen_write_integer(GPIO_PIN_OUT);
	acpigen_emit_byte(LOCAL0_OP);

	acpigen_soc_get_gpio_in_local5(addr);

	if (val) {
		/* Or (Local5, GPIO_PIN_OUT, Local5) */
		acpigen_write_or(LOCAL5_OP, LOCAL0_OP, LOCAL5_OP);
	} else {
		/* Not (GPIO_PIN_OUT, Local6) */
		acpigen_write_not(LOCAL0_OP, LOCAL6_OP);

		/* And (Local5, Local6, Local5) */
		acpigen_write_and(LOCAL5_OP, LOCAL6_OP, LOCAL5_OP);
	}

	/*
	 *   SB.GPW2 (addr, Local5)
	 * \_SB.GPW2 is used to write control byte in control register
	 * / byte 2. It is defined in gpio_lib.asl.
	 */
	acpigen_emit_namestring("\\_SB.GPW2");
	acpigen_write_integer(addr);
	acpigen_emit_byte(LOCAL5_OP);

	return 0;
}

int acpigen_soc_read_rx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_get_gpio_val(gpio_num, GPIO_PIN_IN);
}

int acpigen_soc_get_tx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_get_gpio_val(gpio_num, GPIO_PIN_OUT);
}

int acpigen_soc_set_tx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_set_gpio_val(gpio_num, 1);
}

int acpigen_soc_clear_tx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_set_gpio_val(gpio_num, 0);
}
