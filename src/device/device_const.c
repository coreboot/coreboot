/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <device/pci_def.h>
#include <device/pci_type.h>
#include <fw_config.h>
#include <types.h>

/** Linked list of ALL devices */
DEVTREE_CONST struct device *DEVTREE_CONST all_devices = &dev_root;

/**
 * Given a PCI bus and a devfn number, find the device structure.
 *
 * Note that this function can return the incorrect device prior
 * to PCI enumeration because the secondary field of the bus object
 * is 0. The failing scenario is determined by the order of the
 * devices in all_devices singly-linked list as well as the time
 * when this function is called (secondary reflecting topology).
 *
 * @param bus The bus number.
 * @param devfn A device/function number.
 * @return Pointer to the device structure (if found), 0 otherwise.
 */

static DEVTREE_CONST struct device *dev_find_slot(unsigned int bus,
						unsigned int devfn)
{
	DEVTREE_CONST struct device *dev, *result;

	result = 0;
	for (dev = all_devices; dev; dev = dev->next) {
		if ((dev->path.type == DEVICE_PATH_PCI) &&
		    (dev->bus->secondary == bus) &&
		    (dev->path.pci.devfn == devfn)) {
			result = dev;
			break;
		}
	}
	return result;
}

/**
 * Given a Device Path Type, find the device structure.
 *
 * @param prev_match The previously matched device instance.
 * @param path_type The Device Path Type.
 * @return Pointer to the device structure (if found), 0 otherwise.
 */
DEVTREE_CONST struct device *dev_find_path(
		DEVTREE_CONST struct device *prev_match,
		enum device_path_type path_type)
{
	DEVTREE_CONST struct device *dev, *result = NULL;

	if (prev_match == NULL)
		prev_match = all_devices;
	else
		prev_match = prev_match->next;

	for (dev = prev_match; dev; dev = dev->next) {
		if (dev->path.type == path_type) {
			result = dev;
			break;
		}
	}
	return result;
}

/**
 * Given a device pointer, find the next PCI device.
 *
 * @param previous_dev A pointer to a PCI device structure.
 * @return Pointer to the next device structure (if found), 0 otherwise.
 */
DEVTREE_CONST struct device *dev_find_next_pci_device(
		DEVTREE_CONST struct device *previous_dev)
{
	return dev_find_path(previous_dev, DEVICE_PATH_PCI);
}

static int path_eq(const struct device_path *path1,
		const struct device_path *path2)
{
	int equal = 0;

	if (!path1 || !path2) {
		assert(path1);
		assert(path2);
		/* Return 0 in case assert is considered non-fatal. */
		return 0;
	}

	if (path1->type != path2->type)
		return 0;

	switch (path1->type) {
	case DEVICE_PATH_NONE:
		break;
	case DEVICE_PATH_ROOT:
		equal = 1;
		break;
	case DEVICE_PATH_PCI:
		equal = (path1->pci.devfn == path2->pci.devfn);
		break;
	case DEVICE_PATH_PNP:
		equal = (path1->pnp.port == path2->pnp.port) &&
			(path1->pnp.device == path2->pnp.device);
		break;
	case DEVICE_PATH_I2C:
		equal = (path1->i2c.device == path2->i2c.device) &&
			(path1->i2c.mode_10bit == path2->i2c.mode_10bit);
		break;
	case DEVICE_PATH_APIC:
		equal = (path1->apic.apic_id == path2->apic.apic_id);
		break;
	case DEVICE_PATH_DOMAIN:
		equal = (path1->domain.domain == path2->domain.domain);
		break;
	case DEVICE_PATH_CPU_CLUSTER:
		equal = (path1->cpu_cluster.cluster
			 == path2->cpu_cluster.cluster);
		break;
	case DEVICE_PATH_CPU:
		equal = (path1->cpu.id == path2->cpu.id);
		break;
	case DEVICE_PATH_CPU_BUS:
		equal = (path1->cpu_bus.id == path2->cpu_bus.id);
		break;
	case DEVICE_PATH_GENERIC:
		equal = (path1->generic.id == path2->generic.id) &&
			(path1->generic.subid == path2->generic.subid);
		break;
	case DEVICE_PATH_SPI:
		equal = (path1->spi.cs == path2->spi.cs);
		break;
	case DEVICE_PATH_USB:
		equal = (path1->usb.port_type == path2->usb.port_type) &&
			(path1->usb.port_id == path2->usb.port_id);
		break;
	case DEVICE_PATH_MMIO:
		equal = (path1->mmio.addr == path2->mmio.addr);
		break;
	case DEVICE_PATH_GPIO:
		equal = (path1->gpio.id == path2->gpio.id);
		break;
	case DEVICE_PATH_MDIO:
		equal = (path1->mdio.addr == path2->mdio.addr);
		break;

	default:
		printk(BIOS_ERR, "Unknown device type: %d\n", path1->type);
		break;
	}

	return equal;
}

