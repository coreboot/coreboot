#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <device/pci.h>
#include <string.h>


/**
 * See if a device structure already exists and if not allocate it
 * @param bus The bus to find the device on
 * @param path The relative path from the bus to the appropriate device
 * @return pointer a device structure for the device on bus at path
 */
device_t alloc_find_dev(struct bus *parent, struct device_path *path)
{
	device_t child;
	for(child = parent->children; child; child = child->sibling) {
		if (path_eq(path, &child->path)) {
			return child;
		}
	}
	return alloc_dev(parent, path);
}

/**
 * Given a bus and a devfn number, find the device structure
 * @param bus The bus number
 * @param devfn a device/function number
 * @return pointer to the device structure
 */
struct device *dev_find_slot(unsigned int bus, unsigned int devfn)
{
	struct device *dev, *result;

	result = 0;
	for (dev = all_devices; dev; dev = dev->next) {
		if ((dev->bus->secondary == bus) && 
			(dev->path.u.pci.devfn == devfn)) {
			result = dev;
			break;
		}
	}
	return result;
}

/** Find a device of a given vendor and type
 * @param vendor Vendor ID (e.g. 0x8086 for Intel)
 * @param device Device ID
 * @param from Pointer to the device structure, used as a starting point
 *        in the linked list of all_devices, which can be 0 to start at the 
 *        head of the list (i.e. all_devices)
 * @return Pointer to the device struct 
 */
struct device *dev_find_device(unsigned int vendor, unsigned int device, struct device *from)
{
	if (!from)
		from = all_devices;
	else
		from = from->next;
	while (from && (from->vendor != vendor || from->device != device))
		from = from->next;
	return from;
}

/** Find a device of a given class
 * @param class Class of the device
 * @param from Pointer to the device structure, used as a starting point
 *        in the linked list of all_devices, which can be 0 to start at the 
 *        head of the list (i.e. all_devices)
 * @return Pointer to the device struct 
 */
struct device *dev_find_class(unsigned int class, struct device *from)
{
	if (!from)
		from = all_devices;
	else
		from = from->next;
	while (from && (from->class & 0xffffff00) != class)
		from = from->next;
	return from;
}


const char *dev_path(device_t dev)
{
	static char buffer[DEVICE_PATH_MAX];
	buffer[0] = '\0';
	if (!dev) {
		memcpy(buffer, "<null>", 7);
	}
	else {
		switch(dev->path.type) {
		case DEVICE_PATH_ROOT:
			memcpy(buffer, "Root Device", 12);
			break;
		case DEVICE_PATH_PCI:
			sprintf(buffer, "PCI: %02x:%02x.%01x",
				dev->bus->secondary, 
				PCI_SLOT(dev->path.u.pci.devfn), PCI_FUNC(dev->path.u.pci.devfn));
			break;
		case DEVICE_PATH_PNP:
			sprintf(buffer, "PNP: %04x.%01x",
				dev->path.u.pnp.port, dev->path.u.pnp.device);
			break;
		case DEVICE_PATH_I2C:
			sprintf(buffer, "I2C: %02x",
				dev->path.u.i2c.device);
			break;
		default:
			printk_err("Unknown device path type: %d\n", dev->path.type);
			break;
		}
	}
	return buffer;
}

int path_eq(struct device_path *path1, struct device_path *path2)
{
	int equal = 0;
	if (path1->type == path2->type) {
		switch(path1->type) {
		case DEVICE_PATH_NONE:
			break;
		case DEVICE_PATH_ROOT:
			equal = 1;
			break;
		case DEVICE_PATH_PCI:
			equal = (path1->u.pci.bus == path2->u.pci.bus) &&
				(path1->u.pci.devfn == path2->u.pci.devfn);
			break;
		case DEVICE_PATH_PNP:
			equal = (path1->u.pnp.port == path2->u.pnp.port) &&
				(path1->u.pnp.device == path2->u.pnp.device);
			break;
		case DEVICE_PATH_I2C:
			equal = (path1->u.i2c.device == path2->u.i2c.device);
			break;
		default:
			printk_err("Uknown device type: %d\n", path1->type);
			break;
		}
	}
	return equal;
}
