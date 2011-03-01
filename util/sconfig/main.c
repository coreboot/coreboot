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

#include "sconfig.h"
#include "sconfig.tab.h"

extern int linenum;

struct device *head, *lastdev;

struct header headers;

static int devcount = 0;

static struct device root;
static struct device mainboard = {
	.name = "mainboard",
	.name_underscore = "mainboard",
	.id = 0,
	.chip = &mainboard,
	.type = chip,
	.chiph_exists = 1,
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

struct device *new_chip(struct device *parent, struct device *bus, char *path) {
	struct device *new_chip = new_dev(parent, bus);
	new_chip->chiph_exists = 1;
	new_chip->name = path;
	new_chip->name_underscore = strdup(new_chip->name);
	char *c;
	for (c = new_chip->name_underscore; *c; c++) {
		if (*c == '/') *c = '_';
		if (*c == '-') *c = '_';
	}
	new_chip->type = chip;
	new_chip->chip = new_chip;

	struct stat st;
	char *chip_h = malloc(strlen(path)+12);
	sprintf(chip_h, "src/%s/chip.h", path);
	if ((stat(chip_h, &st) == -1) && (errno == ENOENT))
		new_chip->chiph_exists = 0;

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
	if (dev->chiph_exists) {
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
			h->next->name = dev->name;
			h->next->next = tmp;
		}
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
	if (bus == PCI) {
		new_d->path = ".type=DEVICE_PATH_PCI,{.pci={ .devfn = PCI_DEVFN(0x%x,%d)}}";
	}
	if (bus == PNP) {
		new_d->path = ".type=DEVICE_PATH_PNP,{.pnp={ .port = 0x%x, .device = 0x%x }}";
	}
	if (bus == I2C) {
		new_d->path = ".type=DEVICE_PATH_I2C,{.i2c={ .device = 0x%x }}";
	}
	if (bus == APIC) {
		new_d->path = ".type=DEVICE_PATH_APIC,{.apic={ .apic_id = 0x%x }}";
	}
	if (bus == APIC_CLUSTER) {
		new_d->path = ".type=DEVICE_PATH_APIC_CLUSTER,{.apic_cluster={ .cluster = 0x%x }}";
	}
	if (bus == PCI_DOMAIN) {
		new_d->path = ".type=DEVICE_PATH_PCI_DOMAIN,{.pci_domain={ .domain = 0x%x }}";
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
	if (dev->bustype != PCI && dev->bustype != PCI_DOMAIN) {
		printf("ERROR: 'subsystem' only allowed for PCI devices\n");
		exit(1);
	}

	dev->subsystem_vendor = vendor;
	dev->subsystem_device = device;
	dev->inherit_subsystem = inherit;
}

static void pass0(FILE *fil, struct device *ptr) {
	if (ptr->type == device && ptr->id == 0)
		fprintf(fil, "struct bus %s_links[];\n", ptr->name);
	if ((ptr->type == device) && (ptr->id != 0) && (!ptr->used)) {
		fprintf(fil, "static struct device %s;\n", ptr->name);
		if (ptr->rescnt > 0)
			fprintf(fil, "struct resource %s_res[];\n", ptr->name);
		if (ptr->children || ptr->multidev)
			fprintf(fil, "struct bus %s_links[];\n", ptr->name);
	}
}

static void pass1(FILE *fil, struct device *ptr) {
	if (!ptr->used && (ptr->type == device)) {
		if (ptr->id != 0)
			fprintf(fil, "static ", ptr->name);
		fprintf(fil, "struct device %s = {\n", ptr->name);
		fprintf(fil, "\t.ops = %s,\n", (ptr->ops)?(ptr->ops):"0");
		fprintf(fil, "\t.bus = &%s_links[%d],\n", ptr->bus->name, ptr->bus->link);
		fprintf(fil, "\t.path = {");
		fprintf(fil, ptr->path, ptr->path_a, ptr->path_b);
		fprintf(fil, "},\n");
		fprintf(fil, "\t.enabled = %d,\n", ptr->enabled);
		fprintf(fil, "\t.on_mainboard = 1,\n");
		if (ptr->subsystem_vendor > 0)
			fprintf(fil, "\t.subsystem_vendor = 0x%04x,\n", ptr->subsystem_vendor);

		if (ptr->subsystem_device > 0)
			fprintf(fil, "\t.subsystem_device = 0x%04x,\n", ptr->subsystem_device);

		if (ptr->rescnt > 0) {
			fprintf(fil, "\t.resource_list = &%s_res[0],\n", ptr->name);
		}
		int link = 0;
		if (ptr->children || ptr->multidev)
			fprintf(fil, "\t.link_list = &%s_links[0],\n", ptr->name);
		else
			fprintf(fil, "\t.link_list = NULL,\n", ptr->name);
		if (ptr->sibling)
			fprintf(fil, "\t.sibling = &%s,\n", ptr->sibling->name);
		if (ptr->chip->chiph_exists) {
			fprintf(fil, "\t.chip_ops = &%s_ops,\n", ptr->chip->name_underscore);
			fprintf(fil, "\t.chip_info = &%s_info_%d,\n", ptr->chip->name_underscore, ptr->chip->id);
		}
		if (ptr->nextdev)
			fprintf(fil, "\t.next=&%s\n", ptr->nextdev->name);
		fprintf(fil, "};\n");
	}
	if (ptr->rescnt > 0) {
		int i=1;
		fprintf(fil, "struct resource %s_res[] = {\n", ptr->name);
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
		fprintf(fil, "struct bus %s_links[] = {\n", ptr->name);
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
			fprintf(fil, "struct %s_config %s_info_%d\t= {\n", ptr->name_underscore, ptr->name_underscore, ptr->id);
			struct reg *r = ptr->reg;
			while (r) {
				fprintf(fil, "\t.%s = %s,\n", r->key, r->value);
				r = r->next;
			}
			fprintf(fil, "};\n\n");
		} else {
			fprintf(fil, "struct %s_config %s_info_%d;\n", ptr->name_underscore, ptr->name_underscore, ptr->id);
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
	int i =0;

	if (dev->subsystem_vendor != -1 && dev->subsystem_device != -1) {
		/* user already gave us a subsystem vendor/device */
		return;
	}

	for(p = dev; p && p != p->parent; (p = p->parent), i++) {

		if (p->bustype != PCI && p->bustype != PCI_DOMAIN)
			continue;

		if (p->inherit_subsystem) {
			dev->subsystem_vendor = p->subsystem_vendor;
			dev->subsystem_device = p->subsystem_device;
			break;
		}
	}
}

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("usage: sconfig vendor/mainboard outputdir\n");
		return 1;
	}
	char *mainboard=argv[1];
	char *outputdir=argv[2];
	char *devtree=malloc(strlen(mainboard)+30);
	char *outputc=malloc(strlen(outputdir)+10);
	sprintf(devtree, "src/mainboard/%s/devicetree.cb", mainboard);
	sprintf(outputc, "%s/static.c", outputdir);

	headers.next = malloc(sizeof(struct header));
	headers.next->name = malloc(strlen(mainboard)+12);
	headers.next->next = 0;
	sprintf(headers.next->name, "mainboard/%s", mainboard);

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

	FILE *staticc = fopen(outputc, "w");
	if (!staticc) {
		fprintf(stderr, "Could not open file '%s' for writing: ", outputc);
		perror(NULL);
		exit(1);
	}

	fprintf(staticc, "#include <device/device.h>\n");
	fprintf(staticc, "#include <device/pci.h>\n");
	struct header *h = &headers;
	while (h->next) {
		h = h->next;
		fprintf(staticc, "#include \"%s/chip.h\"\n", h->name);
	}

	walk_device_tree(staticc, &root, inherit_subsystem_ids, NULL);

	fprintf(staticc, "\n/* pass 0 */\n");
	walk_device_tree(staticc, &root, pass0, NULL);
	fprintf(staticc, "\n/* pass 1 */\nstruct mainboard_config mainboard_info_0;\nstruct device *last_dev = &%s;\n", lastdev->name);
	walk_device_tree(staticc, &root, pass1, NULL);

	fclose(staticc);

	return 0;
}
