/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * (Written by Lance Zhao <lijian.zhao@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <cbmem.h>
#include <cpu/x86/smm.h>
#include <cpu/cpu.h>
#include <soc/acpi.h>
#include <soc/intel/common/acpi.h>
#include <soc/iomap.h>
#include <soc/pm.h>
#include <soc/nvs.h>
#include <soc/pci_devs.h>
#include <string.h>
#include <soc/gpio.h>
#include "chip.h"

#define CSTATE_RES(address_space, width, offset, address)		\
	{								\
	.space_id = address_space,					\
	.bit_width = width,						\
	.bit_offset = offset,						\
	.addrl = address,						\
	}

unsigned long acpi_fill_mcfg(unsigned long current)
{
	/* PCI Segment Group 0, Start Bus Number 0, End Bus Number is 255 */
	current += acpi_create_mcfg_mmconfig((void *) current,
					     CONFIG_MMCONF_BASE_ADDRESS, 0, 0,
					     255);
	return current;
}

static int acpi_sci_irq(void)
{
	int sci_irq = 9;
	return sci_irq;
}

static unsigned long acpi_madt_irq_overrides(unsigned long current)
{
	int sci = acpi_sci_irq();
	uint16_t flags = MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW;;

	/* INT_SRC_OVR */
	current += acpi_create_madt_irqoverride((void *)current, 0, 0, 2, 0);

	/* SCI */
	current += acpi_create_madt_irqoverride((void *)current, 0, sci, sci, flags);

	return current;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Local APICs */
	current = acpi_create_madt_lapics(current);

	/* IOAPIC */
	current += acpi_create_madt_ioapic((void *) current,
					     2, IO_APIC_ADDR, 0);

	return acpi_madt_irq_overrides(current);
}

void acpi_fill_fadt(acpi_fadt_t * fadt)
{
	const uint16_t pmbase = ACPI_PMIO_BASE;

	/* Use ACPI 5.0 revision. */
	fadt->header.revision = ACPI_FADT_REV_ACPI_5_0;

	fadt->sci_int = acpi_sci_irq();
	fadt->smi_cmd = APM_CNT;
	fadt->acpi_enable = APM_CNT_ACPI_ENABLE;
	fadt->acpi_disable = APM_CNT_ACPI_DISABLE;

	fadt->pm1a_evt_blk = pmbase + PM1_STS;
	fadt->pm1a_cnt_blk = pmbase + PM1_CNT;
	fadt->pm_tmr_blk = pmbase + PM1_TMR;
	fadt->gpe0_blk = pmbase + GPE0_STS(0);

	fadt->pm1_evt_len = 4;
	fadt->pm1_cnt_len = 2;
	fadt->pm_tmr_len = 4;
	/* There are 4 GPE0 STS/EN pairs each 32 bits wide. */
	fadt->gpe0_blk_len = 2 * GPE0_REG_MAX * sizeof(uint32_t);
	fadt->p_lvl2_lat = ACPI_FADT_C2_NOT_SUPPORTED;
	fadt->p_lvl3_lat = ACPI_FADT_C3_NOT_SUPPORTED;
	fadt->flush_size = 0x400;	/* twice of cache size*/
	fadt->flush_stride = 0x10;	/* Cache line width  */
	fadt->duty_offset = 1;
	fadt->duty_width = 3;
	fadt->day_alrm = 0xd;
	fadt->iapc_boot_arch = ACPI_FADT_LEGACY_DEVICES | ACPI_FADT_8042;

	fadt->flags = ACPI_FADT_WBINVD | ACPI_FADT_C1_SUPPORTED |
	    ACPI_FADT_C2_MP_SUPPORTED | ACPI_FADT_SLEEP_BUTTON |
	    ACPI_FADT_RESET_REGISTER | ACPI_FADT_SEALED_CASE |
	    ACPI_FADT_S4_RTC_WAKE | ACPI_FADT_PLATFORM_CLOCK;

	fadt->reset_reg.space_id = 1;
	fadt->reset_reg.bit_width = 8;
	fadt->reset_reg.addrl = 0xcf9;
	fadt->reset_value = 6;

	fadt->x_pm1a_evt_blk.space_id = 1;
	fadt->x_pm1a_evt_blk.bit_width = fadt->pm1_evt_len * 8;
	fadt->x_pm1a_evt_blk.addrl = pmbase + PM1_STS;

	fadt->x_pm1b_evt_blk.space_id = 1;

	fadt->x_pm1a_cnt_blk.space_id = 1;
	fadt->x_pm1a_cnt_blk.bit_width = fadt->pm1_cnt_len * 8;
	fadt->x_pm1a_cnt_blk.addrl = pmbase + PM1_CNT;

	fadt->x_pm1b_cnt_blk.space_id = 1;

	fadt->x_pm_tmr_blk.space_id = 1;
	fadt->x_pm_tmr_blk.bit_width = fadt->pm_tmr_len * 8;
	fadt->x_pm_tmr_blk.addrl = pmbase + PM1_TMR;

	fadt->x_gpe1_blk.space_id = 1;
}

unsigned long southbridge_write_acpi_tables(device_t device,
		unsigned long current,
		struct acpi_rsdp *rsdp)
{
	return acpi_write_hpet(device, current, rsdp);
}

static void acpi_create_gnvs(struct global_nvs_t *gnvs)
{
	struct soc_intel_apollolake_config *cfg;
	struct device *dev = NB_DEV_ROOT;

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

	/* CPU core count */
	gnvs->pcnt = dev_count_cpu();

	if (!dev || !dev->chip_info) {
		printk(BIOS_ERR, "BUG! Could not find SOC devicetree config\n");
		return;
	}
	cfg = dev->chip_info;

	/* Enable DPTF based on mainboard configuration */
	gnvs->dpte = cfg->dptf_enable;

	/* Assign address of PERST_0 if GPIO is defined in devicetree */
	if (cfg->prt0_gpio != GPIO_PRT0_UDEF)
		gnvs->prt0 = (uintptr_t)gpio_dwx_address(cfg->prt0_gpio);
}

/* Save wake source information for calculating ACPI _SWS values */
int soc_fill_acpi_wake(uint32_t *pm1, uint32_t **gpe0)
{
	struct chipset_power_state *ps;
	static uint32_t gpe0_sts[GPE0_REG_MAX];
	uint32_t pm1_en;
	int i;

	ps = cbmem_find(CBMEM_ID_POWER_STATE);
	if (ps == NULL)
		return -1;

	/*
	 * PM1_EN to check the basic wake events which can happen through
	 * powerbtn or any other wake source like lidopen, key board press etc.
	 * WAK_STS bit is set when the system is in one of the sleep states
	 * (via the SLP_EN bit) and an enabled wake event occurs. Upon setting
	 * this bit, the PMC will transition the system to the ON state and
	 * can only be set by hardware and can only be cleared by writing a one
	 * to this bit position.
	 */
	pm1_en = ps->pm1_en | WAK_STS | RTC_EN | PWRBTN_EN;
	*pm1 = ps->pm1_sts & pm1_en;

	/* Mask off GPE0 status bits that are not enabled */
	*gpe0 = &gpe0_sts[0];
	for (i = 0; i < GPE0_REG_MAX; i++)
		gpe0_sts[i] = ps->gpe0_sts[i] & ps->gpe0_en[i];

	return GPE0_REG_MAX;
}

void southbridge_inject_dsdt(device_t device)
{
	struct global_nvs_t *gnvs;

	gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);

	if (gnvs) {
		acpi_create_gnvs(gnvs);
		acpi_save_gnvs((uintptr_t)gnvs);
		/* And tell SMI about it */
		smm_setup_structures(gnvs, NULL, NULL);

		/* Add it to DSDT.  */
		acpigen_write_scope("\\");
		acpigen_write_name_dword("NVSA", (uintptr_t)gnvs);
		acpigen_pop_len();
	}
}
static acpi_cstate_t cstate_map[] = {
	{
		/* C1 */
		.ctype = 1, /* ACPI C1 */
		.latency = 1,
		.power = 1000,
		.resource = CSTATE_RES(ACPI_ADDRESS_SPACE_FIXED, 0, 0, 0),
	},
	{
		.ctype = 2, /* ACPI C2 */
		.latency = 50,
		.power = 10,
		.resource = CSTATE_RES(ACPI_ADDRESS_SPACE_IO, 8, 0, 0x415),
	},
	{
		.ctype = 3, /* ACPI C3 */
		.latency = 150,
		.power = 10,
		.resource = CSTATE_RES(ACPI_ADDRESS_SPACE_IO, 8, 0, 0x419),
	}
};

