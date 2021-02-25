/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <console/console.h>
#include <device/device.h>
#include "chip.h"

/* Generic DPTF participants have a PTYP field to distinguish them */
enum dptf_generic_participant_type {
	DPTF_GENERIC_PARTICIPANT_TYPE_TSR	= 0x3,
	DPTF_GENERIC_PARTICIPANT_TYPE_CHARGER	= 0xB,
};

#define DEFAULT_CHARGER_STR		"Battery Charger"

#define DPTF_DEVICE_HID_EISAID	"INT3400"
#define GENERIC_HID_EISAID	"INT3403"
#define FAN_HID_EISAID		"INT3404"

#define DPTF_DEVICE_HID		"INTC1040"
#define GENERIC_HID		"INTC1043"
#define FAN_HID			"INTC1044"

/*
 * Helper method to determine if a device is "used" (called out anywhere as a source or a target
 * of any policies, and therefore should be included in the ACPI tables.
 */
static bool is_participant_used(const struct drivers_intel_dptf_config *config,
				enum dptf_participant participant)
{
	int i;

	/* Active? */
	for (i = 0; i < DPTF_MAX_ACTIVE_POLICIES; ++i)
		if (config->policies.active[i].target == participant)
			return true;

	/* Passive? */
	for (i = 0; i < DPTF_MAX_PASSIVE_POLICIES; ++i)
		if (config->policies.passive[i].source == participant ||
		    config->policies.passive[i].target == participant)
			return true;

	/* Critical? */
	for (i = 0; i < DPTF_MAX_CRITICAL_POLICIES; ++i)
		if (config->policies.critical[i].source == participant)
			return true;

	/* Check fan as well (its use is implicit in the Active policy) */
	if (participant == DPTF_FAN && config->policies.active[0].target != DPTF_NONE)
		return true;

	return false;
}

static const char *dptf_acpi_name(const struct device *dev)
{
	return "DPTF";
}

static int get_STA_value(const struct drivers_intel_dptf_config *config,
			 enum dptf_participant participant)
{
	return is_participant_used(config, participant) ?
		ACPI_STATUS_DEVICE_ALL_ON :
		ACPI_STATUS_DEVICE_ALL_OFF;
}

/* Devices with GENERIC _HID (distinguished by PTYP) */
static void dptf_write_generic_participant(const char *name,
					   enum dptf_generic_participant_type ptype,
					   const char *str, int sta_val)
{
	/* Auto-incrementing UID for generic participants */
	static int generic_uid = 0;

	acpigen_write_device(name);
	acpigen_write_name("_HID");
	if (CONFIG(DPTF_USE_EISA_HID))
		acpigen_emit_eisaid(GENERIC_HID_EISAID);
	else
		acpigen_write_string(GENERIC_HID);

	acpigen_write_name_integer("_UID", generic_uid++);
	acpigen_write_STA(sta_val);

	if (str)
		acpigen_write_name_string("_STR", str);

	acpigen_write_name_integer("PTYP", ptype);

	acpigen_pop_len(); /* Device */
}

/* \_SB.PCI0.TCPU */
static void write_tcpu(const struct device *pci_dev,
		       const struct drivers_intel_dptf_config *config)
{
	/* DPTF CPU device - \_SB.PCI0.TCPU */
	acpigen_write_scope(TCPU_SCOPE);
	acpigen_write_device("TCPU");
	acpigen_write_ADR_pci_device(pci_dev);
	acpigen_write_STA(get_STA_value(config, DPTF_CPU));
	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* TCPU Scope */
}

/* \_SB.DPTF.TFN1 */
static void write_fan(const struct drivers_intel_dptf_config *config)
{
	acpigen_write_device("TFN1");
	acpigen_write_name("_HID");
	if (CONFIG(DPTF_USE_EISA_HID))
		acpigen_emit_eisaid(FAN_HID_EISAID);
	else
		acpigen_write_string(FAN_HID);

	acpigen_write_name_integer("_UID", 0);
	acpigen_write_STA(get_STA_value(config, DPTF_FAN));
	acpigen_pop_len(); /* Device */
}

/* \_SB.DPTF.xxxx */
static void write_generic_devices(const struct drivers_intel_dptf_config *config)
{
	enum dptf_participant participant;
	char name[ACPI_NAME_BUFFER_SIZE];
	int i;

	dptf_write_generic_participant("TCHG", DPTF_GENERIC_PARTICIPANT_TYPE_CHARGER,
				       DEFAULT_CHARGER_STR, get_STA_value(config,
									  DPTF_CHARGER));

	for (i = 0, participant = DPTF_TEMP_SENSOR_0; i < 4; ++i, ++participant) {
		snprintf(name, sizeof(name), "TSR%1d", i);
		dptf_write_generic_participant(name, DPTF_GENERIC_PARTICIPANT_TYPE_TSR,
					       NULL, get_STA_value(config, participant));
	}
}

