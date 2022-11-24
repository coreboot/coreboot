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
#include <intelblocks/pcie_rp.h>
#include <soc/iomap.h>
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

/* ACPI path to the mutex that protects accesses to PMC ModPhy power gating registers */
#define RTD3_MUTEX_PATH "\\_SB.PCI0.R3MX"

enum modphy_pg_state {
	PG_DISABLE = 0,
	PG_ENABLE = 1,
};

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

/* Called from _ON/_OFF to disable/enable ModPHY power gating */
static void pcie_rtd3_enable_modphy_pg(unsigned int pcie_rp, enum modphy_pg_state state)
{
	/* Enter the critical section */
	acpigen_emit_ext_op(ACQUIRE_OP);
	acpigen_emit_namestring(RTD3_MUTEX_PATH);
	acpigen_emit_word(ACPI_MUTEX_NO_TIMEOUT);

	acpigen_write_store_int_to_namestr(state, "EMPG");
	acpigen_write_delay_until_namestr_int(100, "AMPG", state);

	/* Exit the critical section */
	acpigen_emit_ext_op(RELEASE_OP);
	acpigen_emit_namestring(RTD3_MUTEX_PATH);
}

/* Method to enter L2/L3 */
static void pcie_rtd3_acpi_method_dl23(void)
{
	acpigen_write_method_serialized("DL23", 0);
	pcie_rtd3_acpi_l23_entry();
	acpigen_pop_len(); /* Method */
}

/* Method to exit L2/L3 */
static void pcie_rtd3_acpi_method_l23d(void)
{
	acpigen_write_method_serialized("L23D", 0);
	pcie_rtd3_acpi_l23_exit();
	acpigen_pop_len(); /* Method */
}

/* Method to disable PCH modPHY power gating */
static void pcie_rtd3_acpi_method_pds0(unsigned int pcie_rp)
{
	acpigen_write_method_serialized("PSD0", 0);
	pcie_rtd3_enable_modphy_pg(pcie_rp, PG_DISABLE);
	acpigen_pop_len(); /* Method */
}

/* Method to enable/disable the source clock */
static void pcie_rtd3_acpi_method_srck(unsigned int pcie_rp,
	const struct soc_intel_common_block_pcie_rtd3_config *config)
{
	acpigen_write_method_serialized("SRCK", 1);

	if (config->srcclk_pin >= 0) {
		acpigen_write_if_lequal_op_op(ARG0_OP, 0);
		pmc_ipc_acpi_set_pci_clock(pcie_rp, config->srcclk_pin, false);
		acpigen_write_else();
		pmc_ipc_acpi_set_pci_clock(pcie_rp, config->srcclk_pin, true);
		acpigen_pop_len(); /* If */
	}
	acpigen_pop_len(); /* Method */
}

static void
pcie_rtd3_acpi_method_on(unsigned int pcie_rp,
			 const struct soc_intel_common_block_pcie_rtd3_config *config,
			 enum pcie_rp_type rp_type,
			 const struct device *dev)
{
	const struct device *parent = dev->bus->dev;

	acpigen_write_method_serialized("_ON", 0);

	/* The _STA returns current power status of device, so we can skip _ON
	 * if _STA returns 1
	 * Example:
	 * Local0 = \_SB.PCI0.RP01.RTD3._STA ()
	 * If ((Local0 == One))
	 * {
	 *   Return (One)
	 * }
	 */
	acpigen_write_store();
	acpigen_emit_namestring(acpi_device_path_join(parent, "RTD3._STA"));
	acpigen_emit_byte(LOCAL0_OP);
	acpigen_write_if_lequal_op_int(LOCAL0_OP, ONE_OP);
	acpigen_write_return_op(ONE_OP);
	acpigen_write_if_end();


	/* When this feature is enabled, ONSK indicates if the previous _OFF was
	 * skipped. If so, since the device was not in Off state, and the current
	 * _ON can be skipped as well.
	 */
	if (config->skip_on_off_support)
		acpigen_write_if_lequal_namestr_int("ONSK", 0);

	/* Disable modPHY power gating for PCH RPs. */
	if (rp_type == PCIE_RP_PCH)
		pcie_rtd3_enable_modphy_pg(pcie_rp, PG_DISABLE);

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

	/* Trigger L23 ready exit flow unless disabled by config. */
	if (!config->disable_l23)
		pcie_rtd3_acpi_l23_exit();

	if (config->skip_on_off_support) {
		/* If current _ON is skipped, ONSK is decremented so that _ON will be
		 * executed normally until _OFF is skipped again.
		 */
		acpigen_write_else();
		acpigen_emit_byte(DECREMENT_OP);
		acpigen_emit_namestring("ONSK");

		acpigen_pop_len(); /* Else */
	}
	acpigen_pop_len(); /* Method */
}

