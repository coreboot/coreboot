/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <drivers/ipmi/ipmi_if.h>
#include <drivers/ocp/include/vpd.h>
#include <drivers/vpd/vpd.h>
#include <fmap_config.h>
#include <device/device.h>
#include <soc/ddr.h>
#include <soc/dimm_slot.h>
#include <soc/iio.h>
#include <soc/romstage.h>
#include <spi_flash.h>
#include <static.h>

#include "chip.h"

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;

	/* FSP log outputs */
	const config_t *config = config_of_soc();
	m_cfg->SerialIoUartDebugIoBase = config->serial_io_uart_debug_io_base;
	m_cfg->SerialIoUartDebugEnable = get_bool_from_vpd(FSP_LOG, FSP_LOG_DEFAULT);
	m_cfg->DebugPrintLevel = config->debug_print_level;
	m_cfg->serialDebugMsgLvl = get_int_from_vpd_range(FSP_MEM_LOG_LEVEL,
		FSP_MEM_LOG_LEVEL_DEFAULT, 0, 4);

	/* Early connect BMC, e.g. to query configuration parameters */
	if (ipmi_premem_init(CONFIG_BMC_KCS_BASE, 0) == CB_SUCCESS)
		printk(BIOS_INFO, "IPMI at 0x%04x initialized successfully\n",
			CONFIG_BMC_KCS_BASE);

	/* Set BIOS regeion UPD, otherwise MTRR might set incorrectly during TempRamExit API */
	struct flash_mmap_window table;
	spi_flash_get_mmap_windows(&table);

	m_cfg->BiosRegionBase = table.host_base + FMAP_SECTION_SI_BIOS_START;
	m_cfg->BiosRegionSize = FMAP_SECTION_SI_BIOS_SIZE;
	printk(BIOS_INFO, "BiosRegionBase is set to %x\n", mupd->FspmConfig.BiosRegionBase);
	printk(BIOS_INFO, "BiosRegionSize is set to %x\n", mupd->FspmConfig.BiosRegionSize);

	/* IIO init */
	int size;
	const struct iio_pe_config *iio_config_table = get_iio_config_table(&size);
	soc_config_iio_pe_ports(mupd, iio_config_table, size);
}

bool mainboard_dimm_slot_exists(uint8_t socket, uint8_t channel, uint8_t dimm)
{
	int size;
	const struct dimm_slot_config *dimm_slot_config_table = get_dimm_slot_config_table(&size);

	for (int i = 0; i < size; i++) {
		if (DIMM_SLOT_EQUAL(dimm_slot_config_table[i], socket, channel, dimm))
			return true;
	}

	return false;
}
