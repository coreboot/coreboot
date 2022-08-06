/* sconfig, coreboot device tree compiler */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

struct resource;
struct resource {
	int type;
	int index;
	int base;
	struct resource *next;
};

struct reg;
struct reg {
	char *key;
	char *value;
	struct reg *next;
};

struct pci_irq_info {
	int ioapic_irq_pin;
	int ioapic_dst_id;
};

struct fw_config_option;
struct fw_config_option {
	const char *name;
	uint64_t value;
	struct fw_config_option *next;
};

struct fw_config_field_bits;
struct fw_config_field_bits {
	unsigned int start_bit;
	unsigned int end_bit;
	struct fw_config_field_bits *next;
};

struct fw_config_field;
struct fw_config_field {
	const char *name;
	struct fw_config_field_bits *bits;
	struct fw_config_field *next;
	struct fw_config_option *options;
};
struct fw_config_probe;
struct fw_config_probe {
	const char *field;
	const char *option;
	struct fw_config_probe *next;
};

struct identifier {
	const char *id;
	struct identifier *next;
};

struct chip;
struct chip_instance {
	/* Monotonically increasing ID for each chip instance. */
	int id;

	/* Pointer to registers for this chip. */
	struct reg *reg;

	/* Pointer to references for this chip. */
	struct reg *ref;

	/* Pointer to chip of which this is instance. */
	struct chip *chip;

	/* Pointer to next instance of the same chip. */
	struct chip_instance *next;

	/*
	 * Pointer to corresponding chip instance in base devicetree.
	 * a) If the chip instance belongs to the base devicetree, then this pointer is set to
	 * NULL.
	 * b) If the chip instance belongs to override tree, then this pointer is set to its
	 * corresponding chip instance in base devicetree (if it exists), else to NULL.
	 *
	 * This is useful when generating chip instances and chip_ops for a device to determine
	 * if this is the instance to emit or if there is a base chip instance to use instead.
	 */
	struct chip_instance *base_chip_instance;
};

struct chip {
	/* Indicates if chip header exists for this chip. */
	int chiph_exists;

	/* Name of current chip. */
	char *name;

	/* Name of current chip normalized to _. */
	char *name_underscore;

	/* Pointer to first instance of this chip. */
	struct chip_instance *instance;

	/* Pointer to next chip. */
	struct chip *next;
};

struct device;
struct bus {
	/* Instance/ID of the bus under the device. */
	int id;

	/* Pointer to device to which this bus belongs. */
	struct device *dev;

	/* Pointer to list of children. */
	struct device *children;

	/* Pointer to next bus for the device. */
	struct bus *next_bus;
};

struct device {
	/* Indicates device status (enabled / hidden or not). */
	int enabled;
	int hidden;
	/* non-zero if the device should be included in all cases */
	int mandatory;

	/* Subsystem IDs for the device. */
	int subsystem_vendor;
	int subsystem_device;
	int inherit_subsystem;

	/* Name of this device. */
	char *name;

	/* Alias of this device (for internal references) */
	char *alias;

	/* Path of this device. */
	char *path;
	int path_a;
	int path_b;

	/* Type of bus that exists under this device. */
	int bustype;

	/* PCI IRQ info. */
	struct pci_irq_info pci_irq_info[4];

	/* Pointer to bus of parent on which this device resides. */
	struct bus *parent;

	/* Pointer to next child under the same parent. */
	struct device *sibling;

	/* Pointer to resources for this device. */
	struct resource *res;

	/* Pointer to chip instance for this device. */
	struct chip_instance *chip_instance;

	/* Pointer to list of buses under this device. */
	struct bus *bus;
	/* Pointer to last bus under this device. */
	struct bus *last_bus;

	/* Global identifier of the ops for this device. */
	char *ops_id;

	/* SMBIOS slot type */
	char *smbios_slot_type;

	/* SMBIOS slot data width */
	char *smbios_slot_data_width;

	/* SMBIOS slot description for reference designation */
	char *smbios_slot_designation;

	/* SMBIOS slot length */
	char *smbios_slot_length;

	/* SMBIOS type41 fields */
	int smbios_instance_id_valid;
	unsigned int smbios_instance_id;
	const char *smbios_refdes;

	/* List of field+option to probe. */
	struct fw_config_probe *probe;
};

extern struct bus *root_parent;

struct device *new_device_raw(struct bus *parent,
			      struct chip_instance *chip_instance,
			      const int bustype, const char *devnum,
			      char *alias, int status);

struct device *new_device_reference(struct bus *parent,
				    struct chip_instance *chip_instance,
				    const char *reference, int status);

void add_resource(struct bus *bus, int type, int index, int base);

void add_pci_subsystem_ids(struct bus *bus, int vendor, int device,
			   int inherit);

void add_ioapic_info(struct bus *bus, int apicid, const char *_srcpin,
		     int irqpin);

void add_slot_desc(struct bus *bus, char *type, char *length, char *designation,
		   char *data_width);

void add_smbios_dev_info(struct bus *bus, long instance_id, const char *refdes);

void yyrestart(FILE *input_file);

/* Add chip data to tail of queue. */
void chip_enqueue_tail(void *data);

/* Retrieve chip data from tail of queue. */
void *chip_dequeue_tail(void);

struct chip_instance *new_chip_instance(char *path);
void add_register(struct chip_instance *chip, char *name, char *val);
void add_reference(struct chip_instance *chip, char *name, char *alias);

struct fw_config_field *get_fw_config_field(const char *name);

void add_fw_config_field_bits(struct fw_config_field *field,
			      unsigned int start_bit, unsigned int end_bit);

struct fw_config_field *new_fw_config_field(const char *name, struct fw_config_field_bits *bits);

void add_fw_config_option(struct fw_config_field *field, const char *name,
			  uint64_t value);

void add_fw_config_probe(struct bus *bus, const char *field, const char *option);

void append_fw_config_bits(struct fw_config_field_bits **bits,
			   unsigned int start_bit, unsigned int end_bit);

void add_device_ops(struct bus *, char *ops_id);
