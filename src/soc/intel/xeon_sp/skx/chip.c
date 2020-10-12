/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cbfs.h>
#include <assert.h>
#include <console/console.h>
#include <console/debug.h>
#include <post.h>
#include <device/pci.h>
#include <soc/acpi.h>
#include <soc/ramstage.h>
#include <soc/soc_util.h>
#include <soc/util.h>

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

static void assign_stack_resources(struct iiostack_resource *stack_list,
	struct device *dev, struct resource *bridge);

static void xeonsp_pci_domain_scan_bus(struct device *dev)
{
	DEV_FUNC_ENTER(dev);
	struct bus *link = dev->link_list;

	printk(BIOS_SPEW, "%s:%s scanning buses under device %s\n",
		__FILE__, __func__, dev_path(dev));
	while (link != NULL) {
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
		if (cur->align == res->align || cur->next == NULL) /* equal or last record */
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
			if (cur->next == NULL) {
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

	assert(nr != NULL && nr->align == res->align);

	struct pci_resource *npr = malloc(sizeof(struct pci_resource));
	if (npr == NULL)
		die("%s: out of memory.\n", __func__);
	npr->res = res;
	npr->dev = dev;
	npr->next = NULL;

	if (nr->children == NULL) {
		nr->children = npr;
	} else {
		struct pci_resource *pr = nr->children;
		while (pr->next != NULL)
			pr = pr->next;
		pr->next = npr;
	}
}

static void reserve_dev_resources(STACK_RES *stack, unsigned long res_type,
	struct stack_dev_resource *res_root, struct resource *bridge)
{
	uint8_t align;
	uint64_t orig_base, base;

	if (res_type & IORESOURCE_IO)
		orig_base = stack->PciResourceIoBase;
	else if ((res_type & IORESOURCE_MEM) && ((res_type & IORESOURCE_PCI64) ||
		(!res_root && bridge && (bridge->flags & IORESOURCE_PREFETCH))))
		orig_base = stack->PciResourceMem64Base;
	else
		orig_base = stack->PciResourceMem32Base;

	align = 0;
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

	/* update new limits */
	if (res_type & IORESOURCE_IO)
		stack->PciResourceIoBase = base;
	else if ((res_type & IORESOURCE_MEM) && ((res_type & IORESOURCE_PCI64) ||
		(!res_root && bridge && (bridge->flags & IORESOURCE_PREFETCH))))
		stack->PciResourceMem64Base = base;
	else
		stack->PciResourceMem32Base = base;
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
			(bridge && ((bridge->flags & (IORESOURCE_IO | IORESOURCE_MEM |
				IORESOURCE_PREFETCH | IORESOURCE_PCI64)) !=
					(res->flags & (IORESOURCE_IO | IORESOURCE_MEM |
						IORESOURCE_PREFETCH | IORESOURCE_PCI64)))))
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
		assert(stack != NULL);

		/* Assign resources to bridge */
		for (curdev = bus->children; curdev; curdev = curdev->sibling)
			assign_bridge_resources(stack_list, curdev, bridge);

		/* Pick non-bridged resources for resource allocation for each resource type */
		unsigned long flags[5] = {IORESOURCE_IO, IORESOURCE_MEM,
			(IORESOURCE_PCI64|IORESOURCE_MEM), (IORESOURCE_MEM|IORESOURCE_PREFETCH),
			(IORESOURCE_PCI64|IORESOURCE_MEM|IORESOURCE_PREFETCH)};
		uint8_t no_res_types = 5;
		if (bridge) {
			flags[0] = bridge->flags &
				(IORESOURCE_IO | IORESOURCE_MEM | IORESOURCE_PREFETCH);
			if ((bridge->flags & IORESOURCE_MEM) &&
				(bridge->flags & IORESOURCE_PREFETCH))
				flags[0] |= IORESOURCE_PCI64;
			no_res_types = 1;
		}

		/* Process each resource type */
		for (int rt = 0; rt < no_res_types; ++rt) {
			struct stack_dev_resource *res_root = NULL;

			for (curdev = bus->children; curdev; curdev = curdev->sibling) {
				struct resource *res;
				if (!curdev->enabled)
					continue;

				for (res = curdev->resource_list; res; res = res->next) {
					if ((res->flags & IORESOURCE_BRIDGE) || (res->flags &
						(IORESOURCE_STORED | IORESOURCE_RESERVE |
							IORESOURCE_FIXED | IORESOURCE_ASSIGNED)
						) || ((res->flags & (IORESOURCE_IO |
							IORESOURCE_MEM | IORESOURCE_PCI64
							| IORESOURCE_PREFETCH))
							!= flags[rt]) || res->size == 0)
						continue;
					else
						add_res_to_stack(&res_root, curdev, res);
				}
			}

			/* Allocate resources and update bridge range */
			if (res_root || (bridge && !(bridge->flags & IORESOURCE_ASSIGNED))) {
				reserve_dev_resources(stack, flags[rt], res_root, bridge);
				reclaim_resource_mem(res_root);
			}
		}
	}
}

