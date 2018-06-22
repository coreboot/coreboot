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

#include <ctype.h>
#include "sconfig.h"
#include "sconfig.tab.h"

extern int linenum;

/* Maintains list of all the unique chip structures for the board. */
static struct chip chip_header;

/*
 * This is intentionally shared between chip and device structure ids because it
 * is easier to track the order of parsing for chip and device.
 */
static int count = 0;

typedef enum {
	UNSLASH,
	SPLIT_1ST,
	TO_LOWER,
	TO_UPPER,
} translate_t;

/*
 * Mainboard is assumed to have a root device whose bus is the parent of all the
 * devices that are added by parsing the devicetree file. This device has a
 * mainboard chip instance associated with it.
 *
 *
 *
 *                 +------------------------+                +----------------------+
 *                 |       Root device      |                |  Mainboard           |
 *       +---------+     (base_root_dev)    +--------------->+  instance            +
 *       |         |                        | chip_instance  |  (mainboard_instance)|
 *       |         +------------------------+                |                      |
 *       |                      |                            +----------------------+
 *       |                      | bus                                  |
 *       | parent               v                                      |
 *       |            +-------------------+                            |
 *       |            |     Root bus      |                            |
 *       +----------->+  (base_root_bus)  |                            |
 *                    |                   |                            |
 *                    +-------------------+                            |
 *                              |                                      |
 *                              | children                             | chip
 *                              v                                      |
 *                              X                                      |
 *                        (new devices will                            |
 *                         be added here as                            |
 *                         children)                                   |
 *                                                                     |
 *                                                                     |
 *                                                                     |
 *                                                             +-------+----------+
 *                                                             |                  |
 *                                                             |  Mainboard chip  +----------->X (new chips will be
 *                                                             | (mainboard_chip) |               added here)
 *                                                             |                  |
 *                                                             +------------------+
 *
 *
 */

/* Root device of primary tree. */
static struct device base_root_dev;

/* Root device of override tree (if applicable). */
static struct device override_root_dev;

static struct chip_instance mainboard_instance;

static struct bus base_root_bus = {
	.id = 0,
	.dev = &base_root_dev,
};

static struct device base_root_dev = {
	.name = "dev_root",
	.id = 0,
	.chip_instance = &mainboard_instance,
	.path = " .type = DEVICE_PATH_ROOT ",
	.ops = "&default_dev_ops_root",
	.parent = &base_root_bus,
	.enabled = 1,
	.bus = &base_root_bus,
};

static struct bus override_root_bus = {
	.id = 0,
	.dev = &override_root_dev,
};

static struct device override_root_dev = {
	.name = "override_root",
	.id = 0,
	/*
	 * Override tree root device points to the same mainboard chip instance
	 * as the base tree root device. It should not cause any side-effects
	 * since the mainboard chip instance pointer in override tree will just
	 * be ignored.
	 */
	.chip_instance = &mainboard_instance,
	.path = " .type = DEVICE_PATH_ROOT ",
	.ops = "&default_dev_ops_root",
	.parent = &override_root_bus,
	.enabled = 1,
	.bus = &override_root_bus,
};

static struct chip mainboard_chip = {
	.name = "mainboard",
	.name_underscore = "mainboard",
	.instance = &mainboard_instance,
};

static struct chip_instance mainboard_instance = {
	.id = 0,
	.chip = &mainboard_chip,
};

/* This is the parent of all devices added by parsing the devicetree file. */
struct bus *root_parent;

struct queue_entry {
	void *data;
	struct queue_entry *next;
	struct queue_entry *prev;
};

#define S_ALLOC(_s)	s_alloc(__func__, _s)

static void *s_alloc(const char *f, size_t s)
{
	void *data = calloc(1, s);
	if (!data) {
		fprintf(stderr, "%s: Failed to alloc mem!\n", f);
		exit(1);
	}
	return data;
}

