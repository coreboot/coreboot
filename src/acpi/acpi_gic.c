/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <device/device.h>

__weak void platform_fill_gicc(acpi_madt_gicc_t *gicc)
{
}

static int acpi_create_madt_one_gicc_v3(acpi_madt_gicc_t *gicc, u32 acpi_uid, u32 pi_gsiv,
					uint32_t vgic_mi, uint64_t mpidr)
{
	memset(gicc, 0, sizeof(acpi_madt_gicc_t));
	gicc->type = GICC;
	gicc->length = sizeof(acpi_madt_gicc_t);
	gicc->reserved = 0;
	gicc->cpu_interface_number = 0; /* V3, no compat mode */
	gicc->acpi_processor_uid = acpi_uid;
	gicc->flags.enabled = 1;
	gicc->parking_protocol_version = 0; /* Assume PSCI exclusively */
	gicc->performance_interrupt_gsiv = pi_gsiv;
	gicc->parked_address = 0;
	gicc->physical_base_address = 0; /* V3, no compat mode */
	gicc->vgic_maintenance_interrupt = vgic_mi;
	gicc->gicr_base_address = 0; /* ignored by OSPM if GICR is present */
	gicc->processor_power_efficiency_class = 0; /* Ignore for now */
	/* For platforms implementing GIC V3 the format must be:
	 * Bits [63:40] Must be zero
	 * Bits [39:32] Aff3 : Match Aff3 of target processor MPIDR
	 * Bits [31:24] Must be zero
	 * Bits [23:16] Aff2 : Match Aff2 of target processor MPIDR
	 * Bits [15:8] Aff1 : Match Aff1 of target processor MPIDR
	 * Bits [7:0] Aff0 : Match Aff0 of target processor MPIDR
	*/
	gicc->mpidr = mpidr & 0xff00fffffful;

	platform_fill_gicc(gicc);

	return gicc->length;
}

static unsigned long acpi_create_madt_giccs_v3(unsigned long current)
{
	// Loop over CPUs GIC
	uint32_t acpi_id = 0;
	for (struct device *dev = dev_find_path(NULL, DEVICE_PATH_GICC_V3); dev;
	     dev = dev_find_path(dev, DEVICE_PATH_GICC_V3)) {
		acpi_madt_gicc_t *gicc = (acpi_madt_gicc_t *)current;
		current += acpi_create_madt_one_gicc_v3(gicc, acpi_id++,
							dev->path.gicc_v3.pi_gsiv,
							dev->path.gicc_v3.vgic_mi,
							dev->path.gicc_v3.mpidr);
	}

	return current;
}

static unsigned long acpi_create_madt_gicd_v3(unsigned long current)
{
	acpi_madt_gicd_t *gicd = (acpi_madt_gicd_t *)current;
	memset(gicd, 0, sizeof(acpi_madt_gicd_t));
	gicd->type = GICD;
	gicd->length = sizeof(acpi_madt_gicd_t);
	gicd->physical_base_address = platform_get_gicd_base();
	gicd->system_vector_base = 0;
	gicd->gic_version = 3;

	return current + gicd->length;
}

/*
 * The redistributor in GICv3 has two 64KB frames per CPU; in
 * GICv4 it has four 64KB frames per CPU.
 */
#define GICV3_REDIST_SIZE 0x20000
#define GICV4_REDIST_SIZE 0x40000
static unsigned long acpi_create_madt_gicr_v3(unsigned long current)
{
	acpi_madt_gicr_t *gicr = (acpi_madt_gicr_t *)current;
	memset(gicr, 0, sizeof(acpi_madt_gicr_t));
	gicr->type = GICR;
	gicr->length = sizeof(acpi_madt_gicr_t);
	gicr->discovery_range_base_address = platform_get_gicr_base();
	gicr->discovery_range_length = GICV3_REDIST_SIZE * CONFIG_MAX_CPUS;

	return current + gicr->length;
}

__weak int platform_get_gic_its(uintptr_t **base)
{
	return 0;
}

static unsigned long acpi_create_madt_gic_its_v3(unsigned long current)
{
	int i, its_count;
	uintptr_t *its_base;

	its_count = platform_get_gic_its(&its_base);

	for (i = 0; i < its_count; i++) {
		acpi_madt_gic_its_t *gic_its = (acpi_madt_gic_its_t *)current;
		memset(gic_its, 0, sizeof(acpi_madt_gic_its_t));
		gic_its->type = GIC_ITS;
		gic_its->gic_its_id = i;
		gic_its->physical_base_address = its_base[i];
		gic_its->length = sizeof(acpi_madt_gic_its_t);

		current = current + gic_its->length;
	}
	return current;
}

unsigned long acpi_arch_fill_madt(acpi_madt_t *madt, unsigned long current)
{
	current = acpi_create_madt_giccs_v3(current);
	current = acpi_create_madt_gicd_v3(current);
	current = acpi_create_madt_gicr_v3(current);
	current = acpi_create_madt_gic_its_v3(current);

	return current;
}
