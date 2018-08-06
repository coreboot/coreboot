/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012, 2017 Advanced Micro Devices, Inc.
 * Copyright (C) 2014 Google Inc.
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

/*
 * ACPI - create the Fixed ACPI Description Tables (FADT)
 */

#include <string.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include <cpu/x86/smm.h>
#include <cbmem.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/acpi.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include <soc/nvs.h>
#include <soc/gpio.h>

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
						current, 0, 9, 9, 0xF);

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
void acpi_create_fadt(acpi_fadt_t *fadt, acpi_facs_t *facs, void *dsdt)
{
	acpi_header_t *header = &(fadt->header);

	printk(BIOS_DEBUG, "pm_base: 0x%04x\n", STONEYRIDGE_ACPI_IO_BASE);

	/* Prepare the header */
	memset((void *)fadt, 0, sizeof(acpi_fadt_t));
	memcpy(header->signature, "FACP", 4);
	header->length = sizeof(acpi_fadt_t);
	header->revision = ACPI_FADT_REV_ACPI_3_0;
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->asl_compiler_revision = 0;

	fadt->firmware_ctrl = (u32) facs;
	fadt->dsdt = (u32) dsdt;
	fadt->model = 0;		/* reserved, should be 0 ACPI 3.0 */
	fadt->preferred_pm_profile = FADT_PM_PROFILE;
	fadt->sci_int = 9;		/* IRQ 09 - ACPI SCI */

	if (IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)) {
		fadt->smi_cmd = APM_CNT;
		fadt->acpi_enable = APM_CNT_ACPI_ENABLE;
		fadt->acpi_disable = APM_CNT_ACPI_DISABLE;
		fadt->s4bios_req = 0;	/* Not supported */
		fadt->pstate_cnt = 0;	/* Not supported */
		fadt->cst_cnt = 0;	/* Not supported */
		outl(0x0, ACPI_PM1_CNT_BLK);	/* clear SCI_EN */
	} else {
		fadt->smi_cmd = 0;	/* disable system management mode */
		fadt->acpi_enable = 0;	/* unused if SMI_CMD = 0 */
		fadt->acpi_disable = 0;	/* unused if SMI_CMD = 0 */
		fadt->s4bios_req = 0;	/* unused if SMI_CMD = 0 */
		fadt->pstate_cnt = 0;	/* unused if SMI_CMD = 0 */
		fadt->cst_cnt = 0x00;	/* unused if SMI_CMD = 0 */
		outl(0x1, ACPI_PM1_CNT_BLK);	/* set SCI_EN */
	}

	fadt->pm1a_evt_blk = ACPI_PM_EVT_BLK;
	fadt->pm1b_evt_blk = 0x0000;
	fadt->pm1a_cnt_blk = ACPI_PM1_CNT_BLK;
	fadt->pm1b_cnt_blk = 0x0000;
	fadt->pm2_cnt_blk = 0x0000;
	fadt->pm_tmr_blk = ACPI_PM_TMR_BLK;
	fadt->gpe0_blk = ACPI_GPE0_BLK;
	fadt->gpe1_blk = 0x0000;		/* No gpe1 block  */

	fadt->pm1_evt_len = 4;	/* 32 bits */
	fadt->pm1_cnt_len = 2;	/* 16 bits */
	fadt->pm2_cnt_len = 0;
	fadt->pm_tmr_len = 4;	/* 32 bits */
	fadt->gpe0_blk_len = 8;	/* 64 bits */
	fadt->gpe1_blk_len = 0;
	fadt->gpe1_base = 0;

	fadt->p_lvl2_lat = ACPI_FADT_C2_NOT_SUPPORTED;
	fadt->p_lvl3_lat = ACPI_FADT_C3_NOT_SUPPORTED;
	fadt->flush_size = 0;	/* set to 0 if WBINVD is 1 in flags */
	fadt->flush_stride = 0;	/* set to 0 if WBINVD is 1 in flags */
	fadt->duty_offset = 1;	/* CLK_VAL bits 3:1 */
	fadt->duty_width = 3;	/* CLK_VAL bits 3:1 */
	fadt->day_alrm = 0;	/* 0x7d these have to be */
	fadt->mon_alrm = 0;	/* 0x7e added to cmos.layout */
	fadt->century = 0;	/* 0x7f to make rtc alarm work */
	fadt->iapc_boot_arch = FADT_BOOT_ARCH;	/* See table 5-10 */
	fadt->res2 = 0;		/* reserved, MUST be 0 ACPI 3.0 */
	fadt->flags = ACPI_FADT_WBINVD | /* See table 5-10 ACPI 3.0a spec */
				ACPI_FADT_C1_SUPPORTED |
				ACPI_FADT_SLEEP_BUTTON |
				ACPI_FADT_S4_RTC_WAKE |
				ACPI_FADT_32BIT_TIMER |
				ACPI_FADT_RESET_REGISTER |
				ACPI_FADT_PCI_EXPRESS_WAKE |
				ACPI_FADT_PLATFORM_CLOCK |
				ACPI_FADT_S4_RTC_VALID |
				ACPI_FADT_REMOTE_POWER_ON;

	/* Format is from 5.2.3.1: Generic Address Structure */
	/* reset_reg: see section 4.7.3.6 ACPI 3.0a spec */
	/* 8 bit write of value 0x06 to 0xCF9 in IO space */
	fadt->reset_reg.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->reset_reg.bit_width = 8;
	fadt->reset_reg.bit_offset = 0;
	fadt->reset_reg.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->reset_reg.addrl = SYS_RESET;
	fadt->reset_reg.addrh = 0x0;

	fadt->reset_value = 6;

	fadt->res3 = 0;		/* reserved, MUST be 0 ACPI 3.0 */
	fadt->res4 = 0;		/* reserved, MUST be 0 ACPI 3.0 */
	fadt->res5 = 0;		/* reserved, MUST be 0 ACPI 3.0 */

	fadt->x_firmware_ctl_l = 0;	/* set to 0 if firmware_ctrl is used */
	fadt->x_firmware_ctl_h = 0;
	fadt->x_dsdt_l = (u32) dsdt;
	fadt->x_dsdt_h = 0;

	fadt->x_pm1a_evt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_evt_blk.bit_width = 32;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;
	fadt->x_pm1a_evt_blk.addrl = ACPI_PM_EVT_BLK;
	fadt->x_pm1a_evt_blk.addrh = 0x0;

	fadt->x_pm1b_evt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1b_evt_blk.bit_width = 0;
	fadt->x_pm1b_evt_blk.bit_offset = 0;
	fadt->x_pm1b_evt_blk.access_size = 0;
	fadt->x_pm1b_evt_blk.addrl = 0x0;
	fadt->x_pm1b_evt_blk.addrh = 0x0;


	fadt->x_pm1a_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_cnt_blk.bit_width = 16;
	fadt->x_pm1a_cnt_blk.bit_offset = 0;
	fadt->x_pm1a_cnt_blk.access_size = 0;
	fadt->x_pm1a_cnt_blk.addrl = ACPI_PM1_CNT_BLK;
	fadt->x_pm1a_cnt_blk.addrh = 0x0;

	fadt->x_pm1b_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1b_cnt_blk.bit_width = 0;
	fadt->x_pm1b_cnt_blk.bit_offset = 0;
	fadt->x_pm1b_cnt_blk.access_size = 0;
	fadt->x_pm1b_cnt_blk.addrl = 0x0;
	fadt->x_pm1b_cnt_blk.addrh = 0x0;

	/*
	 * Note: Under this current AMD C state implementation, this is no
	 *       longer used and should not be reported to OS.
	 */
	fadt->x_pm2_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm2_cnt_blk.bit_width = 0;
	fadt->x_pm2_cnt_blk.bit_offset = 0;
	fadt->x_pm2_cnt_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_pm2_cnt_blk.addrl = 0;
	fadt->x_pm2_cnt_blk.addrh = 0x0;


	fadt->x_pm_tmr_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm_tmr_blk.bit_width = 32;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_pm_tmr_blk.addrl = ACPI_PM_TMR_BLK;
	fadt->x_pm_tmr_blk.addrh = 0x0;


	fadt->x_gpe0_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_gpe0_blk.bit_width = 64; /* EventStatus + Event Enable */
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_gpe0_blk.addrl = ACPI_GPE0_BLK;
	fadt->x_gpe0_blk.addrh = 0x0;


	fadt->x_gpe1_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_gpe1_blk.bit_width = 0;
	fadt->x_gpe1_blk.bit_offset = 0;
	fadt->x_gpe1_blk.access_size = 0;
	fadt->x_gpe1_blk.addrl = 0;
	fadt->x_gpe1_blk.addrh = 0x0;

	header->checksum = acpi_checksum((void *)fadt, sizeof(acpi_fadt_t));
}

