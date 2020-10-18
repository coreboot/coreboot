/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <reset.h>

const char mainboard_name[] = CONFIG_MAINBOARD_VENDOR " " CONFIG_MAINBOARD_PART_NUMBER;

void enable_static_device(struct device *dev)
{
	if (dev->chip_ops && dev->chip_ops->enable_dev)
		dev->chip_ops->enable_dev(dev);

	if (dev->ops && dev->ops->enable)
		dev->ops->enable(dev);

	printk(BIOS_DEBUG, "%s %s\n", dev_path(dev),
	       dev->enabled ? "enabled" : "disabled");
}

/**
 * Enable devices on static buses.
 *
 * The enumeration of certain buses is purely static. The existence of
 * devices on those buses can be completely determined at compile time
 * and is specified in the config file. Typical examples are the 'PNP'
 * devices on a legacy ISA/LPC bus. There is no need of probing of any kind,
 * the only thing we have to do is to walk through the bus and
 * enable or disable devices as indicated in the config file.
 *
 * On the other hand, some devices are virtual and their existence is
 * artificial. They can not be probed at run time. One example is the
 * debug device. Those virtual devices have to be listed in the config
 * file under some static bus in order to be enumerated at run time.
 *
 * @param bus Pointer to the device to which the static buses are attached to.
 */

void enable_static_devices(struct device *bus)
{
	struct device *child;
	struct bus *link;

	for (link = bus->link_list; link; link = link->next) {
		for (child = link->children; child; child = child->sibling) {
			enable_static_device(child);
		}
	}
}

void scan_generic_bus(struct device *bus)
{
	struct device *child;
	struct bus *link;
	static int bus_max = 0;

	printk(BIOS_SPEW, "%s for %s\n", __func__, dev_path(bus));

	for (link = bus->link_list; link; link = link->next) {

		link->secondary = ++bus_max;

		for (child = link->children; child; child = child->sibling) {
			enable_static_device(child);
			printk(BIOS_DEBUG, "bus: %s[%d]->", dev_path(child->bus->dev),
			       child->bus->link_num);
		}
	}

	printk(BIOS_SPEW, "%s for %s done\n", __func__, dev_path(bus));
}

void scan_smbus(struct device *bus)
{
	scan_generic_bus(bus);
}

/*
 * Default scan_bus() implementation
 *
 * This is the default implementation for buses that can't
 * be probed at runtime. It simply walks through the topology
 * given by the mainboard's `devicetree.cb`.
 *
 * First, all direct descendants of the given device are
 * enabled. Then, downstream buses are scanned.
 */
void scan_static_bus(struct device *bus)
{
	struct bus *link;

	printk(BIOS_SPEW, "%s for %s\n", __func__, dev_path(bus));

	enable_static_devices(bus);

	for (link = bus->link_list; link; link = link->next)
		scan_bridges(link);

	printk(BIOS_SPEW, "%s for %s done\n", __func__, dev_path(bus));
}

static void root_dev_reset(struct bus *bus)
{
	printk(BIOS_INFO, "Resetting board...\n");
	board_reset();
}

#if CONFIG(HAVE_ACPI_TABLES)
static const char *root_dev_acpi_name(const struct device *dev)
{
	return "\\_SB";
}
#endif

/**
 * Default device operation for root device.
 *
 * This is the default device operation for root devices. These operations
 * should be fully usable as is. However the chip_operations::enable_dev()
 * of a motherboard can override this if you want non-default behavior.
 */
struct device_operations default_dev_ops_root = {
	.read_resources   = noop_read_resources,
	.set_resources    = noop_set_resources,
	.scan_bus         = scan_static_bus,
	.reset_bus        = root_dev_reset,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name        = root_dev_acpi_name,
#endif
};
