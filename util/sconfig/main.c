/* sconfig, coreboot device tree compiler */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <ctype.h>
#include <getopt.h>
#include <inttypes.h>
#include <libgen.h>
/* stat.h needs to be included before commonlib/helpers.h to avoid errors.*/
#include <sys/stat.h>
#include <commonlib/helpers.h>
#include <stdint.h>
#include "sconfig.h"
#include "sconfig.tab.h"

extern int linenum;

/*
 * Maintains list of all the unique chip structures for the board.
 * This is shared across base and override device trees since we need to
 * generate headers for all chips added by both the trees.
 */
static struct chip chip_header;

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

/* Root device of chipset tree. */
static struct device chipset_root_dev;

/* Root device of override tree (if applicable). */
static struct device override_root_dev;

static struct chip_instance mainboard_instance;

static struct bus base_root_bus = {
	.id = 0,
	.dev = &base_root_dev,
};

static struct device base_root_dev = {
	.name = "dev_root",
	.chip_instance = &mainboard_instance,
	.path = " .type = DEVICE_PATH_ROOT ",
	.parent = &base_root_bus,
	.enabled = 1,
	.bus = &base_root_bus,
};

static struct bus chipset_root_bus = {
	.id = 0,
	.dev = &chipset_root_dev,
};

static struct device chipset_root_dev = {
	.name = "chipset_root",
	.chip_instance = &mainboard_instance,
	.path = " .type = DEVICE_PATH_ROOT ",
	.parent = &chipset_root_bus,
	.enabled = 1,
	.bus = &chipset_root_bus,
};

static struct bus override_root_bus = {
	.id = 0,
	.dev = &override_root_dev,
};

static struct device override_root_dev = {
	.name = "override_root",
	/*
	 * Override tree root device points to the same mainboard chip instance
	 * as the base tree root device. It should not cause any side-effects
	 * since the mainboard chip instance pointer in override tree will just
	 * be ignored.
	 */
	.chip_instance = &mainboard_instance,
	.path = " .type = DEVICE_PATH_ROOT ",
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

/* Global list of all `struct device_operations` identifiers to declare. */
static struct identifier *device_operations;

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
		 * exists on pretty much all AMD chipsets.
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

	instance->chip = chip;
	instance->next = chip->instance;
	chip->instance = instance;

	return instance;
}

/* List of fw_config fields added during parsing. */
static struct fw_config_field *fw_config_fields;

static struct fw_config_option *find_fw_config_option(struct fw_config_field *field,
						      const char *name)
{
	struct fw_config_option *option = field->options;

	while (option && option->name) {
		if (!strcmp(option->name, name))
			return option;
		option = option->next;
	}
	return NULL;
}

static struct fw_config_field *find_fw_config_field(const char *name)
{
	struct fw_config_field *field = fw_config_fields;

	while (field && field->name) {
		if (!strcmp(field->name, name))
			return field;
		field = field->next;
	}
	return NULL;
}

struct fw_config_field *get_fw_config_field(const char *name)
{
	struct fw_config_field *field = find_fw_config_field(name);

	/* Fail if the field does not exist, new fields must be added with a mask. */
	if (!field) {
		printf("ERROR: fw_config field not found: %s\n", name);
		exit(1);
	}
	return field;
}

static void append_fw_config_field(struct fw_config_field *add)
{
	struct fw_config_field *field = fw_config_fields;

	if (!fw_config_fields) {
		fw_config_fields = add;
	} else {
		while (field && field->next)
			field = field->next;
		field->next = add;
	}
}

void append_fw_config_bits(struct fw_config_field_bits **bits,
			   unsigned int start_bit, unsigned int end_bit)
{
	struct fw_config_field_bits *new_bits = S_ALLOC(sizeof(*new_bits));
	new_bits->start_bit = start_bit;
	new_bits->end_bit = end_bit;
	new_bits->next = NULL;

	if (*bits == NULL) {
		*bits = new_bits;
		return;
	}

	struct fw_config_field_bits *tmp = *bits;
	while (tmp->next)
		tmp = tmp->next;

	tmp->next = new_bits;
}

int fw_config_masks_overlap(struct fw_config_field *existing,
			     unsigned int start_bit, unsigned int end_bit)
{
	struct fw_config_field_bits *bits = existing->bits;
	while (bits) {
		if (start_bit <= bits->end_bit && end_bit >= bits->start_bit) {
			printf("ERROR: fw_config field [%u-%u] overlaps %s[%u-%u]\n",
			       start_bit, end_bit,
			       existing->name, bits->start_bit, bits->end_bit);
			return 1;
		}
		bits = bits->next;
	}

	return 0;
}

struct fw_config_field *new_fw_config_field(const char *name, struct fw_config_field_bits *bits)
{
	struct fw_config_field *field = find_fw_config_field(name);
	struct fw_config_field_bits *tmp;

	/* Don't allow re-defining a field, only adding new fields. */
	if (field) {
		printf("ERROR: fw_config field %s already exists\n", name);
		exit(1);
	}

	/* Check that each field is within 64 bits. */
	tmp = bits;
	while (tmp) {
		if (tmp->start_bit > tmp->end_bit || tmp->end_bit > 63) {
			printf("ERROR: fw_config field %s has invalid range %u-%u\n", name,
			       tmp->start_bit, tmp->end_bit);
			exit(1);
		}

		/* Check for overlap with an existing field. */
		struct fw_config_field *existing = fw_config_fields;
		while (existing) {
			if (fw_config_masks_overlap(existing, tmp->start_bit, tmp->end_bit))
				exit(1);
			existing = existing->next;
		}

		tmp = tmp->next;
	}

	field = S_ALLOC(sizeof(*field));
	field->name = name;
	field->bits = bits;
	append_fw_config_field(field);

	return field;
}

static void append_fw_config_option_to_field(struct fw_config_field *field,
					     struct fw_config_option *add)
{
	struct fw_config_option *option = field->options;

	if (!option) {
		field->options = add;
	} else {
		while (option && option->next)
			option = option->next;
		option->next = add;
	}
}

