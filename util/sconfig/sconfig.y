%{
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
} *head, *lastdev, *cur_parent, *cur_bus, root;

struct header;
struct header {
	char *name;
	struct header *next;
} headers;

int devcount = 0;

struct device *new_dev() {
	struct device *dev = malloc(sizeof(struct device));
	memset(dev, 0, sizeof(struct device));
	dev->id = ++devcount;
	dev->parent = cur_parent;
	dev->bus = cur_bus;
	head->next = dev;
	head = dev;
	return dev;
}

int device_match(struct device *a, struct device *b) {
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
	fprintf (stderr, "%s\n", str);
}
%}
%union {
	struct device *device;
	char *string;
	int number;
}
%token CHIP DEVICE REGISTER BOOL BUS RESOURCE END EQUALS HEX STRING PCI PNP I2C APIC APIC_CLUSTER PCI_DOMAIN IRQ DRQ IO NUMBER
%%
devtree: devchip {
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
	};

devchip: chip | device ;

devices: devices devchip | devices registers | ;

devicesorresources: devicesorresources devchip | devicesorresources resource | ;

chip: CHIP STRING /* == path */ {
	$<device>$ = new_dev();
	$<device>$->chiph_exists = 1;
	$<device>$->name = $<string>2;
	$<device>$->name_underscore = strdup($<device>$->name);
	char *c;
	for (c = $<device>$->name_underscore; *c; c++) {
		if (*c == '/') *c = '_';
		if (*c == '-') *c = '_';
	}
	$<device>$->type = chip;
	$<device>$->chip = $<device>$;

	struct stat st;
	char *chip_h = malloc(strlen($<string>2)+12);
	sprintf(chip_h, "src/%s/chip.h", $<string>2);
	if ((stat(chip_h, &st) == -1) && (errno == ENOENT))
		$<device>$->chiph_exists = 0;

	if (cur_parent->latestchild) {
		cur_parent->latestchild->next_sibling = $<device>$;
		cur_parent->latestchild->sibling = $<device>$;
	}
	cur_parent->latestchild = $<device>$;
	if (!cur_parent->children)
		cur_parent->children = $<device>$;

	cur_parent = $<device>$;
}
	devices END {
	cur_parent = $<device>3->parent;

	fold_in($<device>3);

	if ($<device>3->chiph_exists) {
		int include_exists = 0;
		struct header *h = &headers;
		while (h->next) {
			int result = strcmp($<device>3->name, h->next->name);
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
			h->next->name = $<device>3->name;
			h->next->next = tmp;
			break;
		}
	}
};