/* \_SB.DPTF - note: leaves the Scope open for child devices*/
static void write_open_dptf_device(const struct device *dev)
{
	acpigen_write_scope("\\_SB");
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name("_HID");
	if (CONFIG(DPTF_USE_EISA_HID))
		acpigen_emit_eisaid(DPTF_DEVICE_HID_EISAID);
	else
		acpigen_write_string(DPTF_DEVICE_HID);

	acpigen_write_name_integer("_UID", 0);
	acpigen_write_STA(ACPI_STATUS_DEVICE_ALL_ON);
}

/* Add minimal definitions of DPTF devices into the SSDT */
static void write_device_definitions(const struct device *dev)
{
	const struct drivers_intel_dptf_config *config;
	struct device *parent;

	/* The CPU device gets an _ADR that matches the ACPI PCI address for 00:04.00 */
	parent = dev && dev->bus ? dev->bus->dev : NULL;
	if (!parent || parent->path.type != DEVICE_PATH_PCI) {
		printk(BIOS_ERR, "%s: DPTF objects must live under 00:04.0 PCI device\n",
		       __func__);
		return;
	}

	config = config_of(dev);
	write_tcpu(parent, config);
	write_open_dptf_device(dev);
	write_fan(config);
	write_generic_devices(config);

	acpigen_pop_len(); /* DPTF Device (write_open_dptf_device) */
	acpigen_pop_len(); /* Scope */
}

/* Emites policy definitions for each policy type */
static void write_policies(const struct drivers_intel_dptf_config *config)
{
	dptf_write_enabled_policies(config->policies.active, DPTF_MAX_ACTIVE_POLICIES,
				    config->policies.passive, DPTF_MAX_PASSIVE_POLICIES,
				    config->policies.critical, DPTF_MAX_CRITICAL_POLICIES);

	dptf_write_active_policies(config->policies.active,
				   DPTF_MAX_ACTIVE_POLICIES);

	dptf_write_passive_policies(config->policies.passive,
				    DPTF_MAX_PASSIVE_POLICIES);

	dptf_write_critical_policies(config->policies.critical,
				     DPTF_MAX_CRITICAL_POLICIES);
}

/* Writes other static tables that are used by DPTF */
static void write_controls(const struct drivers_intel_dptf_config *config)
{
	dptf_write_charger_perf(config->controls.charger_perf, DPTF_MAX_CHARGER_PERF_STATES);
	dptf_write_fan_perf(config->controls.fan_perf, DPTF_MAX_FAN_PERF_STATES);
	dptf_write_power_limits(&config->controls.power_limits);
}

/* Options to control the behavior of devices */
static void write_options(const struct drivers_intel_dptf_config *config)
{
	enum dptf_participant p;
	int i;

	/* Fan options */
	dptf_write_scope(DPTF_FAN);
	dptf_write_fan_options(config->options.fan.fine_grained_control,
			       config->options.fan.step_size,
			       config->options.fan.low_speed_notify);
	acpigen_pop_len(); /* Scope */

	/* TSR options */
	for (p = DPTF_TEMP_SENSOR_0, i = 0; p <= DPTF_TEMP_SENSOR_3; ++p, ++i) {
		if (is_participant_used(config, p) && (config->options.tsr[i].hysteresis ||
						       config->options.tsr[i].desc)) {
			dptf_write_scope(p);
			dptf_write_tsr_hysteresis(config->options.tsr[i].hysteresis);
			dptf_write_STR(config->options.tsr[i].desc);
			acpigen_pop_len(); /* Scope */
		}
	}
}

/* Add custom tables and methods to SSDT */
static void dptf_fill_ssdt(const struct device *dev)
{
	struct drivers_intel_dptf_config *config = config_of(dev);

	write_device_definitions(dev);
	write_policies(config);
	write_controls(config);
	write_options(config);

	printk(BIOS_INFO, DPTF_DEVICE_PATH ": %s at %s\n", dev->chip_ops->name, dev_path(dev));
}

static struct device_operations dptf_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= dptf_acpi_name,
	.acpi_fill_ssdt		= dptf_fill_ssdt,
};

static void dptf_enable_dev(struct device *dev)
{
	dev->ops = &dptf_ops;
}

struct chip_operations drivers_intel_dptf_ops = {
	CHIP_NAME("Intel DPTF")
	.enable_dev = dptf_enable_dev,
};
