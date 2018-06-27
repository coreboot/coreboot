/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
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

#include <arch/early_variables.h>
#include <arch/io.h>
#include <cbmem.h>
#include <compiler.h>
#include <console/console.h>
#include <halt.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/gpio.h>
#include <soc/pm.h>
#include <string.h>
#include <timer.h>
#include <security/vboot/vboot_common.h>

static struct chipset_power_state power_state CAR_GLOBAL;

struct chipset_power_state *pmc_get_power_state(void)
{
	struct chipset_power_state *ptr = NULL;

	if (cbmem_possibly_online())
		ptr = cbmem_find(CBMEM_ID_POWER_STATE);

	/* cbmem is online but ptr is not populated yet */
	if (ptr == NULL && !(ENV_RAMSTAGE || ENV_POSTCAR))
		return car_get_var_ptr(&power_state);

	return ptr;
}

static void migrate_power_state(int is_recovery)
{
	struct chipset_power_state *ps_cbmem;
	struct chipset_power_state *ps_car;

	ps_car = car_get_var_ptr(&power_state);
	ps_cbmem = cbmem_add(CBMEM_ID_POWER_STATE, sizeof(*ps_cbmem));

	if (ps_cbmem == NULL) {
		printk(BIOS_DEBUG, "Not adding power state to cbmem!\n");
		return;
	}
	memcpy(ps_cbmem, ps_car, sizeof(*ps_cbmem));
}
ROMSTAGE_CBMEM_INIT_HOOK(migrate_power_state)

static void print_num_status_bits(int num_bits, uint32_t status,
				  const char *const bit_names[])
{
	int i;

	if (!status)
		return;

	for (i = num_bits - 1; i >= 0; i--) {
		if (status & (1 << i)) {
			if (bit_names[i])
				printk(BIOS_DEBUG, "%s ", bit_names[i]);
			else
				printk(BIOS_DEBUG, "BIT%d ", i);
		}
	}
}

__weak uint32_t soc_get_smi_status(uint32_t generic_sts)
{
	return generic_sts;
}

/*
 * Set PMC register to know which state system should be after
 * power reapplied
 */
__weak void pmc_soc_restore_power_failure(void)
{
	/*
	 * SoC code should set PMC config register in order to set
	 * MAINBOARD_POWER_ON bit as per EDS.
	 */
}

static uint32_t pmc_reset_smi_status(void)
{
	uint32_t smi_sts = inl(ACPI_BASE_ADDRESS + SMI_STS);
	outl(smi_sts, ACPI_BASE_ADDRESS + SMI_STS);

	return soc_get_smi_status(smi_sts);
}

static uint32_t print_smi_status(uint32_t smi_sts)
{
	size_t array_size;
	const char *const *smi_arr;

	if (!smi_sts)
		return 0;

	printk(BIOS_DEBUG, "SMI_STS: ");

	smi_arr = soc_smi_sts_array(&array_size);

	print_num_status_bits(array_size, smi_sts, smi_arr);
	printk(BIOS_DEBUG, "\n");

	return smi_sts;
}

/*
 * Update supplied events in PM1_EN register. This does not disable any already
 * set events.
 */
void pmc_update_pm1_enable(u16 events)
{
	u16 pm1_en = pmc_read_pm1_enable();
	pm1_en |= events;
	pmc_enable_pm1(pm1_en);
}

/* Read events set in PM1_EN register. */
uint16_t pmc_read_pm1_enable(void)
{
	return inw(ACPI_BASE_ADDRESS + PM1_EN);
}

uint32_t pmc_clear_smi_status(void)
{
	uint32_t sts = pmc_reset_smi_status();

	return print_smi_status(sts);
}

uint32_t pmc_get_smi_en(void)
{
	return inl(ACPI_BASE_ADDRESS + SMI_EN);
}

void pmc_enable_smi(uint32_t mask)
{
	uint32_t smi_en = inl(ACPI_BASE_ADDRESS + SMI_EN);
	smi_en |= mask;
	outl(smi_en, ACPI_BASE_ADDRESS + SMI_EN);
}

void pmc_disable_smi(uint32_t mask)
{
	uint32_t smi_en = inl(ACPI_BASE_ADDRESS + SMI_EN);
	smi_en &= ~mask;
	outl(smi_en, ACPI_BASE_ADDRESS + SMI_EN);
}

/* PM1 */
void pmc_enable_pm1(uint16_t events)
{
	outw(events, ACPI_BASE_ADDRESS + PM1_EN);
}

uint32_t pmc_read_pm1_control(void)
{
	return inl(ACPI_BASE_ADDRESS + PM1_CNT);
}

void pmc_write_pm1_control(uint32_t pm1_cnt)
{
	outl(pm1_cnt, ACPI_BASE_ADDRESS + PM1_CNT);
}

