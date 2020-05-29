/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
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

	/* Check fan as well (its use is implicit in the Active policy) */
	if (participant == DPTF_FAN && config->policies.active[0].target != DPTF_NONE)
		return true;

	return false;
}

static const char *dptf_acpi_name(const struct device *dev)
{
	return "DPTF";
}

/* Add custom tables and methods to SSDT */
static void dptf_fill_ssdt(const struct device *dev)
{
	struct drivers_intel_dptf_config *config = config_of(dev);

	dptf_write_active_policies(config->policies.active,
				   DPTF_MAX_ACTIVE_POLICIES);

	printk(BIOS_INFO, "\\_SB.DPTF: %s at %s\n", dev->chip_ops->name, dev_path(dev));
}

static int get_STA_value(const struct drivers_intel_dptf_config *config,
			 enum dptf_participant participant)
{
	return is_participant_used(config, participant) ?
		ACPI_STATUS_DEVICE_ALL_ON :
		ACPI_STATUS_DEVICE_ALL_OFF;
}

static void dptf_write_generic_participant(const char *name,
					   enum dptf_generic_participant_type ptype,
					   const char *str, int sta_val)
{
	/* Auto-incrementing UID for generic participants */
	static int generic_uid = 0;

	acpigen_write_device(name);

	if (CONFIG(DPTF_USE_EISA_HID)) {
		acpigen_write_name("_HID");
		acpigen_emit_eisaid(GENERIC_HID_EISAID);
	} else {
		acpigen_write_name_string("_HID", GENERIC_HID);
	}

	acpigen_write_name_integer("_UID", generic_uid++);
	acpigen_write_STA(sta_val);

	if (str)
		acpigen_write_name_string("_STR", str);

	acpigen_write_name_integer("PTYP", ptype);

	acpigen_pop_len(); /* Device */
}

/* Add static definitions of DPTF devices into the DSDT */
static void dptf_inject_dsdt(const struct device *dev)
{
	const struct drivers_intel_dptf_config *config;
	enum dptf_participant participant;
	struct device *parent;
	char name[5];
	int i;

	/* The CPU device gets an _ADR that matches the ACPI PCI address for 00:04.00 */
	parent = dev && dev->bus ? dev->bus->dev : NULL;
	if (!parent || parent->path.type != DEVICE_PATH_PCI) {
		printk(BIOS_ERR, "%s: DPTF objects must live under 00:04.0 PCI device\n",
		       __func__);
		return;
	}

	config = config_of(dev);
	acpigen_write_scope("\\_SB");

	/* DPTF CPU device - \_SB.TCPU */
	acpigen_write_device("TCPU");
	acpigen_write_ADR_pci_device(parent);
	acpigen_write_STA(get_STA_value(config, DPTF_CPU));
	acpigen_pop_len(); /* Device */

	/* Toplevel DPTF device - \_SB.DPTF*/
	acpigen_write_device(acpi_device_name(dev));
	if (CONFIG(DPTF_USE_EISA_HID)) {
		acpigen_write_name("_HID");
		acpigen_emit_eisaid(DPTF_DEVICE_HID_EISAID);
	} else {
		acpigen_write_name_string("_HID", DPTF_DEVICE_HID);
	}

	acpigen_write_name_integer("_UID", 0);
	acpigen_write_STA(ACPI_STATUS_DEVICE_ALL_ON);

	/*
	 * The following devices live underneath \_SB.DPTF:
	 * - Fan, \_SB.DPTF.TFN1
	 * - Charger, \_SB.DPTF.TCHG
	 * - Temperature Sensors, \_SB.DPTF.TSRn
	 */

	acpigen_write_device("TFN1");
	if (CONFIG(DPTF_USE_EISA_HID)) {
		acpigen_write_name("_HID");
		acpigen_emit_eisaid(FAN_HID_EISAID);
	} else {
		acpigen_write_name_string("_HID", FAN_HID);
	}

	acpigen_write_name_integer("_UID", 0);
	acpigen_write_STA(get_STA_value(config, DPTF_FAN));
	acpigen_pop_len(); /* Device */

	dptf_write_generic_participant("TCHG", DPTF_GENERIC_PARTICIPANT_TYPE_CHARGER,
				       DEFAULT_CHARGER_STR, get_STA_value(config,
									  DPTF_CHARGER));

	for (i = 0, participant = DPTF_TEMP_SENSOR_0; i < 4; ++i, ++participant) {
		snprintf(name, sizeof(name), "TSR%1d", i);
		dptf_write_generic_participant(name, DPTF_GENERIC_PARTICIPANT_TYPE_TSR,
					       NULL, get_STA_value(config, participant));
	}

	acpigen_pop_len(); /* DPTF Device */
	acpigen_pop_len(); /* Scope */
}

static struct device_operations dptf_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= dptf_acpi_name,
	.acpi_fill_ssdt		= dptf_fill_ssdt,
	.acpi_inject_dsdt	= dptf_inject_dsdt,
};

static void dptf_enable_dev(struct device *dev)
{
	dev->ops = &dptf_ops;
}

struct chip_operations drivers_intel_dptf_ops = {
	CHIP_NAME("Intel DPTF")
	.enable_dev = dptf_enable_dev,
};
