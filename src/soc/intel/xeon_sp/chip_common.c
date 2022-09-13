/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <console/console.h>
#include <post.h>
#include <device/pci.h>
#include <soc/chip_common.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <stdlib.h>

struct pci_resource {
	struct device        *dev;
	struct resource      *res;
	struct pci_resource  *next;
};

struct stack_dev_resource {
	uint8_t                   align;
	struct pci_resource       *children;
	struct stack_dev_resource *next;
};

typedef enum {
	RES_TYPE_IO = 0,
	RES_TYPE_NONPREF_MEM,
	RES_TYPE_PREF_MEM,
	MAX_RES_TYPES
} RES_TYPE;

static RES_TYPE get_res_type(uint64_t flags)
{
	if (flags & IORESOURCE_IO)
		return RES_TYPE_IO;
	if (flags & IORESOURCE_MEM) {
		if (flags & IORESOURCE_PREFETCH) {
			printk(BIOS_DEBUG, "%s:%d flags: 0x%llx\n", __func__, __LINE__, flags);
			return RES_TYPE_PREF_MEM;
		}
		/* both 64-bit and 32-bit use below 4GB address space */
		return RES_TYPE_NONPREF_MEM;
	}
	printk(BIOS_ERR, "Invalid resource type 0x%llx\n", flags);
	die("");
}

static bool need_assignment(uint64_t flags)
{
	if (flags & (IORESOURCE_STORED | IORESOURCE_RESERVE | IORESOURCE_FIXED |
		IORESOURCE_ASSIGNED))
		return false;
	else
		return true;
}

static uint64_t get_resource_base(STACK_RES *stack, RES_TYPE res_type)
{
	if (res_type == RES_TYPE_IO) {
		assert(stack->PciResourceIoBase <= stack->PciResourceIoLimit);
		return stack->PciResourceIoBase;
	}
	if (res_type == RES_TYPE_NONPREF_MEM) {
		assert(stack->PciResourceMem32Base <= stack->PciResourceMem32Limit);
		return stack->PciResourceMem32Base;
	}
	assert(stack->PciResourceMem64Base <= stack->PciResourceMem64Limit);
	return stack->PciResourceMem64Base;
}

static void set_resource_base(STACK_RES *stack, RES_TYPE res_type, uint64_t base)
{
	if (res_type == RES_TYPE_IO) {
		assert(base <= (stack->PciResourceIoLimit + 1));
		stack->PciResourceIoBase = base;
	} else if (res_type == RES_TYPE_NONPREF_MEM) {
		assert(base <= (stack->PciResourceMem32Limit + 1));
		stack->PciResourceMem32Base = base;
	} else {
		assert(base <= (stack->PciResourceMem64Limit + 1));
		stack->PciResourceMem64Base = base;
	}
}

static void assign_stack_resources(struct iiostack_resource *stack_list,
	struct device *dev, struct resource *bridge);

void xeonsp_pci_domain_scan_bus(struct device *dev)
{
	DEV_FUNC_ENTER(dev);
	struct bus *link = dev->link_list;

	printk(BIOS_SPEW, "%s:%s scanning buses under device %s\n",
		__FILE__, __func__, dev_path(dev));
	while (link) {
		if (link->secondary == 0)  { // scan only PSTACK buses
			struct device *d;
			for (d = link->children; d; d = d->sibling)
				pci_probe_dev(d, link, d->path.pci.devfn);
			scan_bridges(link);
		} else {
			pci_scan_bus(link, PCI_DEVFN(0, 0), 0xff);
		}
		link = link->next;
	}
	DEV_FUNC_EXIT(dev);
}

static void xeonsp_pci_dev_iterator(struct bus *bus,
		void (*dev_iterator)(struct device *, void *),
		void (*res_iterator)(struct device *, struct resource *, void *),
		void *data)
{
	struct device *curdev;
	struct resource *res;

	/* Walk through all devices and find which resources they need. */
	for (curdev = bus->children; curdev; curdev = curdev->sibling) {
		struct bus *link;

		if (!curdev->enabled)
			continue;

		if (!curdev->ops || !curdev->ops->read_resources) {
			if (curdev->path.type != DEVICE_PATH_APIC)
				printk(BIOS_ERR, "%s missing read_resources\n",
					dev_path(curdev));
			continue;
		}

		if (dev_iterator)
			dev_iterator(curdev, data);

		if (res_iterator) {
			for (res = curdev->resource_list; res; res = res->next)
				res_iterator(curdev, res, data);
		}

		/* Read in the resources behind the current device's links. */
		for (link = curdev->link_list; link; link = link->next)
			xeonsp_pci_dev_iterator(link, dev_iterator, res_iterator, data);
	}
}

