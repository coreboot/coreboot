/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <superio/common/ssdt.h>

#include <device/device.h>
#include <device/pnp.h>
#include <acpi/acpigen.h>
#include <acpi/acpi.h>
#include <console/console.h>
#include <types.h>
#include <string.h>

struct superio_dev {
	const char *acpi_hid;
	u16 io_base[4];
	u8 irq[2];
};

static const struct superio_dev superio_devs[] = {
	{ACPI_HID_FDC, {0x3f0, 0x3f2, 0x3f7}, {6, } },
	{ACPI_HID_KEYBOARD, {60, 64, }, {1, } },
	{ACPI_HID_MOUSE, {60, 64, }, {12, } },
	{ACPI_HID_COM, {0x3f8, 0x2f8, 0x3e8, 0x2e8}, {4, 3} },
	{ACPI_HID_LPT, {0x378, }, {7, } },
};

static const u8 io_idx[] = {PNP_IDX_IO0, PNP_IDX_IO1, PNP_IDX_IO2, PNP_IDX_IO3};
static const u8 irq_idx[] = {PNP_IDX_IRQ0, PNP_IDX_IRQ1};

static const struct superio_dev *superio_guess_function(const struct device *dev)
{
	for (size_t i = 0; i < ARRAY_SIZE(io_idx); i++) {
		struct resource *res = probe_resource(dev, io_idx[i]);
		if (!res || !res->base)
			continue;

		for (size_t j = 0; j < ARRAY_SIZE(superio_devs); j++) {
			for (size_t k = 0; k < 4; k++) {
				if (!superio_devs[j].io_base[k])
					continue;
				if (superio_devs[j].io_base[k] == res->base)
					return &superio_devs[j];
			}
		}
	}
	for (size_t i = 0; i < ARRAY_SIZE(irq_idx); i++) {
		struct resource *res = probe_resource(dev, irq_idx[i]);
		if (!res || !res->size)
			continue;
		for (size_t j = 0; j < ARRAY_SIZE(superio_devs); j++) {
			for (size_t k = 0; k < 2; k++) {
				if (!superio_devs[j].irq[k])
					continue;
				if (superio_devs[j].irq[k] == res->base)
					return &superio_devs[j];
			}
		}
	}
	return NULL;
}

/* Return true if there are resources to report */
static bool has_resources(const struct device *dev)
{
	for (size_t i = 0; i < ARRAY_SIZE(io_idx); i++) {
		struct resource *res = probe_resource(dev, io_idx[i]);
		if (!res || !res->base || !res->size)
			continue;
		return 1;
	}
	for (size_t i = 0; i < ARRAY_SIZE(irq_idx); i++) {
		struct resource *res = probe_resource(dev, irq_idx[i]);
		if (!res || !res->size || res->base > 16)
			continue;
		return 1;
	}
	return 0;
}

/* Add IO and IRQ resources for _CRS or _PRS */
static void ldn_gen_resources(const struct device *dev)
{
	uint16_t irq = 0;
	for (size_t i = 0; i < ARRAY_SIZE(io_idx); i++) {
		struct resource *res = probe_resource(dev, io_idx[i]);
		if (!res || !res->base)
			continue;
		resource_t base = res->base;
		resource_t size = res->size;
		while (size > 0) {
			resource_t sz = size > 255 ? 255 : size;
			/* TODO: Needs test with regions >= 256 bytes */
			acpigen_write_io16(base, base, 1, sz, 1);
			size -= sz;
			base += sz;
		}
	}
	for (size_t i = 0; i < ARRAY_SIZE(irq_idx); i++) {
		struct resource *res = probe_resource(dev, irq_idx[i]);
		if (!res || !res->size || res->base >= 16)
			continue;
		irq |= 1 << res->base;
	}
	if (irq)
		acpigen_write_irq(irq);

}

/* Add resource base and size for additional SuperIO code */
static void ldn_gen_resources_use(const struct device *dev)
{
	char name[5];
	for (size_t i = 0; i < ARRAY_SIZE(io_idx); i++) {
		struct resource *res = probe_resource(dev, io_idx[i]);
		if (!res || !res->base || !res->size)
			continue;

		snprintf(name, sizeof(name), "IO%zXB", i);
		name[4] = '\0';
		acpigen_write_name_integer(name, res->base);

		snprintf(name, sizeof(name), "IO%zXS", i);
		name[4] = '\0';
		acpigen_write_name_integer(name, res->size);
	}
}

const char *superio_common_ldn_acpi_name(const struct device *dev)
{
	u8 ldn = dev->path.pnp.device & 0xff;
	u8 vldn = (dev->path.pnp.device >> 8) & 0x7;
	static char name[5];

	snprintf(name, sizeof(name), "L%02X%01X", ldn, vldn);

	name[4] = '\0';

	return name;
}

