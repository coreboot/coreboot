/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <acpi/acpigen_pci.h>
#include <cbfs.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_def.h>
#include <soc/pci_devs.h>
#include <gpio.h>
#include <intelblocks/acpi.h>
#include <soc/acpi.h>
#include <soc/chip_common.h>
#include <soc/pch.h>
#include <soc/soc_pch.h>
#include <soc/ramstage.h>
#include <soc/soc_util.h>
#include <soc/util.h>

static struct device_operations cpu_bus_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.init = mp_cpu_bus_init,
#if CONFIG(HAVE_ACPI_TABLES)
	/* defined in src/soc/intel/common/block/acpi/acpi.c */
	.acpi_fill_ssdt = generate_cpu_entries,
#endif
};

static void soc_enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		/* domain ops are assigned at their creation */
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	} else if (dev->path.type == DEVICE_PATH_GPIO) {
		block_gpio_enable(dev);
	}
}

/* Only call this code from socket0! */
static void unlock_pam_regions(void)
{
	uint32_t pam0123_unlock_dram = 0x33333330;
	uint32_t pam456_unlock_dram = 0x00333333;
	/* Get UBOX(1) for socket0 */
	uint32_t bus1 = socket0_get_ubox_busno(PCU_IIO_STACK);

	/* Assume socket0 owns PCI segment 0 */
	pci_io_write_config32(PCI_DEV(bus1, SAD_ALL_DEV, SAD_ALL_FUNC),
		SAD_ALL_PAM0123_CSR, pam0123_unlock_dram);
	pci_io_write_config32(PCI_DEV(bus1, SAD_ALL_DEV, SAD_ALL_FUNC),
		SAD_ALL_PAM456_CSR, pam456_unlock_dram);

	uint32_t reg1 = pci_io_read_config32(PCI_DEV(bus1, SAD_ALL_DEV,
		SAD_ALL_FUNC), SAD_ALL_PAM0123_CSR);
	uint32_t reg2 = pci_io_read_config32(PCI_DEV(bus1, SAD_ALL_DEV,
		SAD_ALL_FUNC), SAD_ALL_PAM456_CSR);
	printk(BIOS_DEBUG, "%s:%s pam0123_csr: 0x%x, pam456_csr: 0x%x\n",
		__FILE__, __func__, reg1, reg2);
}

static void soc_init(void *data)
{
	unlock_pam_regions();

	printk(BIOS_DEBUG, "coreboot: calling fsp_silicon_init\n");
	fsp_silicon_init();

	attach_iio_stacks();

	override_hpet_ioapic_bdf();
	pch_lock_dmictl();
}

static void soc_final(void *data)
{
	// Temp Fix - should be done by FSP, in 2S bios completion
	// is not carried out on socket 2
	set_bios_init_completion();
}

void platform_fsp_silicon_init_params_cb(FSPS_UPD *silupd)
{
	const struct microcode *microcode_file;
	size_t microcode_len;

	microcode_file = cbfs_map("cpu_microcode_blob.bin", &microcode_len);

	if ((microcode_file) && (microcode_len != 0)) {
		/* Update CPU Microcode patch base address/size */
		silupd->FspsConfig.PcdCpuMicrocodePatchBase =
		       (uint32_t)microcode_file;
		silupd->FspsConfig.PcdCpuMicrocodePatchSize =
		       (uint32_t)microcode_len;
	}

	mainboard_silicon_init_params(silupd);
}

struct chip_operations soc_intel_xeon_sp_skx_ops = {
	.name = "Intel Skylake-SP",
	.enable_dev = soc_enable_dev,
	.init = soc_init,
	.final = soc_final
};

struct pci_operations soc_pci_ops = {
	.set_subsystem = pci_dev_set_subsystem,
};
