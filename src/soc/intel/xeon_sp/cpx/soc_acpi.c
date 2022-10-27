/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <arch/smp/mpspec.h>
#include <assert.h>
#include <cpu/intel/turbo.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <intelblocks/acpi.h>
#include <intelblocks/cpulib.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/iomap.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/soc_util.h>
#include <soc/util.h>

int soc_madt_sci_irq_polarity(int sci)
{
	if (sci >= 20)
		return MP_IRQ_POLARITY_LOW;
	else
		return MP_IRQ_POLARITY_HIGH;
}

uint32_t soc_read_sci_irq_select(void)
{
	struct device *dev = PCH_DEV_PMC;

	if (!dev)
		return 0;

	return pci_read_config32(dev, PMC_ACPI_CNT);
}

void soc_fill_fadt(acpi_fadt_t *fadt)
{
	/* Clear flags set by common/block/acpi/acpi.c acpi_fill_fadt() */
	fadt->flags &=  ~(ACPI_FADT_SEALED_CASE | ACPI_FADT_S4_RTC_WAKE);
}

void uncore_inject_dsdt(const struct device *device)
{
	struct iiostack_resource stack_info = {0};

	/* Only add RTxx entries once. */
	if (device->bus->secondary != 0)
		return;

	get_iiostack_info(&stack_info);

	acpigen_write_scope("\\_SB");

	for (uint8_t stack = 0; stack < stack_info.no_of_stacks; ++stack) {
		const STACK_RES *ri = &stack_info.res[stack];
		char rtname[16];

		snprintf(rtname, sizeof(rtname), "RT%02x", stack);

		acpigen_write_name(rtname);
		printk(BIOS_DEBUG, "\tCreating ResourceTemplate %s for stack: %d\n",
			rtname, stack);

		acpigen_write_resourcetemplate_header();

		/* bus resource */
		acpigen_resource_word(2, 0xc, 0, 0, ri->BusBase, ri->BusLimit,
			0x0, (ri->BusLimit - ri->BusBase + 1));

		/* additional io resources on socket 0 bus 0 */
		if (stack == 0) {
			/* ACPI 6.4.2.5 I/O Port Descriptor */
			acpigen_write_io16(0xCF8, 0xCFF, 0x1, 0x8, 1);

			/* IO decode  CF8-CFF */
			acpigen_resource_word(1, 0xc, 0x3, 0, 0x0000, 0x03AF, 0, 0x03B0);
			acpigen_resource_word(1, 0xc, 0x3, 0, 0x03E0, 0x0CF7, 0, 0x0918);
			acpigen_resource_word(1, 0xc, 0x3, 0, 0x03B0, 0x03BB, 0, 0x000C);
			acpigen_resource_word(1, 0xc, 0x3, 0, 0x03C0, 0x03DF, 0, 0x0020);
		}

		/* IO resource */
		acpigen_resource_word(1, 0xc, 0x3, 0, ri->PciResourceIoBase,
			ri->PciResourceIoLimit, 0x0,
			(ri->PciResourceIoLimit - ri->PciResourceIoBase + 1));

		/* additional mem32 resources on socket 0 bus 0 */
		if (stack == 0) {
			acpigen_resource_dword(0, 0xc, 3, 0, VGA_BASE_ADDRESS,
				(VGA_BASE_ADDRESS + VGA_BASE_SIZE - 1), 0x0,
				VGA_BASE_SIZE);
			acpigen_resource_dword(0, 0xc, 1, 0, SPI_BASE_ADDRESS,
				(SPI_BASE_ADDRESS + SPI_BASE_SIZE - 1), 0x0,
				SPI_BASE_SIZE);
		}

		/* Mem32 resource */
		acpigen_resource_dword(0, 0xc, 1, 0, ri->PciResourceMem32Base,
			ri->PciResourceMem32Limit, 0x0,
			(ri->PciResourceMem32Limit - ri->PciResourceMem32Base + 1));

		/* Mem64 resource */
		acpigen_resource_qword(0, 0xc, 1, 0, ri->PciResourceMem64Base,
			ri->PciResourceMem64Limit, 0x0,
			(ri->PciResourceMem64Limit - ri->PciResourceMem64Base + 1));

		acpigen_write_resourcetemplate_footer();
	}
	acpigen_pop_len();
}

