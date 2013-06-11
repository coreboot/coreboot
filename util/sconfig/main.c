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

#include <ctype.h>
#include "sconfig.h"
#include "sconfig.tab.h"

extern int linenum;

struct device *head, *lastdev;

struct header headers;

static int devcount = 0;

typedef enum {
	STATIC_MODE,
	BOOTBLOCK_MODE,
	KCONFIG_MODE
} scan_t;

static scan_t scan_mode = STATIC_MODE;

typedef enum {
	UNSLASH,
	SPLIT_1ST,
	TO_LOWER,
	TO_UPPER,
} translate_t;

static struct device root;
static struct device mainboard = {
	.name = "mainboard",
	.name_underscore = "mainboard",
	.id = 0,
	.chip = &mainboard,
	.type = chip,
#ifdef MAINBOARDS_HAVE_CHIP_H
	.chiph_exists = 1,
#else
	.chiph_exists = 0,
#endif
	.children = &root
};

static struct device root = {
	.name = "dev_root",
	.name_underscore = "dev_root",
	.id = 0,
	.chip = &mainboard,
	.type = device,
	.path = " .type = DEVICE_PATH_ROOT ",
	.ops = "&default_dev_ops_root",
	.parent = &root,
	.bus = &root,
	.enabled = 1
};

static struct device *new_dev(struct device *parent, struct device *bus) {
	struct device *dev = malloc(sizeof(struct device));
	memset(dev, 0, sizeof(struct device));
	dev->id = ++devcount;
	dev->parent = parent;
	dev->bus = bus;
	dev->subsystem_vendor = -1;
	dev->subsystem_device = -1;
	head->next = dev;
	head = dev;
	return dev;
}

static int device_match(struct device *a, struct device *b) {
	if ((a->bustype == b->bustype) && (a->bus == b->bus) && (a->path_a == b->path_a) && (a->path_b == b->path_b))
		return 1;
	return 0;
}

void fold_in(struct device *parent) {
	struct device *child = parent->children;
	struct device *latest = 0;
	while (child != latest) {
		if (child->children) {
			if (!latest) latest = child->children;
			parent->latestchild->next_sibling = child->children;
			parent->latestchild = child->latestchild;
		}
		child = child->next_sibling;
	}
}

int yywrap(void) {
	return 1;
}

void yyerror (char const *str)
{
	extern char *yytext;
	fprintf (stderr, "line %d: %s: %s\n", linenum + 1, yytext, str);
	exit(1);
}

void postprocess_devtree(void) {
	root.next_sibling = root.children;
	root.next_sibling->next_sibling = root.next_sibling->children;

	struct device *dev = &root;
	while (dev) {
		/* skip "chip" elements in children chain */
		while (dev->children && (dev->children->type == chip)) dev->children = dev->children->children;
		/* skip "chip" elements and functions of the same device in sibling chain */
		while (dev->sibling && dev->sibling->used) dev->sibling = dev->sibling->sibling;
		/* If end of chain, and parent is a chip, move on */
		if (!dev->sibling && (dev->parent->type == chip)) dev->sibling = dev->parent->sibling;
		/* skip chips */
		while (dev->sibling && dev->sibling->type == chip) dev->sibling = dev->sibling->children;
		/* skip duplicate function elements in nextdev chain */
		while (dev->nextdev && dev->nextdev->used) dev->nextdev = dev->nextdev->nextdev;
		dev = dev->next_sibling;
	}
}

char * translate_name(const char *str, translate_t mode)
{
	char *b, *c;
	b = c = strdup(str);
	while (c && *c) {
		if ((mode == SPLIT_1ST) && (*c == '/')) {
			*c = 0;
			break;
		}
		if (*c == '/') *c = '_';
		if (*c == '-') *c = '_';
		if (mode == TO_UPPER)
			*c = toupper(*c);
		if (mode == TO_LOWER)
			*c = tolower(*c);
		c++;
	}
	return b;
}