static uint64_t calc_max_field_value(const struct fw_config_field *field)
{
	unsigned int bit_count = 0;

	const struct fw_config_field_bits *bits = field->bits;
	while (bits) {
		bit_count += 1 + bits->end_bit - bits->start_bit;
		bits = bits->next;
	};

	return (1ull << bit_count) - 1ull;
}

void add_fw_config_option(struct fw_config_field *field, const char *name, uint64_t value)
{
	struct fw_config_option *option;

	/* Check that option value fits within field mask. */
	uint64_t field_max_value = calc_max_field_value(field);
	if (value > field_max_value) {
		printf("ERROR: fw_config option %s:%s value %" PRIx64 " larger than field max %"
		       PRIx64 "\n",
		       field->name, name, value, field_max_value);
		exit(1);
	}

	/* Check for existing option with this name or value. */
	option = field->options;
	while (option) {
		if (!strcmp(option->name, name)) {
			printf("ERROR: fw_config option name %s:%s already exists\n",
			       field->name, name);
			exit(1);
		}
		/* Compare values. */
		if (value == option->value) {
			printf("ERROR: fw_config option %s:%s[%" PRIx64 "] redefined as %s\n",
			       field->name, option->name, value, name);
			exit(1);
		}
		option = option->next;
	}

	option = S_ALLOC(sizeof(*option));
	option->name = name;
	option->value = value;

	/* Add option to the current field. */
	append_fw_config_option_to_field(field, option);
}

static void append_fw_config_probe_to_dev(struct device *dev, struct fw_config_probe *add)
{
	struct fw_config_probe *probe = dev->probe;

	if (!probe) {
		dev->probe = add;
	} else {
		while (probe && probe->next)
			probe = probe->next;
		probe->next = add;
	}
}

static int check_probe_exists(struct fw_config_probe *probe, const char *field,
			      const char *option)
{
	while (probe) {
		if (!strcmp(probe->field, field) && !strcmp(probe->option, option)) {
			return 1;
		}
		probe = probe->next;
	}

	return 0;
}

void add_fw_config_probe(struct bus *bus, const char *field, const char *option)
{
	struct fw_config_probe *probe;

	if (check_probe_exists(bus->dev->probe, field, option)) {
		printf("ERROR: fw_config probe %s:%s already exists\n", field, option);
		exit(1);
	}

	probe = S_ALLOC(sizeof(*probe));
	probe->field = field;
	probe->option = option;

	append_fw_config_probe_to_dev(bus->dev, probe);
}

static uint64_t compute_fw_config_mask(const struct fw_config_field_bits *bits)
{
	uint64_t mask = 0;

	while (bits) {
		/* Compute mask from start and end bit. */
		uint64_t tmp = ((1ull << (1ull + bits->end_bit - bits->start_bit)) - 1ull);
		tmp <<= bits->start_bit;
		mask |= tmp;
		bits = bits->next;
	}

	return mask;
}

static unsigned int bits_width(const struct fw_config_field_bits *bits)
{
	return 1 + bits->end_bit - bits->start_bit;
}

static uint64_t calc_option_value(const struct fw_config_field *field,
				  const struct fw_config_option *option)
{
	uint64_t value = 0;
	uint64_t original = option->value;

	struct fw_config_field_bits *bits = field->bits;
	while (bits) {
		const unsigned int width = bits_width(bits);
		const uint64_t orig_mask = (1ull << width) - 1ull;
		const uint64_t orig = (original & orig_mask);
		value |= (orig << bits->start_bit);

		original >>= width;
		bits = bits->next;
	}

	return value;
}

static void emit_fw_config(FILE *fil)
{
	struct fw_config_field *field = fw_config_fields;

	if (!field)
		return;

	while (field) {
		struct fw_config_option *option = field->options;
		uint64_t mask;

		fprintf(fil, "#define FW_CONFIG_FIELD_%s_NAME \"%s\"\n",
			field->name, field->name);

		mask = compute_fw_config_mask(field->bits);
		fprintf(fil, "#define FW_CONFIG_FIELD_%s_MASK 0x%" PRIx64 "\n",
			field->name, mask);

		while (option) {
			const uint64_t value = calc_option_value(field, option);
			fprintf(fil, "#define FW_CONFIG_FIELD_%s_OPTION_%s_NAME \"%s\"\n",
				field->name, option->name, option->name);
			fprintf(fil, "#define FW_CONFIG_FIELD_%s_OPTION_%s_VALUE 0x%"
				PRIx64 "\n", field->name, option->name, value);
			option = option->next;
		}

		field = field->next;
	}

	fprintf(fil, "\n");
}

static int emit_fw_config_probe(FILE *fil, struct device *dev)
{
	struct fw_config_probe *probe = dev->probe;

	fprintf(fil, "STORAGE struct fw_config %s_probe_list[] = {\n", dev->name);

	while (probe) {
		/* Find matching field. */
		struct fw_config_field *field;
		struct fw_config_option *option;
		uint64_t mask, value;

		field = find_fw_config_field(probe->field);
		if (!field) {
			printf("ERROR: fw_config_probe field %s not found\n", probe->field);
			return -1;
		}
		option = find_fw_config_option(field, probe->option);
		if (!option) {
			printf("ERROR: fw_config_probe field %s option %s not found\n",
			       probe->field, probe->option);
			return -1;
		}

		/* Fill out the probe structure with values from emit_fw_config(). */
		fprintf(fil, "\t{\n");
		fprintf(fil, "\t\t.field_name = FW_CONFIG_FIELD_%s_NAME,\n", probe->field);
		fprintf(fil, "\t\t.option_name = FW_CONFIG_FIELD_%s_OPTION_%s_NAME,\n",
			probe->field, probe->option);
		fprintf(fil, "\t\t.mask = FW_CONFIG_FIELD_%s_MASK,\n", probe->field);
		fprintf(fil, "\t\t.value = FW_CONFIG_FIELD_%s_OPTION_%s_VALUE,\n",
			probe->field, probe->option);
		fprintf(fil, "\t},\n");

		probe = probe->next;
	}

	/* Add empty entry to mark end of list. */
	fprintf(fil, "\t{ }\n};\n");
	return 0;
}

