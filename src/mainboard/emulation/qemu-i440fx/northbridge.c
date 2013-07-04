#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/lapic_def.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <stdlib.h>
#include <string.h>
#include <delay.h>
#include <smbios.h>
#include <cbmem.h>

#include "fw_cfg.h"

#include "memory.c"

static unsigned long qemu_get_high_memory_size(void)
{
	unsigned long high;
	outb (HIGH_HIGHRAM_ADDR, CMOS_ADDR_PORT);
	high = ((unsigned long) inb(CMOS_DATA_PORT)) << 22;
	outb (MID_HIGHRAM_ADDR, CMOS_ADDR_PORT);
	high |= ((unsigned long) inb(CMOS_DATA_PORT)) << 14;
	outb (LOW_HIGHRAM_ADDR, CMOS_ADDR_PORT);
	high |= ((unsigned long) inb(CMOS_DATA_PORT)) << 6;
	return high;
}

static void cpu_pci_domain_set_resources(device_t dev)
{
	assign_resources(dev->link_list);
}

static void cpu_pci_domain_read_resources(struct device *dev)
{
	struct resource *res;
	unsigned long tomk = 0, high;
	int idx = 10;

	pci_domain_read_resources(dev);

	tomk = qemu_get_memory_size();
	high = qemu_get_high_memory_size();
	printk(BIOS_DEBUG, "Detected %lu MiB RAM below 4G.\n", tomk / 1024);
	printk(BIOS_DEBUG, "Detected %lu MiB RAM above 4G.\n", high / 1024);

	/* Report the memory regions. */
	idx = 10;
	ram_resource(dev, idx++, 0, 640);
	ram_resource(dev, idx++, 768, tomk - 768);
	if (high)
		ram_resource(dev, idx++, 4 * 1024 * 1024, high);

#if !CONFIG_DYNAMIC_CBMEM
	/* Leave some space for ACPI, PIRQ and MP tables */
	high_tables_base = (tomk * 1024) - HIGH_MEMORY_SIZE;
	high_tables_size = HIGH_MEMORY_SIZE;
#endif

	/* Reserve space for the IOAPIC.  This should be in the Southbridge,
	 * but I couldn't tell which device to put it in. */
	res = new_resource(dev, 2);
	res->base = IO_APIC_ADDR;
	res->size = 0x100000UL;
	res->limit = 0xffffffffUL;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_STORED |
		     IORESOURCE_ASSIGNED;

	/* Reserve space for the LAPIC.  There's one in every processor, but
	 * the space only needs to be reserved once, so we do it here. */
	res = new_resource(dev, 3);
	res->base = LOCAL_APIC_ADDR;
	res->size = 0x10000UL;
	res->limit = 0xffffffffUL;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_STORED |
		     IORESOURCE_ASSIGNED;
}

#if CONFIG_GENERATE_SMBIOS_TABLES
static int qemu_get_smbios_data16(int handle, unsigned long *current)
{
	struct smbios_type16 *t = (struct smbios_type16 *)*current;
	int len = sizeof(struct smbios_type16);

	memset(t, 0, sizeof(struct smbios_type16));
	t->type = SMBIOS_PHYS_MEMORY_ARRAY;
	t->handle = handle;
	t->length = len - 2;
	t->location = 3; /* Location: System Board */
	t->use = 3; /* System memory */
	t->memory_error_correction = 3; /* No error correction */
	t->maximum_capacity = qemu_get_memory_size();
	*current += len;
	return len;
}

static int qemu_get_smbios_data17(int handle, int parent_handle, unsigned long *current)
{
	struct smbios_type17 *t = (struct smbios_type17 *)*current;
	int len;

	memset(t, 0, sizeof(struct smbios_type17));
	t->type = SMBIOS_MEMORY_DEVICE;
	t->handle = handle;
	t->phys_memory_array_handle = parent_handle;
	t->length = sizeof(struct smbios_type17) - 2;
	t->size = qemu_get_memory_size() / 1024;
	t->data_width = 64;
	t->total_width = 64;
	t->form_factor = 9; /* DIMM */
	t->device_locator = smbios_add_string(t->eos, "Virtual");
	t->memory_type = 0x12; /* DDR */
	t->type_detail = 0x80; /* Synchronous */
	t->speed = 200;
	t->clock_speed = 200;
	t->manufacturer = smbios_add_string(t->eos, CONFIG_MAINBOARD_VENDOR);
	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	return len;
}

static int qemu_get_smbios_data(device_t dev, int *handle, unsigned long *current)
{
	int len;
	len = qemu_get_smbios_data16(*handle, current);
	len += qemu_get_smbios_data17(*handle+1, *handle, current);
	*handle += 2;
	return len;
}
#endif
static struct device_operations pci_domain_ops = {
	.read_resources		= cpu_pci_domain_read_resources,
	.set_resources		= cpu_pci_domain_set_resources,
	.enable_resources	= NULL,
	.init			= NULL,
	.scan_bus		= pci_domain_scan_bus,
	.ops_pci_bus	= pci_bus_default_ops,
#if CONFIG_GENERATE_SMBIOS_TABLES
	.get_smbios_data	= qemu_get_smbios_data,
#endif
};

static void cpu_bus_init(device_t dev)
{
	initialize_cpus(dev->link_list);
}

static unsigned int cpu_bus_scan(device_t bus, unsigned int max)
{
	int max_cpus = fw_cfg_max_cpus();
	device_t cpu;
	int i;

	if (max_cpus < 0)
		return 0;

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
	return max_cpus;
}

static void cpu_bus_noop(device_t dev)
{
}

static struct device_operations cpu_bus_ops = {
	.read_resources   = cpu_bus_noop,
	.set_resources    = cpu_bus_noop,
	.enable_resources = cpu_bus_noop,
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
