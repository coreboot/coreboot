/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <amdblocks/amd_pci_util.h>
#include <arch/ioapic.h>
#include <device/device.h>

/* GNB IO-APIC is located after the FCH IO-APIC */
#define FCH_IOAPIC_INTERRUPTS	24
#define GNB_GSI_BASE		FCH_IOAPIC_INTERRUPTS

__weak unsigned int soc_get_gsi_base(const struct device *dev)
{
	return GNB_GSI_BASE;
}

static void acpigen_write_PRT_GSI(const struct device *dev,
				  const struct pci_routing_info *routing_info)
{
	unsigned int irq;

	acpigen_write_package(4); /* Package - APIC Routing */
	for (unsigned int i = 0; i < 4; ++i) {
		irq = pci_calculate_irq(routing_info, i);

		acpigen_write_PRT_GSI_entry(
			0, /* There is only one device attached to the bridge */
			i, /* pin */
			soc_get_gsi_base(dev) + irq);
	}
	acpigen_pop_len(); /* Package - APIC Routing */
}

static void acpigen_write_PRT_PIC(const struct pci_routing_info *routing_info)
{
	unsigned int irq;
	char link_template[] = "\\_SB.INTX";

	acpigen_write_package(4); /* Package - PIC Routing */
	for (unsigned int i = 0; i < 4; ++i) {
		irq = pci_calculate_irq(routing_info, i);

		link_template[8] = 'A' + (irq % 8);

		acpigen_write_PRT_source_entry(
			0, /* There is only one device attached to the bridge */
			i, /* pin */
			link_template /* Source */,
			0 /* Source Index */);
	}
	acpigen_pop_len(); /* Package - PIC Routing */
}

/*
 * This method writes a PCI _PRT table that uses the GNB IO-APIC / PIC :
 *     Method (_PRT, 0, NotSerialized)  // _PRT: PCI Routing Table
 *     {
 *         If (PICM)
 *         {
 *             Return (Package (0x04)
 *             {
 *                 Package (0x04)
 *                 {
 *                     0x0000FFFF,
 *                     0x00,
 *                     0x00,
 *                     0x00000034
 *                 },
 *
 *                 Package (0x04)
 *                 {
 *                     0x0000FFFF,
 *                     0x01,
 *                     0x00,
 *                     0x00000035
 *                 },
 *
 *                 Package (0x04)
 *                 {
 *                     0x0000FFFF,
 *                     0x02,
 *                     0x00,
 *                     0x00000036
 *                 },
 *
 *                 Package (0x04)
 *                 {
 *                     0x0000FFFF,
 *                     0x03,
 *                     0x00,
 *                     0x00000037
 *                 }
 *             })
 *         }
 *         Else
 *         {
 *             Return (Package (0x04)
 *             {
 *                 Package (0x04)
 *                 {
 *                     0x0000FFFF,
 *                     0x00,
 *                     \_SB.INTA,
 *                     0x00000000
 *                 },
 *
 *                 Package (0x04)
 *                 {
 *                     0x0000FFFF,
 *                     0x01,
 *                     \_SB.INTB,
 *                     0x00000000
 *                 },
 *
 *                 Package (0x04)
 *                 {
 *                     0x0000FFFF,
 *                     0x02,
 *                     \_SB.INTC,
 *                     0x00000000
 *                 },
 *
 *                 Package (0x04)
 *                 {
 *                     0x0000FFFF,
 *                     0x03,
 *                     \_SB.INTD,
 *                     0x00000000
 *                 }
 *             })
 *         }
 *    }
 */
void acpigen_write_pci_GNB_PRT(const struct device *dev)
{
	const struct pci_routing_info *routing_info =
		get_pci_routing_info(dev->path.pci.devfn);

	if (!routing_info)
		return;

	acpigen_write_method("_PRT", 0);

	/* If (PICM) */
	acpigen_write_if();
	acpigen_emit_namestring("PICM");

	/* Return (Package{...}) */
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_PRT_GSI(dev, routing_info);

	/* Else */
	acpigen_write_else();

	/* Return (Package{...}) */
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_PRT_PIC(routing_info);

	acpigen_pop_len(); /* End Else */

	acpigen_pop_len(); /* Method */
}

/* Counts table rows with a valid bridge_irq; used only for Package() element count. */
static size_t count_pci_root_prt_entries(const struct pci_routing_info *routing_table,
					 size_t routing_table_entries)
{
	size_t entries = 0;

	for (size_t i = 0; i < routing_table_entries; ++i) {
		if (routing_table[i].bridge_irq != UINT8_MAX)
			entries++;
	}

	return entries;
}

/*
 * Return payload for the PICM branch: one Package per root-port devfn using GSIs:
 *     Package (0xNN)                  // NN = number of root ports
 *     {
 *         Package (0x04)
 *         {
 *             0xDDDDFFFF,
 *             0xPP,
 *             0x00,
 *             0x000000GG
 *         },
 *         ...
 *     }
 */