static struct queue_entry *new_queue_entry(void *data)
{
	struct queue_entry *e = S_ALLOC(sizeof(*e));

	e->data = data;
	e->next = e->prev = e;
	return e;
}

static void enqueue_tail(struct queue_entry **q_head, void *data)
{
	struct queue_entry *tmp = new_queue_entry(data);
	struct queue_entry *q = *q_head;

	if (!q) {
		*q_head = tmp;
		return;
	}

	q->prev->next = tmp;
	tmp->prev = q->prev;
	q->prev = tmp;
	tmp->next = q;
}

static void *dequeue_tail(struct queue_entry **q_head)
{
	struct queue_entry *q = *q_head;
	struct queue_entry *tmp;
	void *data;

	if (!q)
		return NULL;

	tmp = q->prev;

	if (tmp == q)
		*q_head = NULL;
	else {
		tmp->prev->next = q;
		q->prev = tmp->prev;
	}

	data = tmp->data;
	free(tmp);

	return data;
}

static void *dequeue_head(struct queue_entry **q_head)
{
	struct queue_entry *q = *q_head;
	struct queue_entry *tmp = q;
	void *data;

	if (!q)
		return NULL;

	if (q->next == q)
		*q_head = NULL;
	else {
		q->next->prev = q->prev;
		q->prev->next = q->next;
		*q_head = q->next;
	}

	data = tmp->data;
	free(tmp);

	return data;
}

static void *peek_queue_head(struct queue_entry *q_head)
{
	if (!q_head)
		return NULL;

	return q_head->data;
}

static struct queue_entry *chip_q_head;

void chip_enqueue_tail(void *data)
{
	enqueue_tail(&chip_q_head, data);
}

void *chip_dequeue_tail(void)
{
	return dequeue_tail(&chip_q_head);
}

int yywrap(void)
{
	return 1;
}

void yyerror(char const *str)
{
	extern char *yytext;
	fprintf(stderr, "line %d: %s: %s\n", linenum + 1, yytext, str);
	exit(1);
}

char *translate_name(const char *str, translate_t mode)
{
	char *b, *c;
	b = c = strdup(str);
	while (c && *c) {
		if ((mode == SPLIT_1ST) && (*c == '/')) {
			*c = 0;
			break;
		}
		if (*c == '/')
			*c = '_';
		if (*c == '-')
			*c = '_';
		if (mode == TO_UPPER)
			*c = toupper(*c);
		if (mode == TO_LOWER)
			*c = tolower(*c);
		c++;
	}
	return b;
}

static struct chip *get_chip(char *path)
{
	struct chip *h = &chip_header;

	while (h->next) {
		int result = strcmp(path, h->next->name);
		if (result == 0)
			return h->next;

		if (result < 0)
			break;

		h = h->next;
	}

	struct chip *new_chip = S_ALLOC(sizeof(struct chip));
	new_chip->next = h->next;
	h->next = new_chip;

	new_chip->chiph_exists = 1;
	new_chip->name = path;
	new_chip->name_underscore = translate_name(path, UNSLASH);

	struct stat st;
	char *chip_h = S_ALLOC(strlen(path) + 18);
	sprintf(chip_h, "src/%s", path);
	if ((stat(chip_h, &st) == -1) && (errno == ENOENT)) {
		/* root_complex gets away without a separate directory, but
		 * exists on on pretty much all AMD chipsets.
		 */
		if (!strstr(path, "/root_complex")) {
			fprintf(stderr, "ERROR: Chip component %s does not exist.\n",
				path);
			exit(1);
		}
	}

	sprintf(chip_h, "src/%s/chip.h", path);

	if ((stat(chip_h, &st) == -1) && (errno == ENOENT))
		new_chip->chiph_exists = 0;

	free(chip_h);

	return new_chip;
}

struct chip_instance *new_chip_instance(char *path)
{
	struct chip *chip = get_chip(path);
	struct chip_instance *instance = S_ALLOC(sizeof(*instance));