struct device *new_chip(struct device *parent, struct device *bus, char *path) {
	struct device *new_chip = new_dev(parent, bus);
	new_chip->chiph_exists = 1;
	new_chip->name = path;
	new_chip->name_underscore = translate_name(new_chip->name, UNSLASH);
	new_chip->type = chip;
	new_chip->chip = new_chip;

	struct stat st;
	char *chip_h = malloc(strlen(path)+18);
	sprintf(chip_h, "src/%s", path);
	if ((stat(chip_h, &st) == -1) && (errno == ENOENT)) {
		if (strstr(path, "/root_complex")) {
			fprintf(stderr, "WARNING: Use of deprecated chip component %s\n",
				path);
		} else {
			fprintf(stderr, "ERROR: Chip component %s does not exist.\n",
				path);
			exit(1);
		}
	}

	if (scan_mode == STATIC_MODE)
		sprintf(chip_h, "src/%s/chip.h", path);
	else if (scan_mode == BOOTBLOCK_MODE)
		sprintf(chip_h, "src/%s/bootblock.c", path);

	if ((scan_mode == STATIC_MODE) || (scan_mode == BOOTBLOCK_MODE)) {
		if ((stat(chip_h, &st) == -1) && (errno == ENOENT))
			new_chip->chiph_exists = 0;
	}

	if (parent->latestchild) {
		parent->latestchild->next_sibling = new_chip;
		parent->latestchild->sibling = new_chip;
	}
	parent->latestchild = new_chip;
	if (!parent->children)
		parent->children = new_chip;
	return new_chip;
}

void add_header(struct device *dev) {
	int include_exists = 0;
	struct header *h = &headers;
	while (h->next) {
		int result = strcmp(dev->name, h->next->name);
		if (result == 0) {
			include_exists = 1;
			break;
		}
		if (result < 0) break;
		h = h->next;
	}
	if (!include_exists) {
		struct header *tmp = h->next;
		h->next = malloc(sizeof(struct header));
		memset(h->next, 0, sizeof(struct header));
		h->next->chiph_exists = dev->chiph_exists;
		h->next->name = dev->name;
		h->next->next = tmp;
	}
}

struct device *new_device(struct device *parent, struct device *busdev, const int bus, const char *devnum, int enabled) {
	struct device *new_d = new_dev(parent, busdev);
	new_d->bustype = bus;

	char *tmp;
	new_d->path_a = strtol(strdup(devnum), &tmp, 16);
	if (*tmp == '.') {
		tmp++;
		new_d->path_b = strtol(tmp, NULL, 16);
	}

	char *name = malloc(10);
	sprintf(name, "_dev%d", new_d->id);
	new_d->name = name;
	new_d->name_underscore = name; // shouldn't be necessary, but avoid 0-ptr
	new_d->type = device;
	new_d->enabled = enabled;
	new_d->chip = new_d->parent->chip;

	if (parent->latestchild) {
		parent->latestchild->next_sibling = new_d;
		parent->latestchild->sibling = new_d;
	}
	parent->latestchild = new_d;
	if (!parent->children)
		parent->children = new_d;

	lastdev->nextdev = new_d;
	lastdev = new_d;

	switch(bus) {
	case PCI:
		new_d->path = ".type=DEVICE_PATH_PCI,{.pci={ .devfn = PCI_DEVFN(0x%x,%d)}}";
		break;

	case PNP:
		new_d->path = ".type=DEVICE_PATH_PNP,{.pnp={ .port = 0x%x, .device = 0x%x }}";
		break;

	case I2C:
		new_d->path = ".type=DEVICE_PATH_I2C,{.i2c={ .device = 0x%x }}";
		break;

	case APIC:
		new_d->path = ".type=DEVICE_PATH_APIC,{.apic={ .apic_id = 0x%x }}";
		break;

	case CPU_CLUSTER:
		new_d->path = ".type=DEVICE_PATH_CPU_CLUSTER,{.cpu_cluster={ .cluster = 0x%x }}";
		break;

	case DOMAIN:
		new_d->path = ".type=DEVICE_PATH_DOMAIN,{.domain={ .domain = 0x%x }}";
		break;

	case IOAPIC:
		new_d->path = ".type=DEVICE_PATH_IOAPIC,{.ioapic={ .ioapic_id = 0x%x }}";
		break;
	}
	return new_d;
}