static void
pcie_rtd3_acpi_method_off(int pcie_rp,
			  const struct soc_intel_common_block_pcie_rtd3_config *config,
			  enum pcie_rp_type rp_type)
{
	acpigen_write_method_serialized("_OFF", 0);

	/* When this feature is enabled, ONSK is checked to see if the device
	 * wants _OFF to be skipped for once. ONSK is normally incremented in the
	 * device method, such as reset _RST, which is invoked during driver reload.
	 * In such case, _OFF needs to be avoided at the end of driver removal.
	 */
	if (config->skip_on_off_support)
		acpigen_write_if_lequal_namestr_int("OFSK", 0);

	/* Trigger L23 ready entry flow unless disabled by config. */
	if (!config->disable_l23)
		pcie_rtd3_acpi_l23_entry();

	/* Assert reset GPIO to place device into reset. */
	if (config->reset_gpio.pin_count) {
		acpigen_enable_tx_gpio(&config->reset_gpio);
		if (config->reset_off_delay_ms)
			acpigen_write_sleep(config->reset_off_delay_ms);
	}

	/* Enable modPHY power gating for PCH RPs */
	if (rp_type == PCIE_RP_PCH)
		pcie_rtd3_enable_modphy_pg(pcie_rp, PG_ENABLE);

	/* Disable SRCCLK for this root port if pin is defined. */
	if (config->srcclk_pin >= 0)
		pmc_ipc_acpi_set_pci_clock(pcie_rp, config->srcclk_pin, false);

	/* De-assert enable GPIO to turn off device power. */
	if (config->enable_gpio.pin_count) {
		acpigen_disable_tx_gpio(&config->enable_gpio);
		if (config->enable_off_delay_ms)
			acpigen_write_sleep(config->enable_off_delay_ms);
	}

	if (config->skip_on_off_support) {
		/* If current _OFF is skipped, ONSK is incremented so that the
		 * following _ON will also be skipped. In addition, OFSK is decremented
		 * so that next _OFF will be executed normally until the device method
		 * increments OFSK again.
		 */
		acpigen_write_else();
		/* OFSK-- */
		acpigen_emit_byte(DECREMENT_OP);
		acpigen_emit_namestring("OFSK");
		/* ONSK++ */
		acpigen_emit_byte(INCREMENT_OP);
		acpigen_emit_namestring("ONSK");

		acpigen_pop_len(); /* Else */
	}
	acpigen_pop_len(); /* Method */
}

static void
pcie_rtd3_acpi_method_status(const struct soc_intel_common_block_pcie_rtd3_config *config)
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
	acpigen_write_else();
	acpigen_write_return_op(ONE_OP);
	acpigen_pop_len(); /* Else */

	acpigen_pop_len(); /* Method */
}

static void write_modphy_opregion(unsigned int pcie_rp)
{
	/* The register containing the Power Gate enable sequence bits is at
	   PCH_PWRM_BASE + 0x10D0, and the bits to check for sequence completion are at
	   PCH_PWRM_BASE + 0x10D4. */
	const struct opregion opregion = OPREGION("PMCP", SYSTEMMEMORY,
						  PCH_PWRM_BASE_ADDRESS + 0x1000, 0xff);
	const struct fieldlist fieldlist[] = {
		FIELDLIST_OFFSET(0xD0),
		FIELDLIST_RESERVED(pcie_rp),
		FIELDLIST_NAMESTR("EMPG", 1),	/* Enable ModPHY Power Gate */
		FIELDLIST_OFFSET(0xD4),
		FIELDLIST_RESERVED(pcie_rp),
		FIELDLIST_NAMESTR("AMPG", 1),	/* Is ModPHY Power Gate active? */
	};

	acpigen_write_opregion(&opregion);
	acpigen_write_field("PMCP", fieldlist, ARRAY_SIZE(fieldlist),
			    FIELD_DWORDACC | FIELD_NOLOCK | FIELD_PRESERVE);
}

static int get_pcie_rp_pmc_idx(enum pcie_rp_type rp_type, const struct device *dev)
{
	int idx = -1;

	switch (rp_type) {
	case PCIE_RP_PCH:
		/* Read port number of root port that this device is attached to. */
		idx = pci_read_config8(dev, PCH_PCIE_CFG_LCAP_PN);

		/* Port number is 1-based, PMC IPC method expects 0-based. */
		idx--;
		break;
	case PCIE_RP_CPU:
		/* CPU RPs are indexed by their "virtual wire index" to the PCH */
		idx = soc_get_cpu_rp_vw_idx(dev);
		break;
	default:
		break;
	}

	return idx;
}