device: DEVICE BUS NUMBER /* == devnum */ BOOL {
	$<device>$ = new_dev();
	$<device>$->bustype = $<number>2;

	char *tmp;
	$<device>$->path_a = strtol(strdup($<string>3), &tmp, 16);
	if (*tmp == '.') {
		tmp++;
		$<device>$->path_b = strtol(tmp, NULL, 16);
	}

	char *name = malloc(10);
	sprintf(name, "_dev%d", $<device>$->id);
	$<device>$->name = name;
	$<device>$->name_underscore = name; // shouldn't be necessary, but avoid 0-ptr
	$<device>$->type = device;
	$<device>$->enabled = $<number>4;
	$<device>$->chip = $<device>$->parent->chip;

	if (cur_parent->latestchild) {
		cur_parent->latestchild->next_sibling = $<device>$;
		cur_parent->latestchild->sibling = $<device>$;
	}
	cur_parent->latestchild = $<device>$;
	if (!cur_parent->children)
		cur_parent->children = $<device>$;

	lastdev->nextdev = $<device>$;
	lastdev = $<device>$;
	if ($<number>2 == PCI) {
		$<device>$->path = ".type=DEVICE_PATH_PCI,{.pci={ .devfn = PCI_DEVFN(0x%x,%d)}}";
	}
	if ($<number>2 == PNP) {
		$<device>$->path = ".type=DEVICE_PATH_PNP,{.pnp={ .port = 0x%x, .device = 0x%x }}";
	}
	if ($<number>2 == I2C) {
		$<device>$->path = ".type=DEVICE_PATH_I2C,{.i2c={ .device = 0x%x }}";
	}
	if ($<number>2 == APIC) {
		$<device>$->path = ".type=DEVICE_PATH_APIC,{.apic={ .apic_id = 0x%x }}";
	}
	if ($<number>2 == APIC_CLUSTER) {
		$<device>$->path = ".type=DEVICE_PATH_APIC_CLUSTER,{.apic_cluster={ .cluster = 0x%x }}";
	}
	if ($<number>2 == PCI_DOMAIN) {
		$<device>$->path = ".type=DEVICE_PATH_PCI_DOMAIN,{.pci_domain={ .domain = 0x%x }}";
	}
	cur_parent = $<device>$;
	cur_bus = $<device>$;
}
	devicesorresources END {
	cur_parent = $<device>5->parent;
	cur_bus = $<device>5->bus;

	fold_in($<device>5);

	struct device *d = $<device>5->children;
	while (d) {
		int link = 0;
		struct device *cmp = d->next_sibling;
		while (cmp && (cmp->bus == d->bus) && (cmp->path_a == d->path_a) && (cmp->path_b == d->path_b)) {
			if (cmp->type==device && !cmp->used) {
				if (device_match(d, cmp)) {
					d->multidev = 1;

					cmp->aliased_name = malloc(12);
					sprintf(cmp->aliased_name, "_dev%d", cmp->id);
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
};

resource: RESOURCE NUMBER /* == resnum */ EQUALS NUMBER /* == resval */
	{
		struct resource *r = malloc(sizeof(struct resource));
		memset (r, 0, sizeof(struct resource));
		r->type = $<number>1;
		r->index = strtol($<string>2, NULL, 0);
		r->base = strtol($<string>4, NULL, 0);
		if (cur_parent->res) {
			struct resource *head = cur_parent->res;
			while (head->next) head = head->next;
			head->next = r;
		} else {
			cur_parent->res = r;
		}
		cur_parent->rescnt++;
	}
	;

registers: REGISTER STRING /* == regname */ EQUALS STRING /* == regval */
	{
		struct reg *r = malloc(sizeof(struct reg));
		memset (r, 0, sizeof(struct reg));
		r->key = $<string>2;
		r->value = $<string>4;
		if (cur_parent->reg) {
			struct reg *head = cur_parent->reg;
			// sorting to be equal to sconfig's behaviour
			int sort = strcmp(r->key, head->key);
			if (sort == 0) {
				printf("ERROR: duplicate 'register' key.\n");
				exit(1);
			}
			if (sort<0) {
				r->next = head;
				cur_parent->reg = r;
			} else {
				while ((head->next) && (strcmp(head->next->key, r->key)<0)) head = head->next;
				r->next = head->next;
				head->next = r;
			}
		} else {
			cur_parent->reg = r;
		}
	}
	;

%%
void pass0(FILE *fil, struct device *ptr) {
	if ((ptr->type == device) && (ptr->id != 0) && (!ptr->used))
		fprintf(fil, "struct device %s;\n", ptr->name);
	if ((ptr->type == device) && (ptr->id != 0) && ptr->used)
		fprintf(fil, "struct device %s;\n", ptr->aliased_name);
}

void pass1(FILE *fil, struct device *ptr) {
	if (!ptr->used && (ptr->type == device)) {
		fprintf(fil, "struct device %s = {\n", ptr->name);
		fprintf(fil, "\t.ops = %s,\n", (ptr->ops)?(ptr->ops):"0");
		fprintf(fil, "\t.bus = &%s.link[%d],\n", ptr->bus->name, ptr->bus->link);
		fprintf(fil, "\t.path = {");
		fprintf(fil, ptr->path, ptr->path_a, ptr->path_b);
		fprintf(fil, "},\n");
		fprintf(fil, "\t.enabled = %d,\n", ptr->enabled);
		fprintf(fil, "\t.on_mainboard = 1,\n");
		if (ptr->rescnt > 0) {
			fprintf(fil, "\t.resources = %d,\n", ptr->rescnt);
			fprintf(fil, "\t.resource = {\n");
			struct resource *r = ptr->res;
			while (r) {
				fprintf(fil, "\t\t{ .flags=IORESOURCE_FIXED | IORESOURCE_ASSIGNED | IORESOURCE_");
				if (r->type == IRQ) fprintf(fil, "IRQ");
				if (r->type == DRQ) fprintf(fil, "DRQ");
				if (r->type == IO) fprintf(fil, "IO");
				fprintf(fil, ", .index=0x%x, .base=0x%x},\n", r->index, r->base);
				r = r->next;
			}
			fprintf(fil, "\t },\n");
		}
		int link = 0;
		fprintf(fil, "\t.link = {\n");
		if (ptr->multidev) {
			struct device *d = ptr;
			while (d) {
				if (device_match(d, ptr)) {
					fprintf(fil, "\t\t[%d] = {\n", d->link);
					fprintf(fil, "\t\t\t.link = %d,\n", d->link);
					fprintf(fil, "\t\t\t.dev = &%s,\n", d->name);
					if (d->children)
						fprintf(fil, "\t\t\t.children = &%s,\n", d->children->name);
					fprintf(fil, "\t\t},\n");
					link++;
				}
				d = d->next_sibling;
			}
		} else {
			if (ptr->children) {
				fprintf(fil, "\t\t[0] = {\n");
				fprintf(fil, "\t\t\t.link = 0,\n");
				fprintf(fil, "\t\t\t.dev = &%s,\n", ptr->name);
				fprintf(fil, "\t\t\t.children = &%s,\n", ptr->children->name);
				fprintf(fil, "\t\t},\n");
				link++;
			}
		}
		fprintf(fil, "\t},\n");
		fprintf(fil, "\t.links = %d,\n", link);
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

void walk_device_tree(FILE *fil, struct device *ptr, void (*func)(FILE *, struct device*), struct device *chips) {
	do {
		func(fil, ptr);
		ptr = ptr->next_sibling;
	} while (ptr);
}

struct device mainboard = {
	.name = "mainboard",
	.name_underscore = "mainboard",
	.id = 0,
	.chip = &mainboard,
	.type = chip,
	.chiph_exists = 1,
	.children = &root
};

struct device root = {
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
	yyrestart(filec);

	FILE *staticc = fopen(outputc, "w");

	cur_bus = cur_parent = lastdev = head = &root;
	yyparse();
	fclose(filec);

	if ((head->type == chip) && (!head->chiph_exists)) {
		struct device *tmp = head;
		head = &root;
		while (head->next != tmp) head = head->next;
	}

	fprintf(staticc, "#include <device/device.h>\n");
	fprintf(staticc, "#include <device/pci.h>\n");
	struct header *h = &headers;
	while (h->next) {
		h = h->next;
		fprintf(staticc, "#include \"%s/chip.h\"\n", h->name);
	}
	fprintf(staticc, "\n/* pass 0 */\n");
	walk_device_tree(staticc, &root, pass0, NULL);
	fprintf(staticc, "\n/* pass 1 */\nstruct mainboard_config mainboard_info_0;\nstruct device **last_dev_p = &%s.next;\n", lastdev->name);
	walk_device_tree(staticc, &root, pass1, NULL);

	fclose(staticc);
}