void alias_siblings(struct device *d) {
	while (d) {
		int link = 0;
		struct device *cmp = d->next_sibling;
		while (cmp && (cmp->bus == d->bus) && (cmp->path_a == d->path_a) && (cmp->path_b == d->path_b)) {
			if (cmp->type==device && !cmp->used) {
				if (device_match(d, cmp)) {
					d->multidev = 1;

					cmp->id = d->id;
					cmp->name = d->name;
					cmp->used = 1;
					cmp->link = ++link;
				}
			}
			cmp = cmp->next_sibling;
		}
		d = d->next_sibling;
	}
}

void add_resource(struct device *dev, int type, int index, int base) {
	struct resource *r = malloc(sizeof(struct resource));
	memset (r, 0, sizeof(struct resource));
	r->type = type;
	r->index = index;
	r->base = base;
	if (dev->res) {
		struct resource *head = dev->res;
		while (head->next) head = head->next;
		head->next = r;
	} else {
		dev->res = r;
	}
	dev->rescnt++;
}

void add_register(struct device *dev, char *name, char *val) {
	struct reg *r = malloc(sizeof(struct reg));
	memset (r, 0, sizeof(struct reg));
	r->key = name;
	r->value = val;
	if (dev->reg) {
		struct reg *head = dev->reg;
		// sorting to be equal to sconfig's behaviour
		int sort = strcmp(r->key, head->key);
		if (sort == 0) {
			printf("ERROR: duplicate 'register' key.\n");
			exit(1);
		}
		if (sort<0) {
			r->next = head;
			dev->reg = r;
		} else {
			while ((head->next) && (strcmp(head->next->key, r->key)<0)) head = head->next;
			r->next = head->next;
			head->next = r;
		}
	} else {
		dev->reg = r;
	}
}

void add_pci_subsystem_ids(struct device *dev, int vendor, int device, int inherit)
{
	if (dev->bustype != PCI && dev->bustype != DOMAIN) {
		printf("ERROR: 'subsystem' only allowed for PCI devices\n");
		exit(1);
	}

	dev->subsystem_vendor = vendor;
	dev->subsystem_device = device;
	dev->inherit_subsystem = inherit;
}

