/*
 * sconfig, coreboot device tree compiler
 *
 * Copyright (C) 2010 coresystems GmbH
 *                 written by Patrick Georgi <patrick.georgi@coresystems.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

enum devtype { chip, device };

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
struct device;
struct device {
	int id;
	int enabled;
	int used;
	int multidev;
	int link;
	int rescnt;
	int chiph_exists;
	int subsystem_vendor;
	int subsystem_device;
	int inherit_subsystem;
	char *ops;
	char *name;
	char *name_underscore;
	char *path;
	int path_a;
	int path_b;
	int bustype;
	struct pci_irq_info pci_irq_info[4];
	enum devtype type;
	struct device *parent;
	struct device *bus;
	struct device *next;
	struct device *nextdev;
	struct device *children;
	struct device *latestchild;
	struct device *next_sibling;
	struct device *sibling;
	struct device *chip;
	struct resource *res;
	struct reg *reg;
};

struct device *head;

struct header;
struct header {
	char *name;
	int chiph_exists;
	struct header *next;
};

void fold_in(struct device *parent);

void postprocess_devtree(void);
struct device *new_chip(struct device *parent, struct device *bus, char *path);
void add_header(struct device *dev);
struct device *new_device(struct device *parent, struct device *busdev, const int bus, const char *devnum, int enabled);
void alias_siblings(struct device *d);
void add_resource(struct device *dev, int type, int index, int base);
void add_register(struct device *dev, char *name, char *val);
void add_pci_subsystem_ids(struct device *dev, int vendor, int device, int inherit);
