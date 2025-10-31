/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <amdblocks/sata.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <soc/southbridge.h>

void soc_enable_sata_features(struct device *dev)
{
	u8 *ahci_ptr;
	u32 misc_ctl, cap_cfg;

	u32 temp;

	/* unlock the write-protect */
	misc_ctl = pci_read_config32(dev, SATA_MISC_CONTROL_REG);
	misc_ctl |= SATA_MISC_SUBCLASS_WREN;
	pci_write_config32(dev, SATA_MISC_CONTROL_REG, misc_ctl);

	/* set the SATA AHCI mode to allow port expanders */
	ahci_ptr = (u8 *)(uintptr_t)ALIGN_DOWN(
		pci_read_config32(dev, PCI_BASE_ADDRESS_5), 256);

	cap_cfg = read32(ahci_ptr + SATA_CAPABILITIES_REG);
	cap_cfg |= SATA_CAPABILITY_SPM;
	write32(ahci_ptr + SATA_CAPABILITIES_REG, cap_cfg);

	/* lock the write-protect */
	temp = pci_read_config32(dev, SATA_MISC_CONTROL_REG);
	temp &= ~SATA_MISC_SUBCLASS_WREN;
	pci_write_config32(dev, SATA_MISC_CONTROL_REG, temp);
}

#if CONFIG(HAVE_ACPI_TABLES)

/*
 * Generate ACPI SATA OperationRegion and Fields for port status
 * This is only generated if the SATA device (00:11.0) is enabled
 */