acpi_cstate_t *soc_get_cstate_map(int *entries)
{
	*entries = ARRAY_SIZE(cstate_map);
	return cstate_map;
}

uint16_t soc_get_acpi_base_address(void)
{
	return ACPI_PMIO_BASE;
}

static void acpigen_soc_get_dw0_in_local5(uintptr_t addr)
{
	/*
	 * Store (\_SB.GPC0 (addr), Local5)
	 * \_SB.GPC0 is used to read cfg0 value from dw0. It is defined in
	 * gpiolib.asl.
	 */
	acpigen_write_store();
	acpigen_emit_namestring("\\_SB.GPC0");
	acpigen_write_integer(addr);
	acpigen_emit_byte(LOCAL5_OP);
}

static int acpigen_soc_get_gpio_val(unsigned int gpio_num, uint32_t mask)
{
	assert (gpio_num < TOTAL_PADS);
	uintptr_t addr = (uintptr_t)gpio_dwx_address(gpio_num);

	acpigen_soc_get_dw0_in_local5(addr);

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
	assert (gpio_num < TOTAL_PADS);
	uintptr_t addr = (uintptr_t)gpio_dwx_address(gpio_num);

	acpigen_soc_get_dw0_in_local5(addr);

	if (val) {
		/* Or (Local5, PAD_CFG0_TX_STATE, Local5) */
		acpigen_write_or(LOCAL5_OP, PAD_CFG0_TX_STATE, LOCAL5_OP);
	} else {
		/* Not (PAD_CFG0_TX_STATE, Local6) */
		acpigen_write_not(PAD_CFG0_TX_STATE, LOCAL6_OP);

		/* And (Local5, Local6, Local5) */
		acpigen_write_and(LOCAL5_OP, LOCAL6_OP, LOCAL5_OP);
	}

	/*
	 * \_SB.SPC0 (addr, Local5)
	 * \_SB.SPC0 is used to write cfg0 value in dw0. It is defined in
	 * gpiolib.asl.
	 */
	acpigen_emit_namestring("\\_SB.SPC0");
	acpigen_write_integer(addr);
	acpigen_emit_byte(LOCAL5_OP);

	return 0;
}

int acpigen_soc_read_rx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_get_gpio_val(gpio_num, PAD_CFG0_RX_STATE);
}

int acpigen_soc_get_tx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_get_gpio_val(gpio_num, PAD_CFG0_TX_STATE);
}

int acpigen_soc_set_tx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_set_gpio_val(gpio_num, 1);
}

int acpigen_soc_clear_tx_gpio(unsigned int gpio_num)
{
	return acpigen_soc_set_gpio_val(gpio_num, 0);
}
