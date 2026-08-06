/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <amdblocks/amd_pci_util.h>
#include <amdblocks/alib.h>
#include <assert.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <soc/pci_devs.h>
#include <stdio.h>
#include <stdlib.h>

static const char *pcie_gpp_acpi_name(const struct device *dev)
{
	char *name;

	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	name = malloc(ACPI_NAME_BUFFER_SIZE);
	snprintf(name, ACPI_NAME_BUFFER_SIZE, "GP%02X", dev->path.pci.devfn);
	name[4] = '\0';

	return name;
}

/*
 * Generate the ACPI methods for PCIe link control and slot control. This includes methods
 * to disable the link, retrain the link, and disable slot presence detect.
 */
static void acpi_device_write_pcie_ops(const struct device *dev)
{
	const size_t pos = pci_find_capability(dev, PCI_CAP_ID_PCIE);
	if (!pos || pos + 48 > 4096)
		return;

	const struct opregion opregion = OPREGION("PCIE", PCI_CONFIG, pos, 48);
	const struct fieldlist fieldlist[] = {
		FIELDLIST_OFFSET(PCI_EXP_LNKCTL),
		FIELDLIST_NAMESTR("APMC", 2),	/* ASPM Control */
		FIELDLIST_RESERVED(2),
		FIELDLIST_NAMESTR("LKDI", 1),	/* Link Disable */
		FIELDLIST_NAMESTR("RTLK", 1),	/* Retrain Link */
		FIELDLIST_OFFSET(PCI_EXP_LNKSTA),
		FIELDLIST_RESERVED(11),
		FIELDLIST_NAMESTR("LKST", 1),	/* Link Status */
		FIELDLIST_RESERVED(1),
		FIELDLIST_NAMESTR("DLLA", 1),	/* Data Link Layer Link Active */
		FIELDLIST_OFFSET(PCI_EXP_SLTCTL),
		FIELDLIST_RESERVED(3),
		FIELDLIST_NAMESTR("PDCE", 1),	/* Presence Detect Changed Enable */
		FIELDLIST_OFFSET(PCI_EXP_SLTSTA),
		FIELDLIST_RESERVED(3),
		FIELDLIST_NAMESTR("PDC", 1),	/* Presence Detect Changed */
	};

	/*
	 * OperationRegion (PCIE, PCI_Config, 0x58, 0x30)
	 * Field (PCIE, DWordAcc, NoLock, Preserve)
	 * {
	 *    Offset (0x10),
	 *    APMC,   2,
	 *    ,   2,
	 *    LKDI,   1,
	 *    RTLK,   1,
	 *    Offset (0x12),
	 *    ,   11,
	 *    LKST,   1,
	 *    ,   1,
	 *    DLLA,   1,
	 *    Offset (0x18),
	 *    ,   3,
	 *    PDCE,   1,
	 *    Offset (0x1A),
	 *    ,   3,
	 *    PDC,    1
	 * }
	*/
	acpigen_write_opregion(&opregion);
	acpigen_write_field("PCIE", fieldlist, ARRAY_SIZE(fieldlist),
				FIELD_DWORDACC | FIELD_NOLOCK | FIELD_PRESERVE);

	/*
	 * Method (LDIS, 0, NotSerialized) - Disable Link
	 * {
	 *    LKDI = One
	 *    Sleep (0x18)
	 *    \_SB.ALIB (0x12, DEVFN)
	 * }
	*/
	acpigen_write_method("LDIS", 0);
	acpigen_write_store_int_to_namestr(1, "LKDI");
	acpigen_write_sleep(24);
	if (CONFIG(SOC_AMD_COMMON_BLOCK_ACPI_ALIB_HAS_PCIE_HOLD))
		acpigen_write_alib_hold_link_training(dev);
	acpigen_pop_len(); /* Method */

	/* Method (RETR, 0) - Enable Link and retrain
	 * {
	 *   \_SB.ALIB (0x13, DEVFN)
	 *   LKDI = Zero
	 *   Sleep (0x18)
	 *   RTLK = Zero
	 *   RTLK = One
	 *   Local7 = 0x01F4
	 *   While ((Local7 > Zero))
	 *   {
	 *     If ((DLLA == One))
	 *     {
	 *       If ((LKST == Zero))
	 *       {
	 *         Break
	 *       }
	 *     }
	 *     Sleep (One)
	 *     Local7--
	 *   }
	 * }
	*/
	acpigen_write_method("RETR", 0);
	if (CONFIG(SOC_AMD_COMMON_BLOCK_ACPI_ALIB_HAS_PCIE_HOLD))
		acpigen_write_alib_release_link_training(dev);

	acpigen_write_store_int_to_namestr(0, "LKDI");
	acpigen_write_sleep(24);
	/* 0->1 transition on Retrain Link */
	acpigen_write_store_int_to_namestr(0, "RTLK");
	acpigen_write_store_int_to_namestr(1, "RTLK");

	/* Wait 500 msec till DLLA=1 and LINK_TRAINING=0 */
	acpigen_write_store_int_to_op(500, LOCAL7_OP);
	acpigen_emit_byte(WHILE_OP);
	acpigen_write_len_f();
	acpigen_emit_byte(LGREATER_OP);
	acpigen_emit_byte(LOCAL7_OP);
	acpigen_emit_byte(ZERO_OP);

	acpigen_write_if_lequal_namestr_int("DLLA", 1);
	acpigen_write_if_lequal_namestr_int("LKST", 0);
	acpigen_emit_byte(BREAK_OP);
	acpigen_pop_len(); /* If */
	acpigen_pop_len(); /* If */

	acpigen_write_sleep(1);
	acpigen_emit_byte(DECREMENT_OP);
	acpigen_emit_byte(LOCAL7_OP);
	acpigen_pop_len(); /* While */
	acpigen_pop_len(); /* Method */

	/* Method (DSLP, 0) - Clear and disable slot presence detect
	 * {
	 *    PDCE = Zero
	 *    PDC = One
	 * }
	 */
	acpigen_write_method("DSLP", 0);
	acpigen_write_store_int_to_namestr(0, "PDCE");
	acpigen_write_store_int_to_namestr(1, "PDC");
	acpigen_pop_len(); /* Method */

	/* Method (DASP, 0, NotSerialized) - Disable ASPM
	 * {
	 *    APMC = Zero
	 * }
	*/
	acpigen_write_method("DASP", 0);
	acpigen_write_store_int_to_namestr(0, "APMC");
	acpigen_pop_len(); /* Method */
}

