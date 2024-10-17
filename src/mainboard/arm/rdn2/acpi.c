/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_iort.h>
#include <console/console.h>
#include <mainboard/addressmap.h>
#include <stdio.h>

void acpi_fill_fadt(acpi_fadt_t *fadt)
{
}

unsigned long acpi_fill_madt(unsigned long current)
{
	return current;
}

uintptr_t platform_get_gicd_base(void)
{
	return RDN2_GIC_DIST;
}

uintptr_t platform_get_gicr_base(void)
{
	return RDN2_GIC_REDIST;
}

void platform_fill_gicc(acpi_madt_gicc_t *gicc)
{
	gicc->physical_base_address = platform_get_gicd_base();
	gicc->gicv = RDN2_VGIC_BASE;
	gicc->gich = RDN2_HGIC_BASE;

	if (gicc->mpidr >> 16 == 0xf)
		gicc->trbe_interrupt = 0x180c;
	else
		gicc->trbe_interrupt = 0x500b;
}

static uintptr_t gic_its[] = {
	RDN2_GIC_ITS(0),
	RDN2_GIC_ITS(1),
	RDN2_GIC_ITS(2),
	RDN2_GIC_ITS(3),
	RDN2_GIC_ITS(4),
	RDN2_GIC_ITS(5)
};

int platform_get_gic_its(uintptr_t **base)
{
	*base = gic_its;
	return ARRAY_SIZE(gic_its);
}

void mainboard_fill_fadt(acpi_fadt_t *fadt)
{
	fadt->preferred_pm_profile = PM_DESKTOP;
	fadt->ARM_boot_arch = 1;
}

void acpi_soc_fill_gtdt(acpi_gtdt_t *gtdt)
{
	/* his value is optional if the system implements EL3 (Security
	   Extensions). If not provided, this field must be 0xFFFFFFFFFFFFFFFF. */
	gtdt->counter_block_address = UINT64_MAX;
	gtdt->secure_el1_interrupt = SEC_EL1_TIMER_GISV;
	gtdt->secure_el1_flags = RDN2_TIMER_FLAGS;
	gtdt->non_secure_el1_interrupt = NONSEC_EL1_TIMER_GSIV;
	gtdt->non_secure_el1_flags = RDN2_TIMER_FLAGS;
	gtdt->virtual_timer_interrupt = VIRTUAL_TIMER_GSIV;
	gtdt->virtual_timer_flags = RDN2_TIMER_FLAGS;
	gtdt->non_secure_el2_interrupt = NONSEC_EL2_TIMER_GSIV;
	gtdt->non_secure_el2_flags = RDN2_TIMER_FLAGS;
	/* his value is optional if the system implements EL3
	   (Security Extensions). If not provided, this field must be
	   0xFFFFFFFFFFFFFFF. */
	gtdt->counter_read_block_address = UINT64_MAX;
}

unsigned long acpi_soc_gtdt_add_timers(uint32_t *count, unsigned long current)
{
	struct acpi_gtdt_timer_entry timers[2];

	memset(timers, 0, sizeof(timers));
	(*count)++;
	timers[0].frame_number = 0;
	timers[0].base_address = RDN2_GT_FRAME1_CTL_BASE;
	timers[0].el0_base_address = UINT64_MAX;
	timers[0].timer_interrupt = RDN2_GT_FRAME1_GSIV;
	timers[0].timer_flags = 0;
	timers[0].virtual_timer_interrupt = 0;
	timers[0].virtual_timer_flags = 0;
	timers[0].common_flags = ACPI_GTDT_GT_ALWAYS_ON;


	(*count)++;
	timers[1].frame_number = 1;
	timers[1].base_address = RDN2_GT_FRAME0_CTL_BASE;
	timers[1].el0_base_address = UINT64_MAX;
	timers[1].timer_interrupt = RDN2_GT_FRAME0_GSIV;
	timers[1].timer_flags = 0;
	timers[1].virtual_timer_interrupt = 0;
	timers[1].virtual_timer_flags = 0;
	timers[1].common_flags = ACPI_GTDT_GT_ALWAYS_ON | ACPI_GTDT_GT_IS_SECURE_TIMER;

	current = acpi_gtdt_add_timer_block(current, RDN2_GT_CTL_BASE, timers, 2);

	(*count)++;
	current = acpi_gtdt_add_watchdog(current, RDN2_GWDT_REFRESH, RDN2_GWDT_CONTROL,
				      RDN2_GWDT_WS0_GSIV, 0);
	/* Secure */
	current = acpi_gtdt_add_watchdog(current, RDN2_GWDT_REFRESH, RDN2_GWDT_CONTROL,
				      RDN2_GWDT_WS1_GSIV, ACPI_GTDT_WATCHDOG_SECURE);

	return current;
}

