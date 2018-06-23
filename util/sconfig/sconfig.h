/*
 * sconfig, coreboot device tree compiler
 *
 * Copyright (C) 2010 coresystems GmbH
 *   written by Patrick Georgi <patrick@georgi-clan.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
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

struct chip;
struct chip_instance {
	/*
	 * Monotonically increasing ID for each newly allocated
	 * node(chip/device).
	 */
	int id;

	/* Pointer to registers for this chip. */
	struct reg *reg;

	/* Pointer to chip of which this is instance. */
	struct chip *chip;

	/* Pointer to next instance of the same chip. */
	struct chip_instance *next;

	/*
	 * Reference count - Indicates how many devices hold pointer to this
	 * chip instance.
	 */
	int ref_count;
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
	/* Monotonically increasing ID for the device. */
	int id;

	/* Indicates whether this device is enabled. */
	int enabled;

	/* Subsystem IDs for the device. */
	int subsystem_vendor;
	int subsystem_device;
	int inherit_subsystem;

	/* Name of this device. */
	char *name;

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
};

extern struct bus *root_parent;

struct device *new_device(struct bus *parent,
			  struct chip_instance *chip_instance,
			  const int bustype, const char *devnum,
			  int enabled);

void add_resource(struct bus *bus, int type, int index, int base);

void add_pci_subsystem_ids(struct bus *bus, int vendor, int device,
			   int inherit);

void add_ioapic_info(struct bus *bus, int apicid, const char *_srcpin,
		     int irqpin);

void yyrestart(FILE *input_file);

/* Add chip data to tail of queue. */
void chip_enqueue_tail(void *data);

/* Retrieve chip data from tail of queue. */
void *chip_dequeue_tail(void);

struct chip_instance *new_chip_instance(char *path);
void add_register(struct chip_instance *chip, char *name, char *val);
