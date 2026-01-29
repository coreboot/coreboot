/* SPDX-License-Identifier: GPL-2.0-only */

/* Driver for Genesys Logic GL9763E */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <acpi/acpi.h>
#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include "gl9763e.h"

static void gl9763e_init(struct device *dev)
{
	uint32_t ver, value;
	int aer;

	printk(BIOS_INFO, "GL9763E: init\n");
	pci_dev_init(dev);

	/* Set VHS (Vendor Header Space) to be writable */
	pci_update_config32(dev, VHS, ~VHS_REV_MASK, VHS_REV_W);
	/* Set single AXI request */
	pci_or_config32(dev, SCR, SCR_AXI_REQ);
	/* Disable L0s support */
	pci_and_config32(dev, CFG_REG_2, ~CFG_REG_2_L0S);

	if (CONFIG(DRIVERS_GENESYSLOGIC_GL9763E_L1_MAX))
		/* Set  L1 entry delay to MAX */
		pci_or_config32(dev, CFG_REG_2, CFG_REG_2_L1DLY_MAX);

	/* Set SSC to 30000 ppm */
	pci_update_config32(dev, PLL_CTL_2, ~PLL_CTL_2_MAX_SSC_MASK, MAX_SSC_30000PPM);
	/* Enable SSC */
	pci_or_config32(dev, PLL_CTL, PLL_CTL_SSC);
	/* Check chip version */
	ver = pci_read_config32(dev, HW_VER_2);
	if ((ver & HW_VER_MASK) == REVISION_03) {
		/* Set clock source for RX path */
		pci_update_config32(dev, SD_CLKRX_DLY, ~CLK_SRC_MASK, AFTER_OUTPUT_BUFF);
	}
	/* Modify DS delay */
	pci_update_config32(dev, SD_CLKRX_DLY, ~HS400_RX_DELAY_MASK, HS400_RX_DELAY);
	/* Disable Slow mode */
	pci_and_config32(dev, EMMC_CTL, ~SLOW_MODE);
	/* mask the replay timer timeout of AER */
	aer = pciexp_find_extended_cap(dev, PCI_EXT_CAP_ID_ERR, 0);
	if (aer) {
		value = pci_read_config32(dev, aer + PCI_ERR_COR_MASK);
		value |= PCI_ERR_COR_REP_TIMER;
		pci_write_config32(dev, aer + PCI_ERR_COR_MASK, value);
	}
	/* Set VHS to read-only */
	pci_update_config32(dev, VHS, ~VHS_REV_MASK, VHS_REV_R);
}

/*
 * Generate ACPI device for on-board eMMC (SD Host Controller + fixed CARD) under the
 * root port so the OS sees non-removable storage. Scope is the parent root port
 * (e.g. \_SB.PCI0.RP09).
 *
 * Generated ASL:
 * Scope (\_SB.PCI0.RPxx) {
 *     Device (EMMC) {
 *         Name (_ADR, 0x00000000)
 *         Device (CARD) {
 *             Name (_ADR, 0x00000008)
 *             Method (_RMV, 0, NotSerialized) {
 *                 Return (0)  // Fixed (not removable)
 *             }
 *         }
 *     }
 * }
 */
static void gl9763e_fill_ssdt(const struct device *dev)
{
	const char *scope;

	if (!is_dev_enabled(dev))
		return;

	scope = acpi_device_scope(dev);
	if (!scope)
		return;

	/* Emit EMMC device under root port so OS sees fixed (non-removable) storage */
	acpigen_write_scope(scope);
	acpigen_write_device("EMMC");
	acpigen_write_name_integer("_ADR", 0x0);

	/* CARD child: slot at address 0x08, not removable */
	acpigen_write_device("CARD");
	acpigen_write_name_integer("_ADR", 0x8);
	acpigen_write_method_serialized("_RMV", 0);
	acpigen_write_return_integer(0);  /* Fixed (not removable) */
	acpigen_write_method_end();
	acpigen_write_device_end();  /* CARD */

	acpigen_write_device_end();  /* EMMC */
	acpigen_write_scope_end();
}

static struct device_operations gl9763e_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.ops_pci		= &pci_dev_ops_pci,
	.init			= gl9763e_init,
	.acpi_fill_ssdt		= gl9763e_fill_ssdt,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_GLI_9763E,
	0
};

static const struct pci_driver genesyslogic_gl9763e __pci_driver = {
	.ops		= &gl9763e_ops,
	.vendor		= PCI_VID_GLI,
	.devices	= pci_device_ids,
};

struct chip_operations drivers_generic_genesyslogic_ops = {
	.name = "Genesys Logic GL9763E",
};