/* TODO: See if we can use the common generate_p_state_entries */
void soc_power_states_generation(int core, int cores_per_package)
{
	int ratio_min, ratio_max, ratio_turbo, ratio_step;
	int coord_type, power_max, power_unit, num_entries;
	int ratio, power, clock, clock_max;
	msr_t msr;

	/* Determine P-state coordination type from MISC_PWR_MGMT[0] */
	msr = rdmsr(MSR_MISC_PWR_MGMT);
	if (msr.lo & MISC_PWR_MGMT_EIST_HW_DIS)
		coord_type = SW_ANY;
	else
		coord_type = HW_ALL;

	/* Get bus ratio limits and calculate clock speeds */
	msr = rdmsr(MSR_PLATFORM_INFO);
	ratio_min = (msr.hi >> (40-32)) & 0xff; /* Max Efficiency Ratio */

	/* Determine if this CPU has configurable TDP */
	if (cpu_config_tdp_levels()) {
		/* Set max ratio to nominal TDP ratio */
		msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
		ratio_max = msr.lo & 0xff;
	} else {
		/* Max Non-Turbo Ratio */
		ratio_max = (msr.lo >> 8) & 0xff;
	}
	clock_max = ratio_max * CONFIG_CPU_BCLK_MHZ;

	/* Calculate CPU TDP in mW */
	msr = rdmsr(MSR_PKG_POWER_SKU_UNIT);
	power_unit = 2 << ((msr.lo & 0xf) - 1);
	msr = rdmsr(MSR_PKG_POWER_SKU);
	power_max = ((msr.lo & 0x7fff) / power_unit) * 1000;

	/* Write _PCT indicating use of FFixedHW */
	acpigen_write_empty_PCT();

	/* Write _PPC with no limit on supported P-state */
	acpigen_write_PPC_NVS();

	/* Write PSD indicating configured coordination type */
	acpigen_write_PSD_package(core, 1, coord_type);

	/* Add P-state entries in _PSS table */
	acpigen_write_name("_PSS");

	/* Determine ratio points */
	ratio_step = PSS_RATIO_STEP;
	num_entries = ((ratio_max - ratio_min) / ratio_step) + 1;
	if (num_entries > PSS_MAX_ENTRIES) {
		ratio_step += 1;
		num_entries = ((ratio_max - ratio_min) / ratio_step) + 1;
	}

	/* P[T] is Turbo state if enabled */
	if (get_turbo_state() == TURBO_ENABLED) {
		/* _PSS package count including Turbo */
		acpigen_write_package(num_entries + 2);

		msr = rdmsr(MSR_TURBO_RATIO_LIMIT);
		ratio_turbo = msr.lo & 0xff;

		/* Add entry for Turbo ratio */
		acpigen_write_PSS_package(
			clock_max + 1,		/* MHz */
			power_max,		/* mW */
			PSS_LATENCY_TRANSITION,	/* lat1 */
			PSS_LATENCY_BUSMASTER,	/* lat2 */
			ratio_turbo << 8,	/* control */
			ratio_turbo << 8);	/* status */
	} else {
		/* _PSS package count without Turbo */
		acpigen_write_package(num_entries + 1);
	}

	/* First regular entry is max non-turbo ratio */
	acpigen_write_PSS_package(
		clock_max,		/* MHz */
		power_max,		/* mW */
		PSS_LATENCY_TRANSITION,	/* lat1 */
		PSS_LATENCY_BUSMASTER,	/* lat2 */
		ratio_max << 8,		/* control */
		ratio_max << 8);	/* status */

	/* Generate the remaining entries */
	for (ratio = ratio_min + ((num_entries - 1) * ratio_step);
	     ratio >= ratio_min; ratio -= ratio_step) {

		/* Calculate power at this ratio */
		power = common_calculate_power_ratio(power_max, ratio_max, ratio);
		clock = ratio * CONFIG_CPU_BCLK_MHZ;
		//clock = 1;
		acpigen_write_PSS_package(
			clock,			/* MHz */
			power,			/* mW */
			PSS_LATENCY_TRANSITION,	/* lat1 */
			PSS_LATENCY_BUSMASTER,	/* lat2 */
			ratio << 8,		/* control */
			ratio << 8);		/* status */
	}

	/* Fix package length */
	acpigen_pop_len();
}
