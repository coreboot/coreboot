/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <acpi/acpi_device.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pci.h>
#include <intelblocks/pmc.h>
#include <intelblocks/pmc_ipc.h>
#include "chip.h"

/*
 * The "ExternalFacingPort" and "HotPlugSupportInD3" properties are defined at
 * https://docs.microsoft.com/en-us/windows-hardware/drivers/pci/dsd-for-pcie-root-ports
 */
#define PCIE_EXTERNAL_PORT_UUID "EFCC06CC-73AC-4BC3-BFF0-76143807C389"
#define PCIE_EXTERNAL_PORT_PROPERTY "ExternalFacingPort"

#define PCIE_HOTPLUG_IN_D3_UUID "6211E2C0-58A3-4AF3-90E1-927A4E0C55A4"
#define PCIE_HOTPLUG_IN_D3_PROPERTY "HotPlugSupportInD3"

/*
 * This UUID and the resulting ACPI Device Property is defined by the
 * Power Management for Storage Hardware Devices:
 *
 * https://docs.microsoft.com/en-us/windows-hardware/design/component-guidelines/power-management-for-storage-hardware-devices-intro
 */
#define PCIE_RTD3_STORAGE_UUID "5025030F-842F-4AB4-A561-99A5189762D0"
#define PCIE_RTD3_STORAGE_PROPERTY "StorageD3Enable"

/* PCIe Root Port registers for link status and L23 control. */
#define PCH_PCIE_CFG_LSTS 0x52	  /* Link Status Register */
#define PCH_PCIE_CFG_SPR 0xe0	  /* Scratchpad */
#define PCH_PCIE_CFG_RPPGEN 0xe2  /* Root Port Power Gating Enable */
#define PCH_PCIE_CFG_LCAP_PN 0x4f /* Root Port Number */

/* ACPI register names corresponding to PCIe root port registers. */
#define ACPI_REG_PCI_LINK_ACTIVE "LASX"	   /* Link active status */
#define ACPI_REG_PCI_L23_RDY_ENTRY "L23E"  /* L23_Rdy Entry Request */
#define ACPI_REG_PCI_L23_RDY_DETECT "L23R" /* L23_Rdy Detect Transition */
#define ACPI_REG_PCI_L23_SAVE_STATE "NCB7" /* Scratch bit to save L23 state */

/* Called from _ON to get PCIe link back to active state. */
static void pcie_rtd3_acpi_l23_exit(void)
{
	/* Skip if port is not in L2/L3. */
	acpigen_write_if_lequal_namestr_int(ACPI_REG_PCI_L23_SAVE_STATE, 1);

	/* Initiate L2/L3 Ready To Detect transition. */
	acpigen_write_store_int_to_namestr(1, ACPI_REG_PCI_L23_RDY_DETECT);

	/* Wait for transition to detect. */
	acpigen_write_delay_until_namestr_int(320, ACPI_REG_PCI_L23_RDY_DETECT, 0);

	acpigen_write_store_int_to_namestr(0, ACPI_REG_PCI_L23_SAVE_STATE);

	/* Once in detect, wait for link active. */
	acpigen_write_delay_until_namestr_int(128, ACPI_REG_PCI_LINK_ACTIVE, 1);

	acpigen_pop_len(); /* If */
}

/* Called from _OFF to put PCIe link into L2/L3 state. */
static void pcie_rtd3_acpi_l23_entry(void)
{
	/* Initiate L2/L3 Entry request. */
	acpigen_write_store_int_to_namestr(1, ACPI_REG_PCI_L23_RDY_ENTRY);

	/* Wait for L2/L3 Entry request to clear. */
	acpigen_write_delay_until_namestr_int(128, ACPI_REG_PCI_L23_RDY_ENTRY, 0);

	acpigen_write_store_int_to_namestr(1, ACPI_REG_PCI_L23_SAVE_STATE);
}

static void
pcie_rtd3_acpi_method_on(unsigned int pcie_rp,
			 const struct soc_intel_common_block_pcie_rtd3_config *config)
{
	acpigen_write_method_serialized("_ON", 0);

	/* Assert enable GPIO to turn on device power. */
	if (config->enable_gpio.pin_count) {
		acpigen_enable_tx_gpio(&config->enable_gpio);
		if (config->enable_delay_ms)
			acpigen_write_sleep(config->enable_delay_ms);
	}

	/* Enable SRCCLK for root port if pin is defined. */
	if (config->srcclk_pin >= 0)
		pmc_ipc_acpi_set_pci_clock(pcie_rp, config->srcclk_pin, true);

	/* De-assert reset GPIO to bring device out of reset. */
	if (config->reset_gpio.pin_count) {
		acpigen_disable_tx_gpio(&config->reset_gpio);
		if (config->reset_delay_ms)
			acpigen_write_sleep(config->reset_delay_ms);
	}

	/* Trigger L23 ready exit flow unless disabld by config. */
	if (!config->disable_l23)
		pcie_rtd3_acpi_l23_exit();

	acpigen_pop_len(); /* Method */
}

