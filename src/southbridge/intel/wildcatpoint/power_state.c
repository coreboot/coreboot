/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/pci_ops.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
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
CBMEM_CREATION_HOOK(migrate_power_state);

/* Return 0, 3, or 5 to indicate the previous sleep state. */
static int prev_sleep_state(const struct chipset_power_state *ps)
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

	if (ps->gen_pmcon3 & (PWR_FLR | SUS_PWR_FLR))
		prev_sleep_state = ACPI_S5;

	return prev_sleep_state;
}

static void dump_power_state(struct chipset_power_state *ps)
{
	printk(BIOS_DEBUG, "PM1_STS:   %04x\n", ps->pm1_sts);
	printk(BIOS_DEBUG, "PM1_EN:    %04x\n", ps->pm1_en);
	printk(BIOS_DEBUG, "PM1_CNT:   %08x\n", ps->pm1_cnt);
	printk(BIOS_DEBUG, "TCO_STS:   %04x %04x\n",
	       ps->tco1_sts, ps->tco2_sts);

	printk(BIOS_DEBUG, "GPE0_STS:  %08x %08x %08x %08x\n",
	       ps->gpe0_sts[0], ps->gpe0_sts[1],
	       ps->gpe0_sts[2], ps->gpe0_sts[3]);
	printk(BIOS_DEBUG, "GPE0_EN:   %08x %08x %08x %08x\n",
	       ps->gpe0_en[0], ps->gpe0_en[1],
	       ps->gpe0_en[2], ps->gpe0_en[3]);

	printk(BIOS_DEBUG, "GEN_PMCON: %04x %04x %04x\n",
	       ps->gen_pmcon1, ps->gen_pmcon2, ps->gen_pmcon3);

	printk(BIOS_DEBUG, "Previous Sleep State: S%d\n",
	       ps->prev_sleep_state);
}

/* Fill power state structure from ACPI PM registers */
struct chipset_power_state *fill_power_state(void)
{
	struct chipset_power_state *ps = &power_state;

	ps->pm1_sts = inw(ACPI_BASE_ADDRESS + PM1_STS);
	ps->pm1_en = inw(ACPI_BASE_ADDRESS + PM1_EN);
	ps->pm1_cnt = inl(ACPI_BASE_ADDRESS + PM1_CNT);
	ps->tco1_sts = inw(ACPI_BASE_ADDRESS + TCO1_STS);
	ps->tco2_sts = inw(ACPI_BASE_ADDRESS + TCO2_STS);
	ps->gpe0_sts[0] = inl(ACPI_BASE_ADDRESS + GPE0_STS(0));
	ps->gpe0_sts[1] = inl(ACPI_BASE_ADDRESS + GPE0_STS(1));
	ps->gpe0_sts[2] = inl(ACPI_BASE_ADDRESS + GPE0_STS(2));
	ps->gpe0_sts[3] = inl(ACPI_BASE_ADDRESS + GPE0_STS(3));
	ps->gpe0_en[0] = inl(ACPI_BASE_ADDRESS + GPE0_EN(0));
	ps->gpe0_en[1] = inl(ACPI_BASE_ADDRESS + GPE0_EN(1));
	ps->gpe0_en[2] = inl(ACPI_BASE_ADDRESS + GPE0_EN(2));
	ps->gpe0_en[3] = inl(ACPI_BASE_ADDRESS + GPE0_EN(3));

	ps->gen_pmcon1 = pci_read_config16(PCH_DEV_LPC, GEN_PMCON_1);
	ps->gen_pmcon2 = pci_read_config16(PCH_DEV_LPC, GEN_PMCON_2);
	ps->gen_pmcon3 = pci_read_config16(PCH_DEV_LPC, GEN_PMCON_3);

	ps->prev_sleep_state = prev_sleep_state(ps);

	dump_power_state(ps);

	return ps;
}
