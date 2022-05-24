/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/lapic_def.h>
#include <cpu/x86/mp.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <arch/ioapic.h>
#include <device/device.h>
#include <stdlib.h>
#include <smbios.h>
#include <types.h>
#include "memory.h"

#include "fw_cfg.h"
#include "fw_cfg_if.h"

#include "acpi.h"

static void qemu_reserve_ports(struct device *dev, unsigned int idx,
			       unsigned int base, unsigned int size,
			       const char *name)
{
	unsigned int end = base + size -1;
	struct resource *res;

	printk(BIOS_DEBUG, "QEMU: reserve ioports 0x%04x-0x%04x [%s]\n",
	       base, end, name);
	res = new_resource(dev, idx);
	res->base = base;
	res->size = size;
	res->limit = 0xffff;
	res->flags = IORESOURCE_IO | IORESOURCE_FIXED | IORESOURCE_STORED |
		IORESOURCE_ASSIGNED;
}

static void cpu_pci_domain_set_resources(struct device *dev)
{
	assign_resources(dev->link_list);
}

static void cpu_pci_domain_read_resources(struct device *dev)
{
	u16 nbid   = pci_read_config16(pcidev_on_root(0x0, 0), PCI_DEVICE_ID);
	int i440fx = (nbid == 0x1237);
	int q35    = (nbid == 0x29c0);
	struct resource *res;
	unsigned long tomk = 0, high;
	int idx = 10;
	FWCfgFile f;

	pci_domain_read_resources(dev);

	if (!fw_cfg_check_file(&f, "etc/e820") && f.size > 0) {
		/* supported by qemu 1.7+ */
		FwCfgE820Entry *list = malloc(f.size);
		int i;
		fw_cfg_get(f.select, list, f.size);
		for (i = 0; i < f.size / sizeof(*list); i++) {
			switch (list[i].type) {
			case 1: /* RAM */
				printk(BIOS_DEBUG, "QEMU: e820/ram: 0x%08llx + 0x%08llx\n",
				       list[i].address, list[i].length);
				if (list[i].address == 0) {
					tomk = list[i].length / 1024;
					ram_resource_kb(dev, idx++, 0, 640);
					ram_resource_kb(dev, idx++, 768, tomk - 768);
				} else {
					ram_resource_kb(dev, idx++,
						     list[i].address / 1024,
						     list[i].length / 1024);
				}
				break;
			case 2: /* reserved */
				printk(BIOS_DEBUG, "QEMU: e820/res: 0x%08llx +0x%08llx\n",
				       list[i].address, list[i].length);
				res = new_resource(dev, idx++);
				res->base = list[i].address;
				res->size = list[i].length;
				res->limit = 0xffffffff;
				res->flags = IORESOURCE_MEM | IORESOURCE_FIXED |
					IORESOURCE_STORED | IORESOURCE_ASSIGNED;
				break;
			default:
				/* skip unknown */
				break;
			}
		}
		free(list);
	}

	if (!tomk) {
		/* qemu older than 1.7, or reading etc/e820 failed. Fallback to cmos. */
		tomk = qemu_get_memory_size();
		high = qemu_get_high_memory_size();
		printk(BIOS_DEBUG, "QEMU: cmos: %lu MiB RAM below 4G.\n", tomk / 1024);
		printk(BIOS_DEBUG, "QEMU: cmos: %lu MiB RAM above 4G.\n", high / 1024);

		/* Report the memory regions. */
		ram_resource_kb(dev, idx++, 0, 640);
		ram_resource_kb(dev, idx++, 768, tomk - 768);
		if (high)
			ram_resource_kb(dev, idx++, 4 * 1024 * 1024, high);
	}

	/* Reserve I/O ports used by QEMU */
	qemu_reserve_ports(dev, idx++, 0x0510, 0x02, "firmware-config");
	qemu_reserve_ports(dev, idx++, 0x5658, 0x01, "vmware-port");
	if (i440fx) {
		qemu_reserve_ports(dev, idx++, 0xae00, 0x10, "pci-hotplug");
		qemu_reserve_ports(dev, idx++, 0xaf00, 0x20, "cpu-hotplug");
		qemu_reserve_ports(dev, idx++, 0xafe0, 0x04, "piix4-gpe0");
	}
	if (inb(CONFIG_CONSOLE_QEMU_DEBUGCON_PORT) == 0xe9) {
		qemu_reserve_ports(dev, idx++, CONFIG_CONSOLE_QEMU_DEBUGCON_PORT, 1,
				   "debugcon");
	}

	/* A segment is legacy VGA region */
	mmio_resource_kb(dev, idx++, 0xa0000 / KiB, (0xc0000 - 0xa0000) / KiB);

	/* C segment to 1MB is reserved RAM (low tables) */
	reserved_ram_resource_kb(dev, idx++, 0xc0000 / KiB, (1 * MiB - 0xc0000) / KiB);

	if (q35 && ((tomk * 1024) < 0xb0000000)) {
		/*
		 * Reserve the region between top-of-ram and the
		 * mmconf xbar (ar 0xb0000000), so coreboot doesn't
		 * place pci bars there.  The region isn't declared as
		 * pci io window in the ACPI tables (\_SB.PCI0._CRS).
		 */
		res = new_resource(dev, idx++);
		res->base = tomk * 1024;
		res->size = 0xb0000000 - tomk * 1024;
		res->limit = 0xffffffff;
		res->flags = IORESOURCE_MEM | IORESOURCE_FIXED |
			IORESOURCE_STORED | IORESOURCE_ASSIGNED;
	}

	if (i440fx) {
		/* Reserve space for the IOAPIC.  This should be in
		 * the southbridge, but I couldn't tell which device
		 * to put it in. */
		res = new_resource(dev, 2);
		res->base = IO_APIC_ADDR;
		res->size = 0x100000UL;
		res->limit = 0xffffffffUL;
		res->flags = IORESOURCE_MEM | IORESOURCE_FIXED |
			IORESOURCE_STORED | IORESOURCE_ASSIGNED;
	}

	/* Reserve space for the LAPIC.  There's one in every processor, but
	 * the space only needs to be reserved once, so we do it here. */
	res = new_resource(dev, 3);
	res->base = cpu_get_lapic_addr();
	res->size = 0x10000UL;
	res->limit = 0xffffffffUL;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_STORED |
		     IORESOURCE_ASSIGNED;
}