static unsigned long acpi_soc_fill_iort_tcu4(acpi_iort_t *iort, unsigned long current)
{
	acpi_iort_node_t *its, *smmu_v3, *named_comp;
	u32 identifiers[] = {4};
	u32 its_reference, smmuv3_reference;

	current = acpi_iort_its_entry(current, iort, &its, 1, identifiers);
	its_reference = (unsigned long)its - (unsigned long)iort;

	current = acpi_iort_smmuv3_entry(current, iort, &smmu_v3, RDN2_SMMU_V3(4), ACPI_IORT_SMMU_V3_FLAGS);
	smmuv3_reference = (unsigned long)smmu_v3 - (unsigned long)iort;

	/* Individual maps here */
	current = acpi_iort_id_map_entry(current, smmu_v3, 0, 1, 0x80000, its_reference,
			       ACPI_IORT_ID_SINGLE_MAPPING);

	current = acpi_iort_id_map_entry(current, smmu_v3, 0x10000, 10, 0x10000, its_reference, 0);

	current = acpi_iort_id_map_entry(current, smmu_v3, 0x30000, 10, 0x30000, its_reference, 0);

	/* SMMUV3 entry length includes mapping */

	for (int n = 0 ; n < 2 ; n++) {
		/* DMA */
		char dma[32];
		snprintf(dma, sizeof(dma), "\\_SB_.DMA%d", n);

		current = acpi_iort_nc_entry(current, iort, &named_comp, 0, 0, 0x30, dma);

		for (int i = 0 ; i < 9 ; i++) {
			/* ID */
			current = acpi_iort_id_map_entry(current, named_comp, i, 1, 0x10000 + n * 0x20000 + i,
					       smmuv3_reference, ACPI_IORT_ID_SINGLE_MAPPING);
		}
	}


	return current;
}

static unsigned long acpi_soc_fill_iort_tcu(u32 i, u16 id_count, u32 *smmu_offset,
					    acpi_iort_t *iort, unsigned long current)
{

	acpi_iort_node_t *its, *smmu_v3;
	static u32 id_base = 0x30000;
	u32 its_reference;

	current = acpi_iort_its_entry(current, iort, &its, 1, &i);
	its_reference = (unsigned long)its - (unsigned long)iort;

	current = acpi_iort_smmuv3_entry(current, iort, &smmu_v3, RDN2_SMMU_V3(i), ACPI_IORT_SMMU_V3_FLAGS);

	*smmu_offset = (unsigned long)smmu_v3 - (unsigned long)iort;

	/* Individual maps here */
	current = acpi_iort_id_map_entry(current, smmu_v3, 0, 1, 0x80000, its_reference,
			       ACPI_IORT_ID_SINGLE_MAPPING);

	current = acpi_iort_id_map_entry(current, smmu_v3, id_base, id_count, id_base, its_reference, 0);
	id_base += id_count;

	/* SMMUV3 entry length includes mapping */

	return current;
}

unsigned long acpi_soc_fill_iort(acpi_iort_t *iort, unsigned long current)
{
	acpi_iort_node_t *root_comp;
	u32 smmu_offset[4];
	uint32_t id_count[] = {768, 256, 256, 256};

	for (int i = 0; i < 4; i++)
		current = acpi_soc_fill_iort_tcu(i, id_count[i], &smmu_offset[i], iort, current);

	current = acpi_soc_fill_iort_tcu4(iort, current);

	current = acpi_iort_rc_entry(current, iort, &root_comp, 0, ACPI_IORT_ATS_SUPPORTED, 0,
			   0x30, 0);

	u32 base = 0;
	for (int i = 0 ; i < 4 ; i++) {
		current = acpi_iort_id_map_entry(current, root_comp, base, id_count[i], 0x30000 + base,
				       smmu_offset[i], 0);
		base += id_count[i];
	}

	return current;
}