	instance->id = ++count;
	instance->chip = chip;
	instance->next = chip->instance;
	chip->instance = instance;

	return instance;
}

/*
 * Allocate a new bus for the provided device.
 *   - If this is the first bus being allocated under this device, then its id
 *     is set to 0 and bus and last_bus are pointed to the newly allocated bus.
 *   - If this is not the first bus under this device, then its id is set to 1
 *     plus the id of last bus and newly allocated bus is added to the list of
 *     buses under the device. last_bus is updated to point to the newly
 *     allocated bus.
 */
static void alloc_bus(struct device *dev)
{
	struct bus *bus = S_ALLOC(sizeof(*bus));

	bus->dev = dev;

	if (dev->last_bus == NULL)  {
		bus->id = 0;
		dev->bus = bus;
	} else {
		bus->id = dev->last_bus->id + 1;
		dev->last_bus->next_bus = bus;
	}

	dev->last_bus = bus;
}

/*
 * Allocate a new device under the given parent. This function allocates a new
 * device structure under the provided parent bus and allocates a bus structure
 * under the newly allocated device.
 */
static struct device *alloc_dev(struct bus *parent)
{
	struct device *dev = S_ALLOC(sizeof(*dev));

	dev->id = ++count;
	dev->parent = parent;
	dev->subsystem_vendor = -1;
	dev->subsystem_device = -1;

	alloc_bus(dev);

	return dev;
}

/*
 * This function scans the children of given bus to see if any device matches
 * the new device that is requested.
 *
 * Returns pointer to the node if found, else NULL.
 */
static struct device *get_dev(struct bus *parent, int path_a, int path_b,
			      int bustype, struct chip_instance *chip_instance)
{
	struct device *child = parent->children;

	while (child) {
		if ((child->path_a == path_a) && (child->path_b == path_b) &&
		    (child->bustype == bustype) &&
		    (child->chip_instance == chip_instance))
			return child;

		child = child->sibling;
	}

	return NULL;
}

struct device *new_device(struct bus *parent,
			  struct chip_instance *chip_instance,
			  const int bustype, const char *devnum,
			  int enabled)
{
	char *tmp;
	int path_a;
	int path_b = 0;
	struct device *new_d;

	path_a = strtol(devnum, &tmp, 16);
	if (*tmp == '.') {
		tmp++;
		path_b = strtol(tmp, NULL, 16);
	}

	/* If device is found under parent, no need to allocate new device. */
	new_d = get_dev(parent, path_a, path_b, bustype, chip_instance);
	if (new_d) {
		alloc_bus(new_d);
		return new_d;
	}

	new_d = alloc_dev(parent);

	new_d->bustype = bustype;

	new_d->path_a = path_a;
	new_d->path_b = path_b;

	char *name = S_ALLOC(10);
	sprintf(name, "_dev%d", new_d->id);
	new_d->name = name;

	new_d->enabled = enabled;
	new_d->chip_instance = chip_instance;

	struct device *c = parent->children;
	if (c) {
		while (c->sibling)
			c = c->sibling;
		c->sibling = new_d;
	} else
		parent->children = new_d;

	switch (bustype) {
	case PCI:
		new_d->path = ".type=DEVICE_PATH_PCI,{.pci={ .devfn = PCI_DEVFN(0x%x,%d)}}";
		break;

	case PNP:
		new_d->path = ".type=DEVICE_PATH_PNP,{.pnp={ .port = 0x%x, .device = 0x%x }}";
		break;

	case I2C:
		new_d->path = ".type=DEVICE_PATH_I2C,{.i2c={ .device = 0x%x, .mode_10bit = %d }}";
		break;

	case APIC:
		new_d->path = ".type=DEVICE_PATH_APIC,{.apic={ .apic_id = 0x%x }}";
		break;

	case CPU_CLUSTER:
		new_d->path = ".type=DEVICE_PATH_CPU_CLUSTER,{.cpu_cluster={ .cluster = 0x%x }}";
		break;

	case CPU:
		new_d->path = ".type=DEVICE_PATH_CPU,{.cpu={ .id = 0x%x }}";
		break;

	case DOMAIN:
		new_d->path = ".type=DEVICE_PATH_DOMAIN,{.domain={ .domain = 0x%x }}";
		break;

	case IOAPIC:
		new_d->path = ".type=DEVICE_PATH_IOAPIC,{.ioapic={ .ioapic_id = 0x%x }}";
		break;

	case GENERIC:
		new_d->path = ".type=DEVICE_PATH_GENERIC,{.generic={ .id = 0x%x, .subid = 0x%x }}";
		break;

	case SPI:
		new_d->path = ".type=DEVICE_PATH_SPI,{.spi={ .cs = 0x%x }}";
		break;

	case USB:
		new_d->path = ".type=DEVICE_PATH_USB,{.usb={ .port_type = %d, .port_id = %d }}";
		break;

	case MMIO:
		new_d->path = ".type=DEVICE_PATH_MMIO,{.mmio={ .addr = 0x%x }}";
		break;
	}

	return new_d;
}

