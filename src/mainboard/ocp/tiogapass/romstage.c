/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <fsp/api.h>
#include <FspmUpd.h>
#include <soc/romstage.h>

#include "skxsp_tp_gpio.h"
#include "skxsp_tp_iio.h"

/*
* Configure GPIO depend on platform
*/
static void mainboard_config_gpios(FSPM_UPD *mupd)
{
	mupd->FspmConfig.GpioConfig.GpioTable = (UPD_GPIO_INIT_CONFIG *) tp_gpio_table;
	mupd->FspmConfig.GpioConfig.NumberOfEntries =
		sizeof(tp_gpio_table)/sizeof(UPD_GPIO_INIT_CONFIG);
}

static void mainboard_config_iio(FSPM_UPD *mupd)
{
	mupd->FspmConfig.IioBifurcationConfig.IIoBifurcationTable =
		(UPD_IIO_BIFURCATION_DATA_ENTRY *) tp_iio_bifur_table;
	mupd->FspmConfig.IioBifurcationConfig.NumberOfEntries =
		ARRAY_SIZE(tp_iio_bifur_table);

	mupd->FspmConfig.IioPciConfig.ConfigurationTable =
		(UPD_PCI_PORT_CONFIG *) tp_iio_pci_port_skt0;
	mupd->FspmConfig.IioPciConfig.NumberOfEntries =
		ARRAY_SIZE(tp_iio_pci_port_skt0);

	mupd->FspmConfig.PchPciConfig.PciPortConfig =
		(UPD_PCH_PCIE_PORT *) tp_pch_pci_port_skt0;
	mupd->FspmConfig.PchPciConfig.NumberOfEntries =
		ARRAY_SIZE(tp_pch_pci_port_skt0);

	mupd->FspmConfig.PchPciConfig.RootPortFunctionSwapping = 0x00;
	mupd->FspmConfig.PchPciConfig.PciePllSsc = 0x00;
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	mainboard_config_gpios(mupd);
	mainboard_config_iio(mupd);
}
