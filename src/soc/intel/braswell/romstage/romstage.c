/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <stdint.h>
#include <commonlib/helpers.h>
#include <arch/io.h>
#include <device/mmio.h>
#include <console/console.h>
#include <soc/iomap.h>
#include <soc/iosf.h>
#include <soc/romstage.h>

#include "../chip.h"

static struct chipset_power_state power_state;

static void migrate_power_state(int is_recovery)
{
	struct chipset_power_state *ps_cbmem;

	ps_cbmem = cbmem_add(CBMEM_ID_POWER_STATE, sizeof(*ps_cbmem));

	if (ps_cbmem == NULL) {
		printk(BIOS_DEBUG, "Not adding power state to cbmem!\n");
		return;
	}
	memcpy(ps_cbmem, &power_state, sizeof(*ps_cbmem));
}
CBMEM_CREATION_HOOK(migrate_power_state);

struct chipset_power_state *fill_power_state(void)
{
	power_state.pm1_sts  = inw(ACPI_BASE_ADDRESS + PM1_STS);
	power_state.pm1_en   = inw(ACPI_BASE_ADDRESS + PM1_EN);
	power_state.pm1_cnt  = inl(ACPI_BASE_ADDRESS + PM1_CNT);
	power_state.gpe0_sts = inl(ACPI_BASE_ADDRESS + GPE0_STS);
	power_state.gpe0_en  = inl(ACPI_BASE_ADDRESS + GPE0_EN);
	power_state.tco_sts  = inl(ACPI_BASE_ADDRESS + TCO_STS);

	power_state.prsts      = read32((void *)(PMC_BASE_ADDRESS + PRSTS));
	power_state.gen_pmcon1 = read32((void *)(PMC_BASE_ADDRESS + GEN_PMCON1));
	power_state.gen_pmcon2 = read32((void *)(PMC_BASE_ADDRESS + GEN_PMCON2));

	power_state.prev_sleep_state = chipset_prev_sleep_state(&power_state);

	printk(BIOS_DEBUG, "pm1_sts: %04x pm1_en: %04x pm1_cnt: %08x\n",
		power_state.pm1_sts, power_state.pm1_en, power_state.pm1_cnt);

	printk(BIOS_DEBUG, "gpe0_sts: %08x gpe0_en: %08x tco_sts: %08x\n",
		power_state.gpe0_sts, power_state.gpe0_en, power_state.tco_sts);

	printk(BIOS_DEBUG, "prsts: %08x gen_pmcon1: %08x gen_pmcon2: %08x\n",
		power_state.prsts, power_state.gen_pmcon1, power_state.gen_pmcon2);

	printk(BIOS_DEBUG, "prev_sleep_state %d\n", power_state.prev_sleep_state);
	return &power_state;
}

/* Return 0, 3, or 5 to indicate the previous sleep state. */
int chipset_prev_sleep_state(const struct chipset_power_state *ps)
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

	if (ps->gen_pmcon1 & (PWR_FLR | SUS_PWR_FLR))
		prev_sleep_state = ACPI_S5;

	return prev_sleep_state;
}

/* SOC initialization after RAM is enabled */
void soc_after_ram_init(struct romstage_params *params)
{
	u32 value;

	/* Make sure that E0000 and F0000 are RAM */
	printk(BIOS_DEBUG, "Disable ROM shadow below 1MB.\n");
	value = iosf_bunit_read(BUNIT_BMISC);
	value |= 3;
	iosf_bunit_write(BUNIT_BMISC, value);
}

/* Initialize the UPD parameters for MemoryInit */
void soc_memory_init_params(struct romstage_params *params, MEMORY_INIT_UPD *upd)
{
	const struct device *dev;
	const struct soc_intel_braswell_config *config;

	/* Set the parameters for MemoryInit */
	dev = pcidev_on_root(LPC_DEV, LPC_FUNC);

	if (!dev) {
		printk(BIOS_ERR, "Error! Device (PCI:0:%02x.%01x) not found, %s!\n",
			LPC_DEV, LPC_FUNC, __func__);
		return;
	}

	config = config_of(dev);
	printk(BIOS_DEBUG, "Updating UPD values for MemoryInit\n");

	upd->PcdMrcInitTsegSize   = CONFIG_SMM_TSEG_SIZE / MiB;
	upd->PcdMrcInitMmioSize   = 0x800;
	upd->PcdMrcInitSpdAddr1   = config->PcdMrcInitSpdAddr1;
	upd->PcdMrcInitSpdAddr2   = config->PcdMrcInitSpdAddr2;
	upd->PcdIgdDvmt50PreAlloc = config->PcdIgdDvmt50PreAlloc;
	upd->PcdApertureSize      = 2;
	upd->PcdGttSize           = 1;
	upd->PcdLegacySegDecode   = 0;
	upd->PcdDvfsEnable        = config->PcdDvfsEnable;
	upd->PcdCaMirrorEn        = config->PcdCaMirrorEn;
}

void soc_display_memory_init_params(const MEMORY_INIT_UPD *old,
	MEMORY_INIT_UPD *new)
{
	/* Display the parameters for MemoryInit */
	printk(BIOS_SPEW, "UPD values for MemoryInit:\n");
	fsp_display_upd_value("PcdMrcInitTsegSize", 2,
			  old->PcdMrcInitTsegSize,
			  new->PcdMrcInitTsegSize);
	fsp_display_upd_value("PcdMrcInitMmioSize", 2,
			  old->PcdMrcInitMmioSize,
			  new->PcdMrcInitMmioSize);
	fsp_display_upd_value("PcdMrcInitSpdAddr1", 1,
			  old->PcdMrcInitSpdAddr1,
			  new->PcdMrcInitSpdAddr1);
	fsp_display_upd_value("PcdMrcInitSpdAddr2", 1,
			  old->PcdMrcInitSpdAddr2,
			  new->PcdMrcInitSpdAddr2);
	fsp_display_upd_value("PcdMemChannel0Config", 1,
			  old->PcdMemChannel0Config,
			  new->PcdMemChannel0Config);
	fsp_display_upd_value("PcdMemChannel1Config", 1,
			  old->PcdMemChannel1Config,
			  new->PcdMemChannel1Config);
	fsp_display_upd_value("PcdMemorySpdPtr", 4,
			  old->PcdMemorySpdPtr,
			  new->PcdMemorySpdPtr);
	fsp_display_upd_value("PcdIgdDvmt50PreAlloc", 1,
			  old->PcdIgdDvmt50PreAlloc,
			  new->PcdIgdDvmt50PreAlloc);
	fsp_display_upd_value("PcdApertureSize", 1,
			  old->PcdApertureSize,
			  new->PcdApertureSize);
	fsp_display_upd_value("PcdGttSize", 1,
			  old->PcdGttSize,
			  new->PcdGttSize);
	fsp_display_upd_value("PcdLegacySegDecode", 1,
			  old->PcdLegacySegDecode,
			  new->PcdLegacySegDecode);
	fsp_display_upd_value("PcdDvfsEnable", 1,
			  old->PcdDvfsEnable,
			  new->PcdDvfsEnable);
}
