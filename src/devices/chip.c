/* chips are arbitrary chips (superio, southbridge, etc.)
 * They have private structures that define chip resources and default 
 * settings. They have four externally visible functions for control. 
 * They have a generic component which applies to all chips for 
 * path, etc. 
 */

#include <console/console.h>
#include <device/chip.h>
#include <device/pci.h>

/**
 * @brief Configure static devices
 *
 * Starting from the static device 'root', walk the tree and configure each
 * device by calling the device specific chip_control::enable().
 *
 * This function is only an iterator, the exact definition of 'configure'
 * depends on the device specific implementation of chip_control::enable(). 
 *
 * @param root root of the static device tree to be configured.
 * @param pass pass of the configuration operation to be perfromed.
 *
 * @see chip_pass
 * @see chip_control::enable
 */  
void chip_configure(struct chip *root, enum chip_pass pass)
{
	struct chip *c;

	for (c = root; c; c = c->next) {
		if (c->control && c->control->enable)
			c->control->enable(c, pass);
	}

	for (c = root; c; c = c->next) {
		if (c->children)
			chip_configure(c->children, pass);
	}
}

/**
 * @brief Convert static device structures to dynamic structures.
 *
 * A static device may contain one or more dynamic devices. Dynamic device
 * structures of these devices have to be generated before the enumeration
 * of dynamic devices. This function converts a static chip structure to a
 * set of dynamic device structures.
 *
 * This function is the generic method called by enumerate_static_device_chain()
 * for static devices. Devices extend this default behavior by defining their
 * own chip_controll::enumerate(). Generally, device specific
 * chip_control::enumerate() method calls this function as its last operation.
 *
 * @param chip static chip structure to be converted.
 *
 */
void chip_enumerate(struct chip *chip)
{
	struct chip *child;
	device_t dev;
	int link;
	int i;

	dev = 0;
	link = 0;

	if (chip->control && chip->control->name) {
		printk_debug("Enumerating: %s\n", chip->control->name);
	}

	for (i = 0; i < MAX_CHIP_PATHS; i++) {
		int identical_paths;
		identical_paths = 
			(i > 0) &&
			(path_eq(&chip->path[i - 1].path, &chip->path[i].path));
		if (!identical_paths) {
			struct bus *parent;
			int bus;
			link = 0;
			dev = 0;
			parent = chip->bus;
			switch(chip->path[i].path.type) {
			case DEVICE_PATH_NONE:
				/* no dynamic device associated */
				break;
			case DEVICE_PATH_PCI:
				bus = chip->path[i].path.u.pci.bus;
				if (bus != 0) {
					device_t dev;
					int i = 1;
					dev = chip->dev;
					while (dev && (i != bus)) {
						dev = dev->next;
						i++;
					}
					if ((i == bus) && dev) {
						parent = &dev->link[0];
					}
				}
				/* Fall through */
			default:
				dev = alloc_dev(parent, &chip->path[i].path);
				break;
			}
		} else {
			link += 1;
		}

		if (dev) {
			struct chip_resource *res, *res_limit;
			printk_spew("path (%p) %s %s",
				    dev, dev_path(dev),
				    identical_paths?"identical":"");
			printk_spew(" parent: (%p) %s\n",
				    dev->bus->dev,  dev_path(dev->bus->dev));
			dev->chip = chip;
			dev->enable = chip->path[i].enable;
			dev->links = link + 1;
			for (child = chip->children; child; child = child->next) {
				if (!child->bus && child->link == i) {
					child->bus = &dev->link[link];
				}
			}
			res = &chip->path[i].resource[0];
			res_limit = &chip->path[i].resource[MAX_RESOURCES];
			for(; res < res_limit; res++) {
				if (res->flags) {
					struct resource *resource;
					resource = get_resource(dev, res->index);
					resource->flags = res->flags | IORESOURCE_FIXED | IORESOURCE_ASSIGNED;
					resource->base = res->base;
				}
			}
		}

		if (dev && !chip->dev) {
			chip->dev = dev;
		}
	}

	for (child = chip->children; child; child = child->next) {
		if (!child->bus) {
			child->bus = &chip->dev->link[0];
		}
	}
}

/**
 * @brief Enumerate a static device tree.
 *
 * A static device chain is a linked list of static device structures which are
 * on the same branch of the static device tree. This function does not only
 * enumerate the devices on a single chain, as its name suggest, it also walks
 * into the subordinary chains. It calls the device specific
 * chip_control::enumerate() of the device if one exists or calls the generic
 * chip_enumerate(). 
 *
 * This function is only an iterator, the exact definition of 'enumerate'
 * depends on the implementation of the generic chip_enumerate() and/or device
 * specific chip_control::enumerate(). 
 *
 * @param root static chip structure to start with.
 *
 * @see chip_control::enumerate()
 */
static void enumerate_static_device_chain(struct chip *root)
{
	struct chip *chip;

	for (chip = root; chip; chip = chip->next) {
		void (*enumerate)(struct chip *chip);
		enumerate = chip_enumerate;
		if (chip->control && chip->control->enumerate) {
			enumerate = chip->control->enumerate;
		}
		enumerate(chip);
	}

	for (chip = root; chip; chip = chip->next) {
		if (chip->children) {
			enumerate_static_device_chain(chip->children);
		}
	}
}

/**
 * @brief Enumerate static devices in the system.
 *
 * \note The definition of 'enumerate' is not clear in this context. Does it mean
 * probe ?
 *
 * \note How do we determine the existence of the static devices ? Static
 * devices are listed in the config file and generated at compile time by config
 * tool. This function is called at certain point in the early stage of
 * LinuxBIOS. It uses the chip_enumerate() function to convert the static
 * structures into dynamic ones. What if the static devices listed in the config
 * file does actually not exist in the system ? Is there any side effect of
 * these 'phantom' device structures
 *
 * The static device does not necesarry conform to the dynamic device tree in
 * the system.
 */
void enumerate_static_devices(void)
{
	enumerate_static_device_chain(&static_root);
}
