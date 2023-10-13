/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <amdblocks/data_fabric.h>
#include <amdblocks/pci_devs.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <soc/data_fabric.h>
#include <soc/pci_devs.h>
#include <types.h>

static void data_fabric_set_indirect_address(uint16_t fn_reg, uint8_t instance_id)
{
	union df_ficaa ficaa = { .cfg_inst_acc_en = 1 }; /* target only specific instance */
	/* convert register address to 32-bit register number */
	ficaa.reg_num = DF_REG_OFFSET(fn_reg) >> 2;
	ficaa.func_num = DF_REG_FN(fn_reg);
	ficaa.inst_id = instance_id;
	data_fabric_broadcast_write32(DF_FICAA_BIOS, ficaa.raw);
}

uint32_t data_fabric_read32(uint16_t fn_reg, uint8_t instance_id)
{
	/* Broadcast reads might return unexpected results when a register has different
	   contents in the different instances. */
	if (instance_id == BROADCAST_FABRIC_ID)
		return data_fabric_broadcast_read32(fn_reg);

	/* non-broadcast data fabric accesses need to be done via indirect access */
	data_fabric_set_indirect_address(fn_reg, instance_id);
	return data_fabric_broadcast_read32(DF_FICAD_LO);
}

void data_fabric_write32(uint16_t fn_reg, uint8_t instance_id, uint32_t data)
{
	if (instance_id == BROADCAST_FABRIC_ID) {
		data_fabric_broadcast_write32(fn_reg, data);
		return;
	}

	/* non-broadcast data fabric accesses need to be done via indirect access */
	data_fabric_set_indirect_address(fn_reg, instance_id);
	data_fabric_broadcast_write32(DF_FICAD_LO, data);
}

void data_fabric_print_mmio_conf(void)
{
	union df_mmio_control control;
	uint64_t base, limit;
	printk(BIOS_SPEW,
		"=== Data Fabric MMIO configuration registers ===\n"
		"idx             base            limit  control R W NP F-ID\n");
	for (unsigned int i = 0; i < DF_MMIO_REG_SET_COUNT; i++) {
		control.raw = data_fabric_broadcast_read32(DF_MMIO_CONTROL(i));
		data_fabric_get_mmio_base_size(i, &base, &limit);
		printk(BIOS_SPEW, " %2u %16llx %16llx %8x %s %s  %s %4x\n",
		       i, base, limit, control.raw,
		       control.re ? "x" : " ",
		       control.we ? "x" : " ",
		       control.np ? "x" : " ",
		       control.dst_fabric_id);
	}
}

#if CONFIG(HAVE_ACPI_TABLES)
static const char *data_fabric_acpi_name(const struct device *dev)
{
	const char *df_acpi_names[8] = {
		"DFD0",
		"DFD1",
		"DFD2",
		"DFD3",
		"DFD4",
		"DFD5",
		"DFD6",
		"DFD7"
	};

	if (dev->path.type == DEVICE_PATH_PCI &&
	    PCI_SLOT(dev->path.pci.devfn) == DF_DEV)
		return df_acpi_names[PCI_FUNC(dev->path.pci.devfn)];

	printk(BIOS_ERR, "%s: Unhandled device id 0x%x\n", __func__, dev->device);
	return NULL;
}
#endif

struct device_operations amd_data_fabric_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name		= data_fabric_acpi_name,
	.acpi_fill_ssdt		= acpi_device_write_pci_dev,
#endif
};