void add_resource(struct bus *bus, int type, int index, int base)
{
	struct device *dev = bus->dev;
	struct resource *r = S_ALLOC(sizeof(struct resource));

	r->type = type;
	r->index = index;
	r->base = base;
	if (dev->res) {
		struct resource *head = dev->res;
		while (head->next)
			head = head->next;
		head->next = r;
	} else {
		dev->res = r;
	}
}

void add_register(struct chip_instance *chip_instance, char *name, char *val)
{
	struct reg *r = S_ALLOC(sizeof(struct reg));

	r->key = name;
	r->value = val;
	if (chip_instance->reg) {
		struct reg *head = chip_instance->reg;
		// sorting to be equal to sconfig's behaviour
		int sort = strcmp(r->key, head->key);
		if (sort == 0) {
			printf("ERROR: duplicate 'register' key.\n");
			exit(1);
		}
		if (sort < 0) {
			r->next = head;
			chip_instance->reg = r;
		} else {
			while ((head->next)
			       && (strcmp(head->next->key, r->key) < 0))
				head = head->next;
			r->next = head->next;
			head->next = r;
		}
	} else {
		chip_instance->reg = r;
	}
}

void add_pci_subsystem_ids(struct bus *bus, int vendor, int device,
			   int inherit)
{
	struct device *dev = bus->dev;

	if (dev->bustype != PCI && dev->bustype != DOMAIN) {
		printf("ERROR: 'subsystem' only allowed for PCI devices\n");
		exit(1);
	}

	dev->subsystem_vendor = vendor;
	dev->subsystem_device = device;
	dev->inherit_subsystem = inherit;
}

void add_ioapic_info(struct bus *bus, int apicid, const char *_srcpin,
		     int irqpin)
{
	int srcpin;
	struct device *dev = bus->dev;

	if (!_srcpin || strlen(_srcpin) < 4 || strncasecmp(_srcpin, "INT", 3) ||
	    _srcpin[3] < 'A' || _srcpin[3] > 'D') {
		printf("ERROR: malformed ioapic_irq args: %s\n", _srcpin);
		exit(1);
	}

	srcpin = _srcpin[3] - 'A';

	if (dev->bustype != PCI && dev->bustype != DOMAIN) {
		printf("ERROR: ioapic config only allowed for PCI devices\n");
		exit(1);
	}

	if (srcpin > 3) {
		printf("ERROR: srcpin '%d' invalid\n", srcpin);
		exit(1);
	}
	dev->pci_irq_info[srcpin].ioapic_irq_pin = irqpin;
	dev->pci_irq_info[srcpin].ioapic_dst_id = apicid;
}

