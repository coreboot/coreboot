/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2012 The Linux Foundation. All rights reserved.
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

#ifndef  _IPQ806X_CDP_H_
#define  _IPQ806X_CDP_H_

#include <types.h>
#include <compiler.h>

unsigned smem_get_board_machtype(void);

typedef enum {
        PHY_INTERFACE_MODE_MII,
        PHY_INTERFACE_MODE_GMII,
        PHY_INTERFACE_MODE_SGMII,
        PHY_INTERFACE_MODE_QSGMII,
        PHY_INTERFACE_MODE_TBI,
        PHY_INTERFACE_MODE_RMII,
        PHY_INTERFACE_MODE_RGMII,
        PHY_INTERFACE_MODE_RGMII_ID,
        PHY_INTERFACE_MODE_RGMII_RXID,
        PHY_INTERFACE_MODE_RGMII_TXID,
        PHY_INTERFACE_MODE_RTBI,
        PHY_INTERFACE_MODE_XGMII,
        PHY_INTERFACE_MODE_NONE /* Must be last */
} phy_interface_t;

typedef struct {
	unsigned int gpio;
	unsigned int func;
	unsigned int dir;
	unsigned int pull;
	unsigned int drvstr;
	unsigned int enable;
} gpio_func_data_t;

typedef struct {
	unsigned int m_value;
	unsigned int n_value;
	unsigned int d_value;
} uart_clk_mnd_t;

/* SPI Mode */

typedef enum {
	NOR_SPI_MODE_0,
	NOR_SPI_MODE_1,
	NOR_SPI_MODE_2,
	NOR_SPI_MODE_3,
} spi_mode;

/* SPI GSBI Bus number */

typedef enum {
	GSBI_BUS_5 = 0,
	GSBI_BUS_6,
	GSBI_BUS_7,
} spi_gsbi_bus_num;

/* SPI Chip selects */

typedef enum {
	SPI_CS_0 ,
	SPI_CS_1,
	SPI_CS_2,
	SPI_CS_3,
} spi_cs;

/* Flash Types */

typedef enum {
	ONLY_NAND,
	ONLY_NOR,
	NAND_NOR,
	NOR_MMC,
} flash_desc;

#define NO_OF_DBG_UART_GPIOS	2

#define SPI_NOR_FLASH_VENDOR_MICRON       0x1
#define SPI_NOR_FLASH_VENDOR_SPANSION     0x2

/* SPI parameters */

typedef struct {
	spi_mode mode;
	spi_gsbi_bus_num bus_number;
	spi_cs chip_select;
	int vendor;
} spinorflash_params_t;

typedef struct {
	unsigned count;
	uint8_t addr[7];
} ipq_gmac_phy_addr_t;

typedef struct {
	unsigned base;
	int unit;
	unsigned is_macsec;
	unsigned mac_pwr0;
	unsigned mac_pwr1;
	unsigned mac_conn_to_phy;
	phy_interface_t phy;
	ipq_gmac_phy_addr_t phy_addr;
} ipq_gmac_board_cfg_t;

#define IPQ_GMAC_NMACS		4

enum storm_board_id {
	BOARD_ID_PROTO_0 = 0,
	BOARD_ID_PROTO_0_2 = 1,
	BOARD_ID_WHIRLWIND = 2,
	BOARD_ID_WHIRLWIND_SP5 = 3,
	BOARD_ID_PROTO_0_2_NAND = 26,
};

/* Board specific parameters */
typedef struct {
#if 0
	unsigned int gmac_gpio_count;
	gpio_func_data_t *gmac_gpio;
	ipq_gmac_board_cfg_t gmac_cfg[IPQ_GMAC_NMACS];
	flash_desc flashdesc;
	spinorflash_params_t flash_param;
#endif
} __packed board_ipq806x_params_t;

extern board_ipq806x_params_t *gboard_param;

#if 0
static inline int gmac_cfg_is_valid(ipq_gmac_board_cfg_t *cfg)
{
	/*
	 * 'cfg' is valid if and only if
	 *	unit number is non-negative and less than IPQ_GMAC_NMACS.
	 *	'cfg' pointer lies within the array range of
	 *		board_ipq806x_params_t->gmac_cfg[]
	 */
	return ((cfg >= &gboard_param->gmac_cfg[0]) &&
		(cfg < &gboard_param->gmac_cfg[IPQ_GMAC_NMACS]) &&
		(cfg->unit >= 0) && (cfg->unit < IPQ_GMAC_NMACS));
}
#endif

unsigned int get_board_index(unsigned machid);
void ipq_configure_gpio(const gpio_func_data_t *gpio, unsigned count);

void board_nand_init(void);

#endif	/* _IPQ806X_CDP_H_ */
