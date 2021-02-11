/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/psp.h>
#include <amdblocks/smi.h>
#include <amdblocks/smm.h>
#include <arch/hlt.h>
#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <soc/smi.h>
#include <types.h>

static void fch_apmc_smi_handler(void)
{
	const uint8_t cmd = inb(pm_acpi_smi_cmd_port());

	switch (cmd) {
	case APM_CNT_ACPI_ENABLE:
		acpi_enable_sci();
		break;
	case APM_CNT_ACPI_DISABLE:
		acpi_disable_sci();
		break;
	case APM_CNT_SMMINFO:
		psp_notify_smm();
		break;
	}

	mainboard_smi_apmc(cmd);
}

static void fch_slp_typ_handler(void)
{
	uint16_t pm1cnt;
	uint8_t slp_typ;

	/* Figure out SLP_TYP */
	pm1cnt = acpi_read16(MMIO_ACPI_PM1_CNT_BLK);
	printk(BIOS_SPEW, "SMI#: SLP = 0x%04x\n", pm1cnt);
	slp_typ = acpi_sleep_from_pm1(pm1cnt);

	/* Do any mainboard sleep handling */
	mainboard_smi_sleep(slp_typ);

	switch (slp_typ) {
	case ACPI_S0:
		printk(BIOS_DEBUG, "SMI#: Entering S0 (On)\n");
		break;
	case ACPI_S3:
		printk(BIOS_DEBUG, "SMI#: Entering S3 (Suspend-To-RAM)\n");
		break;
	case ACPI_S4:
		printk(BIOS_DEBUG, "SMI#: Entering S4 (Suspend-To-Disk)\n");
		break;
	case ACPI_S5:
		printk(BIOS_DEBUG, "SMI#: Entering S5 (Soft Power off)\n");
		break;
	default:
		printk(BIOS_DEBUG, "SMI#: ERROR: SLP_TYP reserved\n");
		break;
	}

	if (slp_typ >= ACPI_S3) {
		printk(BIOS_ERR, "Error: System did not go to sleep\n");
		hlt();
	}
}

int southbridge_io_trap_handler(int smif)
{
	return 0;
}

/*
 * Table of functions supported in the SMI handler.  Note that SMI source setup
 * in fch.c is unrelated to this list.
 */
static const struct smi_sources_t smi_sources[] = {
	{ .type = SMITYPE_SMI_CMD_PORT, .handler = fch_apmc_smi_handler },
	{ .type = SMITYPE_SLP_TYP, .handler = fch_slp_typ_handler},
};

void *get_smi_source_handler(int source)
{
	size_t i;

	for (i = 0 ; i < ARRAY_SIZE(smi_sources) ; i++)
		if (smi_sources[i].type == source)
			return smi_sources[i].handler;

	return NULL;
}
