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

struct device;
struct device {
	int id;
	int enabled;
	int used;
	int multidev;
	int link;
	int rescnt;
	int chiph_exists;
	char *ops;
	char *name;
	char *aliased_name;
	char *name_underscore;
	char *path;
	int path_a;
	int path_b;
	int bustype;
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

extern struct device *cur_parent, *cur_bus;

struct header;
struct header {
	char *name;
	struct header *next;
};

void fold_in(struct device *parent);

void postprocess_devtree(void);
struct device *new_chip(char *path);
void add_header(struct device *dev);
struct device *new_device(const int bus, const char *devnum, int enabled);
void alias_siblings(struct device *d);
void add_resource(int type, int index, int base);
void add_register(char *name, char *val);