static void pcie_rtd3_acpi_fill_ssdt(const struct device *dev)
{
	static bool mutex_created = false;

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
	int pcie_rp;
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
	if (config->srcclk_pin > CONFIG_MAX_PCIE_CLOCK_SRC) {
		printk(BIOS_ERR, "%s: Invalid clock pin %u for %s.\n", __func__,
		       config->srcclk_pin, scope);
		return;
	}

	const enum pcie_rp_type rp_type = soc_get_pcie_rp_type(parent);
	pcie_rp = get_pcie_rp_pmc_idx(rp_type, parent);
	if (pcie_rp < 0) {
		printk(BIOS_ERR, "%s: Unknown PCIe root port\n", __func__);
		return;
	}
	if (config->disable_l23) {
		if (config->ext_pm_support & ACPI_PCIE_RP_EMIT_L23) {
			printk(BIOS_ERR, "%s: Can not export L23 methods\n", __func__);
			return;
		}
	}
	if (rp_type != PCIE_RP_PCH) {
		if (config->ext_pm_support & ACPI_PCIE_RP_EMIT_PSD0) {
			printk(BIOS_ERR, "%s: Can not export PSD0 method\n", __func__);
			return;
		}
	}
	if (config->srcclk_pin == 0) {
		if (config->ext_pm_support & ACPI_PCIE_RP_EMIT_SRCK) {
			printk(BIOS_ERR, "%s: Can not export SRCK method\n", __func__);
			return;
		}
	}

	printk(BIOS_INFO, "%s: Enable RTD3 for %s (%s)\n", scope, dev_path(parent),
	       config->desc ?: dev->chip_ops->name);

	/* Create a mutex for exclusive access to the PMC registers. */
	if (rp_type == PCIE_RP_PCH && !mutex_created) {
		acpigen_write_scope("\\_SB.PCI0");
		acpigen_write_mutex("R3MX", 0);
		acpigen_write_scope_end();
		mutex_created = true;
	}

	/* The RTD3 power resource is added to the root port, not the device. */
	acpigen_write_scope(scope);

	if (config->desc)
		acpigen_write_name_string("_DDN", config->desc);

	/* Create OpRegions for MMIO accesses. */
	acpigen_write_opregion(&opregion);
	acpigen_write_field("PXCS", fieldlist, ARRAY_SIZE(fieldlist),
			    FIELD_ANYACC | FIELD_NOLOCK | FIELD_PRESERVE);

	if (config->ext_pm_support & ACPI_PCIE_RP_EMIT_L23) {
		pcie_rtd3_acpi_method_dl23();
		pcie_rtd3_acpi_method_l23d();
	}

	/* Create the OpRegion to access the ModPHY PG registers (PCH RPs only) */
	if (rp_type == PCIE_RP_PCH)
		write_modphy_opregion(pcie_rp);

	if (config->ext_pm_support & ACPI_PCIE_RP_EMIT_PSD0)
		pcie_rtd3_acpi_method_pds0(pcie_rp);

	if (config->ext_pm_support & ACPI_PCIE_RP_EMIT_SRCK)
		pcie_rtd3_acpi_method_srck(pcie_rp, config);

	/* ACPI Power Resource for controlling the attached device power. */
	acpigen_write_power_res("RTD3", 0, 0, power_res_states, ARRAY_SIZE(power_res_states));

	if (config->skip_on_off_support) {
		/* OFSK:  0 = _OFF Method will be executed normally when called;
		 *       >1 = _OFF will be skipped.
		 *       _OFF Method to decrement OFSK and increment ONSK if the
		 *       current execution is skipped.
		 * ONSK:  0 = _ON Method will be executed normally when called;
		 *       >1 = _ONF will be skipped.
		 *       _ON Method to decrement ONSK if the current execution is
		 *       skipped.
		 */
		acpigen_write_name_integer("ONSK", 0);
		acpigen_write_name_integer("OFSK", 0);
	}

	pcie_rtd3_acpi_method_status(config);
	pcie_rtd3_acpi_method_on(pcie_rp, config, rp_type, dev);
	pcie_rtd3_acpi_method_off(pcie_rp, config, rp_type);
	acpigen_pop_len(); /* PowerResource */

	/* Indicate to the OS that device supports hotplug in D3. */
	dsd = acpi_dp_new_table("_DSD");
	pkg = acpi_dp_new_table(PCIE_HOTPLUG_IN_D3_UUID);
	acpi_dp_add_integer(pkg, PCIE_HOTPLUG_IN_D3_PROPERTY, 1);
	acpi_dp_add_package(dsd, pkg);

	/* Indicate to the OS if the device provides an External facing port. */
	if (config->add_acpi_external_facing_port) {
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
		acpigen_write_name_integer("_S0W", ACPI_DEVICE_SLEEP_D3_COLD);

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
