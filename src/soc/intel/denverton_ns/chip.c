/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <bootstate.h>
#include <cbfs.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/acpi.h>
#include <intelblocks/gpio.h>
#include <soc/iomap.h>
#include <soc/intel/common/vbt.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/fiamux.h>
#include <spi-generic.h>
#include <soc/hob_mem.h>

const char *soc_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	if (dev->path.type == DEVICE_PATH_USB) {
		switch (dev->path.usb.port_type) {
		case 0:
			/* Root Hub */
			return "RHUB";
		case 2:
			/* USB2 ports */
			switch (dev->path.usb.port_id) {
			case 0: return "HS01";
			case 1: return "HS02";
			case 2: return "HS03";
			case 3: return "HS04";
			}
			break;
		case 3:
			/* USB3 ports */
			switch (dev->path.usb.port_id) {
			case 4: return "SS01";
			case 5: return "SS02";
			case 6: return "SS03";
			case 7: return "SS04";
			}
			break;
		}
		return NULL;
	}

	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	switch (dev->path.pci.devfn) {
	case SA_DEVFN_ROOT:
		return "MCHC";
	case PCH_DEVFN_XHCI:
		return "XHCI";
	case PCH_DEVFN_UART0:
		return "UAR0";
	case PCH_DEVFN_UART1:
		return "UAR1";
	case PCH_DEVFN_UART2:
		return "UAR2";
	case PCH_DEVFN_PCIE1:
		return "RP01";
	case PCH_DEVFN_PCIE2:
		return "RP02";
	case PCH_DEVFN_PCIE3:
		return "RP03";
	case PCH_DEVFN_PCIE4:
		return "RP04";
	case PCH_DEVFN_PCIE5:
		return "RP05";
	case PCH_DEVFN_PCIE6:
		return "RP06";
	case PCH_DEVFN_PCIE7:
		return "RP07";
	case PCH_DEVFN_PCIE8:
		return "RP08";
	case PCH_DEVFN_LPC:
		return "LPCB";
	case PCH_DEVFN_SMBUS:
		return "SBUS";
	case PCH_DEVFN_SATA_0:
		return "SAT0";
	case PCH_DEVFN_SATA_1:
		return "SAT1";
	case PCH_DEVFN_EMMC:
		return "EMMC";
	case PCH_DEVFN_SPI:
		return "SPI0";
	case PCH_DEVFN_PMC:
		return "PMC_";
	case PCH_DEVFN_QAT:
		return "QAT_";
	case PCH_DEVFN_LAN0:
		return "LAN0";
	case PCH_DEVFN_LAN1:
		return "LAN1";
	}

	return NULL;
}

static struct device_operations pci_domain_ops = {
	.read_resources = &pci_domain_read_resources,
	.set_resources = &pci_domain_set_resources,
	.scan_bus = &pci_domain_scan_bus,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name = &soc_acpi_name,
#endif
};

static struct device_operations cpu_bus_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.init = mp_cpu_bus_init,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt = generate_cpu_entries,
#endif
};

static void soc_enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		dev->ops = &pci_domain_ops;
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &cpu_bus_ops;
	else if (dev->path.type == DEVICE_PATH_GPIO)
		block_gpio_enable(dev);
}

static void soc_init(void *data)
{
	fsp_silicon_init();
	soc_save_dimm_info();
}

static void soc_final(void *data) {}

static void soc_silicon_init_params(FSPS_UPD *silupd)
{
	size_t num;
	uint16_t supported_hsio_lanes;
	BL_HSIO_INFORMATION *hsio_config;
	BL_FIA_MUX_CONFIG_HOB *fiamux_hob_data = get_fiamux_hob_data();

	/* Configure FIA MUX PCD */
	supported_hsio_lanes =
		(uint16_t)fiamux_hob_data->FiaMuxConfig.SkuNumLanesAllowed;

	num = mainboard_get_hsio_config(&hsio_config);

	if (get_fiamux_hsio_info(supported_hsio_lanes, num, &hsio_config))
		die("HSIO Configuration is invalid, please correct it!");

	/* Initialize PCIE Bifurcation & HSIO configuration */
	silupd->FspsConfig.PcdBifurcationPcie0 = hsio_config->PcieBifCtr[0];
	silupd->FspsConfig.PcdBifurcationPcie1 = hsio_config->PcieBifCtr[1];

	silupd->FspsConfig.PcdFiaMuxConfigRequestPtr =
		(uint32_t)&hsio_config->FiaConfig;
}

void platform_fsp_silicon_init_params_cb(FSPS_UPD *silupd)
{
	const struct microcode *microcode_file;
	size_t microcode_len;

	microcode_file = cbfs_map("cpu_microcode_blob.bin", &microcode_len);

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

struct chip_operations soc_intel_denverton_ns_ops = {
	CHIP_NAME("Intel Denverton-NS SOC")
	.enable_dev = soc_enable_dev,
	.init = soc_init,
	.final = soc_final
};

struct pci_operations soc_pci_ops = {
	.set_subsystem = pci_dev_set_subsystem,
};

/*
 * spi_flash init() needs to run unconditionally on every boot (including
 * resume) to allow write protect to be disabled for eventlog and nvram
 * updates. This needs to be done as early as possible in ramstage. Thus, add a
 * callback for entry into BS_PRE_DEVICE.
 */
static void spi_flash_init_cb(void *unused)
{
	fast_spi_init();
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, spi_flash_init_cb, NULL);
