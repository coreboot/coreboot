/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <delay.h>
#include <drivers/ipmi/ipmi_if.h>
#include <drivers/ipmi/ocp/ipmi_ocp.h>
#include <fsp/api.h>
#include <FspmUpd.h>
#include <gpio.h>
#include <soc/ddr.h>
#include <soc/gpio_soc_defs.h>
#include <soc/romstage.h>
#include <string.h>
#include <skxsp_tp_iio.h>
#include <timer.h>

#include "ipmi.h"
#include "tp_pch_gpio.h"

static uint8_t iio_table_buf[sizeof(tp_iio_bifur_table)];

static void oem_update_iio(FSPM_UPD *mupd)
{
	/* Read GPIO to decide IIO bifurcation at run-time. */
	int slot_config0 = gpio_get(GPP_C15);
	int slot_config1 = gpio_get(GPP_C16);

	/* It's a single side 3 slots riser card, to tell which AICs are on each slot requires
	   reading the GPIO expander PCA9555 via SMBUS, and then configure the bifurcation
	   accordingly is left for future work.	*/
	if (!slot_config0 && slot_config1)
		mupd->FspmConfig.IioBifurcationConfig.IIoBifurcationTable[Skt0_Iou0].Bifurcation
			= IIO_BIFURCATE_xxx8xxx8;
}

static void mainboard_config_iio(FSPM_UPD *mupd)
{
	memcpy(iio_table_buf, tp_iio_bifur_table, sizeof(tp_iio_bifur_table));
	mupd->FspmConfig.IioBifurcationConfig.IIoBifurcationTable =
		(UPD_IIO_BIFURCATION_DATA_ENTRY *)iio_table_buf;
	mupd->FspmConfig.IioBifurcationConfig.NumberOfEntries =
		ARRAY_SIZE(tp_iio_bifur_table);

	mupd->FspmConfig.IioPciConfig.ConfigurationTable =
		(UPD_PCI_PORT_CONFIG *)tp_iio_pci_port_skt0;
	mupd->FspmConfig.IioPciConfig.NumberOfEntries =
		ARRAY_SIZE(tp_iio_pci_port_skt0);

	mupd->FspmConfig.PchPciConfig.PciPortConfig =
		(UPD_PCH_PCIE_PORT *)tp_pch_pci_port_skt0;
	mupd->FspmConfig.PchPciConfig.NumberOfEntries =
		ARRAY_SIZE(tp_pch_pci_port_skt0);

	mupd->FspmConfig.PchPciConfig.RootPortFunctionSwapping = 0x00;
	mupd->FspmConfig.PchPciConfig.PciePllSsc = 0x00;
	oem_update_iio(mupd);
}

static void mainboard_wait_for_bmc_ready(void)
{
	struct stopwatch sw;
	static const long timeout_ms = 300 * 1000;

	printk(BIOS_DEBUG, "Waiting for BMC ready\n");
	gpio_input(GPIO_BMC_READY_N);

	stopwatch_init_msecs_expire(&sw, timeout_ms);
	while (gpio_get(GPIO_BMC_READY_N)) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_WARNING,
			       "BMC not ready after %ld ms. Abort.\n", timeout_ms);
			return;
		}
	}
	printk(BIOS_DEBUG, "BMC ready after %lld ms\n",
	       stopwatch_duration_msecs(&sw));
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	/* Need to wait for BMC ready so that IPMI works. */
	mainboard_wait_for_bmc_ready();

	/* It's better to run get BMC selftest result first */
	if (ipmi_premem_init(CONFIG_BMC_KCS_BASE, 0) == CB_SUCCESS) {
		ipmi_set_post_start(CONFIG_BMC_KCS_BASE);
		init_frb2_wdt();
	}
	mainboard_config_iio(mupd);

	/* do not configure GPIO controller inside FSP-M */
	mupd->FspmConfig.GpioConfig.GpioTable = NULL;
	mupd->FspmConfig.GpioConfig.NumberOfEntries = 0;
}

bool mainboard_dimm_slot_exists(uint8_t socket, uint8_t channel, uint8_t dimm)
{
	if (socket >= CONFIG_MAX_SOCKET)
		return false;
	if (channel >= 6)
		return false;
	if (dimm >= 2)
		return false;

	return true;
}