static void sata_acpi_fill_ssdt(const struct device *dev)
{
	const char *scope;

	if (!dev->enabled)
		return;

	/* First generate the standard PCI device info */
	acpi_device_write_pci_dev(dev);

	/* Get the scope for the SATA device */
	scope = acpi_device_path(dev);
	if (!scope)
		return;

	acpigen_write_scope(scope);

	/*
	 * Generate Field in PCFG OperationRegion for SATA BAR5 register
	 * PCFG is the PCI config space OperationRegion defined elsewhere
	 *
	 * Field(PCFG, ByteAcc, NoLock, Preserve) {
	 *	Offset(0x00088024),
	 *	STB5, 32,
	 * }
	 *
	 */
	struct fieldlist sata_pcfg_field[] = {
		FIELDLIST_OFFSET(0x00088024),  /* SATA device register 24h */
		FIELDLIST_NAMESTR("STB5", 32), /* SATA BAR5 (ABAR) */
	};
	acpigen_write_field("PCFG", sata_pcfg_field, ARRAY_SIZE(sata_pcfg_field),
			    FIELD_BYTEACC | FIELD_NOLOCK | FIELD_PRESERVE);

	/*
	 * Generate OperationRegion for SATA AHCI memory space
	 * This maps to the BAR5 (ABAR) memory region:
	 *
	 * OperationRegion(SB5, SystemMemory, STB5, 0x1000)
	 *
	 */
	acpigen_emit_ext_op(OPREGION_OP);
	acpigen_emit_namestring("SB5");
	acpigen_emit_byte(SYSTEMMEMORY);
	acpigen_emit_namestring("STB5");  /* Reference to the STB5 field defined above */
	acpigen_write_integer(0x1000);

	/*
	 * Generate Field in SB5 OperationRegion for SATA port registers
	 * These fields allow access to AHCI port status registers:
	 *
	 * Field(SB5, AnyAcc, NoLock, Preserve) {
	 *	Offset(0x120),
	 *	P0ER, 1,
	 *	, 2,
	 *	P0DQ, 1,
	 *	, 3,
	 *	P0BY, 1,
	 *	Offset(0x128),
	 *	P0DD, 4,
	 *	, 4,
	 *	P0IS, 4,
	 *	Offset(0x12c),
	 *	P0DI, 4,
	 *	Offset(0x130),
	 *	, 16,
	 *	P0PR, 1,
	 *	Offset(0x1a0),
	 *	P1ER, 1,
	 *	, 2,
	 *	P1DQ, 1,
	 *	, 3,
	 *	P1BY, 1,
	 *	Offset(0x1a8),
	 *	P1DD, 4,
	 *	, 4,
	 *	P1IS, 4,
	 *	Offset(0x1ac),
	 *	P1DI, 4,
	 *	Offset(0x1b0),
	 *	, 16,
	 *	P1PR, 1,
	 *	Offset(0x220),
	 *	P2ER, 1,
	 *	, 2,
	 *	P2DQ, 1,
	 *	, 3,
	 *	P2BY, 1,
	 *	Offset(0x228),
	 *	P2DD, 4,
	 *	, 4,
	 *	P2IS, 4,
	 *	Offset(0x22c),
	 *	P2DI, 4,
	 *	Offset(0x230),
	 *	, 16,
	 *	P2PR, 1,
	 *	Offset(0x2a0),
	 *	P3ER, 1,
	 *	, 2,
	 *	P3DQ, 1,
	 *	, 3,
	 *	P3BY, 1,
	 *	Offset(0x2a8),
	 *	P3DD, 4,
	 *	, 4,
	 *	P3IS, 4,
	 *	Offset(0x2ac),
	 *	P3DI, 4,
	 *	Offset(0x2b0),
	 *	, 16,
	 *	P3PR, 1,
	 * }
	 *
	 */
	struct fieldlist sata_port_fields[] = {
		/* Port 0 - offset 0x120 */
		FIELDLIST_OFFSET(0x120),
		FIELDLIST_NAMESTR("P0ER", 1),  /* Error bit */
		FIELDLIST_RESERVED(2),
		FIELDLIST_NAMESTR("P0DQ", 1),  /* Data request */
		FIELDLIST_RESERVED(3),
		FIELDLIST_NAMESTR("P0BY", 1),  /* Busy */
		FIELDLIST_OFFSET(0x128),
		FIELDLIST_NAMESTR("P0DD", 4),  /* Device detection */
		FIELDLIST_RESERVED(4),
		FIELDLIST_NAMESTR("P0IS", 4),  /* Interface speed */
		FIELDLIST_OFFSET(0x12c),
		FIELDLIST_NAMESTR("P0DI", 4),  /* Device idle */
		FIELDLIST_OFFSET(0x130),
		FIELDLIST_RESERVED(16),
		FIELDLIST_NAMESTR("P0PR", 1),  /* Port ready */

		/* Port 1 - offset 0x1a0 */
		FIELDLIST_OFFSET(0x1a0),
		FIELDLIST_NAMESTR("P1ER", 1),
		FIELDLIST_RESERVED(2),
		FIELDLIST_NAMESTR("P1DQ", 1),
		FIELDLIST_RESERVED(3),
		FIELDLIST_NAMESTR("P1BY", 1),
		FIELDLIST_OFFSET(0x1a8),
		FIELDLIST_NAMESTR("P1DD", 4),
		FIELDLIST_RESERVED(4),
		FIELDLIST_NAMESTR("P1IS", 4),
		FIELDLIST_OFFSET(0x1ac),
		FIELDLIST_NAMESTR("P1DI", 4),
		FIELDLIST_OFFSET(0x1b0),
		FIELDLIST_RESERVED(16),
		FIELDLIST_NAMESTR("P1PR", 1),

		/* Port 2 - offset 0x220 */
		FIELDLIST_OFFSET(0x220),
		FIELDLIST_NAMESTR("P2ER", 1),
		FIELDLIST_RESERVED(2),
		FIELDLIST_NAMESTR("P2DQ", 1),
		FIELDLIST_RESERVED(3),
		FIELDLIST_NAMESTR("P2BY", 1),
		FIELDLIST_OFFSET(0x228),
		FIELDLIST_NAMESTR("P2DD", 4),
		FIELDLIST_RESERVED(4),
		FIELDLIST_NAMESTR("P2IS", 4),
		FIELDLIST_OFFSET(0x22c),
		FIELDLIST_NAMESTR("P2DI", 4),
		FIELDLIST_OFFSET(0x230),
		FIELDLIST_RESERVED(16),
		FIELDLIST_NAMESTR("P2PR", 1),

		/* Port 3 - offset 0x2a0 */
		FIELDLIST_OFFSET(0x2a0),
		FIELDLIST_NAMESTR("P3ER", 1),
		FIELDLIST_RESERVED(2),
		FIELDLIST_NAMESTR("P3DQ", 1),
		FIELDLIST_RESERVED(3),
		FIELDLIST_NAMESTR("P3BY", 1),
		FIELDLIST_OFFSET(0x2a8),
		FIELDLIST_NAMESTR("P3DD", 4),
		FIELDLIST_RESERVED(4),
		FIELDLIST_NAMESTR("P3IS", 4),
		FIELDLIST_OFFSET(0x2ac),
		FIELDLIST_NAMESTR("P3DI", 4),
		FIELDLIST_OFFSET(0x2b0),
		FIELDLIST_RESERVED(16),
		FIELDLIST_NAMESTR("P3PR", 1),
	};
	acpigen_write_field("SB5", sata_port_fields, ARRAY_SIZE(sata_port_fields),
			    FIELD_ANYACC | FIELD_NOLOCK | FIELD_PRESERVE);

	acpigen_pop_len(); /* Scope */
}

static const char *sata_acpi_name(const struct device *dev)
{
	return "STCR";
}

#endif /* CONFIG(HAVE_ACPI_TABLES) */

struct device_operations stoneyridge_sata_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= soc_enable_sata_features,
	.ops_pci		= &pci_dev_ops_pci,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name		= sata_acpi_name,
	.acpi_fill_ssdt		= sata_acpi_fill_ssdt,
#endif
};
