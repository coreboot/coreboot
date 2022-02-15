/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <arch/romstage.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <cbmem.h>
#include <elog.h>
#include <romstage_handoff.h>
#include <string.h>
#include <timestamp.h>
#include <soc/iomap.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/romstage.h>

static struct chipset_power_state power_state;

static void migrate_power_state(int is_recovery)
{
	struct chipset_power_state *ps_cbmem;
	struct chipset_power_state *ps_car;

	ps_car = &power_state;
	ps_cbmem = cbmem_add(CBMEM_ID_POWER_STATE, sizeof(*ps_cbmem));

	if (ps_cbmem == NULL) {
		printk(BIOS_DEBUG, "Not adding power state to cbmem!\n");
		return;
	}
	memcpy(ps_cbmem, ps_car, sizeof(*ps_cbmem));
}
ROMSTAGE_CBMEM_INIT_HOOK(migrate_power_state)

static struct chipset_power_state *fill_power_state(void)
{
	struct chipset_power_state *ps = &power_state;

	ps->pm1_sts = inw(ACPI_BASE_ADDRESS + PM1_STS);
	ps->pm1_en = inw(ACPI_BASE_ADDRESS + PM1_EN);
	ps->pm1_cnt = inl(ACPI_BASE_ADDRESS + PM1_CNT);
	ps->gpe0_sts = inl(ACPI_BASE_ADDRESS + GPE0_STS);
	ps->gpe0_en = inl(ACPI_BASE_ADDRESS + GPE0_EN);
	ps->tco_sts = inl(ACPI_BASE_ADDRESS + TCO_STS);
	ps->prsts = read32((u32 *)(PMC_BASE_ADDRESS + PRSTS));
	ps->gen_pmcon1 = read32((u32 *)(PMC_BASE_ADDRESS + GEN_PMCON1));
	ps->gen_pmcon2 = read32((u32 *)(PMC_BASE_ADDRESS + GEN_PMCON2));

	printk(BIOS_DEBUG, "pm1_sts: %04x pm1_en: %04x pm1_cnt: %08x\n",
		ps->pm1_sts, ps->pm1_en, ps->pm1_cnt);
	printk(BIOS_DEBUG, "gpe0_sts: %08x gpe0_en: %08x tco_sts: %08x\n",
		ps->gpe0_sts, ps->gpe0_en, ps->tco_sts);
	printk(BIOS_DEBUG, "prsts: %08x gen_pmcon1: %08x gen_pmcon2: %08x\n",
		ps->prsts, ps->gen_pmcon1, ps->gen_pmcon2);

	return ps;
}

/* Return 0, 3, or 5 to indicate the previous sleep state. */
static int chipset_prev_sleep_state(const struct chipset_power_state *ps)
{
	/* Default to S0. */
	int prev_sleep_state = ACPI_S0;

	if (ps->pm1_sts & WAK_STS) {
		switch (acpi_sleep_from_pm1(ps->pm1_cnt)) {
		case ACPI_S3:
			if (CONFIG(HAVE_ACPI_RESUME))
				prev_sleep_state = ACPI_S3;
			break;
		case ACPI_S5:
			prev_sleep_state = ACPI_S5;
			break;
		}
		/* Clear SLP_TYP. */
		outl(ps->pm1_cnt & ~(SLP_TYP), ACPI_BASE_ADDRESS + PM1_CNT);
	}

	if (ps->gen_pmcon1 & (PWR_FLR | SUS_PWR_FLR)) {
		prev_sleep_state = ACPI_S5;
	}

	return prev_sleep_state;
}

/* Entry from cpu/intel/car/romstage.c */
void mainboard_romstage_entry(void)
{
	struct chipset_power_state *ps;
	int prev_sleep_state;
	struct mrc_params mp;

	set_max_freq();

	punit_init();

	gfx_init();

	memset(&mp, 0, sizeof(mp));
	mainboard_fill_mrc_params(&mp);

	timestamp_add_now(TS_INITRAM_START);

	ps = fill_power_state();
	prev_sleep_state = chipset_prev_sleep_state(ps);

	printk(BIOS_DEBUG, "prev_sleep_state = S%d\n", prev_sleep_state);

	int s3resume = prev_sleep_state == ACPI_S3;

	elog_boot_notify(s3resume);

	/* Initialize RAM */
	raminit(&mp, prev_sleep_state);

	timestamp_add_now(TS_INITRAM_END);

	romstage_handoff_init(s3resume);
}