void pmc_enable_pm1_control(uint32_t mask)
{
	uint32_t pm1_cnt = pmc_read_pm1_control();
	pm1_cnt |= mask;
	pmc_write_pm1_control(pm1_cnt);
}

void pmc_disable_pm1_control(uint32_t mask)
{
	uint32_t pm1_cnt = pmc_read_pm1_control();
	pm1_cnt &= ~mask;
	pmc_write_pm1_control(pm1_cnt);
}

static uint16_t reset_pm1_status(void)
{
	uint16_t pm1_sts = inw(ACPI_BASE_ADDRESS + PM1_STS);
	outw(pm1_sts, ACPI_BASE_ADDRESS + PM1_STS);
	return pm1_sts;
}

static uint16_t print_pm1_status(uint16_t pm1_sts)
{
	static const char *const pm1_sts_bits[] = {
		[0] = "TMROF",
		[5] = "GBL",
		[8] = "PWRBTN",
		[10] = "RTC",
		[11] = "PRBTNOR",
		[13] = "USB",
		[14] = "PCIEXPWAK",
		[15] = "WAK",
	};

	if (!pm1_sts)
		return 0;

	printk(BIOS_SPEW, "PM1_STS: ");
	print_num_status_bits(ARRAY_SIZE(pm1_sts_bits), pm1_sts, pm1_sts_bits);
	printk(BIOS_SPEW, "\n");

	return pm1_sts;
}

uint16_t pmc_clear_pm1_status(void)
{
	return print_pm1_status(reset_pm1_status());
}

/* TCO */

static uint32_t print_tco_status(uint32_t tco_sts)
{
	size_t array_size;
	const char *const *tco_arr;

	if (!tco_sts)
		return 0;

	printk(BIOS_DEBUG, "TCO_STS: ");

	tco_arr = soc_tco_sts_array(&array_size);

	print_num_status_bits(array_size, tco_sts, tco_arr);
	printk(BIOS_DEBUG, "\n");

	return tco_sts;
}

uint32_t pmc_clear_tco_status(void)
{
	return print_tco_status(soc_reset_tco_status());
}

/* GPE */
static void pmc_enable_gpe(int gpe, uint32_t mask)
{
	uint32_t gpe0_en = inl(ACPI_BASE_ADDRESS + GPE0_EN(gpe));
	gpe0_en |= mask;
	outl(gpe0_en, ACPI_BASE_ADDRESS + GPE0_EN(gpe));
}

static void pmc_disable_gpe(int gpe, uint32_t mask)
{
	uint32_t gpe0_en = inl(ACPI_BASE_ADDRESS + GPE0_EN(gpe));
	gpe0_en &= ~mask;
	outl(gpe0_en, ACPI_BASE_ADDRESS + GPE0_EN(gpe));
}

void pmc_enable_std_gpe(uint32_t mask)
{
	pmc_enable_gpe(GPE_STD, mask);
}

void pmc_disable_std_gpe(uint32_t mask)
{
	pmc_disable_gpe(GPE_STD, mask);
}

void pmc_disable_all_gpe(void)
{
	int i;
	for (i = 0; i < GPE0_REG_MAX; i++)
		pmc_disable_gpe(i, ~0);
}

/* Clear the gpio gpe0 status bits in ACPI registers */
static void pmc_clear_gpi_gpe_status(void)
{
	int i;

	for (i = 0; i < GPE0_REG_MAX; i++) {
		/* This is reserved GPE block and specific to chipset */
		if (i == GPE_STD)
			continue;
		uint32_t gpe_sts = inl(ACPI_BASE_ADDRESS + GPE0_STS(i));
		outl(gpe_sts, ACPI_BASE_ADDRESS + GPE0_STS(i));
	}
}

static uint32_t reset_std_gpe_status(void)
{
	uint32_t gpe_sts = inl(ACPI_BASE_ADDRESS + GPE0_STS(GPE_STD));
	outl(gpe_sts, ACPI_BASE_ADDRESS + GPE0_STS(GPE_STD));
	return gpe_sts;
}

static uint32_t print_std_gpe_sts(uint32_t gpe_sts)
{
	size_t array_size;
	const char *const *sts_arr;

	if (!gpe_sts)
		return gpe_sts;

	printk(BIOS_DEBUG, "GPE0 STD STS: ");

	sts_arr = soc_std_gpe_sts_array(&array_size);
	print_num_status_bits(array_size, gpe_sts, sts_arr);
	printk(BIOS_DEBUG, "\n");

	return gpe_sts;
}

static void pmc_clear_std_gpe_status(void)
{
	print_std_gpe_sts(reset_std_gpe_status());
}