static int dev_has_children(struct device *dev)
{
	struct bus *bus = dev->bus;

	while (bus) {
		if (bus->children)
			return 1;
		bus = bus->next_bus;
	}

	return 0;
}

static void pass0(FILE *fil, struct device *ptr, struct device *next)
{
	if (ptr == &base_root_dev) {
		fprintf(fil, "DEVTREE_CONST struct bus %s_links[];\n",
			ptr->name);
		return;
	}

	fprintf(fil, "DEVTREE_CONST static struct device %s;\n", ptr->name);
	if (ptr->res)
		fprintf(fil, "DEVTREE_CONST struct resource %s_res[];\n",
			ptr->name);
	if (dev_has_children(ptr))
		fprintf(fil, "DEVTREE_CONST struct bus %s_links[];\n",
			ptr->name);

	if (next)
		return;

	fprintf(fil,
		"DEVTREE_CONST struct device * DEVTREE_CONST last_dev = &%s;\n",
		ptr->name);
}

static void emit_resources(FILE *fil, struct device *ptr)
{
	if (ptr->res == NULL)
		return;

	int i = 1;
	fprintf(fil, "DEVTREE_CONST struct resource %s_res[] = {\n", ptr->name);
	struct resource *r = ptr->res;
	while (r) {
		fprintf(fil,
			"\t\t{ .flags=IORESOURCE_FIXED | IORESOURCE_ASSIGNED | IORESOURCE_");
		if (r->type == IRQ)
			fprintf(fil, "IRQ");
		if (r->type == DRQ)
			fprintf(fil, "DRQ");
		if (r->type == IO)
			fprintf(fil, "IO");
		fprintf(fil, ", .index=0x%x, .base=0x%x,", r->index,
			r->base);
		if (r->next)
			fprintf(fil, ".next=&%s_res[%d]},\n", ptr->name,
				i++);
		else
			fprintf(fil, ".next=NULL },\n");
		r = r->next;
	}

	fprintf(fil, "\t };\n");
}

static void emit_bus(FILE *fil, struct bus *bus)
{
	fprintf(fil, "\t\t[%d] = {\n", bus->id);
	fprintf(fil, "\t\t\t.link_num = %d,\n", bus->id);
	fprintf(fil, "\t\t\t.dev = &%s,\n", bus->dev->name);
	if (bus->children)
		fprintf(fil, "\t\t\t.children = &%s,\n", bus->children->name);

	if (bus->next_bus)
		fprintf(fil, "\t\t\t.next=&%s_links[%d],\n", bus->dev->name,
			bus->id + 1);
	else
		fprintf(fil, "\t\t\t.next = NULL,\n");
	fprintf(fil, "\t\t},\n");
}

static void emit_dev_links(FILE *fil, struct device *ptr)
{
	fprintf(fil, "DEVTREE_CONST struct bus %s_links[] = {\n",
		ptr->name);

	struct bus *bus = ptr->bus;

	while (bus) {
		emit_bus(fil, bus);
		bus = bus->next_bus;
	}

	fprintf(fil, "\t};\n");
}

