#include <console/console.h>
#include <device.h>

/**
 * Given a bus and a devfn number, find the device structure
 * @param bus The bus number
 * @param devfn a device/function number
 * @return pointer to the device structure
 */
struct device *dev_find_slot(unsigned int bus, unsigned int devfn)
{
	struct device *dev;

	for (dev = all_devices; dev; dev = dev->next)
		if (dev->bus->secondary == bus && dev->devfn == devfn)
			break;
	return dev;
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
	while (from && from->class != class)
		from = from->next;
	return from;
}