/* Enqueue identifier to list with head `*it`, if not already present. */
void add_identifier(struct identifier **it, const char *id)
{
	for (; *it != NULL; it = &(*it)->next) {
		if (!strcmp((*it)->id, id))
			return;
	}

	*it = S_ALLOC(sizeof(**it));
	(*it)->id = id;
}

void add_device_ops(struct bus *bus, char *ops_id)
{
	if (bus->dev->ops_id) {
		printf("ERROR: Device operations may only be specified once,\n"
		       "       found '%s', '%s'.\n", bus->dev->ops_id, ops_id);
		exit(1);
	}

	add_identifier(&device_operations, ops_id);
	bus->dev->ops_id = ops_id;
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

/*
 * Add given node as child of the provided parent. If this is the first child of
 * the parent, update parent->children pointer as well.
 */
static void set_new_child(struct bus *parent, struct device *child)
{
	struct device *c = parent->children;
	if (c) {
		while (c->sibling)
			c = c->sibling;
		c->sibling = child;
	} else
		parent->children = child;

	child->sibling = NULL;
	child->parent = parent;
}

static const struct device *find_alias(const struct device *const parent,
				       const char *const alias)
{
	if (parent->alias && !strcmp(parent->alias, alias))
		return parent;

	const struct bus *bus;
	for (bus = parent->bus; bus; bus = bus->next_bus) {
		const struct device *child;
		for (child = bus->children; child; child = child->sibling) {
			const struct device *const ret = find_alias(child, alias);
			if (ret)
				return ret;
		}
	}

	return NULL;
}

static struct device *new_device_with_path(struct bus *parent,
					   struct chip_instance *chip_instance,
					   const int bustype, int path_a, int path_b,
					   char *alias, int status)
{
	struct device *new_d;

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
	new_d->alias = alias;

	new_d->enabled = status & 0x01;
	new_d->hidden = (status >> 1) & 0x01;
	new_d->mandatory = (status >> 2) & 0x01;
	new_d->chip_instance = chip_instance;

	set_new_child(parent, new_d);

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

	case GPIO:
		new_d->path = ".type=DEVICE_PATH_GPIO,{.gpio={ .id = 0x%x }}";
		break;

	case MDIO:
		new_d->path = ".type=DEVICE_PATH_MDIO,{.mdio={ .addr = 0x%x }}";
		break;
	}

	return new_d;
}

struct device *new_device_reference(struct bus *parent,
				    struct chip_instance *chip_instance,
				    const char *reference, int status)
{
	const struct device *dev = find_alias(&base_root_dev, reference);

	if (!dev) {
		printf("ERROR: Unable to find device reference %s\n", reference);
		exit(1);
	}

	return new_device_with_path(parent, chip_instance, dev->bustype, dev->path_a,
				    dev->path_b, NULL, status);
}

struct device *new_device_raw(struct bus *parent,
			      struct chip_instance *chip_instance,
			      const int bustype, const char *devnum,
			      char *alias, int status)
{
	char *tmp;
	int path_a;
	int path_b = 0;

	/* Check for alias name conflicts. */
	if (alias && find_alias(root_parent->dev, alias)) {
		printf("ERROR: Alias already exists: %s\n", alias);
		exit(1);
	}

	path_a = strtol(devnum, &tmp, 16);
	if (*tmp == '.') {
		tmp++;
		path_b = strtol(tmp, NULL, 16);
	}

	return new_device_with_path(parent, chip_instance, bustype, path_a, path_b, alias,
				    status);
}