/*
 * Generate the ACPI methods for PCI power management. This includes methods to set
 * the power state, backup the current power state, and restore the power state.
 */
static void acpi_device_write_pci_pm_ops(const struct device *dev)
{
	const size_t pos = pci_find_capability(dev, PCI_CAP_ID_PM);
	if (!pos || pos + 8 > 4096)
		return;

	const struct opregion opregion = OPREGION("PCIP", PCI_CONFIG, pos, 8);
	const struct fieldlist fieldlist[] = {
		FIELDLIST_OFFSET(PCI_PM_CTRL),
		FIELDLIST_NAMESTR("PMCT", 16),	/* PM control and status register */
	};

	/*
	 * OperationRegion (PCIP, PCI_Config, 0x50, 0x08)
	 * Field (PCIP, DWordAcc, NoLock, Preserve)
	 * {
	 *    Offset (0x04),
	 *    PMCT,   16
	 * }
	 */
	acpigen_write_opregion(&opregion);
	acpigen_write_field("PCIP", fieldlist, ARRAY_SIZE(fieldlist),
				FIELD_DWORDACC | FIELD_NOLOCK | FIELD_PRESERVE);

	/* Method (SPMD, 1, NotSerialized) - Set PM D[0-3] state to Arg0
	 * {
	 *    Local0 = (PMCT & 0x7FFC)
	 *    Local1 = (0x03 & Arg0)
	 *    Local0 |= Local1
	 *    PMCT = Local0
	 *    Sleep (One)
	 * }
	 */
	acpigen_write_method("SPMD", 1);
	acpigen_emit_byte(AND_OP);
	acpigen_emit_namestring("PMCT");
	/* Mask PCI_PM_CTRL_PME_STATUS since it's write one to clear */
	acpigen_write_integer((uint16_t)~(PCI_PM_CTRL_STATE_MASK | PCI_PM_CTRL_PME_STATUS));
	acpigen_emit_byte(LOCAL0_OP);

	acpigen_emit_byte(AND_OP);
	acpigen_write_integer(PCI_PM_CTRL_STATE_MASK);
	acpigen_emit_byte(ARG0_OP);
	acpigen_emit_byte(LOCAL1_OP);

	acpigen_write_or(LOCAL0_OP, LOCAL1_OP, LOCAL0_OP);
	acpigen_write_store_op_to_namestr(LOCAL0_OP, "PMCT");
	acpigen_write_sleep(1);
	acpigen_pop_len(); /* Method */

	/*
	 * Method (STPM, 0, NotSerialized) - Returns PCI_PM_CTRL.
	 * {
	 *    Return (PMCT)
	 * }
	 */
	acpigen_write_method("STPM", 0);
	acpigen_write_return_namestr("PMCT");
	acpigen_pop_len(); /* Method */

	/*
	 * Method (RSPM, 1, NotSerialized) - Restore PCI_PM_CTRL to Arg0
	 * {
	 *    Local0 = 0x7FFF
	 *    Local0 &= Arg0
	 *    PMCT = Local0
	 * }
	 */
	acpigen_write_method("RSPM", 1);
	/* Mask PCI_PM_CTRL_PME_STATUS since it's write one to clear */
	acpigen_write_store_int_to_op((uint16_t)~PCI_PM_CTRL_PME_STATUS, LOCAL0_OP);
	acpigen_write_and(ARG0_OP, LOCAL0_OP, LOCAL0_OP);
	acpigen_write_store_op_to_namestr(LOCAL0_OP, "PMCT");
	acpigen_pop_len(); /* Method */
}

