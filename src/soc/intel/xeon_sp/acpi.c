/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <assert.h>
#include <device/pci_ops.h>
#include <intelblocks/acpi.h>
#include <soc/chip_common.h>
#include <soc/pci_devs.h>
#include <soc/util.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "chip.h"

/*
 * List of supported C-states in this processor.
 */
enum {
	C_STATE_C1,	/* 0 */
	C_STATE_C3,	/* 1 */
	C_STATE_C6,	/* 2 */
	C_STATE_C7,	/* 3 */
	NUM_C_STATES
};

static const acpi_cstate_t cstate_map[NUM_C_STATES] = {
	[C_STATE_C1] = {
		/* C1 */
		.latency = 1,
		.power = 0x3e8,
		.resource = MWAIT_RES(0, 0),
	},
	[C_STATE_C3] = {
		/* C3 */
		.latency = 15,
		.power = 0x1f4,
		.resource = MWAIT_RES(1, 0),
	},
	[C_STATE_C6] = {
		/* C6 */
		.latency = 41,
		.power = 0x15e,
		.resource = MWAIT_RES(2, 0),
	},
	[C_STATE_C7] = {
		/* C7 */
		.latency = 41,
		.power = 0x0c8,
		.resource = MWAIT_RES(3, 0),
	}
};

/* Max states supported */
static int cstate_set_all[] = {
	C_STATE_C1,
	C_STATE_C3,
	C_STATE_C6,
	C_STATE_C7
};

static int cstate_set_c1_c6[] = {
	C_STATE_C1,
	C_STATE_C6,
};

const acpi_cstate_t *soc_get_cstate_map(size_t *entries)
{
	static acpi_cstate_t map[ARRAY_SIZE(cstate_set_all)];
	int *cstate_set;
	int i;

	const config_t *config = config_of_soc();

	const enum acpi_cstate_mode states = config->cstate_states;

	switch (states) {
	case CSTATES_C1C6:
		*entries = ARRAY_SIZE(cstate_set_c1_c6);
		cstate_set = cstate_set_c1_c6;
		break;
	case CSTATES_ALL:
	default:
		*entries = ARRAY_SIZE(cstate_set_all);
		cstate_set = cstate_set_all;
		break;
	}

	for (i = 0; i < *entries; i++) {
		map[i] = cstate_map[cstate_set[i]];
		map[i].ctype = i + 1;
	}
	return map;
}

void iio_domain_set_acpi_name(struct device *dev, const char *prefix)
{
	const union xeon_domain_path dn = {
		.domain_path = dev_get_domain_id(dev)
	};

	assert(dn.socket < 8);
	assert(dn.stack < 32);
	assert(prefix != NULL && strlen(prefix) == 2);

	if (dn.socket >= 8 || dn.stack >= 32 ||
	    !prefix || strlen(prefix) != 2)
		return;

	char *name = xmalloc(ACPI_NAME_BUFFER_SIZE);
	snprintf(name, ACPI_NAME_BUFFER_SIZE, "%s%02X", prefix, ((dn.socket << 5) + dn.stack));
	dev->name = name;
}

const char *soc_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return dev->name;

	/* FIXME: Add SoC specific device names here */

	return NULL;
}

void acpigen_write_OSC_pci_domain_fixed_caps(const struct device *domain,
				const uint32_t granted_pcie_features,
				const bool is_cxl_domain,
				const uint32_t granted_cxl_features)
{
	acpigen_write_method("_OSC", 4);

	acpigen_write_return_namestr("\\_SB.POSC");
	acpigen_emit_byte(ARG0_OP);
	acpigen_emit_byte(ARG1_OP);
	acpigen_emit_byte(ARG2_OP);
	acpigen_emit_byte(ARG3_OP);
	acpigen_write_integer(granted_pcie_features);
	acpigen_write_integer(is_cxl_domain);
	acpigen_write_integer(granted_cxl_features);

	acpigen_pop_len();
}

static bool read_physical_slot_number(const struct device *dev, uint8_t *psn)
{
	if (!is_pci(dev))
		return false;

	const size_t pos = pci_find_capability(dev, PCI_CAP_ID_PCIE);
	if (!pos)
		return false;

	u32 sltcap = pci_read_config32(dev, pos + PCI_EXP_SLTCAP);
	*psn = ((sltcap >> 19) & 0x1FF);
	return true;
}

static void acpigen_write_pci_root_port_devices(const struct device *rp)
{
	uint8_t psn;
	bool have_psn = read_physical_slot_number(rp, &psn);

	struct device *dev = NULL;
	while ((dev = dev_bus_each_child(rp->downstream, dev))) {
		if (!is_pci(dev))
			continue;
		const char *name = acpi_device_name(dev);
		if (!name)
			continue;
		acpigen_write_device(name);
		acpigen_write_ADR_pci_device(dev);
		if (have_psn)
			acpigen_write_name_integer("_SUN", psn);
		acpigen_pop_len();
	}
}

void acpigen_write_pci_root_port(const struct device *rp)
{
	const char *acpi_scope = acpi_device_scope(rp);
	if (!acpi_scope)
		return;
	acpigen_write_scope(acpi_scope);

	const char *acpi_name = acpi_device_name(rp);
	if (!acpi_name)
		return;
	acpigen_write_device(acpi_name);
	acpigen_write_ADR_pci_device(rp);
	acpigen_write_pci_root_port_devices(rp);

	acpigen_pop_len();
	acpigen_pop_len();
}