static void new_resource(struct device *dev, int type, int index, int base)
{
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

void add_resource(struct bus *bus, int type, int index, int base)
{
	new_resource(bus->dev, type, index, base);
}

static void add_reg(struct reg **const head, char *const name, char *const val)
{
	struct reg *const r = S_ALLOC(sizeof(struct reg));
	struct reg *prev = NULL;
	struct reg *cur;

	r->key = name;
	r->value = val;

	for (cur = *head; cur != NULL; prev = cur, cur = cur->next) {
		const int sort = strcmp(r->key, cur->key);
		if (sort == 0) {
			printf("ERROR: duplicate 'register' key '%s'.\n", r->key);
			exit(1);
		}
		if (sort < 0)
			break;
	}
	r->next = cur;
	if (prev)
		prev->next = r;
	else
		*head = r;
}

void add_register(struct chip_instance *chip_instance, char *name, char *val)
{
	add_reg(&chip_instance->reg, name, val);
}

void add_reference(struct chip_instance *const chip_instance,
		   char *const name, char *const alias)
{
	add_reg(&chip_instance->ref, name, alias);
}

static void set_reference(struct chip_instance *const chip_instance,
			  char *const name, char *const alias)
{
	const struct device *const dev = find_alias(&base_root_dev, alias);
	if (!dev) {
		printf("ERROR: Cannot find device alias '%s'.\n", alias);
		exit(1);
	}

	char *const ref_name = S_ALLOC(strlen(dev->name) + 2);
	sprintf(ref_name, "&%s", dev->name);
	add_register(chip_instance, name, ref_name);
}

static void update_references(FILE *file, FILE *head, struct device *dev,
			      struct device *next)
{
	struct reg *ref;

	for (ref = dev->chip_instance->ref; ref; ref = ref->next)
		set_reference(dev->chip_instance, ref->key, ref->value);
}

void add_slot_desc(struct bus *bus, char *type, char *length, char *designation,
		   char *data_width)
{
	struct device *dev = bus->dev;

	if (dev->bustype != PCI && dev->bustype != DOMAIN) {
		printf("ERROR: 'slot_type' only allowed for PCI devices\n");
		exit(1);
	}

	dev->smbios_slot_type = type;
	dev->smbios_slot_length = length;
	dev->smbios_slot_data_width = data_width;
	dev->smbios_slot_designation = designation;
}

void add_smbios_dev_info(struct bus *bus, long instance_id, const char *refdes)
{
	struct device *dev = bus->dev;

	if (dev->bustype != PCI && dev->bustype != DOMAIN) {
		printf("ERROR: 'dev_info' only allowed for PCI devices\n");
		exit(1);
	}

	if (instance_id < 0 || instance_id > UINT8_MAX) {
		printf("ERROR: SMBIOS dev info instance ID '%ld' out of range\n", instance_id);
		exit(1);
	}

	dev->smbios_instance_id_valid = 1;
	dev->smbios_instance_id = (unsigned int)instance_id;
	dev->smbios_refdes = refdes;
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

static void pass0(FILE *fil, FILE *head, struct device *ptr, struct device *next)
{
	static int dev_id;

	if (ptr == &base_root_dev) {
		fprintf(fil, "STORAGE struct bus %s_links[];\n",
			ptr->name);
		return;
	}

	char *name;

	if (ptr->alias) {
		name = S_ALLOC(6 + strlen(ptr->alias));
		sprintf(name, "_dev_%s", ptr->alias);
	} else {
		name = S_ALLOC(11);
		sprintf(name, "_dev_%d", dev_id++);
	}

	ptr->name = name;

	fprintf(fil, "STORAGE struct device %s;\n", ptr->name);
	if (ptr->res)
		fprintf(fil, "STORAGE struct resource %s_res[];\n",
			ptr->name);
	if (dev_has_children(ptr))
		fprintf(fil, "STORAGE struct bus %s_links[];\n",
			ptr->name);

	if (next)
		return;

	fprintf(fil,
		"DEVTREE_CONST struct device * DEVTREE_CONST last_dev = &%s;\n",
		ptr->name);
}

static void emit_smbios_data(FILE *fil, struct device *ptr)
{
	fprintf(fil, "#if !DEVTREE_EARLY\n");
	fprintf(fil, "#if CONFIG(GENERATE_SMBIOS_TABLES)\n");

	/* SMBIOS types start at 1, if zero it hasn't been set */
	if (ptr->smbios_slot_type)
		fprintf(fil, "\t.smbios_slot_type = %s,\n",
			ptr->smbios_slot_type);
	if (ptr->smbios_slot_data_width)
		fprintf(fil, "\t.smbios_slot_data_width = %s,\n",
			ptr->smbios_slot_data_width);
	if (ptr->smbios_slot_designation)
		fprintf(fil, "\t.smbios_slot_designation = \"%s\",\n",
			ptr->smbios_slot_designation);
	if (ptr->smbios_slot_length)
		fprintf(fil, "\t.smbios_slot_length = %s,\n",
			ptr->smbios_slot_length);

	/* Fill in SMBIOS type41 fields */
	if (ptr->smbios_instance_id_valid) {
		fprintf(fil, "\t.smbios_instance_id_valid = true,\n");
		fprintf(fil, "\t.smbios_instance_id = %u,\n", ptr->smbios_instance_id);
		if (ptr->smbios_refdes)
			fprintf(fil, "\t.smbios_refdes = \"%s\",\n", ptr->smbios_refdes);
	}

	fprintf(fil, "#endif\n");
	fprintf(fil, "#endif\n");
}

static void emit_resources(FILE *fil, struct device *ptr)
{
	if (ptr->res == NULL)
		return;

	int i = 1;
	fprintf(fil, "STORAGE struct resource %s_res[] = {\n", ptr->name);
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
	fprintf(fil, "STORAGE struct bus %s_links[] = {\n",
		ptr->name);

	struct bus *bus = ptr->bus;

	while (bus) {
		emit_bus(fil, bus);
		bus = bus->next_bus;
	}

	fprintf(fil, "\t};\n");
}

static struct chip_instance *get_chip_instance(const struct device *dev)
{
	struct chip_instance *chip_ins = dev->chip_instance;
	/*
	 * If the chip instance of device has base_chip_instance pointer set, then follow that
	 * to update the chip instance for current device.
	 */
	if (chip_ins->base_chip_instance)
		chip_ins = chip_ins->base_chip_instance;

	return chip_ins;
}

static void pass1(FILE *fil, FILE *head, struct device *ptr, struct device *next)
{
	int pin;
	struct chip_instance *chip_ins = get_chip_instance(ptr);
	int has_children = dev_has_children(ptr);

	/* Emit probe structures. */
	if (ptr->probe && (emit_fw_config_probe(fil, ptr) < 0)) {
		if (head)
			fclose(head);
		fclose(fil);
		exit(1);
	}

	if (ptr == &base_root_dev)
		fprintf(fil, "DEVTREE_CONST struct device %s = {\n", ptr->name);
	else
		fprintf(fil, "STORAGE struct device %s = {\n", ptr->name);

	fprintf(fil, "#if !DEVTREE_EARLY\n");

	/*
	 * ops field can be set in the devicetree. If unspecified, it is set
	 * to default_dev_ops_root only for the root device, other devices
	 * get it set by the driver at runtime.
	 */
	if (ptr->ops_id)
		fprintf(fil, "\t.ops = &%s,\n", ptr->ops_id);
	else if (ptr == &base_root_dev)
		fprintf(fil, "\t.ops = &default_dev_ops_root,\n");
	else
		fprintf(fil, "\t.ops = NULL,\n");
	fprintf(fil, "#endif\n");
	fprintf(fil, "\t.bus = &%s_links[%d],\n", ptr->parent->dev->name,
		ptr->parent->id);
	fprintf(fil, "\t.path = {");
	fprintf(fil, ptr->path, ptr->path_a, ptr->path_b);
	fprintf(fil, "},\n");
	fprintf(fil, "\t.enabled = %d,\n", ptr->enabled);
	fprintf(fil, "\t.hidden = %d,\n", ptr->hidden);
	fprintf(fil, "\t.mandatory = %d,\n", ptr->mandatory);
	fprintf(fil, "\t.on_mainboard = 1,\n");
	if (ptr->subsystem_vendor > 0)
		fprintf(fil, "\t.subsystem_vendor = 0x%04x,\n",
			ptr->subsystem_vendor);

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
	else
		fprintf(fil, "\t.sibling = NULL,\n");
	if (ptr->probe)
		fprintf(fil, "\t.probe_list = %s_probe_list,\n", ptr->name);
	fprintf(fil, "#if !DEVTREE_EARLY\n");
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
	fprintf(fil, "\t.chip_ops = &%s_ops,\n",
		chip_ins->chip->name_underscore);
	if (chip_ins == &mainboard_instance)
		fprintf(fil, "\t.name = mainboard_name,\n");
	fprintf(fil, "#endif\n");
	if (chip_ins->chip->chiph_exists)
		fprintf(fil, "\t.chip_info = &%s_info_%d,\n",
			chip_ins->chip->name_underscore, chip_ins->id);
	if (next)
		fprintf(fil, "\t.next=&%s,\n", next->name);

	emit_smbios_data(fil, ptr);

	fprintf(fil, "};\n");

	emit_resources(fil, ptr);

	if (has_children)
		emit_dev_links(fil, ptr);
}

static void expose_device_names(FILE *fil, FILE *head, struct device *ptr, struct device *next)
{
	struct chip_instance *chip_ins = get_chip_instance(ptr);

	/* Only devices on root bus here. */
	if (ptr->bustype == PCI && ptr->parent->dev->bustype == DOMAIN) {
		fprintf(head, "extern DEVTREE_CONST struct device *const __pci_0_%02x_%d;\n",
			ptr->path_a, ptr->path_b);
		fprintf(fil, "DEVTREE_CONST struct device *const __pci_0_%02x_%d = &%s;\n",
			ptr->path_a, ptr->path_b, ptr->name);

		if (chip_ins->chip->chiph_exists) {
			fprintf(head, "extern DEVTREE_CONST void *const __pci_0_%02x_%d_config;\n",
				ptr->path_a, ptr->path_b);
			fprintf(fil, "DEVTREE_CONST void *const __pci_0_%02x_%d_config = &%s_info_%d;\n",
				ptr->path_a, ptr->path_b, chip_ins->chip->name_underscore, chip_ins->id);
		}
	}

	if (ptr->bustype == PNP) {
		fprintf(head, "extern DEVTREE_CONST struct device *const __pnp_%04x_%02x;\n",
			ptr->path_a, ptr->path_b);
		fprintf(fil, "DEVTREE_CONST struct device *const __pnp_%04x_%02x = &%s;\n",
			ptr->path_a, ptr->path_b, ptr->name);
	}

	if (ptr->alias) {
		fprintf(head, "extern DEVTREE_CONST struct device *const %s_ptr;\n", ptr->name);
		fprintf(fil, "DEVTREE_CONST struct device *const %s_ptr = &%s;\n",
			ptr->name, ptr->name);
	}
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

static void walk_device_tree(FILE *fil, FILE *head, struct device *ptr,
			     void (*func)(FILE *, FILE *, struct device *,
					  struct device *))
{
	struct queue_entry *bfs_q_head = NULL;

	enqueue_tail(&bfs_q_head, ptr);

	while ((ptr = dequeue_head(&bfs_q_head))) {
		add_children_to_queue(&bfs_q_head, ptr);
		func(fil, head, ptr, peek_queue_head(bfs_q_head));
	}
}

static void emit_chip_headers(FILE *fil, struct chip *chip)
{
	struct chip *tmp = chip;

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
		/* A lot of cpus do not define chip_operations at all, and the ones
		   that do only initialise .name. */
		if (strstr(chip->name_underscore, "cpu_") == chip->name_underscore) {
			fprintf(fil,
				"__attribute__((weak)) struct chip_operations %s_ops = {};\n",
				chip->name_underscore);
		} else {
			fprintf(fil, "extern struct chip_operations %s_ops;\n",
				chip->name_underscore);
		}
		chip = chip->next;
	}
	fprintf(fil, "#endif\n");
}

static void emit_chip_instance(FILE *fil, struct chip_instance *instance)
{
	fprintf(fil, "STORAGE struct %s_config %s_info_%d = {",
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

static void emit_chip_configs(FILE *fil)
{
	struct chip *chip = chip_header.next;
	struct chip_instance *instance;
	int chip_id;

	for (; chip; chip = chip->next) {
		if (!chip->chiph_exists)
			continue;

		chip_id = 1;
		instance = chip->instance;
		while (instance) {
			/*
			 * Emit this chip instance only if there is no forwarding pointer to the
			 * base tree chip instance.
			 */
			if (instance->base_chip_instance == NULL) {
				instance->id = chip_id++;
				emit_chip_instance(fil, instance);
			}
			instance = instance->next;
		}
	}
}

static void emit_identifiers(FILE *fil, const char *decl, const struct identifier *it)
{
	for (; it != NULL; it = it->next)
		fprintf(fil, "extern %s %s;\n", decl, it->id);
}

static void inherit_subsystem_ids(FILE *file, FILE *head, struct device *dev,
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
	printf("usage: sconfig <options>\n");
	printf("  -c | --output_c          : Path to output static.c file (required)\n");
	printf("  -r | --output_h          : Path to header static.h file (required)\n");
	printf("  -d | --output_d          : Path to header static_devices.h file (required)\n");
	printf("  -f | --output_f          : Path to header static_fw_config.h file (required)\n");
	printf("  -m | --mainboard_devtree : Path to mainboard devicetree file (required)\n");
	printf("  -o | --override_devtree  : Path to override devicetree file (optional)\n");
	printf("  -p | --chipset_devtree   : Path to chipset/SOC devicetree file (optional)\n");

	exit(1);
}

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

static int device_probe_count(struct fw_config_probe *probe)
{
	int count = 0;
	while (probe) {
		probe = probe->next;
		count++;
	}

	return count;
}

/*
 * When overriding devices, use the following rules:
 * 1. If probe count matches and:
 *    a. Entire probe list matches for both devices -> Same device, override.
 *    b. No probe entries match -> Different devices, do not override.
 *    c. Partial list matches -> Bad device tree entries, fail build.
 *
 * 2. If probe counts do not match and:
 *    a. No probe entries match -> Different devices, do not override.
 *    b. Partial list matches -> Bad device tree entries, fail build.
 */
static int device_probes_match(struct device *a, struct device *b)
{
	struct fw_config_probe *a_probe = a->probe;
	struct fw_config_probe *b_probe = b->probe;
	int a_probe_count = device_probe_count(a_probe);
	int b_probe_count = device_probe_count(b_probe);
	int match_count = 0;

	while (a_probe) {
		if (check_probe_exists(b_probe, a_probe->field, a_probe->option))
			match_count++;
		a_probe = a_probe->next;
	}

	if ((a_probe_count == b_probe_count) && (a_probe_count == match_count))
		return 1;

	if (match_count) {
		printf("ERROR: devices with overlapping probes: ");
		printf(a->path, a->path_a, a->path_b);
		printf(b->path, b->path_a, b->path_b);
		printf("\n");
		exit(1);
	}

	return 0;
}

/*
 * Match device nodes from base and override tree to see if they are the same
 * node.
 */
static int device_match(struct device *a, struct device *b)
{
	return ((a->path_a == b->path_a) &&
		(a->path_b == b->path_b) &&
		(a->bustype == b->bustype) &&
		(a->chip_instance->chip ==
		 b->chip_instance->chip));
}

/*
 * Match resource nodes from base and override tree to see if they are the same
 * node.
 */
static int res_match(struct resource *a, struct resource *b)
{
	return ((a->type == b->type) &&
		(a->index == b->index));
}

/*
 * Add resource to device. If resource is already present, then update its base
 * and index. If not, then add a new resource to the device.
 */
static void update_resource(struct device *dev, struct resource *res)
{
	struct resource *base_res = dev->res;

	while (base_res) {
		if (res_match(base_res, res)) {
			base_res->base = res->base;
			return;
		}
		base_res = base_res->next;
	}

	new_resource(dev, res->type, res->index, res->base);
}

/*
 * Add register to chip instance. If register is already present, then update
 * its value. If not, then add a new register to the chip instance.
 */
static void update_register(struct reg **const head, struct reg *reg)
{
	struct reg *base_reg = *head;

	while (base_reg) {
		if (!strcmp(base_reg->key, reg->key)) {
			base_reg->value = reg->value;
			return;
		}
		base_reg = base_reg->next;
	}

	add_reg(head, reg->key, reg->value);
}

static void override_devicetree(struct bus *base_parent,
				struct bus *override_parent);

/*
 * Update the base device properties using the properties of override device. In
 * addition to that, call override_devicetree for all the buses under the
 * override device.
 *
 * Override Rules:
 * +--------------------+--------------------------------------------+
 * |                    |                                            |
 * |struct device member|                 Rule                       |
 * |                    |                                            |
 * +-----------------------------------------------------------------+
 * |                    |                                            |
 * | id                 | Unchanged. This is used to generate device |
 * |                    | structure name in static.c. So, no need to |
 * |                    | override.                                  |
 * |                    |                                            |
 * +-----------------------------------------------------------------+
 * |                    |                                            |
 * | enabled            | Copy enabled state from override device.   |
 * |                    | This allows variants to override device    |
 * |                    | state.                                     |
 * |                    |                                            |
 * +-----------------------------------------------------------------+
 * |                    |                                            |
 * | subsystem_vendor   | Copy from override device only if any one  |
 * | subsystem_device   | of the ids is non-zero.                    |
 * |                    |                                            |
 * +-----------------------------------------------------------------+
 * |                    |                                            |
 * | inherit_subsystem  | Copy from override device only if it is    |
 * |                    | non-zero. This allows variant to only      |
 * |                    | enable inherit flag for a device.          |
 * |                    |                                            |
 * +-----------------------------------------------------------------+
 * |                    |                                            |
 * | path               | Unchanged since these are same for both    |
 * | path_a             | base and override device (Used for         |
 * | path_b             | matching devices).                         |
 * |                    |                                            |
 * +-----------------------------------------------------------------+
 * |                    |                                            |
 * | bustype            | Unchanged since this is same for both base |
 * |                    | and override device (User for matching     |
 * |                    | devices).                                  |
 * |                    |                                            |
 * +-----------------------------------------------------------------+
 * |                    |                                            |
 * | pci_irq_info       | Unchanged.                                 |
 * |                    |                                            |
 * +-----------------------------------------------------------------+
 * |                    |                                            |
 * | parent             | Unchanged. This is meaningful only within  |
 * | sibling            | the parse tree, hence not being copied.    |
 * |                    |                                            |
 * +-----------------------------------------------------------------+
 * |                    |                                            |
 * | res                | Each resource that is present in override  |
 * |                    | device is copied over to base device:      |
 * |                    | 1. If resource of same type and index is   |
 * |                    |    present in base device, then base of    |
 * |                    |    the resource is copied.                 |
 * |                    | 2. If not, then a new resource is allocated|
 * |                    |    under the base device using type, index |
 * |                    |    and base from override res.             |
 * |                    |                                            |
 * +-----------------------------------------------------------------+
 * |                    |                                            |
 * | ref                | Each reference that is present in override |
 * |                    | device is copied over to base device with  |
 * |                    | the same rules as registers.               |
 * |                    |                                            |
 * +-----------------------------------------------------------------+
 * |                    |                                            |
 * | alias              | Base device alias is copied to override.   |
 * |                    | Override devices cannot change/remove an   |
 * |                    | existing alias, but they can add an alias  |
 * |                    | if one does not exist.                     |
 * |                    |                                            |
 * +-----------------------------------------------------------------+
 * |                    |                                            |
 * | smbios_slot info   | Copy SMBIOS slot information from override.|
 * |                    | This allows variants to override PCI(e)    |
 * |                    | slot information in SMBIOS tables.         |
 * |                    |                                            |
 * +-----------------------------------------------------------------+
 * |                    |                                            |
 * | chip_instance      | Each register of chip_instance is copied   |
 * |                    | over from override device to base device:  |
 * |                    | 1. If register with same key is present in |
 * |                    |    base device, then value of the register |
 * |                    |    is copied.                              |
 * |                    | 2. If not, then a new register is allocated|
 * |                    |    under the base chip_instance using key  |
 * |                    |    and value from override register.       |
 * |                    |                                            |
 * +-----------------------------------------------------------------+
 * |                    |                                            |
 * | bus                | Recursively call override_devicetree on    |
 * | last_bus           | each bus of override device. It is assumed |
 * |                    | that bus with id X under base device       |
 * |                    | to bus with id X under override device. If |
 * |                    | override device has more buses than base   |
 * |                    | device, then new buses are allocated under |
 * |                    | base device.                               |
 * |                    |                                            |
 * +-----------------------------------------------------------------+
 */
static void update_device(struct device *base_dev, struct device *override_dev)
{
	/*
	 * Copy the enabled state of override device to base device. This allows
	 * override tree to enable or disable a particular device.
	 */
	base_dev->enabled = override_dev->enabled;

	/*
	 * Copy the hidden state of override device to base device. This allows
	 * override tree to hide or unhide a particular device.
	 */
	base_dev->hidden = override_dev->hidden;

	/*
	 * Copy subsystem vendor and device ids from override device to base
	 * device only if the ids are non-zero in override device. Else, honor
	 * the values in base device.
	 */
	if (override_dev->subsystem_vendor ||
	    override_dev->subsystem_device) {
		base_dev->subsystem_vendor = override_dev->subsystem_vendor;
		base_dev->subsystem_device = override_dev->subsystem_device;
	}

	/*
	 * Copy value of inherity_subsystem from override device to base device
	 * only if it is non-zero in override device. This allows override
	 * tree to only enable inhert flag for a device.
	 */
	if (override_dev->inherit_subsystem)
		base_dev->inherit_subsystem = override_dev->inherit_subsystem;

	/*
	 * Copy resources of override device to base device.
	 * 1. If resource is already present in base device, then index and base
	 * of the resource will be copied over.
	 * 2. If resource is not already present in base device, a new resource
	 * will be allocated.
	 */
	struct resource *res = override_dev->res;
	while (res) {
		update_resource(base_dev, res);
		res = res->next;
	}

	/*
	 * Copy registers of override chip instance to base chip instance.
	 * 1. If register key is already present in base chip instance, then
	 * value for the register is copied over.
	 * 2. If register key is not already present in base chip instance, then
	 * a new register will be allocated.
	 */
	struct reg *reg = override_dev->chip_instance->reg;
	while (reg) {
		update_register(&base_dev->chip_instance->reg, reg);
		reg = reg->next;
	}

	/* Copy references just as with registers. */
	reg = override_dev->chip_instance->ref;
	while (reg) {
		update_register(&base_dev->chip_instance->ref, reg);
		reg = reg->next;
	}

	/* Check for alias name conflicts. */
	if (override_dev->alias && find_alias(&base_root_dev, override_dev->alias)) {
		printf("ERROR: alias already exists: %s\n", override_dev->alias);
		exit(1);
	}

	/*
	 * Copy alias from base device.
	 *
	 * Override devices cannot change/remove an existing alias,
	 * but they can add an alias to a device if one does not exist yet.
	 */
	if (base_dev->alias)
		override_dev->alias = base_dev->alias;
	else
		base_dev->alias = override_dev->alias;

	/*
	 * Use probe list from override device in place of base device, in order
	 * to allow an override to remove a probe from the base device.
	 */
	base_dev->probe = override_dev->probe;

	/* Copy SMBIOS slot information from base device */
	base_dev->smbios_slot_type = override_dev->smbios_slot_type;
	base_dev->smbios_slot_length = override_dev->smbios_slot_length;
	base_dev->smbios_slot_data_width = override_dev->smbios_slot_data_width;
	base_dev->smbios_slot_designation = override_dev->smbios_slot_designation;

	/*
	 * Update base_chip_instance member in chip instance of override tree to forward it to
	 * the chip instance in base tree.
	 */
	override_dev->chip_instance->base_chip_instance = get_chip_instance(base_dev);

	/* Allow to override the ops of a device */
	if (override_dev->ops_id)
		base_dev->ops_id = override_dev->ops_id;

	/*
	 * Now that the device properties are all copied over, look at each bus
	 * of the override device and run override_devicetree in a recursive
	 * manner. The assumption here is that first bus of override device
	 * corresponds to first bus of base device and so on. If base device has
	 * lesser buses than override tree, then new buses are allocated for it.
	 */
	struct bus *override_bus = override_dev->bus;
	struct bus *base_bus = base_dev->bus;

	while (override_bus) {

		/*
		 * If we have more buses in override tree device, then allocate
		 * a new bus for the base tree device as well.
		 */
		if (!base_bus) {
			alloc_bus(base_dev);
			base_bus = base_dev->last_bus;
		}

		override_devicetree(base_dev->bus, override_dev->bus);

		override_bus = override_bus->next_bus;
		base_bus = base_bus->next_bus;
	}
}

/*
 * Perform copy of device and properties from override parent to base parent.
 * This function walks through the override tree in a depth-first manner
 * performing following actions:
 * 1. If matching device is found in base tree, then copy the properties of
 * override device to base tree device. Call override_devicetree recursively on
 * the bus of override device.
 * 2. If matching device is not found in base tree, then set override tree
 * device as new child of base_parent and update the chip pointers in override
 * device subtree to ensure the nodes do not point to override tree chip
 * instance.
 */
static void override_devicetree(struct bus *base_parent,
				struct bus *override_parent)
{
	struct device *base_child;
	struct device *override_child = override_parent->children;
	struct device *next_child;

	while (override_child) {

		/* Look for a matching device in base tree. */
		for (base_child = base_parent->children;
		     base_child; base_child = base_child->sibling) {
			if (!device_match(base_child, override_child))
				continue;
			/* If base device has no probe statement, nothing else to compare. */
			if (base_child->probe == NULL)
				break;
			/*
			 * If base device has probe statements, ensure that all probe conditions
			 * match for base and override device.
			 */
			if (device_probes_match(base_child, override_child))
				break;
		}

		next_child = override_child->sibling;

		/*
		 * If matching device is found, copy properties of
		 * override_child to base_child.
		 */
		if (base_child)
			update_device(base_child, override_child);
		else {
			/*
			 * If matching device is not found, set override_child
			 * as a new child of base_parent.
			 */
			set_new_child(base_parent, override_child);
		}

		override_child = next_child;
	}
}

static void parse_override_devicetree(const char *file, struct device *dev)
{
	parse_devicetree(file, dev->bus);

	if (!dev_has_children(dev)) {
		fprintf(stderr, "ERROR: Override tree needs at least one device!\n");
		exit(1);
	}

	override_devicetree(&base_root_bus, dev->bus);
}

static void generate_outputh(FILE *f, const char *fw_conf_header, const char *device_header)
{
	fprintf(f, "#ifndef __STATIC_DEVICE_TREE_H\n");
	fprintf(f, "#define __STATIC_DEVICE_TREE_H\n\n");

	fprintf(f, "#include <%s>\n", fw_conf_header);
	fprintf(f, "#include <%s>\n\n", device_header);

	fprintf(f, "\n#endif /* __STATIC_DEVICE_TREE_H */\n");
}

static void generate_outputc(FILE *f, const char *static_header)
{
	fprintf(f, "#include <boot/coreboot_tables.h>\n");
	fprintf(f, "#include <device/device.h>\n");
	fprintf(f, "#include <device/pci.h>\n");
	fprintf(f, "#include <fw_config.h>\n");
	fprintf(f, "#include <%s>\n", static_header);
	emit_chip_headers(f, chip_header.next);
	emit_identifiers(f, "struct device_operations", device_operations);
	fprintf(f, "\n#define STORAGE static __maybe_unused DEVTREE_CONST\n\n");

	walk_device_tree(NULL, NULL, &base_root_dev, inherit_subsystem_ids);
	fprintf(f, "\n/* pass 0 */\n");
	walk_device_tree(f, NULL, &base_root_dev, pass0);
	walk_device_tree(NULL, NULL, &base_root_dev, update_references);
	fprintf(f, "\n/* chip configs */\n");
	emit_chip_configs(f);
	fprintf(f, "\n/* pass 1 */\n");
	walk_device_tree(f, NULL, &base_root_dev, pass1);
}

static void generate_outputd(FILE *gen, FILE *dev)
{
	fprintf(dev, "#ifndef __STATIC_DEVICES_H\n");
	fprintf(dev, "#define __STATIC_DEVICES_H\n\n");
	fprintf(dev, "#include <device/device.h>\n\n");
	fprintf(dev, "/* expose_device_names */\n");
	walk_device_tree(gen, dev, &base_root_dev, expose_device_names);
	fprintf(dev, "\n#endif /* __STATIC_DEVICE_NAMES_H */\n");
}

static void generate_outputf(FILE *f)
{
	fprintf(f, "#ifndef __STATIC_FW_CONFIG_H\n");
	fprintf(f, "#define __STATIC_FW_CONFIG_H\n\n");
	emit_fw_config(f);
	fprintf(f, "\n#endif /* __STATIC_FW_CONFIG_H */\n");
}

int main(int argc, char **argv)
{
	static const struct option long_options[] = {
		{ "mainboard_devtree", 1, NULL, 'm' },
		{ "override_devtree", 1, NULL, 'o' },
		{ "chipset_devtree", 1, NULL, 'p' },
		{ "output_c", 1, NULL, 'c' },
		{ "output_h", 1, NULL, 'r' },
		{ "output_d", 1, NULL, 'd' },
		{ "output_f", 1, NULL, 'f' },
		{ "help", 1, NULL, 'h' },
		{ }
	};
	const char *override_devtree = NULL;
	const char *base_devtree = NULL;
	const char *chipset_devtree = NULL;
	const char *outputc = NULL;
	const char *outputh = NULL;
	const char *outputd = NULL;
	const char *outputf = NULL;
	int opt, option_index;

	while ((opt = getopt_long(argc, argv, "m:o:p:c:r:d:f:h", long_options,
				  &option_index)) != EOF) {
		switch (opt) {
		case 'm':
			base_devtree = strdup(optarg);
			break;
		case 'o':
			override_devtree = strdup(optarg);
			break;
		case 'p':
			chipset_devtree = strdup(optarg);
			break;
		case 'c':
			outputc = strdup(optarg);
			break;
		case 'r':
			outputh = strdup(optarg);
			break;
		case 'd':
			outputd = strdup(optarg);
			break;
		case 'f':
			outputf = strdup(optarg);
			break;
		case 'h':
		default:
			usage();
		}
	}

	if (!base_devtree || !outputc || !outputh || !outputd || !outputf)
		usage();

	if (chipset_devtree) {
		/* Use the chipset devicetree as the base, then override
		   with the mainboard "base" devicetree. */
		parse_devicetree(chipset_devtree, &base_root_bus);
		parse_override_devicetree(base_devtree, &chipset_root_dev);
	} else {
		parse_devicetree(base_devtree, &base_root_bus);
	}

	if (override_devtree)
		parse_override_devicetree(override_devtree, &override_root_dev);


	FILE *autogen = fopen(outputc, "w");
	if (!autogen) {
		fprintf(stderr, "Could not open file '%s' for writing: ",
			outputc);
		perror(NULL);
		exit(1);
	}

	FILE *autohead = fopen(outputh, "w");
	if (!autohead) {
		fprintf(stderr, "Could not open file '%s' for writing: ", outputh);
		perror(NULL);
		fclose(autogen);
		exit(1);
	}

	FILE *autodev = fopen(outputd, "w");
	if (!autodev) {
		fprintf(stderr, "Could not open file '%s' for writing: ", outputd);
		perror(NULL);
		fclose(autogen);
		fclose(autohead);
		exit(1);
	}

	FILE *autofwconf = fopen(outputf, "w");
	if (!autofwconf) {
		fprintf(stderr, "Could not open file '%s' for writing: ", outputf);
		perror(NULL);
		fclose(autogen);
		fclose(autohead);
		fclose(autodev);
		exit(1);
	}

	char *f = strdup(outputf);
	assert(f);
	char *d = strdup(outputd);
	assert(d);
	char *h = strdup(outputh);
	assert(h);

	const char *fw_conf_header = basename(f);
	const char *device_header = basename(d);
	const char *static_header = basename(h);

	generate_outputh(autohead, fw_conf_header, device_header);
	generate_outputc(autogen, static_header);
	generate_outputd(autogen, autodev);
	generate_outputf(autofwconf);

	fclose(autohead);
	fclose(autogen);
	fclose(autodev);
	fclose(autofwconf);
	free(f);
	free(d);
	free(h);

	return 0;
}