static void
pcie_rtd3_acpi_method_off(int pcie_rp,
			  const struct soc_intel_common_block_pcie_rtd3_config *config)
{
	acpigen_write_method_serialized("_OFF", 0);

	/* Trigger L23 ready entry flow unless disabled by config. */
	if (!config->disable_l23)
		pcie_rtd3_acpi_l23_entry();

	/* Assert reset GPIO to place device into reset. */
	if (config->reset_gpio.pin_count) {
		acpigen_enable_tx_gpio(&config->reset_gpio);
		if (config->reset_off_delay_ms)
			acpigen_write_sleep(config->reset_off_delay_ms);
	}

	/* Disable SRCCLK for this root port if pin is defined. */
	if (config->srcclk_pin >= 0)
		pmc_ipc_acpi_set_pci_clock(pcie_rp, config->srcclk_pin, false);

	/* De-assert enable GPIO to turn off device power. */
	if (config->enable_gpio.pin_count) {
		acpigen_disable_tx_gpio(&config->enable_gpio);
		if (config->enable_off_delay_ms)
			acpigen_write_sleep(config->enable_off_delay_ms);
	}

	acpigen_pop_len(); /* Method */
}

static void
pcie_rtd3_acpi_method_status(int pcie_rp,
			     const struct soc_intel_common_block_pcie_rtd3_config *config)
{
	const struct acpi_gpio *gpio;

	acpigen_write_method("_STA", 0);

	/* Use enable GPIO for status if provided, otherwise use reset GPIO. */
	if (config->enable_gpio.pin_count)
		gpio = &config->enable_gpio;
	else
		gpio = &config->reset_gpio;

	/* Read current GPIO value into Local0. */
	acpigen_get_tx_gpio(gpio);

	/* Ensure check works for both active low and active high GPIOs. */
	acpigen_write_store_int_to_op(gpio->active_low, LOCAL1_OP);

	acpigen_write_if_lequal_op_op(LOCAL0_OP, LOCAL1_OP);
	acpigen_write_return_op(ZERO_OP);
	acpigen_pop_len(); /* If */
	acpigen_write_else();
	acpigen_write_return_op(ONE_OP);
	acpigen_pop_len(); /* Else */

	acpigen_pop_len(); /* Method */
}