static void xeonsp_pci_dev_read_resources(struct device *dev, void *data)
{
	post_log_path(dev);
	dev->ops->read_resources(dev);
}

static void xeonsp_pci_dev_dummy_func(struct device *dev)
{
}

static void xeonsp_reset_pci_op(struct device *dev, void *data)
{
	if (dev->ops)
		dev->ops->read_resources = xeonsp_pci_dev_dummy_func;
}

static STACK_RES *find_stack_for_bus(struct iiostack_resource *info, uint8_t bus)
{
	for (int i = 0; i < info->no_of_stacks; ++i) {
		if (bus >= info->res[i].BusBase && bus <= info->res[i].BusLimit)
			return &info->res[i];
	}
	return NULL;
}

static void add_res_to_stack(struct stack_dev_resource **root,
	struct device *dev, struct resource *res)
{
	struct stack_dev_resource *cur = *root;
	while (cur) {
		if (cur->align == res->align || !cur->next) /* equal or last record */
			break;
		else if (cur->align > res->align) {
			if (cur->next->align < res->align) /* need to insert new record here */
				break;
			cur = cur->next;
		} else {
			break;
		}
	}

	struct stack_dev_resource *nr;
	if (!cur || cur->align != res->align) { /* need to add new record */
		nr = malloc(sizeof(struct stack_dev_resource));
		if (nr == 0)
			die("assign_resource_to_stack(): out of memory.\n");
		memset(nr, 0, sizeof(struct stack_dev_resource));
		nr->align = res->align;
		if (!cur) {
			*root = nr; /* head node */
		} else if (cur->align > nr->align) {
			if (!cur->next) {
				cur->next = nr;
			} else {
				nr->next = cur->next;
				cur->next = nr;
			}
		} else { /* insert in the beginning */
			nr->next = cur;
			*root = nr;
		}
	} else {
		nr = cur;
	}

	assert(nr && nr->align == res->align);

	struct pci_resource *npr = malloc(sizeof(struct pci_resource));
	if (!npr)
		die("%s: out of memory.\n", __func__);
	npr->res = res;
	npr->dev = dev;
	npr->next = NULL;

	if (!nr->children) {
		nr->children = npr;
	} else {
		struct pci_resource *pr = nr->children;
		while (pr->next)
			pr = pr->next;
		pr->next = npr;
	}
}

static void reserve_dev_resources(STACK_RES *stack, RES_TYPE res_type,
	struct stack_dev_resource *res_root, struct resource *bridge)
{
	uint64_t orig_base, base;

	orig_base = get_resource_base(stack, res_type);

	base = orig_base;
	int first = 1;
	while (res_root) { /* loop through all devices grouped by alignment requirements */
		struct pci_resource *pr = res_root->children;
		while (pr) {
			if (first) {
				if (bridge) { /* takes highest alignment */
					if (bridge->align < pr->res->align)
						bridge->align = pr->res->align;
					orig_base = ALIGN_UP(orig_base, 1 << bridge->align);
				} else {
					orig_base = ALIGN_UP(orig_base, 1 << pr->res->align);
				}
				base = orig_base;

				if (bridge)
					bridge->base = base;
				pr->res->base = base;
				first = 0;
			} else {
				pr->res->base = ALIGN_UP(base, 1 << pr->res->align);
			}
			pr->res->limit = pr->res->base + pr->res->size - 1;
			base = pr->res->limit + 1;
			pr->res->flags |= (IORESOURCE_ASSIGNED);
			pr = pr->next;
		}
		res_root = res_root->next;
	}

	if (bridge) {
		/* this bridge doesn't have any resources, will set it to default window */
		if (first) {
			orig_base = ALIGN_UP(orig_base, 1 << bridge->align);
			bridge->base = orig_base;
			base = orig_base + (1ULL << bridge->gran);
		}

		bridge->size = ALIGN_UP(base, 1 << bridge->align) - bridge->base;

		bridge->limit = bridge->base + bridge->size - 1;
		bridge->flags |= (IORESOURCE_ASSIGNED);
		base = bridge->limit + 1;
	}

	set_resource_base(stack, res_type, base);
}

