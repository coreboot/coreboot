/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * Copyright (C) 2017 Siemens AG
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
#include <arch/io.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <cbmem.h>
#include <cpu/x86/smm.h>
#include <cpu/cpu.h>
#include <gpio.h>
#include <intelblocks/acpi.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/sgx.h>
#include <soc/iomap.h>
#include <soc/pm.h>
#include <soc/nvs.h>
#include <soc/pci_devs.h>
#include <string.h>
#include "chip.h"

#define CSTATE_RES(address_space, width, offset, address)		\
	{								\
	.space_id = address_space,					\
	.bit_width = width,						\
	.bit_offset = offset,						\
	.addrl = address,						\
	}

static acpi_cstate_t cstate_map[] = {
	{
		/* C1 */
		.ctype = 1,		/* ACPI C1 */
		.latency = 1,
		.power = 1000,
		.resource = CSTATE_RES(ACPI_ADDRESS_SPACE_FIXED, 0, 0, 0),
	},
	{
		.ctype = 2,		/* ACPI C2 */
		.latency = 50,
		.power = 10,
		.resource = CSTATE_RES(ACPI_ADDRESS_SPACE_IO, 8, 0, 0x415),
	},
	{
		.ctype = 3,		/* ACPI C3 */
		.latency = 150,
		.power = 10,
		.resource = CSTATE_RES(ACPI_ADDRESS_SPACE_IO, 8, 0, 0x419),
	}
};

uint32_t soc_read_sci_irq_select(void)
{
	uintptr_t pmc_bar = soc_read_pmc_base();
	return read32((void *)pmc_bar + IRQ_REG);
}

void soc_write_sci_irq_select(uint32_t scis)
{
	uintptr_t pmc_bar = soc_read_pmc_base();
	write32((void *)pmc_bar + IRQ_REG, scis);
}

acpi_cstate_t *soc_get_cstate_map(size_t *entries)
{
	*entries = ARRAY_SIZE(cstate_map);
	return cstate_map;
}

void acpi_create_gnvs(struct global_nvs_t *gnvs)
{
	struct soc_intel_apollolake_config *cfg;
	struct device *dev = SA_DEV_ROOT;

	/* Clear out GNVS. */
	memset(gnvs, 0, sizeof(*gnvs));

	if (IS_ENABLED(CONFIG_CONSOLE_CBMEM))
		gnvs->cbmc = (uintptr_t) cbmem_find(CBMEM_ID_CONSOLE);

	if (IS_ENABLED(CONFIG_CHROMEOS)) {
		/* Initialize Verified Boot data */
		chromeos_init_chromeos_acpi(&gnvs->chromeos);
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
		gnvs->prt0 = (uintptr_t) gpio_dwx_address(cfg->prt0_gpio);

	/* Get sdcard cd GPIO portid if GPIO is defined in devicetree.
	 * Get offset of sdcard cd pin.
	 */
	if (cfg->sdcard_cd_gpio) {
		gnvs->scdp = gpio_get_pad_portid(cfg->sdcard_cd_gpio);
		gnvs->scdo = gpio_acpi_pin(cfg->sdcard_cd_gpio);
	}

	if (IS_ENABLED(CONFIG_SOC_INTEL_COMMON_BLOCK_SGX))
		sgx_fill_gnvs(gnvs);
}

uint32_t acpi_fill_soc_wake(uint32_t generic_pm1_en,
			    const struct chipset_power_state *ps)
{
	/*
	 * WAK_STS bit is set when the system is in one of the sleep states
	 * (via the SLP_EN bit) and an enabled wake event occurs. Upon setting
	 * this bit, the PMC will transition the system to the ON state and
	 * can only be set by hardware and can only be cleared by writing a one
	 * to this bit position.
	 */

	generic_pm1_en |= WAK_STS | RTC_EN | PWRBTN_EN;
	return generic_pm1_en;
}

int soc_madt_sci_irq_polarity(int sci)
{
	return MP_IRQ_POLARITY_LOW;
}

void soc_fill_fadt(acpi_fadt_t *fadt)
{
	const struct soc_intel_apollolake_config *cfg;
	struct device *dev = SA_DEV_ROOT;

	fadt->pm_tmr_blk = ACPI_BASE_ADDRESS + PM1_TMR;

	fadt->p_lvl2_lat = ACPI_FADT_C2_NOT_SUPPORTED;
	fadt->p_lvl3_lat = ACPI_FADT_C3_NOT_SUPPORTED;

	fadt->pm_tmr_len = 4;
	fadt->duty_width = 3;

	fadt->iapc_boot_arch = ACPI_FADT_LEGACY_DEVICES | ACPI_FADT_8042;

	fadt->x_pm_tmr_blk.space_id = 1;
	fadt->x_pm_tmr_blk.bit_width = fadt->pm_tmr_len * 8;
	fadt->x_pm_tmr_blk.addrl = ACPI_BASE_ADDRESS + PM1_TMR;

	if (!dev || !dev->chip_info) {
		printk(BIOS_ERR, "BUG! Could not find SOC devicetree config\n");
		return;
	}
        cfg = dev->chip_info;

	if(cfg->lpss_s0ix_enable)
		fadt->flags |= ACPI_FADT_LOW_PWR_IDLE_S0;
}

void soc_power_states_generation(int core_id, int cores_per_package)
{
	/* Generate P-state tables */
	generate_p_state_entries(core_id, cores_per_package);

	/* Generate T-state tables */
	generate_t_state_entries(core_id, cores_per_package);
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
	assert(gpio_num < TOTAL_PADS);
	uintptr_t addr = (uintptr_t) gpio_dwx_address(gpio_num);

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
	assert(gpio_num < TOTAL_PADS);
	uintptr_t addr = (uintptr_t) gpio_dwx_address(gpio_num);

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
