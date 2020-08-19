/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _IPQ40XX_CDP_H_
#define _IPQ40XX_CDP_H_

#include <types.h>

unsigned int smem_get_board_machtype(void);

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
	unsigned int count;
	uint8_t addr[7];
} ipq_gmac_phy_addr_t;

typedef struct {
	unsigned int base;
	int unit;
	unsigned int is_macsec;
	unsigned int mac_pwr0;
	unsigned int mac_pwr1;
	unsigned int mac_conn_to_phy;
	phy_interface_t phy;
	ipq_gmac_phy_addr_t phy_addr;
} ipq_gmac_board_cfg_t;

#define IPQ_GMAC_NMACS		4

enum gale_board_id {
	BOARD_ID_GALE_PROTO = 0,
	BOARD_ID_GALE_EVT = 1,
	BOARD_ID_GALE_EVT2_0 = 2,
	BOARD_ID_GALE_EVT2_1 = 6,
	BOARD_ID_GALE_EVT3 = 5,
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
} __packed board_ipq40xx_params_t;

extern board_ipq40xx_params_t *gboard_param;

#if 0
static inline int gmac_cfg_is_valid(ipq_gmac_board_cfg_t *cfg)
{
	/*
	 * 'cfg' is valid if and only if
	 *	unit number is non-negative and less than IPQ_GMAC_NMACS.
	 *	'cfg' pointer lies within the array range of
	 *		board_ipq40xx_params_t->gmac_cfg[]
	 */
	return ((cfg >= &gboard_param->gmac_cfg[0]) &&
		(cfg < &gboard_param->gmac_cfg[IPQ_GMAC_NMACS]) &&
		(cfg->unit >= 0) && (cfg->unit < IPQ_GMAC_NMACS));
}
#endif

unsigned int get_board_index(unsigned int machid);
void ipq_configure_gpio(const gpio_func_data_t *gpio, unsigned int count);

#endif	/* _IPQ40XX_CDP_H_ */