static void reclaim_resource_mem(struct stack_dev_resource *res_root)
{
	while (res_root) { /* loop through all devices grouped by alignment requirements */
		/* free pci_resource */
		struct pci_resource *pr = res_root->children;
		while (pr) {
			struct pci_resource *dpr = pr;
			pr = pr->next;
			free(dpr);
		}

		/* free stack_dev_resource */
		struct stack_dev_resource *ddr = res_root;
		res_root = res_root->next;
		free(ddr);
	}
}

static void assign_bridge_resources(struct iiostack_resource *stack_list,
	struct device *dev, struct resource *bridge)
{
	struct resource *res;
	if (!dev->enabled)
		return;

	for (res = dev->resource_list; res; res = res->next) {
		if (!(res->flags & IORESOURCE_BRIDGE) ||
		    (bridge && (get_res_type(bridge->flags) != get_res_type(res->flags))))
			continue;

		assign_stack_resources(stack_list, dev, res);

		if (!bridge)
			continue;

		/* for 1st time update, overlading IORESOURCE_ASSIGNED */
		if (!(bridge->flags & IORESOURCE_ASSIGNED)) {
			bridge->base = res->base;
			bridge->limit = res->limit;
			bridge->flags |= (IORESOURCE_ASSIGNED);
		} else {
			/* update bridge range from child bridge range */
			if (res->base < bridge->base)
				bridge->base = res->base;
			if (res->limit > bridge->limit)
				bridge->limit = res->limit;
		}
		bridge->size = (bridge->limit - bridge->base + 1);
	}
}

static void assign_stack_resources(struct iiostack_resource *stack_list,
	struct device *dev, struct resource *bridge)
{
	struct bus *bus;

	/* Read in the resources behind the current device's links. */
	for (bus = dev->link_list; bus; bus = bus->next) {
		struct device *curdev;
		STACK_RES *stack;

		/* get IIO stack for this bus */
		stack = find_stack_for_bus(stack_list, bus->secondary);
		assert(stack);

		/* Assign resources to bridge */
		for (curdev = bus->children; curdev; curdev = curdev->sibling)
			assign_bridge_resources(stack_list, curdev, bridge);

		/* Pick non-bridged resources for resource allocation for each resource type */
		RES_TYPE res_types[MAX_RES_TYPES] = {
			RES_TYPE_IO,
			RES_TYPE_NONPREF_MEM,
			RES_TYPE_PREF_MEM
		};

		uint8_t no_res_types = MAX_RES_TYPES;

		/* if it is a bridge, only process matching bridge resource type */
		if (bridge) {
			res_types[0] = get_res_type(bridge->flags);
			no_res_types = 1;
		}

		printk(BIOS_DEBUG, "%s:%d no_res_types: %d\n", __func__, __LINE__,
			no_res_types);

		/* Process each resource type */
		for (int rt = 0; rt < no_res_types; ++rt) {
			struct stack_dev_resource *res_root = NULL;
			printk(BIOS_DEBUG, "%s:%d rt: %d\n", __func__, __LINE__, rt);
			for (curdev = bus->children; curdev; curdev = curdev->sibling) {
				struct resource *res;
				printk(BIOS_DEBUG, "%s:%d dev: %s\n",
					__func__, __LINE__, dev_path(curdev));
				if (!curdev->enabled)
					continue;

				for (res = curdev->resource_list; res; res = res->next) {
					printk(BIOS_DEBUG, "%s:%d dev: %s, flags: 0x%lx\n",
						__func__, __LINE__,
						dev_path(curdev), res->flags);
					if (res->size == 0                            ||
					    get_res_type(res->flags) != res_types[rt] ||
					    (res->flags & IORESOURCE_BRIDGE)          ||
					    !need_assignment(res->flags))
						continue;
					else
						add_res_to_stack(&res_root, curdev, res);
				}
			}

			/* Allocate resources and update bridge range */
			if (res_root || (bridge && !(bridge->flags & IORESOURCE_ASSIGNED))) {
				reserve_dev_resources(stack, res_types[rt], res_root, bridge);
				reclaim_resource_mem(res_root);
			}
		}
	}
}

static uint8_t is_pci64bit_alloc(void)
{
	const IIO_UDS *hob = get_iio_uds();

	return hob->PlatformData.Pci64BitResourceAllocation;
}