void add_ioapic_info(struct device *dev, int apicid, const char *_srcpin, int irqpin)
{

	int srcpin;

	if (!_srcpin || strlen(_srcpin) < 4 ||strncasecmp(_srcpin, "INT", 3) ||
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

static void pass0(FILE *fil, struct device *ptr) {
	if (ptr->type == device && ptr->id == 0)
		fprintf(fil, "ROMSTAGE_CONST struct bus %s_links[];\n", ptr->name);

	if ((ptr->type == device) && (ptr->id != 0) && (!ptr->used)) {
		fprintf(fil, "ROMSTAGE_CONST static struct device %s;\n", ptr->name);
		if (ptr->rescnt > 0)
			fprintf(fil, "ROMSTAGE_CONST struct resource %s_res[];\n", ptr->name);
		if (ptr->children || ptr->multidev)
			fprintf(fil, "ROMSTAGE_CONST struct bus %s_links[];\n",
					ptr->name);
	}
}

static void pass1(FILE *fil, struct device *ptr)
{
	int pin;
	if (!ptr->used && (ptr->type == device)) {
		if (ptr->id != 0)
			fprintf(fil, "static ");
		fprintf(fil, "ROMSTAGE_CONST struct device %s = {\n", ptr->name);
		fprintf(fil, "#ifndef __PRE_RAM__\n");
		fprintf(fil, "\t.ops = %s,\n", (ptr->ops)?(ptr->ops):"0");
		fprintf(fil, "#endif\n");
		fprintf(fil, "\t.bus = &%s_links[%d],\n", ptr->bus->name, ptr->bus->link);
		fprintf(fil, "\t.path = {");
		fprintf(fil, ptr->path, ptr->path_a, ptr->path_b);
		fprintf(fil, "},\n");
		fprintf(fil, "\t.enabled = %d,\n", ptr->enabled);
		fprintf(fil, "\t.on_mainboard = 1,\n");
		if (ptr->subsystem_vendor > 0)
			fprintf(fil, "\t.subsystem_vendor = 0x%04x,\n", ptr->subsystem_vendor);

		for(pin = 0; pin < 4; pin++) {
			if (ptr->pci_irq_info[pin].ioapic_irq_pin > 0)
				fprintf(fil, "\t.pci_irq_info[%d].ioapic_irq_pin = %d,\n", pin, ptr->pci_irq_info[pin].ioapic_irq_pin);

			if (ptr->pci_irq_info[pin].ioapic_dst_id > 0)
				fprintf(fil, "\t.pci_irq_info[%d].ioapic_dst_id = %d,\n", pin, ptr->pci_irq_info[pin].ioapic_dst_id);
		}

		if (ptr->subsystem_device > 0)
			fprintf(fil, "\t.subsystem_device = 0x%04x,\n", ptr->subsystem_device);

		if (ptr->rescnt > 0) {
			fprintf(fil, "\t.resource_list = &%s_res[0],\n", ptr->name);
		}
		int link = 0;
		if (ptr->children || ptr->multidev)
			fprintf(fil, "\t.link_list = &%s_links[0],\n", ptr->name);
		else
			fprintf(fil, "\t.link_list = NULL,\n");
		if (ptr->sibling)
			fprintf(fil, "\t.sibling = &%s,\n", ptr->sibling->name);
		fprintf(fil, "#ifndef __PRE_RAM__\n");
		fprintf(fil, "\t.chip_ops = &%s_ops,\n", ptr->chip->name_underscore);
		if (ptr->chip->chip == &mainboard)
			fprintf(fil, "\t.name = mainboard_name,\n");
		fprintf(fil, "#endif\n");
		if (ptr->chip->chiph_exists)
			fprintf(fil, "\t.chip_info = &%s_info_%d,\n", ptr->chip->name_underscore, ptr->chip->id);
		if (ptr->nextdev)
			fprintf(fil, "\t.next=&%s\n", ptr->nextdev->name);
		fprintf(fil, "};\n");
	}
	if (ptr->rescnt > 0) {
		int i=1;
		fprintf(fil, "ROMSTAGE_CONST struct resource %s_res[] = {\n",
				ptr->name);
		struct resource *r = ptr->res;
		while (r) {
			fprintf(fil, "\t\t{ .flags=IORESOURCE_FIXED | IORESOURCE_ASSIGNED | IORESOURCE_");
			if (r->type == IRQ) fprintf(fil, "IRQ");
			if (r->type == DRQ) fprintf(fil, "DRQ");
			if (r->type == IO) fprintf(fil, "IO");
			fprintf(fil, ", .index=0x%x, .base=0x%x,", r->index, r->base);
			if (r->next)
				fprintf(fil, ".next=&%s_res[%d]},\n", ptr->name, i++);
			else
				fprintf(fil, ".next=NULL },\n");
			r = r->next;
		}
		fprintf(fil, "\t };\n");
	}
	if (!ptr->used && ptr->type == device && (ptr->children || ptr->multidev)) {
		fprintf(fil, "ROMSTAGE_CONST struct bus %s_links[] = {\n", ptr->name);
		if (ptr->multidev) {
			struct device *d = ptr;
			while (d) {
				if (device_match(d, ptr)) {
					fprintf(fil, "\t\t[%d] = {\n", d->link);
					fprintf(fil, "\t\t\t.link_num = %d,\n", d->link);
					fprintf(fil, "\t\t\t.dev = &%s,\n", d->name);
					if (d->children)
						fprintf(fil, "\t\t\t.children = &%s,\n", d->children->name);
					if (d->next_sibling && device_match(d->next_sibling, ptr))
						fprintf(fil, "\t\t\t.next=&%s_links[%d],\n", d->name, d->link+1);
					else
						fprintf(fil, "\t\t\t.next = NULL,\n");
					fprintf(fil, "\t\t},\n");
				}
				d = d->next_sibling;
			}
		} else {
			if (ptr->children) {
				fprintf(fil, "\t\t[0] = {\n");
				fprintf(fil, "\t\t\t.link_num = 0,\n");
				fprintf(fil, "\t\t\t.dev = &%s,\n", ptr->name);
				fprintf(fil, "\t\t\t.children = &%s,\n", ptr->children->name);
				fprintf(fil, "\t\t\t.next = NULL,\n");
				fprintf(fil, "\t\t},\n");
			}
		}
		fprintf(fil, "\t};\n");
	}
	if ((ptr->type == chip) && (ptr->chiph_exists)) {
		if (ptr->reg) {
			fprintf(fil, "ROMSTAGE_CONST struct %s_config ROMSTAGE_CONST %s_info_%d = {\n",
				ptr->name_underscore, ptr->name_underscore,
				ptr->id);
			struct reg *r = ptr->reg;
			while (r) {
				fprintf(fil, "\t.%s = %s,\n", r->key, r->value);
				r = r->next;
			}
			fprintf(fil, "};\n\n");
		} else {
			fprintf(fil, "ROMSTAGE_CONST struct %s_config ROMSTAGE_CONST %s_info_%d = { };\n",
				ptr->name_underscore, ptr->name_underscore, ptr->id);
		}
	}
}

static void walk_device_tree(FILE *fil, struct device *ptr, void (*func)(FILE *, struct device*), struct device *chips) {
	do {
		func(fil, ptr);
		ptr = ptr->next_sibling;
	} while (ptr);
}

static void inherit_subsystem_ids(FILE *file, struct device *dev)
{
	struct device *p;

	if (dev->subsystem_vendor != -1 && dev->subsystem_device != -1) {
		/* user already gave us a subsystem vendor/device */
		return;
	}

	for(p = dev; p && p != p->parent; p = p->parent) {

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
	printf("usage: sconfig vendor/mainboard outputdir [-{s|b|k} outputfile]\n");
	printf("\t-s file\tcreate ramstage static device map\n");
	printf("\t-b file\tcreate bootblock init_mainboard()\n");
	printf("\t-k file\tcreate Kconfig devicetree section\n");
	printf("Defaults to \"-s static.c\" if no {s|b|k} specified.\n");
	exit (1);
}


int main(int argc, char** argv) {
	if (argc < 3)
		usage();

	char *mainboard=argv[1];
	char *outputdir=argv[2];
	char *devtree=malloc(strlen(mainboard)+30);
	sprintf(devtree, "src/mainboard/%s/devicetree.cb", mainboard);
	char *outputc;

	if (argc == 3) {
		scan_mode = STATIC_MODE;
		outputc=malloc(strlen(outputdir)+20);
		sprintf(outputc, "%s/static.c", outputdir);
	} else if ((argc == 5) && (argv[3][0] == '-') && (argv[3][2] == 0)) {

		switch (argv[3][1]) {
		case 's':
			scan_mode = STATIC_MODE;
			break;
		case 'b':
			scan_mode = BOOTBLOCK_MODE;
			break;
		case 'k':
			scan_mode = KCONFIG_MODE;
			break;
		default:
			usage();
			break;
		}
		char *outputfile=argv[4];

		outputc=malloc(strlen(outputdir)+strlen(outputfile)+2);
		sprintf(outputc, "%s/%s", outputdir, outputfile);
	}

	headers.next = 0;
#ifdef MAINBOARDS_HAVE_CHIP_H
	if (scan_mode == STATIC_MODE) {
		headers.next = malloc(sizeof(struct header));
		headers.next->name = malloc(strlen(mainboard)+12);
		headers.next->next = 0;
		sprintf(headers.next->name, "mainboard/%s", mainboard);
	}
#endif

	FILE *filec = fopen(devtree, "r");
	if (!filec) {
		fprintf(stderr, "Could not open file '%s' for reading: ", devtree);
		perror(NULL);
		exit(1);
	}

	yyrestart(filec);

	lastdev = head = &root;

	yyparse();

	fclose(filec);

	if ((head->type == chip) && (!head->chiph_exists)) {
		struct device *tmp = head;
		head = &root;
		while (head->next != tmp) head = head->next;
	}

	FILE *autogen = fopen(outputc, "w");
	if (!autogen) {
		fprintf(stderr, "Could not open file '%s' for writing: ", outputc);
		perror(NULL);
		exit(1);
	}

	struct header *h;
	if (scan_mode == STATIC_MODE) {

		fprintf(autogen, "#include <device/device.h>\n");
		fprintf(autogen, "#include <device/pci.h>\n");
		h = &headers;
		while (h->next) {
			h = h->next;
			if (h->chiph_exists)
				fprintf(autogen, "#include \"%s/chip.h\"\n", h->name);
		}
		fprintf(autogen, "\n#ifndef __PRE_RAM__\n");
		fprintf(autogen, "__attribute__((weak)) struct chip_operations mainboard_ops = {};\n");
		h = &headers;
		while (h->next) {
			h = h->next;
			char *name_underscore = translate_name(h->name, UNSLASH);
			fprintf(autogen, "__attribute__((weak)) struct chip_operations %s_ops = {};\n", name_underscore);
			free(name_underscore);
		}
		fprintf(autogen, "#endif\n");

		walk_device_tree(autogen, &root, inherit_subsystem_ids, NULL);
		fprintf(autogen, "\n/* pass 0 */\n");
		walk_device_tree(autogen, &root, pass0, NULL);
		fprintf(autogen, "\n/* pass 1 */\n"
			    "ROMSTAGE_CONST struct device * ROMSTAGE_CONST last_dev = &%s;\n", lastdev->name);
#ifdef MAINBOARDS_HAVE_CHIP_H
		fprintf(autogen, "static ROMSTAGE_CONST struct mainboard_config ROMSTAGE_CONST mainboard_info_0;\n");
#endif
		walk_device_tree(autogen, &root, pass1, NULL);

	} else if (scan_mode == BOOTBLOCK_MODE) {
		h = &headers;
		while (h->next) {
			h = h->next;
			fprintf(autogen, "#include \"%s/bootblock.c\"\n", h->name);
		}

		fprintf(autogen, "\n#if CONFIG_HAS_MAINBOARD_BOOTBLOCK\n");
		fprintf(autogen, "#include \"mainboard/%s/bootblock.c\"\n", mainboard);
		fprintf(autogen, "#else\n");
		fprintf(autogen, "static unsigned long init_mainboard(int bsp_cpu)\n{\n");
		fprintf(autogen, "\tif (! bsp_cpu) return 0;\n");
		h = &headers;
		while (h->next) {
			h = h->next;
			char * buf = translate_name(h->name, UNSLASH);
			if (buf) {
				fprintf(autogen, "\tinit_%s();\n", buf);
				free(buf);
			}
		}

		fprintf(autogen, "\treturn 0;\n}\n");
		fprintf(autogen, "#endif\n");

	} else if (scan_mode == KCONFIG_MODE) {
		fprintf(autogen, "\nconfig MAINBOARD_DIR\n\tstring\n");
		fprintf(autogen, "\tdefault %s\n", mainboard);

		fprintf(autogen, "\nconfig MAINBOARD_DEVTREE\n\tdef_bool y\n");
		h = &headers;
		while (h->next) {
			h = h->next;
			char * buf = translate_name(h->name, TO_UPPER);
			if (buf) {
				fprintf(autogen, "\tselect %s\n", buf);
				free(buf);
			}
		}
	}

	fclose(autogen);

	return 0;
}
