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
#include "fw_cfg_if.h"

#include "memory.c"
#include "acpi.h"

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
	u16 nbid   = pci_read_config16(dev_find_slot(0, 0), PCI_DEVICE_ID);
	int i440fx = (nbid == 0x1237);
	int q35    = (nbid == 0x29c0);
	struct resource *res;
	unsigned long tomk = 0, high;
	int idx = 10;
	int size;

	pci_domain_read_resources(dev);

	size = fw_cfg_check_file("etc/e820");
	if (size > 0) {
		/* supported by qemu 1.7+ */
		FwCfgE820Entry *list = malloc(size);
		int i;
		fw_cfg_load_file("etc/e820", list);
		for (i = 0; i < size/sizeof(*list); i++) {
			switch (list[i].type) {
			case 1: /* RAM */
				printk(BIOS_DEBUG, "QEMU: e820/ram: 0x%08llx +0x%08llx\n",
				       list[i].address, list[i].length);
				if (list[i].address == 0) {
					tomk = list[i].length / 1024;
					ram_resource(dev, idx++, 0, 640);
					ram_resource(dev, idx++, 768, tomk - 768);
				} else {
					ram_resource(dev, idx++,
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
		ram_resource(dev, idx++, 0, 640);
		ram_resource(dev, idx++, 768, tomk - 768);
		if (high)
			ram_resource(dev, idx++, 4 * 1024 * 1024, high);
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
	res->base = LOCAL_APIC_ADDR;
	res->size = 0x10000UL;
	res->limit = 0xffffffffUL;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_STORED |
		     IORESOURCE_ASSIGNED;
}

#if IS_ENABLED(CONFIG_GENERATE_SMBIOS_TABLES)
static int qemu_get_smbios_data16(int handle, unsigned long *current)
{
	struct smbios_type16 *t = (struct smbios_type16 *)*current;
	int len = sizeof(struct smbios_type16);

	memset(t, 0, sizeof(struct smbios_type16));
	t->type = SMBIOS_PHYS_MEMORY_ARRAY;
	t->handle = handle;
	t->length = len - 2;
	t->location = MEMORY_ARRAY_LOCATION_SYSTEM_BOARD;
	t->use = MEMORY_ARRAY_USE_SYSTEM;
	t->memory_error_correction = MEMORY_ARRAY_ECC_NONE;
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
static struct device_operations pci_domain_ops = {
	.read_resources		= cpu_pci_domain_read_resources,
	.set_resources		= cpu_pci_domain_set_resources,
	.enable_resources	= NULL,
	.init			= NULL,
	.scan_bus		= pci_domain_scan_bus,
#if IS_ENABLED(CONFIG_GENERATE_SMBIOS_TABLES)
	.get_smbios_data	= qemu_get_smbios_data,
#endif
};

static void cpu_bus_init(struct device *dev)
{
	initialize_cpus(dev->link_list);
}

static void cpu_bus_scan(struct device *bus)
{
	int max_cpus = fw_cfg_max_cpus();
	struct device *cpu;
	int i;

	if (max_cpus < 0)
		return;

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
	.read_resources   = DEVICE_NOOP,
	.set_resources    = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
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
