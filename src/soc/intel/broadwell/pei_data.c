/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/streams.h>
#include <device/device.h>
#include <soc/iomap.h>
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include <soc/intel/broadwell/chip.h>

static void ABI_X86 send_to_console(unsigned char b)
{
	console_tx_byte(b);
}

void broadwell_fill_pei_data(struct pei_data *pei_data)
{
	const struct soc_intel_broadwell_config *cfg = config_of_soc();

	pei_data->pei_version = PEI_VERSION;
	pei_data->board_type = BOARD_TYPE_ULT;
	pei_data->usbdebug = CONFIG(USBDEBUG);
	pei_data->pciexbar = CONFIG_ECAM_MMCONF_BASE_ADDRESS;
	pei_data->smbusbar = CONFIG_FIXED_SMBUS_IO_BASE;
	pei_data->ehcibar = CONFIG_EHCI_BAR;
	pei_data->xhcibar = 0xd7000000;
	pei_data->gttbar = 0xe0000000;
	pei_data->pmbase = ACPI_BASE_ADDRESS;
	pei_data->gpiobase = GPIO_BASE_ADDRESS;
	pei_data->tseg_size = CONFIG_SMM_TSEG_SIZE;
	pei_data->temp_mmio_base = 0xfed08000;
	pei_data->ec_present = cfg->ec_present,
	pei_data->dq_pins_interleaved = cfg->dq_pins_interleaved,
	pei_data->tx_byte = &send_to_console;
	pei_data->ddr_refresh_2x = 1;
}
