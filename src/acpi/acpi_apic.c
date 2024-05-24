/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <commonlib/sort.h>
#include <cpu/cpu.h>
#include <device/device.h>

static int acpi_create_madt_lapic(acpi_madt_lapic_t *lapic, u8 cpu, u8 apic)
{
	lapic->type = LOCAL_APIC; /* Local APIC structure */
	lapic->length = sizeof(acpi_madt_lapic_t);
	lapic->flags = ACPI_MADT_LAPIC_ENABLED;
	lapic->processor_id = cpu;
	lapic->apic_id = apic;

	return lapic->length;
}

static int acpi_create_madt_lx2apic(acpi_madt_lx2apic_t *lapic, u32 cpu, u32 apic)
{
	lapic->type = LOCAL_X2APIC; /* Local APIC structure */
	lapic->reserved = 0;
	lapic->length = sizeof(acpi_madt_lx2apic_t);
	lapic->flags = ACPI_MADT_LAPIC_ENABLED;
	lapic->processor_id = cpu;
	lapic->x2apic_id = apic;

	return lapic->length;
}

unsigned long acpi_create_madt_one_lapic(unsigned long current, u32 index, u32 lapic_id)
{
	if (lapic_id <= ACPI_MADT_MAX_LAPIC_ID)
		current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, index,
						  lapic_id);
	else
		current += acpi_create_madt_lx2apic((acpi_madt_lx2apic_t *)current, index,
						    lapic_id);

	return current;
}

/* Increase if necessary. Currently all x86 CPUs only have 2 SMP threads */
#define MAX_THREAD_ID 1
/*
 * From ACPI 6.4 spec:
 * "The advent of multi-threaded processors yielded multiple logical processors
 * executing on common processor hardware. ACPI defines logical processors in
 * an identical manner as physical processors. To ensure that non
 * multi-threading aware OSPM implementations realize optimal performance on
 * platforms containing multi-threaded processors, two guidelines should be
 * followed. The first is the same as above, that is, OSPM should initialize
 * processors in the order that they appear in the MADT. The second is that
 * platform firmware should list the first logical processor of each of the
 * individual multi-threaded processors in the MADT before listing any of the
 * second logical processors. This approach should be used for all successive
 * logical processors."
 */
static unsigned long acpi_create_madt_lapics(unsigned long current)
{
	struct device *cpu;
	int index, apic_ids[CONFIG_MAX_CPUS] = {0}, num_cpus = 0, sort_start = 0;
	for (unsigned int thread_id = 0; thread_id <= MAX_THREAD_ID; thread_id++) {
		for (cpu = all_devices; cpu; cpu = cpu->next) {
			if (!is_enabled_cpu(cpu))
				continue;
			if (num_cpus >= ARRAY_SIZE(apic_ids))
				break;
			if (cpu->path.apic.thread_id != thread_id)
				continue;
			apic_ids[num_cpus++] = cpu->path.apic.apic_id;
		}
		bubblesort(&apic_ids[sort_start], num_cpus - sort_start, NUM_ASCENDING);
		sort_start = num_cpus;
	}
	for (index = 0; index < num_cpus; index++)
		current = acpi_create_madt_one_lapic(current, index, apic_ids[index]);

	return current;
}

static int acpi_create_madt_ioapic(acpi_madt_ioapic_t *ioapic, u8 id, u32 addr,
				u32 gsi_base)
{
	ioapic->type = IO_APIC; /* I/O APIC structure */
	ioapic->length = sizeof(acpi_madt_ioapic_t);
	ioapic->reserved = 0x00;
	ioapic->gsi_base = gsi_base;
	ioapic->ioapic_id = id;
	ioapic->ioapic_addr = addr;

	return ioapic->length;
}

/* For a system with multiple I/O APICs it's required that the one potentially
   routing i8259 via ExtNMI delivery calls this first to get GSI #0. */
int acpi_create_madt_ioapic_from_hw(acpi_madt_ioapic_t *ioapic, u32 addr)
{
	static u32 gsi_base;
	u32 my_base;
	u8 id = get_ioapic_id((uintptr_t)addr);
	u8 count = ioapic_get_max_vectors((uintptr_t)addr);

	my_base = gsi_base;
	gsi_base += count;
	return acpi_create_madt_ioapic(ioapic, id, addr, my_base);
}

static int acpi_create_madt_irqoverride(acpi_madt_irqoverride_t *irqoverride,
		u8 bus, u8 source, u32 gsirq, u16 flags)
{
	irqoverride->type = IRQ_SOURCE_OVERRIDE; /* Interrupt source override */
	irqoverride->length = sizeof(acpi_madt_irqoverride_t);
	irqoverride->bus = bus;
	irqoverride->source = source;
	irqoverride->gsirq = gsirq;
	irqoverride->flags = flags;

	return irqoverride->length;
}