static void xeonsp_constrain_pci_resources(struct device *dev, struct resource *res, void *data)
{
	STACK_RES *stack = (STACK_RES *) data;
	if (!(res->flags & IORESOURCE_FIXED))
		return;

	uint64_t base, limit;
	if (res->flags & IORESOURCE_IO) {
		base = stack->PciResourceIoBase;
		limit = stack->PciResourceIoLimit;
	} else if ((res->flags & IORESOURCE_MEM) && (res->flags & IORESOURCE_PCI64)) {
		base = stack->PciResourceMem64Base;
		limit = stack->PciResourceMem64Limit;
	} else {
		base = stack->PciResourceMem32Base;
		limit = stack->PciResourceMem32Limit;
	}

	if (((res->base + res->size - 1) < base) || (res->base > limit)) /* outside window */
		return;

	if (res->limit > limit) /* resource end is out of limit */
		limit = res->base - 1;
	else
		base = res->base + res->size;

	if (res->flags & IORESOURCE_IO) {
		stack->PciResourceIoBase = base;
		stack->PciResourceIoLimit = limit;
	} else if ((res->flags & IORESOURCE_MEM) && (res->flags & IORESOURCE_PCI64)) {
		stack->PciResourceMem64Base = base;
		stack->PciResourceMem64Limit = limit;
	} else {
		stack->PciResourceMem32Base = base;
		stack->PciResourceMem32Limit = limit;
	}
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

	/*
	 * 1. group devices, resources for each stack
	 * 2. order resources in descending order of requested resource allocation sizes
	 */
	struct iiostack_resource stack_info = {0};
	get_iiostack_info(&stack_info);

	/* constrain stack window */
	for (link = dev->link_list; link; link = link->next) {
		STACK_RES *stack = find_stack_for_bus(&stack_info, link->secondary);
		assert(stack != 0);
		xeonsp_pci_dev_iterator(link, NULL, xeonsp_constrain_pci_resources, stack);
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

static void xeonsp_pci_domain_set_resources(struct device *dev)
{
	DEV_FUNC_ENTER(dev);

	print_resource_tree(dev, BIOS_SPEW, "Before xeonsp pci domain set resource");

	/* reset bus 0 dev resource assignment - need to change them to FSP IIOStack window */
	xeonsp_pci_dev_iterator(dev->link_list, NULL, reset_resource_to_unassigned, NULL);

	/* update dev resources based on IIOStack IO/Mem32/Mem64 windows */
	xeonsp_pci_domain_read_resources(dev);

	struct bus *link = dev->link_list;
	while (link != NULL) {
		assign_resources(link);
		link = link->next;
	}

	print_resource_tree(dev, BIOS_SPEW, "After xeonsp pci domain set resource");

	DEV_FUNC_EXIT(dev);
}

static struct device_operations pci_domain_ops = {
	.read_resources = &pci_domain_read_resources,
	.set_resources = &xeonsp_pci_domain_set_resources,
	.scan_bus = &xeonsp_pci_domain_scan_bus,
#if CONFIG(HAVE_ACPI_TABLES)
	.write_acpi_tables  = &northbridge_write_acpi_tables,
#endif
};

static struct device_operations cpu_bus_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.init = xeon_sp_init_cpus,
#if CONFIG(HAVE_ACPI_TABLES)
	/* defined in src/soc/intel/common/block/acpi/acpi.c */
	.acpi_fill_ssdt = generate_cpu_entries,
#endif
};

/* Attach IIO stack bus numbers with dummy device to PCI DOMAIN 0000 device */
static void attach_iio_stacks(struct device *dev)
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
		if (iiostack_bus == NULL)
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

		if (dev->link_list == NULL) {
			dev->link_list = iiostack_bus;
		} else {
			struct bus *nlink = dev->link_list;
			while (nlink->next != NULL)
				nlink = nlink->next;
			nlink->next = iiostack_bus;
		}
	}

	DEV_FUNC_EXIT(dev);
}

static void soc_enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
		attach_iio_stacks(dev);
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

static void soc_init(void *data)
{
	printk(BIOS_DEBUG, "coreboot: calling fsp_silicon_init\n");
	fsp_silicon_init(false);
}

static void soc_final(void *data)
{
	// Temp Fix - should be done by FSP, in 2S bios completion
	// is not carried out on socket 2
	set_bios_init_completion();
}

static void soc_silicon_init_params(FSPS_UPD *silupd)
{
}

void platform_fsp_silicon_init_params_cb(FSPS_UPD *silupd)
{
	const struct microcode *microcode_file;
	size_t microcode_len;

	microcode_file = cbfs_boot_map_with_leak("cpu_microcode_blob.bin",
		CBFS_TYPE_MICROCODE, &microcode_len);

	if ((microcode_file != NULL) && (microcode_len != 0)) {
		/* Update CPU Microcode patch base address/size */
		silupd->FspsConfig.PcdCpuMicrocodePatchBase =
		       (uint32_t)microcode_file;
		silupd->FspsConfig.PcdCpuMicrocodePatchSize =
		       (uint32_t)microcode_len;
	}

	soc_silicon_init_params(silupd);
	mainboard_silicon_init_params(silupd);
}

struct chip_operations soc_intel_xeon_sp_skx_ops = {
	CHIP_NAME("Intel Skylake-SP")
	.enable_dev = soc_enable_dev,
	.init = soc_init,
	.final = soc_final
};

struct pci_operations soc_pci_ops = {
	.set_subsystem = pci_dev_set_subsystem,
};