void generate_cpu_entries(struct device *device)
{
	int cores, cpu;
	struct device *cdb_dev;

	/* Stoney Ridge is single node, just report # of cores */
	cdb_dev = dev_find_slot(0, NB_DEVFN);
	cores = (pci_read_config32(cdb_dev, 0x84) & 0xff) + 1;

	printk(BIOS_DEBUG, "ACPI \\_PR report %d core(s)\n", cores);

	/* Generate BSP \_PR.P000 */
	acpigen_write_processor(0, ACPI_GPE0_BLK, 6);
	acpigen_pop_len();

	/* Generate AP \_PR.Pxxx */
	for (cpu = 1; cpu < cores; cpu++) {
		acpigen_write_processor(cpu, 0, 0);
		acpigen_pop_len();
	}
}

unsigned long southbridge_write_acpi_tables(struct device *device,
		unsigned long current,
		struct acpi_rsdp *rsdp)
{
	return acpi_write_hpet(device, current, rsdp);
}

static void acpi_create_gnvs(struct global_nvs_t *gnvs)
{
	/* Clear out GNVS. */
	memset(gnvs, 0, sizeof(*gnvs));

	if (IS_ENABLED(CONFIG_CONSOLE_CBMEM))
		gnvs->cbmc = (uintptr_t)cbmem_find(CBMEM_ID_CONSOLE);

	if (IS_ENABLED(CONFIG_CHROMEOS)) {
		/* Initialize Verified Boot data */
		chromeos_init_vboot(&gnvs->chromeos);
		gnvs->chromeos.vbt2 = ACTIVE_ECFW_RO;
	}

	/* Set unknown wake source */
	gnvs->pm1i = ~0ULL;
	gnvs->gpei = ~0ULL;

	/* CPU core count */
	gnvs->pcnt = dev_count_cpu();
}

void southbridge_inject_dsdt(struct device *device)
{
	struct global_nvs_t *gnvs;

	gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);

	if (gnvs) {
		acpi_create_gnvs(gnvs);
		acpi_save_gnvs((uintptr_t)gnvs);

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
	if (gpio_num >= GPIO_TOTAL_PINS) {
		printk(BIOS_WARNING, "Warning: Pin %d should be smaller than"
					" %d\n", gpio_num, GPIO_TOTAL_PINS);
		return -1;
	}
	uintptr_t addr = (uintptr_t) gpio_get_address(gpio_num);

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
	if (gpio_num >= GPIO_TOTAL_PINS) {
		printk(BIOS_WARNING, "Warning: Pin %d should be smaller than"
					" %d\n", gpio_num, GPIO_TOTAL_PINS);
		return -1;
	}
	uintptr_t addr = (uintptr_t) gpio_get_address(gpio_num);

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
