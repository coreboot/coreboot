/* chips are arbitrary chips (superio, southbridge, etc.)
 * They have private structures that define chip resources and default 
 * settings. They have four externally visible functions for control. 
 * They have a generic component which applies to all chips for 
 * path, etc. 
 */

#include <console/console.h>
#include <device/chip.h>
#include <device/pci.h>

void
chip_configure(struct chip *root, enum chip_pass pass)
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

/** Convert a static struct chip structure to a set of dynamic device structures.
 * @param chip Static chip structure to start with.
 */

void chip_enumerate(struct chip *chip)
{
	struct chip *child;
	device_t dev;
	int link;
	int i;
	dev = 0;
	link = 0;
#if 1
	if (chip->control && chip->control->name) {
		printk_debug("Enumerating: %s\n", chip->control->name);
	}
#endif
	for(i = 0; i < MAX_CHIP_PATHS; i++) {
		int identical_paths;
		identical_paths = 
			(i > 0) &&
			(path_eq(&chip->path[i - 1].path, &chip->path[i].path)) &&
			(chip->path[i - 1].channel == chip->path[i].channel);
		if (!identical_paths) {
			link = 0;
			dev = 0;
			switch(chip->path[i].path.type) {
			case DEVICE_PATH_NONE:
				break;
			default:
				dev = alloc_dev(chip->bus, &chip->path[i].path);
				break;
			}
		}
		else {
			link += 1;
		}
		if (dev) {
			printk_spew("path %s %s\n", dev_path(dev), identical_paths?"identical":"");
			dev->enable = chip->path[i].enable;
			dev->links = link + 1;
			for(child = chip->children; child; child = child->next) {
				if (!child->bus &&
					child->path[0].channel == i) {
					child->bus = &dev->link[link];
				}
			}
		}
		if (dev && !chip->dev) {
			chip->dev = dev;
		}
	}
	for(child = chip->children; child; child = child->next) {
		if (!child->bus) {
			child->bus = &chip->dev->link[0];
		}
	}
}

static void enumerate_static_device_chain(struct chip *root)
{
	struct chip *chip;
	for(chip = root; chip; chip = chip->next) {
		void (*enumerate)(struct chip *chip);
		enumerate = chip_enumerate;
		if (chip->control && chip->control->enumerate) {
			enumerate = chip->control->enumerate;
		}
		enumerate(chip);
	}

	for(chip = root; chip; chip = chip->next) {
		if (chip->children) {
			enumerate_static_device_chain(chip->children);
		}
	}
}

void enumerate_static_devices(void)
{
	enumerate_static_device_chain(&static_root);
}