static void pcie_rtd3_acpi_fill_ssdt(const struct device *dev)
{
	const struct soc_intel_common_block_pcie_rtd3_config *config = config_of(dev);
	static const char *const power_res_states[] = {"_PR0"};
	const struct device *parent = dev->bus->dev;
	const char *scope = acpi_device_path(parent);
	const struct opregion opregion = OPREGION("PXCS", PCI_CONFIG, 0, 0xff);
	const struct fieldlist fieldlist[] = {
		FIELDLIST_OFFSET(PCH_PCIE_CFG_LSTS),
		FIELDLIST_RESERVED(13),
		FIELDLIST_NAMESTR(ACPI_REG_PCI_LINK_ACTIVE, 1),
		FIELDLIST_OFFSET(PCH_PCIE_CFG_SPR),
		FIELDLIST_RESERVED(7),
		FIELDLIST_NAMESTR(ACPI_REG_PCI_L23_SAVE_STATE, 1),
		FIELDLIST_OFFSET(PCH_PCIE_CFG_RPPGEN),
		FIELDLIST_RESERVED(2),
		FIELDLIST_NAMESTR(ACPI_REG_PCI_L23_RDY_ENTRY, 1),
		FIELDLIST_NAMESTR(ACPI_REG_PCI_L23_RDY_DETECT, 1),
	};
	uint8_t pcie_rp;
	struct acpi_dp *dsd, *pkg;

	if (!is_dev_enabled(parent)) {
		printk(BIOS_ERR, "%s: root port not enabled\n", __func__);
		return;
	}
	if (!scope) {
		printk(BIOS_ERR, "%s: root port scope not found\n", __func__);
		return;
	}
	if (!config->enable_gpio.pin_count && !config->reset_gpio.pin_count) {
		printk(BIOS_ERR, "%s: Enable and/or Reset GPIO required for %s.\n",
		       __func__, scope);
		return;
	}
	if (config->srcclk_pin > CONFIG_MAX_PCIE_CLOCKS) {
		printk(BIOS_ERR, "%s: Invalid clock pin %u for %s.\n", __func__,
		       config->srcclk_pin, scope);
		return;
	}

	/* Read port number of root port that this device is attached to. */
	pcie_rp = pci_read_config8(parent, PCH_PCIE_CFG_LCAP_PN);
	if (pcie_rp == 0 || pcie_rp > CONFIG_MAX_ROOT_PORTS) {
		printk(BIOS_ERR, "%s: Invalid root port number: %u\n", __func__, pcie_rp);
		return;
	}
	/* Port number is 1-based, PMC IPC method expects 0-based. */
	pcie_rp--;

	printk(BIOS_INFO, "%s: Enable RTD3 for %s (%s)\n", scope, dev_path(parent),
	       config->desc ?: dev->chip_ops->name);

	/* The RTD3 power resource is added to the root port, not the device. */
	acpigen_write_scope(scope);

	if (config->desc)
		acpigen_write_name_string("_DDN", config->desc);

	acpigen_write_opregion(&opregion);
	acpigen_write_field("PXCS", fieldlist, ARRAY_SIZE(fieldlist),
			    FIELD_ANYACC | FIELD_NOLOCK | FIELD_PRESERVE);

	/* ACPI Power Resource for controlling the attached device power. */
	acpigen_write_power_res("RTD3", 0, 0, power_res_states, ARRAY_SIZE(power_res_states));
	pcie_rtd3_acpi_method_status(pcie_rp, config);
	pcie_rtd3_acpi_method_on(pcie_rp, config);
	pcie_rtd3_acpi_method_off(pcie_rp, config);
	acpigen_pop_len(); /* PowerResource */

	/* Indicate to the OS that device supports hotplug in D3. */
	dsd = acpi_dp_new_table("_DSD");
	pkg = acpi_dp_new_table(PCIE_HOTPLUG_IN_D3_UUID);
	acpi_dp_add_integer(pkg, PCIE_HOTPLUG_IN_D3_PROPERTY, 1);
	acpi_dp_add_package(dsd, pkg);

	/* Indicate to the OS if the device provides an External facing port. */
	if (config->is_external) {
		pkg = acpi_dp_new_table(PCIE_EXTERNAL_PORT_UUID);
		acpi_dp_add_integer(pkg, PCIE_EXTERNAL_PORT_PROPERTY, 1);
		acpi_dp_add_package(dsd, pkg);
	}
	acpi_dp_write(dsd);

	/*
	 * Check the sibling device on the root port to see if it is storage class and add the
	 * property for the OS to enable storage D3, or allow it to be enabled by config.
	 */
	if (config->is_storage
	    || (dev->sibling && (dev->sibling->class >> 16) == PCI_BASE_CLASS_STORAGE)) {
		acpigen_write_device(acpi_device_name(dev));
		acpigen_write_ADR(0);
		acpigen_write_STA(ACPI_STATUS_DEVICE_ALL_ON);
		acpigen_write_name_integer("_S0W", 4);

		dsd = acpi_dp_new_table("_DSD");
		pkg = acpi_dp_new_table(PCIE_RTD3_STORAGE_UUID);
		acpi_dp_add_integer(pkg, PCIE_RTD3_STORAGE_PROPERTY, 1);
		acpi_dp_add_package(dsd, pkg);
		acpi_dp_write(dsd);

		acpigen_pop_len(); /* Device */

		printk(BIOS_INFO, "%s: Added StorageD3Enable property\n", scope);
	}

	acpigen_pop_len(); /* Scope */
}

static const char *pcie_rtd3_acpi_name(const struct device *dev)
{
	/* Attached device name must be "PXSX" for the Linux Kernel to recognize it. */
	return "PXSX";
}

static struct device_operations pcie_rtd3_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.acpi_fill_ssdt = pcie_rtd3_acpi_fill_ssdt,
	.acpi_name = pcie_rtd3_acpi_name,
};

static void pcie_rtd3_acpi_enable(struct device *dev)
{
	dev->ops = &pcie_rtd3_ops;
}

struct chip_operations soc_intel_common_block_pcie_rtd3_ops = {
	CHIP_NAME("Intel PCIe Runtime D3")
	.enable_dev = pcie_rtd3_acpi_enable
};
