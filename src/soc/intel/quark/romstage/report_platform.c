/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/cpu.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <soc/cpu.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>

static const struct {
	u32 cpuid;
	u32 extended_temp;
	u32 ecc;
	u32 secure_boot;
	u32 d_variant;
	u32 mm_number;
	const char *name;
} cpu_table[] = {
	{ CPUID_QUARK_X1000,	0, 0, 0, 0, 930237, "Quark X1000" },
	{ CPUID_QUARK_X1000,	0, 1, 0, 0, 930239, "Quark X1010" },
	{ CPUID_QUARK_X1000,	0, 1, 1, 0, 934775, "Quark X1020" },
	{ CPUID_QUARK_X1000,	0, 1, 1, 1, 930236, "Quark X1020D" },
	{ CPUID_QUARK_X1000,	1, 0, 0, 0, 934413, "Quark X1001" },
	{ CPUID_QUARK_X1000,	1, 1, 0, 0, 934415, "Quark X1011" },
	{ CPUID_QUARK_X1000,	1, 1, 1, 0, 934943, "Quark X1021" },
	{ CPUID_QUARK_X1000,	1, 1, 1, 1, 934411, "Quark X1021D" },
};

static struct {
	u8 revision_id;
	const char *stepping;
} stepping_table[] = {
	{ 0,	"A0" },
};

static uint32_t memory_cntrl_read(uint32_t offset)
{
	/* Read the memory controller register */
	mea_write(offset);
	mcr_write(QUARK_OPCODE_READ, QUARK_NC_MEMORY_CONTROLLER_SB_PORT_ID,
		offset);
	return mdr_read();
}

static uint32_t fuse_port_read(uint32_t offset)
{
	/* Read the SoC unit register */
	mea_write(offset);
	mcr_write(QUARK_ALT_OPCODE_READ, QUARK_SCSS_FUSE_SB_PORT_ID, offset);
	return mdr_read();
}

static void report_cpu_info(void)
{
	const char *cpu_type = "Unknown";
	u32 d_variant;
	u32 ecc_enabled;
	u32 extended_temp;
	u32 i;
	u8 revision;
	u32 secure_boot, cpu_id;
	const char *stepping = "Unknown";

	/* Determine if ECC is enabled */
	ecc_enabled = (0 == (B_DFUSESTAT_ECC_DIS
		& memory_cntrl_read(QUARK_NC_MEMORY_CONTROLLER_REG_DFUSESTAT)));

	/* Determine if secure boot is enabled */
	secure_boot = (0 != (fuse_port_read(QUARK_SCSS_SOC_UNIT_SPI_ROM_FUSE)
		& B_ROM_FUSE_IN_SECURE_SKU));

	/* TODO: Determine if this is a D variant */
	d_variant = 0;
	if (ecc_enabled && secure_boot)
		d_variant = 0;	/* or 1 */

	/* TODO: Determine the temperature variant */
	extended_temp = 0;

	/* Look for string to match the CPU ID value */
	cpu_id = cpu_get_cpuid();
	for (i = 0; i < ARRAY_SIZE(cpu_table); i++) {
		if ((cpu_table[i].cpuid == cpu_id)
			&& (cpu_table[i].extended_temp == extended_temp)
			&& (cpu_table[i].ecc == ecc_enabled)
			&& (cpu_table[i].secure_boot == secure_boot)
			&& (cpu_table[i].d_variant == d_variant)) {
			cpu_type = cpu_table[i].name;
			break;
		}
	}

	/*
	 * Translate the host bridge revision ID into the stepping
	 * Developer's Manual Section C.2
	 */
	revision = pci_read_config8(MC_BDF, PCI_REVISION_ID);
	for (i = 0; i < ARRAY_SIZE(stepping_table); i++) {
		if (stepping_table[i].revision_id == revision) {
			stepping = stepping_table[i].stepping;
			break;
		}
	}

	printk(BIOS_DEBUG, "CPU: ID %x:%x, %s %s Stepping\n", cpu_id,
		revision, cpu_type, stepping);
}

void report_platform_info(void)
{
	report_cpu_info();
}
