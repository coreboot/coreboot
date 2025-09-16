/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/lpc.h>
#include <device/pci.h>
#include <soc/pci_devs.h>

void soc_lpc_tpm_decode_spi(void)
{
	/* SoC-specific SPI TPM setting */
	pci_or_config32(SOC_LPC_DEV, 0xdc, 1);
}