/**
 * See if a device structure exists for path.
 *
 * @param parent The bus to find the device on.
 * @param path The relative path from the bus to the appropriate device.
 * @return Pointer to a device structure for the device on bus at path
 *         or 0/NULL if no device is found.
 */
DEVTREE_CONST struct device *find_dev_path(
	const struct bus *parent, const struct device_path *path)
{
	DEVTREE_CONST struct device *child;

	if (!parent) {
		BUG();
		/* Return NULL in case asserts are considered non-fatal. */
		return NULL;
	}

	for (child = parent->children; child; child = child->sibling) {
		if (path_eq(path, &child->path))
			break;
	}
	return child;
}

/**
 * Find the device structure given an array of nested device paths,
 *
 * @param parent The parent bus to start the search on.
 * @param nested_path An array of relative paths from the parent bus to the target device.
 * @param nested_path_length Number of path elements in nested_path array.
 * @return Pointer to a device structure for the device at nested path
 *         or 0/NULL if no device is found.
 */
DEVTREE_CONST struct device *find_dev_nested_path(
	const struct bus *parent, const struct device_path nested_path[],
	size_t nested_path_length)
{
	DEVTREE_CONST struct device *child;

	if (!parent || !nested_path || !nested_path_length)
		return NULL;

	child = find_dev_path(parent, nested_path);

	/* Terminate recursion at end of nested path or child not found */
	if (nested_path_length == 1 || !child)
		return child;

	return find_dev_nested_path(child->link_list, nested_path + 1, nested_path_length - 1);
}

DEVTREE_CONST struct device *pcidev_path_behind(
	const struct bus *parent, pci_devfn_t devfn)
{
	const struct device_path path = {
		.type = DEVICE_PATH_PCI,
		.pci.devfn = devfn,
	};
	return find_dev_path(parent, &path);
}

DEVTREE_CONST struct device *pcidev_path_on_bus(unsigned int bus, pci_devfn_t devfn)
{
	DEVTREE_CONST struct bus *parent = pci_root_bus();
	DEVTREE_CONST struct device *dev = parent->children;

	/* FIXME: Write the loop with topology links. */
	while (dev) {
		if (dev->path.type != DEVICE_PATH_PCI) {
			dev = dev->next;
			continue;
		}
		if (dev->bus->secondary == bus)
			return pcidev_path_behind(dev->bus, devfn);
		dev = dev->next;
	}
	return NULL;
}

DEVTREE_CONST struct bus *pci_root_bus(void)
{
	DEVTREE_CONST struct device *pci_domain;
	static DEVTREE_CONST struct bus *pci_root;

	if (pci_root)
		return pci_root;

	pci_domain = dev_find_path(NULL, DEVICE_PATH_DOMAIN);
	if (!pci_domain)
		return NULL;

	pci_root = pci_domain->link_list;
	return pci_root;
}

