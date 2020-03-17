/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SOC_INTEL_COMMON_BLOCK_MMC_H
#define SOC_INTEL_COMMON_BLOCK_MMC_H

#include <stdint.h>

/*
 * Structure for the following delay registers
 * emmc_tx_data_cntl1: Tx Delay Control 1 (Tx_DATA_dly_1)-Offset 824h
 * emmc_tx_data_cntl2: Tx Delay Control 2 (Tx_DATA_dly_2)-Offset 828h
 * emmc_rx_cmd_data_cntl1: Rx CMD Data Delay Control 1
 *					(Rx_CMD_Data_dly_1)-Offset 82Ch
 * emmc_rx_cmd_data_cntl2: Rx CMD Data Path Delay Control 2
 *					(Rx_CMD_Data_dly_2)-Offset 834h
 * emmc_rx_strobe_cntl: Rx Strobe Delay Control
 *					(Rx_Strobe_Ctrl_Path)-Offset 830h
 * emmc_tx_cmd_cntl: Tx CMD Delay Control (Tx_CMD_dly)-Offset 820h
 */
struct mmc_dll_params {
	uint32_t emmc_tx_data_cntl1;
	uint32_t emmc_tx_data_cntl2;
	uint32_t emmc_rx_cmd_data_cntl1;
	uint32_t emmc_rx_cmd_data_cntl2;
	uint32_t emmc_rx_strobe_cntl;
	uint32_t emmc_tx_cmd_cntl;
};

/*
 * SOC specific API to get mmc min max frequencies.
 * returns 0, if able to get f_min, f_max; otherwise returns -1
 */
int soc_get_mmc_frequencies(uint32_t *f_min, uint32_t *f_max);
/*
 * SOC specific API to configure mmc gpios.
 * returns 0, if able to configure gpios; otherwise returns -1
 */
int soc_configure_mmc_gpios(void);
/*
 * SOC specific API to get mmc delay register settings.
 * returns 0, if able to get register settings; otherwise returns -1
 */
int soc_get_mmc_dll(struct mmc_dll_params *params);
/*
 * Set mmc delay register settings.
 * bar: eMMC controller MMIO base address.
 * returns 0, if able to set register settings; otherwise returns -1
 */
int set_mmc_dll(void *bar);

#define EMMC_TX_CMD_CNTL_OFFSET			0x820
#define EMMC_TX_DATA_CNTL1_OFFSET		0x824
#define EMMC_TX_DATA_CNTL2_OFFSET		0x828
#define EMMC_RX_CMD_DATA_CNTL1_OFFSET		0x82C
#define EMMC_RX_STROBE_CNTL_OFFSET		0x830
#define EMMC_RX_CMD_DATA_CNTL2_OFFSET		0x834

#if CONFIG(SOC_INTEL_COMMON_EARLY_MMC_WAKE)
/*
 * Following should be defined in soc/iomap.h
 * PRERAM_MMC_BASE_ADDRESS - Provide an address to setup emmc controller's
			PCI BAR.
 */

/*
 * Initializes sdhci / mmc controller and sends CMD0, CMD1 to emmc card.
 * In case of success: It returns 0 and adds cbmem entry CBMEM_ID_MMC_STATUS
 * and sets it to 1. Payload can start by sending CMD1, there is no need to
 * send CMD0 and wait for the card to be ready.
 * In case of failure: It returns -1 and doesn't add cbmem entry. Payload
 * should do complete initialization starting with CMD0.
 */
int early_mmc_wake_hw(void);
#else
static inline int early_mmc_wake_hw(void)
{
	return -1;
}
#endif /* CONFIG_SOC_INTEL_COMMON_EARLY_MMC_WAKE */
#endif /* SOC_INTEL_COMMON_BLOCK_MMC_H */