static void xeonsp_pci_domain_read_resources(struct device *dev)
{
	struct bus *link;

	DEV_FUNC_ENTER(dev);

	pci_domain_read_resources(dev);

	/*
	 * Walk through all devices in this domain and read resources.
	 * Since there is no callback when read resource operation is
	 * complete for all devices, domain read resource function initiates
	 * read resources for all devices and swaps read resource operation
	 * with dummy function to avoid warning.
	 */
	for (link = dev->link_list; link; link = link->next)
		xeonsp_pci_dev_iterator(link, xeonsp_pci_dev_read_resources, NULL, NULL);

	for (link = dev->link_list; link; link = link->next)
		xeonsp_pci_dev_iterator(link, xeonsp_reset_pci_op, NULL, NULL);

	struct iiostack_resource stack_info = {0};
	get_iiostack_info(&stack_info);
	if (!is_pci64bit_alloc()) {
		/*
		 * Split 32 bit address space between prefetchable and
		 * non-prefetchable windows
		 */
		for (int s = 0; s < stack_info.no_of_stacks; ++s) {
			STACK_RES *res = &stack_info.res[s];
			uint64_t length = (res->PciResourceMem32Limit -
				res->PciResourceMem32Base + 1)/2;
			res->PciResourceMem64Limit = res->PciResourceMem32Limit;
			res->PciResourceMem32Limit = (res->PciResourceMem32Base + length - 1);
			res->PciResourceMem64Base = res->PciResourceMem32Limit + 1;
		}
	}

	/* assign resources */
	assign_stack_resources(&stack_info, dev, NULL);

	DEV_FUNC_EXIT(dev);
}

static void reset_resource_to_unassigned(struct device *dev, struct resource *res, void *data)
{
	if ((res->flags & (IORESOURCE_IO | IORESOURCE_MEM)) &&
		!(res->flags & (IORESOURCE_FIXED | IORESOURCE_RESERVE))) {
		res->flags &= ~IORESOURCE_ASSIGNED;
	}
}

void xeonsp_pci_domain_set_resources(struct device *dev)
{
	DEV_FUNC_ENTER(dev);

	print_resource_tree(dev, BIOS_SPEW, "Before xeonsp pci domain set resource");

	/* reset bus 0 dev resource assignment - need to change them to FSP IIOStack window */
	xeonsp_pci_dev_iterator(dev->link_list, NULL, reset_resource_to_unassigned, NULL);

	/* update dev resources based on IIOStack IO/Mem32/Mem64 windows */
	xeonsp_pci_domain_read_resources(dev);

	struct bus *link = dev->link_list;
	while (link) {
		assign_resources(link);
		link = link->next;
	}

	print_resource_tree(dev, BIOS_SPEW, "After xeonsp pci domain set resource");

	DEV_FUNC_EXIT(dev);
}

/* Attach IIO stack bus numbers with dummy device to PCI DOMAIN 0000 device */
void attach_iio_stacks(struct device *dev)
{
	struct bus *iiostack_bus;
	struct device dummy;
	struct iiostack_resource stack_info = {0};

	DEV_FUNC_ENTER(dev);

	get_iiostack_info(&stack_info);
	for (int s = 0; s < stack_info.no_of_stacks; ++s) {
		/* only non zero bus no. needs to be enumerated */
		if (stack_info.res[s].BusBase == 0)
			continue;

		iiostack_bus = malloc(sizeof(struct bus));
		if (!iiostack_bus)
			die("%s: out of memory.\n", __func__);
		memset(iiostack_bus, 0, sizeof(*iiostack_bus));
		memcpy(iiostack_bus, dev->bus, sizeof(*iiostack_bus));
		iiostack_bus->secondary = stack_info.res[s].BusBase;
		iiostack_bus->subordinate = stack_info.res[s].BusBase;
		iiostack_bus->dev = NULL;
		iiostack_bus->children = NULL;
		iiostack_bus->next = NULL;
		iiostack_bus->link_num = 1;

		dummy.bus = iiostack_bus;
		dummy.path.type = DEVICE_PATH_PCI;
		dummy.path.pci.devfn = 0;
		uint32_t id = pci_read_config32(&dummy, PCI_VENDOR_ID);
		if (id == 0xffffffff)
			printk(BIOS_WARNING, "IIO Stack device %s not visible\n",
				dev_path(&dummy));

		if (!dev->link_list) {
			dev->link_list = iiostack_bus;
		} else {
			struct bus *nlink = dev->link_list;
			while (nlink->next)
				nlink = nlink->next;
			nlink->next = iiostack_bus;
		}
	}

	DEV_FUNC_EXIT(dev);
}