DEVTREE_CONST struct device *pcidev_path_on_root(pci_devfn_t devfn)
{
	return pcidev_path_behind(pci_root_bus(), devfn);
}

DEVTREE_CONST struct device *pcidev_on_root(uint8_t dev, uint8_t fn)
{
	return pcidev_path_on_root(PCI_DEVFN(dev, fn));
}

DEVTREE_CONST struct device *pcidev_path_behind_pci2pci_bridge(
							const struct device *bridge,
							pci_devfn_t devfn)
{
	if (!bridge || (bridge->path.type != DEVICE_PATH_PCI)) {
		BUG();
		/* Return NULL in case asserts are non-fatal. */
		return NULL;
	}

	return pcidev_path_behind(bridge->link_list, devfn);
}

DEVTREE_CONST struct device *pcidev_path_on_root_debug(pci_devfn_t devfn, const char *func)
{
	DEVTREE_CONST struct device *dev = pcidev_path_on_root(devfn);
	if (dev)
		return dev;

	devtree_bug(func, devfn);

	/* FIXME: This can return wrong device. */
	return dev_find_slot(0, devfn);
}

void devtree_bug(const char *func, pci_devfn_t devfn)
{
	printk(BIOS_ERR, "BUG: %s requests hidden 00:%02x.%u\n", func, devfn >> 3, devfn & 7);
}

void __noreturn devtree_die(void)
{
	die("DEVTREE: dev or chip_info is NULL\n");
}

/**
 * Given an SMBus bus and a device number, find the device structure.
 *
 * @param bus The bus number.
 * @param addr A device number.
 * @return Pointer to the device structure (if found), 0 otherwise.
 */
DEVTREE_CONST struct device *dev_find_slot_on_smbus(unsigned int bus,
							unsigned int addr)
{
	DEVTREE_CONST struct device *dev, *result;

	result = 0;
	for (dev = all_devices; dev; dev = dev->next) {
		if ((dev->path.type == DEVICE_PATH_I2C) &&
		    (dev->bus->secondary == bus) &&
		    (dev->path.i2c.device == addr)) {
			result = dev;
			break;
		}
	}
	return result;
}

/**
 * Given a PnP port and a device number, find the device structure.
 *
 * @param port The I/O port.
 * @param device Logical device number.
 * @return Pointer to the device structure (if found), 0 otherwise.
 */
DEVTREE_CONST struct device *dev_find_slot_pnp(u16 port, u16 device)
{
	DEVTREE_CONST struct device *dev;

	for (dev = all_devices; dev; dev = dev->next) {
		if ((dev->path.type == DEVICE_PATH_PNP) &&
		    (dev->path.pnp.port == port) &&
		    (dev->path.pnp.device == device)) {
			return dev;
		}
	}
	return 0;
}

/**
 * Given a device and previous match iterate through all the children.
 *
 * @param bus parent device's bus holding all the children
 * @param prev_child previous child already traversed, if NULL start at
 *        children of parent bus.
 * @return pointer to child or NULL when no more children
 */
DEVTREE_CONST struct device *dev_bus_each_child(const struct bus *parent,
					DEVTREE_CONST struct device *prev_child)
{
	DEVTREE_CONST struct device *dev;

	if (parent == NULL)
		return NULL;

	if (prev_child == NULL)
		dev = parent->children;
	else
		dev = prev_child->sibling;

	return dev;
}

bool is_dev_enabled(const struct device *dev)
{
	if (!dev)
		return false;

	/* For stages with immutable device tree, first check if device is disabled because of
	   fw_config probing. In these stages, dev->enabled does not reflect the true state of a
	   device that uses fw_config probing. */
	if (DEVTREE_EARLY && !fw_config_probe_dev(dev, NULL))
		return false;
	return dev->enabled;
}

bool is_devfn_enabled(unsigned int devfn)
{
	const struct device *dev = pcidev_path_on_root(devfn);
	return is_dev_enabled(dev);
}