static void acpi_device_write_gpp_pci_dev(const struct device *dev)
{
	const char *path = acpi_device_path(dev);

	assert(dev->path.type == DEVICE_PATH_PCI);
	assert(path);

	acpigen_write_scope(path);

	acpigen_write_store_int_to_namestr(acpi_device_status(dev), "STAT");
	acpigen_write_pci_GNB_PRT(dev);

	acpigen_pop_len(); /* Scope */
}

static void acpi_device_write_ext_gpp_pci_dev(const struct device *dev)
{
	const char *path = acpi_device_path(dev);

	assert(dev->path.type == DEVICE_PATH_PCI);
	assert(path);

	acpigen_write_scope(path);

	acpigen_write_store_int_to_namestr(acpi_device_status(dev), "STAT");
	acpigen_write_pci_GNB_PRT(dev);

	acpi_device_write_pcie_ops(dev);
	acpi_device_write_pci_pm_ops(dev);

	acpigen_pop_len(); /* Scope */
}


/* Latency tolerance reporting, max snoop/non-snoop latency value 1.049ms */
#define PCIE_LTR_MAX_LATENCY_1049US 0x1001

static void pcie_get_ltr_max_latencies(u16 *max_snoop, u16 *max_nosnoop)
{
	*max_snoop = PCIE_LTR_MAX_LATENCY_1049US;
	*max_nosnoop = PCIE_LTR_MAX_LATENCY_1049US;
}

static struct pci_operations pcie_ops = {
	.get_ltr_max_latencies	= pcie_get_ltr_max_latencies,
	.set_subsystem		= pci_dev_set_subsystem,
};

struct device_operations amd_internal_pcie_gpp_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.scan_bus		= pci_scan_bridge,
	.reset_bus		= pci_bus_reset,
	.acpi_name		= pcie_gpp_acpi_name,
	.acpi_fill_ssdt		= acpi_device_write_gpp_pci_dev,
};

struct device_operations amd_external_pcie_gpp_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.scan_bus		= pciexp_scan_bridge,
	.reset_bus		= pci_bus_reset,
	.acpi_name		= pcie_gpp_acpi_name,
	.acpi_fill_ssdt		= acpi_device_write_ext_gpp_pci_dev,
	.ops_pci		= &pcie_ops,
};