static const char *name_from_hid(const char *hid)
{
	static const struct {
		const char *hid;
		const char *name;
	} lookup[] = {
		{ACPI_HID_FDC, "FDC" },
		{ACPI_HID_KEYBOARD, "PS2 Keyboard" },
		{ACPI_HID_MOUSE, "PS2 Mouse"},
		{ACPI_HID_COM, "COM port" },
		{ACPI_HID_LPT, "LPT" },
		{ACPI_HID_PNP, "Generic PNP device" },
	};

	for (size_t i = 0; hid && i < ARRAY_SIZE(lookup); i++) {
		if (strcmp(hid, lookup[i].hid) == 0)
			return lookup[i].name;
	}
	return "Generic device";
}

void superio_common_fill_ssdt_generator(const struct device *dev)
{
	if (!dev || !dev->bus || !dev->bus->dev) {
		printk(BIOS_CRIT, "BUG: Invalid argument in %s!\n", __func__);
		return;
	}

	const char *scope = acpi_device_scope(dev);
	const char *name = acpi_device_name(dev);
	const u8 ldn = dev->path.pnp.device & 0xff;
	const u8 vldn = (dev->path.pnp.device >> 8) & 0x7;
	const char *hid;

	/* Validate devicetree settings */
	bool bug = false;
	if (dev->bus->dev->path.type != DEVICE_PATH_PNP) {
		bug = true;
		printk(BIOS_CRIT, "BUG: Parent of device %s is not a PNP device\n",
			dev_path(dev));
	} else if (dev->bus->dev->path.pnp.port != dev->path.pnp.port) {
		bug = true;
		printk(BIOS_CRIT, "BUG: Parent of device %s has wrong I/O port\n",
			dev_path(dev));
	}
	if (bug) {
		printk(BIOS_CRIT, "BUG: Check your devicetree!\n");
		return;
	}

	if (!scope || !name) {
		printk(BIOS_ERR, "%s: Missing ACPI path/scope\n", dev_path(dev));
		return;
	}
	if (vldn) {
		printk(BIOS_DEBUG, "%s: Ignoring virtual LDN\n", dev_path(dev));
		return;
	}

	printk(BIOS_DEBUG, "%s.%s: %s\n", scope, name, dev_path(dev));

	/* Scope */
	acpigen_write_scope(scope);

	/* Device */
	acpigen_write_device(name);

	acpi_device_write_uid(dev);

	acpigen_write_name_byte("LDN", ldn);
	acpigen_write_name_byte("VLDN", vldn);

	acpigen_write_method("_STA", 0);
	{
		acpigen_write_store();
		acpigen_emit_namestring("^^QLDN");
		acpigen_write_integer(ldn);
		acpigen_emit_byte(LOCAL0_OP);

		/* Multiply (Local0, 0xf, Local0) */
		acpigen_emit_byte(MULTIPLY_OP);
		acpigen_emit_byte(LOCAL0_OP);
		acpigen_write_integer(0xf);
		acpigen_emit_byte(LOCAL0_OP);

		acpigen_emit_byte(RETURN_OP);
		acpigen_emit_byte(LOCAL0_OP);

	}
	acpigen_pop_len(); /* Method */

	/*
	 * The ACPI6.2 spec Chapter 6.1.5 requires to set a  _HID if no _ADR
	 * is present. Tests on Windows 10 showed that this is also true for
	 * disabled (_STA = 0) devices, otherwise it BSODs.
	 */

	hid = acpi_device_hid(dev);
	if (!hid) {
		printk(BIOS_ERR, "%s: SuperIO driver doesn't provide a _HID\n", dev_path(dev));
		/* Try to guess it... */
		const struct superio_dev *sdev = superio_guess_function(dev);
		if (sdev && sdev->acpi_hid) {
			hid = sdev->acpi_hid;
			printk(BIOS_WARNING, "%s: Guessed _HID is '%s'\n", dev_path(dev), hid);
		} else {
			hid = ACPI_HID_PNP;
			printk(BIOS_ERR, "%s: Failed to guessed _HID\n", dev_path(dev));
		}
	}

	acpigen_write_name_string("_HID", hid);
	acpigen_write_name_string("_DDN", name_from_hid(hid));

	acpigen_write_method("_DIS", 0);
	{
		acpigen_emit_namestring("^^DLDN");
		acpigen_write_integer(ldn);
	}
	acpigen_pop_len(); /* Method */

	if (dev->enabled && has_resources(dev)) {
		/* Resources - _CRS */
		acpigen_write_name("_CRS");
		acpigen_write_resourcetemplate_header();
		ldn_gen_resources(dev);
		acpigen_write_resourcetemplate_footer();

		/* Resources - _PRS */
		acpigen_write_name("_PRS");
		acpigen_write_resourcetemplate_header();
		ldn_gen_resources(dev);
		acpigen_write_resourcetemplate_footer();

		/* Resources base and size for 3rd party ACPI code */
		ldn_gen_resources_use(dev);
	}

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}