#if CONFIG(GENERATE_SMBIOS_TABLES)
static int qemu_get_smbios_data16(int handle, unsigned long *current)
{
	struct smbios_type16 *t = smbios_carve_table(*current, SMBIOS_PHYS_MEMORY_ARRAY,
						     sizeof(*t), handle);

	t->location = MEMORY_ARRAY_LOCATION_SYSTEM_BOARD;
	t->use = MEMORY_ARRAY_USE_SYSTEM;
	t->memory_error_correction = MEMORY_ARRAY_ECC_NONE;
	t->maximum_capacity = qemu_get_memory_size();

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	return len;
}

static int qemu_get_smbios_data17(int handle, int parent_handle, unsigned long *current)
{
	struct smbios_type17 *t = smbios_carve_table(*current, SMBIOS_MEMORY_DEVICE,
						     sizeof(*t), handle);

	t->phys_memory_array_handle = parent_handle;
	t->size = qemu_get_memory_size() / 1024;
	t->data_width = 64;
	t->total_width = 64;
	t->form_factor = MEMORY_FORMFACTOR_DIMM;
	t->device_locator = smbios_add_string(t->eos, "Virtual");
	t->memory_type = MEMORY_TYPE_DDR;
	t->type_detail = MEMORY_TYPE_DETAIL_SYNCHRONOUS;
	t->speed = 200;
	t->clock_speed = 200;
	t->manufacturer = smbios_add_string(t->eos, CONFIG_MAINBOARD_VENDOR);

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	return len;
}

static int qemu_get_smbios_data(struct device *dev, int *handle, unsigned long *current)
{
	int len;

	len = fw_cfg_smbios_tables(handle, current);
	if (len != 0)
		return len;

	len = qemu_get_smbios_data16(*handle, current);
	len += qemu_get_smbios_data17(*handle+1, *handle, current);
	*handle += 2;
	return len;
}
#endif

#if CONFIG(HAVE_ACPI_TABLES)
static const char *qemu_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	if (dev->path.type != DEVICE_PATH_PCI || dev->bus->secondary != 0)
		return NULL;

	return NULL;
}
#endif

static struct device_operations pci_domain_ops = {
	.read_resources		= cpu_pci_domain_read_resources,
	.set_resources		= cpu_pci_domain_set_resources,
	.scan_bus		= pci_domain_scan_bus,
#if CONFIG(GENERATE_SMBIOS_TABLES)
	.get_smbios_data	= qemu_get_smbios_data,
#endif
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name		= qemu_acpi_name,
#endif
};

static const struct mp_ops mp_ops_no_smm = {
	.get_cpu_count = fw_cfg_max_cpus,
};

extern const struct mp_ops mp_ops_with_smm;

void mp_init_cpus(struct bus *cpu_bus)
{
	const struct mp_ops *ops = CONFIG(NO_SMM) ? &mp_ops_no_smm : &mp_ops_with_smm;

	/* TODO: Handle mp_init_with_smm failure? */
	mp_init_with_smm(cpu_bus, ops);
}

static void cpu_bus_init(struct device *dev)
{
	if (CONFIG(PARALLEL_MP))
		mp_cpu_bus_init(dev);
	else
		initialize_cpus(dev->link_list);
}

static void cpu_bus_scan(struct device *bus)
{
	unsigned int max_cpus = fw_cfg_max_cpus();
	struct device *cpu;
	int i;

	if (max_cpus == 0)
		return;
	/*
	 * Do not install more CPUs than supported by coreboot.
	 * This will cause a buffer overflow where fixed arrays of CONFIG_MAX_CPUS
	 * are used and might result in a boot failure.
	 */
	max_cpus = MIN(max_cpus, CONFIG_MAX_CPUS);

	/*
	 * TODO: This only handles the simple "qemu -smp $nr" case
	 * correctly.  qemu also allows to specify the number of
	 * cores, threads & sockets.
	 */
	printk(BIOS_INFO, "QEMU: max_cpus is %d\n", max_cpus);
	for (i = 0; i < max_cpus; i++) {
		cpu = add_cpu_device(bus->link_list, i, 1);
		if (cpu)
			set_cpu_topology(cpu, 1, 0, i, 0);
	}
}

static struct device_operations cpu_bus_ops = {
	.read_resources   = noop_read_resources,
	.set_resources    = noop_set_resources,
	.init             = cpu_bus_init,
	.scan_bus         = cpu_bus_scan,
};

static void northbridge_enable(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	}
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations mainboard_emulation_qemu_i440fx_ops = {
	CHIP_NAME("QEMU Northbridge i440fx")
	.enable_dev = northbridge_enable,
};

struct chip_operations mainboard_emulation_qemu_q35_ops = {
	CHIP_NAME("QEMU Northbridge q35")
	.enable_dev = northbridge_enable,
};