static void pass1(FILE *fil, struct device *ptr, struct device *next)
{
	int pin;
	struct chip_instance *chip_ins = ptr->chip_instance;
	int has_children = dev_has_children(ptr);

	if (ptr != &base_root_dev)
		fprintf(fil, "static ");
	fprintf(fil, "DEVTREE_CONST struct device %s = {\n", ptr->name);
	fprintf(fil, "#if !DEVTREE_EARLY\n");
	fprintf(fil, "\t.ops = %s,\n", (ptr->ops) ? (ptr->ops) : "0");
	fprintf(fil, "#endif\n");
	fprintf(fil, "\t.bus = &%s_links[%d],\n", ptr->parent->dev->name,
		ptr->parent->id);
	fprintf(fil, "\t.path = {");
	fprintf(fil, ptr->path, ptr->path_a, ptr->path_b);
	fprintf(fil, "},\n");
	fprintf(fil, "\t.enabled = %d,\n", ptr->enabled);
	fprintf(fil, "\t.on_mainboard = 1,\n");
	if (ptr->subsystem_vendor > 0)
		fprintf(fil, "\t.subsystem_vendor = 0x%04x,\n",
			ptr->subsystem_vendor);

	for (pin = 0; pin < 4; pin++) {
		if (ptr->pci_irq_info[pin].ioapic_irq_pin > 0)
			fprintf(fil,
				"\t.pci_irq_info[%d].ioapic_irq_pin = %d,\n",
				pin, ptr->pci_irq_info[pin].ioapic_irq_pin);

		if (ptr->pci_irq_info[pin].ioapic_dst_id > 0)
			fprintf(fil,
				"\t.pci_irq_info[%d].ioapic_dst_id = %d,\n",
				pin, ptr->pci_irq_info[pin].ioapic_dst_id);
	}

	if (ptr->subsystem_device > 0)
		fprintf(fil, "\t.subsystem_device = 0x%04x,\n",
			ptr->subsystem_device);

	if (ptr->res) {
		fprintf(fil, "\t.resource_list = &%s_res[0],\n",
			ptr->name);
	}
	if (has_children)
		fprintf(fil, "\t.link_list = &%s_links[0],\n",
			ptr->name);
	else
		fprintf(fil, "\t.link_list = NULL,\n");
	if (ptr->sibling)
		fprintf(fil, "\t.sibling = &%s,\n", ptr->sibling->name);
	fprintf(fil, "#if !DEVTREE_EARLY\n");
	fprintf(fil, "\t.chip_ops = &%s_ops,\n",
		chip_ins->chip->name_underscore);
	if (chip_ins == &mainboard_instance)
		fprintf(fil, "\t.name = mainboard_name,\n");
	fprintf(fil, "#endif\n");
	if (chip_ins->chip->chiph_exists)
		fprintf(fil, "\t.chip_info = &%s_info_%d,\n",
			chip_ins->chip->name_underscore, chip_ins->id);
	if (next)
		fprintf(fil, "\t.next=&%s\n", next->name);
	fprintf(fil, "};\n");

	emit_resources(fil, ptr);

	if (has_children)
		emit_dev_links(fil, ptr);
}

static void add_siblings_to_queue(struct queue_entry **bfs_q_head,
				  struct device *d)
{
	while (d) {
		enqueue_tail(bfs_q_head, d);
		d = d->sibling;
	}
}

static void add_children_to_queue(struct queue_entry **bfs_q_head,
				  struct device *d)
{
	struct bus *bus = d->bus;

	while (bus) {
		if (bus->children)
			add_siblings_to_queue(bfs_q_head, bus->children);
		bus = bus->next_bus;
	}
}

static void walk_device_tree(FILE *fil, struct device *ptr,
			     void (*func)(FILE *, struct device *,
					  struct device *))
{
	struct queue_entry *bfs_q_head = NULL;

	enqueue_tail(&bfs_q_head, ptr);

	while ((ptr = dequeue_head(&bfs_q_head))) {
		add_children_to_queue(&bfs_q_head, ptr);
		func(fil, ptr, peek_queue_head(bfs_q_head));
	}
}

static void emit_chip_headers(FILE *fil, struct chip *chip)
{
	struct chip *tmp = chip;

	fprintf(fil, "#include <device/device.h>\n");
	fprintf(fil, "#include <device/pci.h>\n");

	while (chip) {
		if (chip->chiph_exists)
			fprintf(fil, "#include \"%s/chip.h\"\n", chip->name);
		chip = chip->next;
	}
	fprintf(fil, "\n#if !DEVTREE_EARLY\n");
	fprintf(fil,
		"__attribute__((weak)) struct chip_operations mainboard_ops = {};\n");

	chip = tmp;
	while (chip) {
		fprintf(fil,
			"__attribute__((weak)) struct chip_operations %s_ops = {};\n",
			chip->name_underscore);
		chip = chip->next;
	}
	fprintf(fil, "#endif\n");
}