static void acpigen_write_root_PRT_GSI(const struct pci_routing_info *routing_table,
				       size_t routing_table_entries, size_t prt_entries)
{
	acpigen_write_package(prt_entries); /* Package - APIC Routing */
	for (size_t i = 0; i < routing_table_entries; ++i) {
		if (routing_table[i].bridge_irq == UINT8_MAX)
			continue;

		/*
		 * The HOB's bridge_irq/map byte is not an OS-visible APIC GSI here.
		 * Route the root port's own INTx through its GNB group/swizzle instead.
		 */
		acpigen_write_PRT_GSI_entry_devfn(routing_table[i].devfn,
						  0, /* root port interrupt pin A */
						  GNB_GSI_BASE + pci_calculate_irq(&routing_table[i], 0));
	}
	acpigen_pop_len(); /* Package - APIC Routing */
}

/*
 * Return payload for the PIC branch: one Package per root-port devfn using link objects:
 *     Package (0xNN)
 *     {
 *         Package (0x04)
 *         {
 *             0xDDDDFFFF,
 *             0xPP,
 *             \_SB.INTx,
 *             0x00000000
 *         },
 *         ...
 *     }
 */
static void acpigen_write_root_PRT_PIC(const struct pci_routing_info *routing_table,
				       size_t routing_table_entries, size_t prt_entries)
{
	char link_template[] = "\\_SB.INTX";
	unsigned int irq;

	acpigen_write_package(prt_entries); /* Package - PIC Routing */
	for (size_t i = 0; i < routing_table_entries; ++i) {
		if (routing_table[i].bridge_irq == UINT8_MAX)
			continue;

		irq = pci_calculate_irq(&routing_table[i], 0);
		link_template[8] = 'A' + (irq % 8);
		acpigen_write_PRT_source_entry_devfn(routing_table[i].devfn,
						     0, /* root port interrupt pin A */
						     link_template,
						     0);
	}
	acpigen_pop_len(); /* Package - PIC Routing */
}

/*
 * Host-bridge _PRT for root-bus devices (per devfn from the routing table)
 *
 *     Method (_PRT, 0, NotSerialized)
 *     {
 *         If (PICM)
 *         {
 *             Return (Package (0xNN)
 *             {
 *                 Package (0x04) { 0xDDDDFFFF, 0xPP, 0x00, 0x000000GG },
 *                 ...
 *             })
 *         }
 *         Else
 *         {
 *             Return (Package (0xNN)
 *             {
 *                 Package (0x04) { 0xDDDDFFFF, 0xPP, \_SB.INTx, 0x00000000 },
 *                 ...
 *             })
 *         }
 *     }
 */
void acpigen_write_pci_root_PRT(void)
{
	const struct pci_routing_info *routing_table;
	size_t routing_table_entries = 0;
	size_t prt_entries;

	routing_table = get_pci_routing_table(&routing_table_entries);
	if (!routing_table || !routing_table_entries)
		return;

	prt_entries = count_pci_root_prt_entries(routing_table, routing_table_entries);
	if (!prt_entries)
		return;

	acpigen_write_method("_PRT", 0);

	/* If (PICM) */
	acpigen_write_if();
	acpigen_emit_namestring("PICM");

	/* Return (Package{...}) */
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_root_PRT_GSI(routing_table, routing_table_entries, prt_entries);

	/* Else */
	acpigen_write_else();

	/* Return (Package{...}) */
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_root_PRT_PIC(routing_table, routing_table_entries, prt_entries);

	acpigen_pop_len(); /* End Else */

	acpigen_pop_len(); /* Method */
}

 /*
 * This method writes a PCI _PRT table that uses the FCH IO-APIC / PIC :
 *     Name (_PRT, Package (0x04)
 *     {
 *         Package (0x04)
 *         {
 *             0x0000FFFF,
 *             0x00,
 *             \_SB.INTA,
 *             0x00000000
 *         },
 *
 *         Package (0x04)
 *         {
 *             0x0000FFFF,
 *             0x01,
 *             \_SB.INTB,
 *             0x00000000
 *         },
 *
 *         Package (0x04)
 *         {
 *             0x0000FFFF,
 *             0x02,
 *             \_SB.INTC,
 *             0x00000000
 *         },
 *
 *         Package (0x04)
 *         {
 *             0x0000FFFF,
 *             0x03,
 *             \_SB.INTD,
 *             0x00000000
 *         }
 *     })
 */
void acpigen_write_pci_FCH_PRT(const struct device *dev)
{
	const struct pci_routing_info *routing_info =
		get_pci_routing_info(dev->path.pci.devfn);

	if (!routing_info)
		return;

	acpigen_write_name("_PRT");
	acpigen_write_PRT_PIC(routing_info);
}