void pmc_clear_all_gpe_status(void)
{
	pmc_clear_std_gpe_status();
	pmc_clear_gpi_gpe_status();
}

__weak
void soc_clear_pm_registers(uintptr_t pmc_bar)
{
}

void pmc_clear_prsts(void)
{
	uint32_t prsts;
	uintptr_t pmc_bar;

	/* Read PMC base address from soc */
	pmc_bar = soc_read_pmc_base();

	prsts = read32((void *)(pmc_bar + PRSTS));
	write32((void *)(pmc_bar + PRSTS), prsts);

	soc_clear_pm_registers(pmc_bar);
}

__weak
int soc_prev_sleep_state(const struct chipset_power_state *ps,
			      int prev_sleep_state)
{
	return prev_sleep_state;
}

/*
 * Returns prev_sleep_state and also prints all power management registers.
 * Calls soc_prev_sleep_state which may be implemented by SOC.
 */
static int pmc_prev_sleep_state(const struct chipset_power_state *ps)
{
	/* Default to S0. */
	int prev_sleep_state = ACPI_S0;

	if (ps->pm1_sts & WAK_STS) {
		switch (acpi_sleep_from_pm1(ps->pm1_cnt)) {
		case ACPI_S3:
			if (IS_ENABLED(CONFIG_HAVE_ACPI_RESUME))
				prev_sleep_state = ACPI_S3;
			break;
		case ACPI_S5:
			prev_sleep_state = ACPI_S5;
			break;
		}

		/* Clear SLP_TYP. */
		pmc_write_pm1_control(ps->pm1_cnt & ~(SLP_TYP));
	}
	return soc_prev_sleep_state(ps, prev_sleep_state);
}

void pmc_fill_pm_reg_info(struct chipset_power_state *ps)
{
	int i;

	memset(ps, 0, sizeof(*ps));

	ps->pm1_sts = inw(ACPI_BASE_ADDRESS + PM1_STS);
	ps->pm1_en = inw(ACPI_BASE_ADDRESS + PM1_EN);
	ps->pm1_cnt = pmc_read_pm1_control();

	printk(BIOS_DEBUG, "pm1_sts: %04x pm1_en: %04x pm1_cnt: %08x\n",
	       ps->pm1_sts, ps->pm1_en, ps->pm1_cnt);

	for (i = 0; i < GPE0_REG_MAX; i++) {
		ps->gpe0_sts[i] = inl(ACPI_BASE_ADDRESS + GPE0_STS(i));
		ps->gpe0_en[i] = inl(ACPI_BASE_ADDRESS + GPE0_EN(i));
		printk(BIOS_DEBUG, "gpe0_sts[%d]: %08x gpe0_en[%d]: %08x\n",
		       i, ps->gpe0_sts[i], i, ps->gpe0_en[i]);
	}

	soc_fill_power_state(ps);
}

/* Reads and prints ACPI specific PM registers */
int pmc_fill_power_state(struct chipset_power_state *ps)
{
	pmc_fill_pm_reg_info(ps);

	ps->prev_sleep_state = pmc_prev_sleep_state(ps);
	printk(BIOS_DEBUG, "prev_sleep_state %d\n", ps->prev_sleep_state);

	return ps->prev_sleep_state;
}

/*
 * If possible, lock 0xcf9. Once the register is locked, it can't be changed.
 * This lock is reset on cold boot, hard reset, soft reset and Sx.
 */
void pmc_global_reset_lock(void)
{
	/* Read PMC base address from soc */
	uintptr_t etr = soc_read_pmc_base() + ETR;
	uint32_t reg;

	reg = read32((void *)etr);
	if (reg & CF9_LOCK)
		return;
	reg |= CF9_LOCK;
	write32((void *)etr, reg);
}

/*
 * Enable or disable global reset. If global reset is enabled, hard reset and
 * soft reset will trigger global reset, where both host and TXE are reset.
 * This is cleared on cold boot, hard reset, soft reset and Sx.
 */
void pmc_global_reset_enable(bool enable)
{
	/* Read PMC base address from soc */
	uintptr_t etr = soc_read_pmc_base() + ETR;
	uint32_t reg;

	reg = read32((void *)etr);
	reg = enable ? reg | CF9_GLB_RST : reg & ~CF9_GLB_RST;
	write32((void *)etr, reg);
}

int vboot_platform_is_resuming(void)
{
	if (!(inw(ACPI_BASE_ADDRESS + PM1_STS) & WAK_STS))
		return 0;

	return acpi_sleep_from_pm1(pmc_read_pm1_control()) == ACPI_S3;
}

