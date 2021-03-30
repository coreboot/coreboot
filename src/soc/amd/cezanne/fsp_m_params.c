/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/apob_cache.h>
#include <amdblocks/memmap.h>
#include <assert.h>
#include <console/uart.h>
#include <fsp/api.h>
#include <soc/platform_descriptors.h>
#include <string.h>
#include <types.h>

static void fill_dxio_descriptors(FSP_M_CONFIG *mcfg,
			const fsp_dxio_descriptor *descs, size_t num)
{
	size_t i;

	ASSERT_MSG(num <= FSPM_UPD_DXIO_DESCRIPTOR_COUNT,
			"Too many DXIO descriptors provided.");

	for (i = 0; i < num; i++) {
		memcpy(mcfg->dxio_descriptor[i], &descs[i], sizeof(mcfg->dxio_descriptor[0]));
	}
}

static void fill_ddi_descriptors(FSP_M_CONFIG *mcfg,
			const fsp_ddi_descriptor *descs, size_t num)
{
	size_t i;

	ASSERT_MSG(num <= FSPM_UPD_DDI_DESCRIPTOR_COUNT,
			"Too many DDI descriptors provided.");

	for (i = 0; i < num; i++) {
		memcpy(&mcfg->ddi_descriptor[i], &descs[i], sizeof(mcfg->ddi_descriptor[0]));
	}
}

static void fsp_fill_pcie_ddi_descriptors(FSP_M_CONFIG *mcfg)
{
	const fsp_dxio_descriptor *fsp_dxio;
	const fsp_ddi_descriptor *fsp_ddi;
	size_t num_dxio;
	size_t num_ddi;

	mainboard_get_dxio_ddi_descriptors(&fsp_dxio, &num_dxio,
						&fsp_ddi, &num_ddi);
	fill_dxio_descriptors(mcfg, fsp_dxio, num_dxio);
	fill_ddi_descriptors(mcfg, fsp_ddi, num_ddi);
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	FSP_M_CONFIG *mcfg = &mupd->FspmConfig;

	mupd->FspmArchUpd.NvsBufferPtr = (uintptr_t)soc_fill_apob_cache();

	mcfg->pci_express_base_addr = CONFIG_MMCONF_BASE_ADDRESS;
	mcfg->tseg_size = CONFIG_SMM_TSEG_SIZE;
	mcfg->bert_size = CONFIG_ACPI_BERT_SIZE;
	mcfg->serial_port_base = uart_platform_base(CONFIG_UART_FOR_CONSOLE);
	mcfg->serial_port_use_mmio = CONFIG(DRIVERS_UART_8250MEM);
	mcfg->serial_port_baudrate = get_uart_baudrate();
	mcfg->serial_port_refclk = uart_platform_refclk();

	fsp_fill_pcie_ddi_descriptors(mcfg);
}