static int acpi_create_madt_sci_override(acpi_madt_irqoverride_t *irqoverride)
{
	u8 gsi, irq, flags;

	ioapic_get_sci_pin(&gsi, &irq, &flags);

	if (!CONFIG(ACPI_HAVE_PCAT_8259))
		irq = gsi;

	return acpi_create_madt_irqoverride(irqoverride, MP_BUS_ISA, irq, gsi, flags);
}

static unsigned long acpi_create_madt_ioapic_gsi0_default(unsigned long current)
{
	current += acpi_create_madt_ioapic_from_hw((acpi_madt_ioapic_t *)current, IO_APIC_ADDR);

	current += acpi_create_madt_irqoverride((void *)current, MP_BUS_ISA, 0, 2,
						MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH);

	current += acpi_create_madt_sci_override((void *)current);

	return current;
}

static int acpi_create_madt_lapic_nmi(acpi_madt_lapic_nmi_t *lapic_nmi, u8 cpu,
				u16 flags, u8 lint)
{
	lapic_nmi->type = LOCAL_APIC_NMI; /* Local APIC NMI structure */
	lapic_nmi->length = sizeof(acpi_madt_lapic_nmi_t);
	lapic_nmi->flags = flags;
	lapic_nmi->processor_id = cpu;
	lapic_nmi->lint = lint;

	return lapic_nmi->length;
}

static int acpi_create_madt_lx2apic_nmi(acpi_madt_lx2apic_nmi_t *lapic_nmi, u32 cpu,
				 u16 flags, u8 lint)
{
	lapic_nmi->type = LOCAL_X2APIC_NMI; /* Local APIC NMI structure */
	lapic_nmi->length = sizeof(acpi_madt_lx2apic_nmi_t);
	lapic_nmi->flags = flags;
	lapic_nmi->processor_id = cpu;
	lapic_nmi->lint = lint;
	lapic_nmi->reserved[0] = 0;
	lapic_nmi->reserved[1] = 0;
	lapic_nmi->reserved[2] = 0;

	return lapic_nmi->length;
}

unsigned long acpi_create_madt_lapic_nmis(unsigned long current)
{
	const u16 flags = MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH;

	/* 1: LINT1 connect to NMI */
	/* create all subtables for processors */
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)current,
			ACPI_MADT_LAPIC_NMI_ALL_PROCESSORS, flags, 1);

	if (!CONFIG(XAPIC_ONLY))
		current += acpi_create_madt_lx2apic_nmi((acpi_madt_lx2apic_nmi_t *)current,
			ACPI_MADT_LX2APIC_NMI_ALL_PROCESSORS, flags, 1);

	return current;
}

static unsigned long acpi_create_madt_lapics_with_nmis(unsigned long current)
{
	current = acpi_create_madt_lapics(current);
	current = acpi_create_madt_lapic_nmis(current);
	return current;
}

int acpi_create_srat_lapic(acpi_srat_lapic_t *lapic, u8 node, u8 apic)
{
	memset((void *)lapic, 0, sizeof(acpi_srat_lapic_t));

	lapic->type = 0; /* Processor local APIC/SAPIC affinity structure */
	lapic->length = sizeof(acpi_srat_lapic_t);
	lapic->flags = (1 << 0); /* Enabled (the use of this structure). */
	lapic->proximity_domain_7_0 = node;
	/* TODO: proximity_domain_31_8, local SAPIC EID, clock domain. */
	lapic->apic_id = apic;

	return lapic->length;
}

int acpi_create_srat_x2apic(acpi_srat_x2apic_t *x2apic, u32 node, u32 apic)
{
	memset((void *)x2apic, 0, sizeof(acpi_srat_x2apic_t));

	x2apic->type = 2; /* Processor x2APIC structure */
	x2apic->length = sizeof(acpi_srat_x2apic_t);
	x2apic->flags = (1 << 0); /* Enabled (the use of this structure). */
	x2apic->proximity_domain = node;
	x2apic->x2apic_id = apic;

	return x2apic->length;
}

unsigned long acpi_arch_fill_madt(acpi_madt_t *madt, unsigned long current)
{
	madt->lapic_addr = cpu_get_lapic_addr();

	if (CONFIG(ACPI_HAVE_PCAT_8259))
		madt->flags |= ACPI_MADT_PCAT_COMPAT;

	if (CONFIG(ACPI_COMMON_MADT_LAPIC))
		current = acpi_create_madt_lapics_with_nmis(current);

	if (CONFIG(ACPI_COMMON_MADT_IOAPIC))
		current = acpi_create_madt_ioapic_gsi0_default(current);

	return current;
}