/* Read and clear GPE status (defined in arch/acpi.h) */
int acpi_get_gpe(int gpe)
{
	int bank;
	uint32_t mask, sts;
	struct stopwatch sw;
	int rc = 0;

	if (gpe < 0 || gpe > GPE_MAX)
		return -1;

	bank = gpe / 32;
	mask = 1 << (gpe % 32);

	/* Wait up to 1ms for GPE status to clear */
	stopwatch_init_msecs_expire(&sw, 1);
	do {
		if (stopwatch_expired(&sw))
			return rc;

		sts = inl(ACPI_BASE_ADDRESS + GPE0_STS(bank));
		if (sts & mask) {
			outl(mask, ACPI_BASE_ADDRESS + GPE0_STS(bank));
			rc = 1;
		}
	} while (sts & mask);

	return rc;
}

/*
 * The PM1 control is set to S5 when vboot requests a reboot because the power
 * state code above may not have collected its data yet. Therefore, set it to
 * S5 when vboot requests a reboot. That's necessary if vboot fails in the
 * resume path and requests a reboot. This prevents a reboot loop where the
 * error is continually hit on the failing vboot resume path.
 */
void vboot_platform_prepare_reboot(void)
{
	uint32_t pm1_cnt;
	pm1_cnt = (pmc_read_pm1_control() & ~(SLP_TYP)) |
		(SLP_TYP_S5 << SLP_TYP_SHIFT);
	pmc_write_pm1_control(pm1_cnt);
}

void poweroff(void)
{
	pmc_enable_pm1_control(SLP_EN | (SLP_TYP_S5 << SLP_TYP_SHIFT));

	/*
	 * Setting SLP_TYP_S5 in PM1 triggers SLP_SMI, which is handled by SMM
	 * to transition to S5 state. If halt is called in SMM, then it prevents
	 * the SMI handler from being triggered and system never enters S5.
	 */
	if (!ENV_SMM)
		halt();
}

void pmc_gpe_init(void)
{
	uint32_t gpio_cfg = 0;
	uint32_t gpio_cfg_reg;
	uint8_t dw0, dw1, dw2;

	/* Read PMC base address from soc. This is implemented in soc */
	uintptr_t pmc_bar = soc_read_pmc_base();

	/*
	 * Get the dwX values for pmc gpe settings.
	 */
	soc_get_gpi_gpe_configs(&dw0, &dw1, &dw2);

	const uint32_t gpio_cfg_mask =
	    (GPE0_DWX_MASK << GPE0_DW_SHIFT(0)) |
	    (GPE0_DWX_MASK << GPE0_DW_SHIFT(1)) |
	    (GPE0_DWX_MASK << GPE0_DW_SHIFT(2));

	/* Making sure that bad values don't bleed into the other fields */
	dw0 &= GPE0_DWX_MASK;
	dw1 &= GPE0_DWX_MASK;
	dw2 &= GPE0_DWX_MASK;

	/*
	 * Route the GPIOs to the GPE0 block. Determine that all values
	 * are different, and if they aren't use the reset values.
	 */
	if (dw0 == dw1 || dw1 == dw2) {
		printk(BIOS_INFO, "PMC: Using default GPE route.\n");
		gpio_cfg = read32((void *)pmc_bar + GPIO_GPE_CFG);

		dw0 = (gpio_cfg >> GPE0_DW_SHIFT(0)) & GPE0_DWX_MASK;
		dw1 = (gpio_cfg >> GPE0_DW_SHIFT(1)) & GPE0_DWX_MASK;
		dw2 = (gpio_cfg >> GPE0_DW_SHIFT(2)) & GPE0_DWX_MASK;
	} else {
		gpio_cfg |= (uint32_t) dw0 << GPE0_DW_SHIFT(0);
		gpio_cfg |= (uint32_t) dw1 << GPE0_DW_SHIFT(1);
		gpio_cfg |= (uint32_t) dw2 << GPE0_DW_SHIFT(2);
	}

	gpio_cfg_reg = read32((void *)pmc_bar + GPIO_GPE_CFG) & ~gpio_cfg_mask;
	gpio_cfg_reg |= gpio_cfg & gpio_cfg_mask;

	write32((void *)pmc_bar + GPIO_GPE_CFG, gpio_cfg_reg);

	/* Set the routes in the GPIO communities as well. */
	gpio_route_gpe(dw0, dw1, dw2);
}

/*
 * Determines what state to go to when power is reapplied
 * after a power failure (G3 State)
 */
int pmc_get_mainboard_power_failure_state_choice(void)
{
	if (IS_ENABLED(CONFIG_POWER_STATE_PREVIOUS_AFTER_FAILURE))
		return MAINBOARD_POWER_STATE_PREVIOUS;
	else if (IS_ENABLED(CONFIG_POWER_STATE_ON_AFTER_FAILURE))
		return MAINBOARD_POWER_STATE_ON;

	return MAINBOARD_POWER_STATE_OFF;
}
