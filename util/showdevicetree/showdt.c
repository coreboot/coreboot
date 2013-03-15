/*
 * Compile and dump the device tree
 *
 * Copyright 2013 Google Inc.
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
#include  <stdio.h>
/* you can't include string.h due to conflicts with coreboot prototypes. */
void *memcpy(void *m1, void *m2, size_t s);

struct device_operations default_dev_ops_root = {
	.read_resources   = NULL,
	.set_resources    = NULL,
	.enable_resources = NULL,
	.init             = NULL,
	.scan_bus         = NULL,
	.reset_bus        = NULL,
};

extern struct device dev_root;
struct device *all_devices = &dev_root;

const char mainboard_name[] = "showdt";

/*
 * Warning: This function uses a static buffer. Don't call it more than once
 * from the same print statement!
 */
const char *dev_path(device_t dev)
{
	static char buffer[DEVICE_PATH_MAX];

	buffer[0] = '\0';
	if (!dev) {
		memcpy(buffer, "<null>", 7);
	} else {
		switch(dev->path.type) {
		case DEVICE_PATH_ROOT:
			memcpy(buffer, "Root Device", 12);
			break;
		case DEVICE_PATH_PCI:
#if CONFIG_PCI_BUS_SEGN_BITS
			sprintf(buffer, "PCI: %04x:%02x:%02x.%01x",
				dev->bus->secondary >> 8,
				dev->bus->secondary & 0xff,
				PCI_SLOT(dev->path.pci.devfn),
				PCI_FUNC(dev->path.pci.devfn));
#else
			sprintf(buffer, "PCI: %02x:%02x.%01x",
				dev->bus->secondary,
				PCI_SLOT(dev->path.pci.devfn),
				PCI_FUNC(dev->path.pci.devfn));
#endif
			break;
		case DEVICE_PATH_PNP:
			sprintf(buffer, "PNP: %04x.%01x",
				dev->path.pnp.port, dev->path.pnp.device);
			break;
		case DEVICE_PATH_I2C:
			sprintf(buffer, "I2C: %02x:%02x",
				dev->bus->secondary,
				dev->path.i2c.device);
			break;
		case DEVICE_PATH_APIC:
			sprintf(buffer, "APIC: %02x",
				dev->path.apic.apic_id);
			break;
		case DEVICE_PATH_IOAPIC:
			sprintf(buffer, "IOAPIC: %02x",
				dev->path.ioapic.ioapic_id);
			break;
		case DEVICE_PATH_DOMAIN:
			sprintf(buffer, "DOMAIN: %04x",
				dev->path.domain.domain);
			break;
		case DEVICE_PATH_CPU_CLUSTER:
			sprintf(buffer, "CPU_CLUSTER: %01x",
				dev->path.cpu_cluster.cluster);
			break;
		case DEVICE_PATH_CPU:
			sprintf(buffer, "CPU: %02x", dev->path.cpu.id);
			break;
		case DEVICE_PATH_CPU_BUS:
			sprintf(buffer, "CPU_BUS: %02x", dev->path.cpu_bus.id);
			break;
		default:
			printf("Unknown device path type: %d\n",
			       dev->path.type);
			break;
		}
	}
	return buffer;
}


void show_devs_tree(struct device *dev, int debug_level, int depth, int linknum)
{
	char depth_str[20] = "";
	int i;
	struct device *sibling;
	struct bus *link;

	for (i = 0; i < depth; i++)
		depth_str[i] = ' ';
	depth_str[i] = '\0';

	printf("%s%s: enabled %d%s\n",
		depth_str, dev_path(dev), dev->enabled,
		dev->chip_ops ? ":has a chip":"");

	for (link = dev->link_list; link; link = link->next) {
		for (sibling = link->children; sibling;
		     sibling = sibling->sibling)
			show_devs_tree(sibling, debug_level, depth + 1, i);
	}
}

void show_all_devs_tree(int debug_level, const char *msg)
{
	printf("Show all devs in tree form...%s\n", msg);

	show_devs_tree(all_devices, debug_level, 0, -1);
}

void show_devs_subtree(struct device *root, int debug_level, const char *msg)
{
	printf("Show all devs in subtree %s...%s\n",
	       dev_path(root), msg);

	printf("%s\n", msg);
	show_devs_tree(root, debug_level, 0, -1);
}

void show_all_devs(int debug_level, const char *msg)
{
	struct device *dev;

	printf("Show all devs...%s\n", msg);
	for (dev = all_devices; dev; dev = dev->next) {
		printf("%s: enabled %d%s\n",
			dev_path(dev), dev->enabled,
			dev->chip_ops ? ":has a chip":"");
	}
}

main()
{
	show_all_devs(1, "");
	show_all_devs_tree(1, "");
}

/*
 * Example: (yank this and paste into M-x compile in emacs)
 * or tail -2 showdt.c | head -1 |sh
 * or whatever.
   cc -I ../src -I ../src/include -I ../src/arch/armv7/include/ -include build/mainboard/google/snow/static.c showdt.c
*/
