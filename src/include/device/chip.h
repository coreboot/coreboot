#ifndef DEVICE_CHIP_H
#define DEVICE_CHIP_H

#include <device/path.h>
#include <device/device.h>

/* chips are arbitrary chips (superio, southbridge, etc.)
 * They have private structures that define chip resources and default 
 * settings. They have four externally visible functions for control. 
 * They have a generic component which applies to all chips for 
 * path, etc. 
 */

/* some of the types of resources chips can control */
#if CONFIG_CHIP_CONFIGURE == 1
#define CONFIGURE(pass) chip_configure(&static_root, pass)
#else
#define CONFIGURE(pass)
#endif

enum chip_pass {
	CONF_PASS_PRE_CONSOLE,
	CONF_PASS_PRE_PCI,
	CONF_PASS_PRE_DEVICE_ENUMERATE,
	CONF_PASS_PRE_DEVICE_CONFIGURE,
	CONF_PASS_PRE_DEVICE_ENABLE,
	CONF_PASS_PRE_DEVICE_INITIALIZE,
	CONF_PASS_POST_PCI,
	CONF_PASS_PRE_BOOT
};


/* linkages from devices of a type (e.g. superio devices) 
 * to the actual physical PCI device. This type is used in an array of 
 * structs built by NLBConfig.py. We owe this idea to Plan 9.
 */

struct chip;
struct device;

/* there is one of these for each TYPE of chip */
struct chip_control {
	/* This is the print name for debugging */
	char *name;
	void (*enable)(struct chip *, enum chip_pass);
	void (*enumerate)(struct chip *chip);
	void (*enable_dev)(struct device *dev);
};

struct chip_resource {
	unsigned long flags;
	unsigned long index;
	unsigned long base;
};

struct chip_device_path {
	struct device_path path;
	unsigned channel;
	int enable;
	struct chip_resource resource[MAX_RESOURCES];
};

struct device;
struct bus;

#ifndef MAX_CHIP_PATHS
#define MAX_CHIP_PATHS 16
#endif
struct chip {
	unsigned link;
	struct chip_control *control; /* for this device */
	struct chip_device_path path[MAX_CHIP_PATHS]; /* can be 0, in which case the default is taken */
	char *configuration; /* can be 0. */
	struct chip *next, *children;
	/* there is one of these for each INSTANCE of a chip */
	void *chip_info; /* the dreaded "void *" */
	/* bus and device links into the device tree */
	struct bus *bus;
	struct device *dev;
};

extern struct chip static_root;
extern void chip_configure(struct chip *, enum chip_pass);
extern void chip_enumerate(struct chip *chip);
#endif /* DEVICE_CHIP_H */