static void emit_chip_instance(FILE *fil, struct chip_instance *instance)
{
	fprintf(fil, "DEVTREE_CONST struct %s_config %s_info_%d = {",
		instance->chip->name_underscore,
		instance->chip->name_underscore,
		instance->id);

	if (instance->reg) {
		fprintf(fil, "\n");
		struct reg *r = instance->reg;
		while (r) {
			fprintf(fil, "\t.%s = %s,\n", r->key, r->value);
			r = r->next;
		}
	}
	fprintf(fil, "};\n\n");
}

static void emit_chips(FILE *fil)
{
	struct chip *chip = chip_header.next;
	struct chip_instance *instance;

	emit_chip_headers(fil, chip);

	for (; chip; chip = chip->next) {
		if (!chip->chiph_exists)
			continue;

		instance = chip->instance;
		while (instance) {
			emit_chip_instance(fil, instance);
			instance = instance->next;
		}
	}
}

static void inherit_subsystem_ids(FILE *file, struct device *dev,
				  struct device *next)
{
	struct device *p;

	if (dev->subsystem_vendor != -1 && dev->subsystem_device != -1) {
		/* user already gave us a subsystem vendor/device */
		return;
	}

	for (p = dev; p && p->parent->dev != p; p = p->parent->dev) {

		if (p->bustype != PCI && p->bustype != DOMAIN)
			continue;

		if (p->inherit_subsystem) {
			dev->subsystem_vendor = p->subsystem_vendor;
			dev->subsystem_device = p->subsystem_device;
			break;
		}
	}
}

static void usage(void)
{
	printf("usage: sconfig devicetree_file output_file [override_devicetree_file]\n");
	exit(1);
}

enum {
	DEVICEFILE_ARG = 1,
	OUTPUTFILE_ARG,
	OVERRIDE_DEVICEFILE_ARG,
};

#define MANDATORY_ARG_COUNT		3
#define OPTIONAL_ARG_COUNT		1
#define TOTAL_ARG_COUNT		(MANDATORY_ARG_COUNT + OPTIONAL_ARG_COUNT)

static void parse_devicetree(const char *file, struct bus *parent)
{
	FILE *filec = fopen(file, "r");
	if (!filec) {
		perror(NULL);
		exit(1);
	}

	yyrestart(filec);

	root_parent = parent;
	linenum = 0;

	yyparse();

	fclose(filec);
}

int main(int argc, char **argv)
{
	if ((argc < MANDATORY_ARG_COUNT) || (argc > TOTAL_ARG_COUNT))
		usage();

	const char *base_devtree = argv[DEVICEFILE_ARG];
	const char *outputc = argv[OUTPUTFILE_ARG];
	const char *override_devtree;

	parse_devicetree(base_devtree, &base_root_bus);

	if (argc == TOTAL_ARG_COUNT) {
		override_devtree = argv[OVERRIDE_DEVICEFILE_ARG];
		parse_devicetree(override_devtree, &override_root_bus);
	}

	FILE *autogen = fopen(outputc, "w");
	if (!autogen) {
		fprintf(stderr, "Could not open file '%s' for writing: ",
			outputc);
		perror(NULL);
		exit(1);
	}

	emit_chips(autogen);

	walk_device_tree(autogen, &base_root_dev, inherit_subsystem_ids);
	fprintf(autogen, "\n/* pass 0 */\n");
	walk_device_tree(autogen, &base_root_dev, pass0);
	fprintf(autogen, "\n/* pass 1 */\n");
	walk_device_tree(autogen, &base_root_dev, pass1);

	fclose(autogen);

	return 0;
}
