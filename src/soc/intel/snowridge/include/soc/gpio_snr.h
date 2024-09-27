/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_SNOWRIDGE_GPIO_SNR_H_
#define _SOC_SNOWRIDGE_GPIO_SNR_H_

#include <intelblocks/gpio.h>

enum snr_pad_host_sw_owner_t { GPIO_HOSTSW_OWN_DEFAULT, GPIO_HOSTSW_OWN_ACPI, GPIO_HOSTSW_OWN_DRIVER };

struct snr_pad_config {
	struct pad_config cfg;
	uint32_t pad_config_mask[GPIO_NUM_PAD_CFG_REGS];
	enum snr_pad_host_sw_owner_t hostsw_own;
};

/**
 * @note Intel common block uses the fourth bits in DW1 to indicate GPIO host software ownership.
 */
#define SNR_PAD_CFG_STRUCT1(__pad, __config0, __mask0, __config1, __mask1, __hostsw_own) \
	{                                                                                \
		.cfg = _PAD_CFG_STRUCT(__pad, __config0, __config1),                     \
		.pad_config_mask[0] = __mask0, .pad_config_mask[1] = __mask1 | BIT(4), \
		.hostsw_own = __hostsw_own,                                              \
	}

#define SNR_PAD_CFG_STRUCT0(__pad, __config0, __mask0, __hostsw_own) \
	SNR_PAD_CFG_STRUCT1(__pad, __config0, __mask0, 0, 0, __hostsw_own)

/**
 * @brief GPIO pad format:
 * 31   24                16            8            0
 * ---------------------------------------------------
 * |    | Community Index | Group Index | Pad Number |
 * ---------------------------------------------------
 */

#define SNR_GPIO_DEF(COMMUNITY, PAD) ((uint32_t)(((COMMUNITY & 0xff) << 16) | (PAD & 0xff)))
#define SNR_GPIO_COMMUNITY(GPIO_PAD) ((GPIO_PAD >> 16) & 0xff)
#define SNR_GPIO_PAD(GPIO_PAD)       (GPIO_PAD & 0xff)

enum {
	GPIO_COMM_WEST2 = 0,
	GPIO_COMM_WEST3,
	GPIO_COMM_WEST01,
	GPIO_COMM_WEST5,
	GPIO_COMM_WESTB,
	GPIO_COMM_WESTD_PECI,
	GPIO_COMM_EAST2,
	GPIO_COMM_EAST3,
	GPIO_COMM_EAST0,
	GPIO_COMM_EMMC
};

#define GPIO_WEST2_0      SNR_GPIO_DEF(GPIO_COMM_WEST2, 0x0)      /**< GBE_SDP_TIMESYNC0. */
#define GPIO_WEST2_1      SNR_GPIO_DEF(GPIO_COMM_WEST2, 0x1)      /**< GBE_SDP_TIMESYNC1. */
#define GPIO_WEST2_2      SNR_GPIO_DEF(GPIO_COMM_WEST2, 0x2)      /**< GBE_SDP_TIMESYNC2. */
#define GPIO_WEST2_3      SNR_GPIO_DEF(GPIO_COMM_WEST2, 0x3)      /**< GBE_SDP_TIMESYNC3. */
#define GPIO_WEST2_4      SNR_GPIO_DEF(GPIO_COMM_WEST2, 0x4)      /**< GBE0_I2C_CLK. */
#define GPIO_WEST2_5      SNR_GPIO_DEF(GPIO_COMM_WEST2, 0x5)      /**< GBE0_I2C_DATA. */
#define GPIO_WEST2_6      SNR_GPIO_DEF(GPIO_COMM_WEST2, 0x6)      /**< GBE1_I2C_CLK. */
#define GPIO_WEST2_7      SNR_GPIO_DEF(GPIO_COMM_WEST2, 0x7)      /**< GBE1_I2C_DATA. */
#define GPIO_WEST2_8      SNR_GPIO_DEF(GPIO_COMM_WEST2, 0x8)      /**< GBE2_I2C_CLK. */
#define GPIO_WEST2_9      SNR_GPIO_DEF(GPIO_COMM_WEST2, 0x9)      /**< GBE2_I2C_DATA. */
#define GPIO_WEST2_10     SNR_GPIO_DEF(GPIO_COMM_WEST2, 0xa)      /**< GBE3_I2C_CLK. */
#define GPIO_WEST2_11     SNR_GPIO_DEF(GPIO_COMM_WEST2, 0xb)      /**< GBE3_I2C_DATA. */
#define GPIO_WEST2_12     SNR_GPIO_DEF(GPIO_COMM_WEST2, 0xc)      /**< GBE0_LED0. */
#define GPIO_WEST2_13     SNR_GPIO_DEF(GPIO_COMM_WEST2, 0xd)      /**< GBE0_LED1. */
#define GPIO_WEST2_14     SNR_GPIO_DEF(GPIO_COMM_WEST2, 0xe)      /**< GBE0_LED2. */
#define GPIO_WEST2_15     SNR_GPIO_DEF(GPIO_COMM_WEST2, 0xf)      /**< GBE1_LED0. */
#define GPIO_WEST2_16     SNR_GPIO_DEF(GPIO_COMM_WEST2, 0x10)     /**< GBE1_LED1. */
#define GPIO_WEST2_17     SNR_GPIO_DEF(GPIO_COMM_WEST2, 0x11)     /**< GBE1_LED2. */
#define GPIO_WEST2_18     SNR_GPIO_DEF(GPIO_COMM_WEST2, 0x12)     /**< GBE2_LED0. */
#define GPIO_WEST2_19     SNR_GPIO_DEF(GPIO_COMM_WEST2, 0x13)     /**< GBE2_LED1. */
#define GPIO_WEST2_20     SNR_GPIO_DEF(GPIO_COMM_WEST2, 0x14)     /**< GBE2_LED2. */
#define GPIO_WEST2_21     SNR_GPIO_DEF(GPIO_COMM_WEST2, 0x15)     /**< GBE3_LED0. */
#define GPIO_WEST2_22     SNR_GPIO_DEF(GPIO_COMM_WEST2, 0x16)     /**< GBE3_LED1. */
#define GPIO_WEST2_23     SNR_GPIO_DEF(GPIO_COMM_WEST2, 0x17)     /**< GBE3_LED2. */
#define GPIO_WEST3_0      SNR_GPIO_DEF(GPIO_COMM_WEST3, 0x0)      /**< NCSI_RXD0. */
#define GPIO_WEST3_1      SNR_GPIO_DEF(GPIO_COMM_WEST3, 0x1)      /**< NCSI_CLK_IN. */
#define GPIO_WEST3_2      SNR_GPIO_DEF(GPIO_COMM_WEST3, 0x2)      /**< NCSI_RXD1. */
#define GPIO_WEST3_3      SNR_GPIO_DEF(GPIO_COMM_WEST3, 0x3)      /**< NCSI_CRS_DV. */
#define GPIO_WEST3_4      SNR_GPIO_DEF(GPIO_COMM_WEST3, 0x4)      /**< NCSI_ARB_IN. */
#define GPIO_WEST3_5      SNR_GPIO_DEF(GPIO_COMM_WEST3, 0x5)      /**< NCSI_TX_EN. */
#define GPIO_WEST3_6      SNR_GPIO_DEF(GPIO_COMM_WEST3, 0x6)      /**< NCSI_TXD0. */
#define GPIO_WEST3_7      SNR_GPIO_DEF(GPIO_COMM_WEST3, 0x7)      /**< NCSI_TXD1. */
#define GPIO_WEST3_8      SNR_GPIO_DEF(GPIO_COMM_WEST3, 0x8)      /**< NCSI_ARB_OUT. */
#define GPIO_WEST3_9      SNR_GPIO_DEF(GPIO_COMM_WEST3, 0x9)      /**< GBE_SMB_CLK. */
#define GPIO_WEST3_10     SNR_GPIO_DEF(GPIO_COMM_WEST3, 0xa)      /**< GBE_SMB_DATA. */
#define GPIO_WEST3_11     SNR_GPIO_DEF(GPIO_COMM_WEST3, 0xb)      /**< GBE_SMB_ALRT_N. */
#define GPIO_WEST3_20     SNR_GPIO_DEF(GPIO_COMM_WEST3, 0x14)     /**< UART0_RXD. */
#define GPIO_WEST3_21     SNR_GPIO_DEF(GPIO_COMM_WEST3, 0x15)     /**< UART0_TXD. */
#define GPIO_WEST3_22     SNR_GPIO_DEF(GPIO_COMM_WEST3, 0x16)     /**< UART1_RXD. */
#define GPIO_WEST3_23     SNR_GPIO_DEF(GPIO_COMM_WEST3, 0x17)     /**< UART1_TXD. */
#define GPIO_WEST01_0     SNR_GPIO_DEF(GPIO_COMM_WEST01, 0x0)     /**< CPU_GP_0. */
#define GPIO_WEST01_1     SNR_GPIO_DEF(GPIO_COMM_WEST01, 0x1)     /**< CPU_GP_1. */
#define GPIO_WEST01_2     SNR_GPIO_DEF(GPIO_COMM_WEST01, 0x2)     /**< CPU_GP_2. */
#define GPIO_WEST01_3     SNR_GPIO_DEF(GPIO_COMM_WEST01, 0x3)     /**< CPU_GP_3. */
#define GPIO_WEST01_4     SNR_GPIO_DEF(GPIO_COMM_WEST01, 0x4)     /**< FAN_PWM_0. */
#define GPIO_WEST01_5     SNR_GPIO_DEF(GPIO_COMM_WEST01, 0x5)     /**< FAN_PWM_1. */
#define GPIO_WEST01_6     SNR_GPIO_DEF(GPIO_COMM_WEST01, 0x6)     /**< FAN_PWM_2. */
#define GPIO_WEST01_7     SNR_GPIO_DEF(GPIO_COMM_WEST01, 0x7)     /**< FAN_PWM_3. */
#define GPIO_WEST01_8     SNR_GPIO_DEF(GPIO_COMM_WEST01, 0x8)     /**< FAN_TACH_0. */
#define GPIO_WEST01_9     SNR_GPIO_DEF(GPIO_COMM_WEST01, 0x9)     /**< FAN_TACH_1. */
#define GPIO_WEST01_10    SNR_GPIO_DEF(GPIO_COMM_WEST01, 0xa)     /**< FAN_TACH_2. */
#define GPIO_WEST01_11    SNR_GPIO_DEF(GPIO_COMM_WEST01, 0xb)     /**< FAN_TACH_3. */
#define GPIO_WEST01_12    SNR_GPIO_DEF(GPIO_COMM_WEST01, 0xc)     /**< ME_SMB0_CLK. */
#define GPIO_WEST01_13    SNR_GPIO_DEF(GPIO_COMM_WEST01, 0xd)     /**< ME_SMB0_DATA. */
#define GPIO_WEST01_14    SNR_GPIO_DEF(GPIO_COMM_WEST01, 0xe)     /**< ME_SMB0_ALRT_N. */
#define GPIO_WEST01_15    SNR_GPIO_DEF(GPIO_COMM_WEST01, 0xf)     /**< ME_SMB1_CLK. */
#define GPIO_WEST01_16    SNR_GPIO_DEF(GPIO_COMM_WEST01, 0x10)    /**< ME_SMB1_DATA. */
#define GPIO_WEST01_17    SNR_GPIO_DEF(GPIO_COMM_WEST01, 0x11)    /**< ME_SMB1_ALRT_N. */
#define GPIO_WEST01_18    SNR_GPIO_DEF(GPIO_COMM_WEST01, 0x12)    /**< ME_SMB2_CLK. */
#define GPIO_WEST01_19    SNR_GPIO_DEF(GPIO_COMM_WEST01, 0x13)    /**< ME_SMB2_DATA. */
#define GPIO_WEST01_20    SNR_GPIO_DEF(GPIO_COMM_WEST01, 0x14)    /**< ME_SMB2_ALRT_N. */
#define GPIO_WEST01_21    SNR_GPIO_DEF(GPIO_COMM_WEST01, 0x15)    /**< GBE_MNG_I2C_CLK. */
#define GPIO_WEST01_22    SNR_GPIO_DEF(GPIO_COMM_WEST01, 0x16)    /**< GBE_MNG_I2C_DATA. */
#define GPIO_WEST5_0      SNR_GPIO_DEF(GPIO_COMM_WEST5, 0x0)      /**< IE_UART_RXD. */
#define GPIO_WEST5_1      SNR_GPIO_DEF(GPIO_COMM_WEST5, 0x1)      /**< IE_UART_TXD. */
#define GPIO_WEST5_2      SNR_GPIO_DEF(GPIO_COMM_WEST5, 0x2)      /**< VPP_SMB_CLK. */
#define GPIO_WEST5_3      SNR_GPIO_DEF(GPIO_COMM_WEST5, 0x3)      /**< VPP_SMB_DATA. */
#define GPIO_WEST5_4      SNR_GPIO_DEF(GPIO_COMM_WEST5, 0x4)      /**< VPP_SMB_ALRT_N. */
#define GPIO_WEST5_5      SNR_GPIO_DEF(GPIO_COMM_WEST5, 0x5)      /**< PCIE_CLKREQ0_N. */
#define GPIO_WEST5_6      SNR_GPIO_DEF(GPIO_COMM_WEST5, 0x6)      /**< PCIE_CLKREQ1_N. */
#define GPIO_WEST5_7      SNR_GPIO_DEF(GPIO_COMM_WEST5, 0x7)      /**< PCIE_CLKREQ2_N. */
#define GPIO_WEST5_8      SNR_GPIO_DEF(GPIO_COMM_WEST5, 0x8)      /**< PCIE_CLKREQ3_N. */
#define GPIO_WEST5_15     SNR_GPIO_DEF(GPIO_COMM_WEST5, 0xf)      /**< FLEX_CLK_SE0. */
#define GPIO_WEST5_16     SNR_GPIO_DEF(GPIO_COMM_WEST5, 0x10)     /**< FLEX_CLK_SE1. */
#define GPIO_WEST5_17     SNR_GPIO_DEF(GPIO_COMM_WEST5, 0x11)     /**< FLEX_CLK1_50. */
#define GPIO_WEST5_18     SNR_GPIO_DEF(GPIO_COMM_WEST5, 0x12)     /**< FLEX_CLK2_50. */
#define GPIO_WESTB_0      SNR_GPIO_DEF(GPIO_COMM_WESTB, 0x0)      /**< Reserved. */
#define GPIO_WESTB_8      SNR_GPIO_DEF(GPIO_COMM_WESTB, 0x8)      /**< DBG_SPARE0. */
#define GPIO_WESTB_9      SNR_GPIO_DEF(GPIO_COMM_WESTB, 0x9)      /**< DBG_SPARE1. */
#define GPIO_WESTB_10     SNR_GPIO_DEF(GPIO_COMM_WESTB, 0xa)      /**< DBG_SPARE2. */
#define GPIO_WESTB_11     SNR_GPIO_DEF(GPIO_COMM_WESTB, 0xb)      /**< DBG_SPARE3. */
#define GPIO_WESTD_PECI_0 SNR_GPIO_DEF(GPIO_COMM_WESTD_PECI, 0x0) /**< PECI_PCH. For P5900. */
#define GPIO_EAST2_0      SNR_GPIO_DEF(GPIO_COMM_EAST2, 0x0)      /**< USB_OC0_N. */
#define GPIO_EAST2_1      SNR_GPIO_DEF(GPIO_COMM_EAST2, 0x1)      /**< GPIO_0. */
#define GPIO_EAST2_2      SNR_GPIO_DEF(GPIO_COMM_EAST2, 0x2)      /**< GPIO_1. */
#define GPIO_EAST2_3      SNR_GPIO_DEF(GPIO_COMM_EAST2, 0x3)      /**< GPIO_2. */
#define GPIO_EAST2_4      SNR_GPIO_DEF(GPIO_COMM_EAST2, 0x4)      /**< GPIO_3. */
#define GPIO_EAST2_5      SNR_GPIO_DEF(GPIO_COMM_EAST2, 0x5)      /**< GPIO_4. */
#define GPIO_EAST2_6      SNR_GPIO_DEF(GPIO_COMM_EAST2, 0x6)      /**< GPIO_5. */
#define GPIO_EAST2_7      SNR_GPIO_DEF(GPIO_COMM_EAST2, 0x7)      /**< GPIO_6. */
#define GPIO_EAST2_8      SNR_GPIO_DEF(GPIO_COMM_EAST2, 0x8)      /**< GPIO_7. */
#define GPIO_EAST2_9      SNR_GPIO_DEF(GPIO_COMM_EAST2, 0x9)      /**< GPIO_8. */
#define GPIO_EAST2_10     SNR_GPIO_DEF(GPIO_COMM_EAST2, 0xa)      /**< GPIO_9. */
#define GPIO_EAST2_11     SNR_GPIO_DEF(GPIO_COMM_EAST2, 0xb)      /**< GPIO_10. */
#define GPIO_EAST2_12     SNR_GPIO_DEF(GPIO_COMM_EAST2, 0xc)      /**< GPIO_11. */
#define GPIO_EAST2_13     SNR_GPIO_DEF(GPIO_COMM_EAST2, 0xd)      /**< GPIO_12. */
#define GPIO_EAST2_14     SNR_GPIO_DEF(GPIO_COMM_EAST2, 0xe)      /**< PECI_SMB_DATA. */
#define GPIO_EAST2_15     SNR_GPIO_DEF(GPIO_COMM_EAST2, 0xf)      /**< SATA0_LED_N. */
#define GPIO_EAST2_17     SNR_GPIO_DEF(GPIO_COMM_EAST2, 0x11)     /**< SATA_PDETECT0. */
#define GPIO_EAST2_18     SNR_GPIO_DEF(GPIO_COMM_EAST2, 0x12)     /**< SATA_PDETECT1. */
#define GPIO_EAST2_19     SNR_GPIO_DEF(GPIO_COMM_EAST2, 0x13)     /**< SATA0_SDOUT. */
#define GPIO_EAST2_20     SNR_GPIO_DEF(GPIO_COMM_EAST2, 0x14)     /**< SATA1_SDOUT. */
#define GPIO_EAST2_21     SNR_GPIO_DEF(GPIO_COMM_EAST2, 0x15)     /**< SATA2_LED_N. */
#define GPIO_EAST2_22     SNR_GPIO_DEF(GPIO_COMM_EAST2, 0x16)     /**< SATA_PDETECT2. */
#define GPIO_EAST2_23     SNR_GPIO_DEF(GPIO_COMM_EAST2, 0x17)     /**< SATA2_SDOUT. */
#define GPIO_EAST3_0      SNR_GPIO_DEF(GPIO_COMM_EAST3, 0x0)      /**< ESPI_IO0. For P5900. */
#define GPIO_EAST3_1      SNR_GPIO_DEF(GPIO_COMM_EAST3, 0x1)      /**< ESPI_IO1. */
#define GPIO_EAST3_2      SNR_GPIO_DEF(GPIO_COMM_EAST3, 0x2)      /**< ESPI_IO2. */
#define GPIO_EAST3_3      SNR_GPIO_DEF(GPIO_COMM_EAST3, 0x3)      /**< ESPI_IO3. */
#define GPIO_EAST3_4      SNR_GPIO_DEF(GPIO_COMM_EAST3, 0x4)      /**< ESPI_CLK. */
#define GPIO_EAST3_5      SNR_GPIO_DEF(GPIO_COMM_EAST3, 0x5)      /**< ESPI_RST_N. */
#define GPIO_EAST3_6      SNR_GPIO_DEF(GPIO_COMM_EAST3, 0x6)      /**< ESPI_CS0_N. */
#define GPIO_EAST3_7      SNR_GPIO_DEF(GPIO_COMM_EAST3, 0x7)      /**< ESPI_ALRT0_N. */
#define GPIO_EAST3_8      SNR_GPIO_DEF(GPIO_COMM_EAST3, 0x8)      /**< ESPI_CS1_N. */
#define GPIO_EAST3_9      SNR_GPIO_DEF(GPIO_COMM_EAST3, 0x9)      /**< ESPI_ALRT1_N. */
#define GPIO_EAST0_0      SNR_GPIO_DEF(GPIO_COMM_EAST0, 0x0)      /**< Reserved. */
#define GPIO_EAST0_10     SNR_GPIO_DEF(GPIO_COMM_EAST0, 0xa)      /**< ADR_COMPLETE. */
#define GPIO_EAST0_11     SNR_GPIO_DEF(GPIO_COMM_EAST0, 0xb)      /**< ADR_TRIGGER_N. */
#define GPIO_EAST0_13     SNR_GPIO_DEF(GPIO_COMM_EAST0, 0xd)      /**< PMU_SLP_S3_N. */
#define GPIO_EAST0_18     SNR_GPIO_DEF(GPIO_COMM_EAST0, 0x12) /**< SUS_STAT_N. Not documented. */
#define GPIO_EAST0_19     SNR_GPIO_DEF(GPIO_COMM_EAST0, 0x13) /**< PMU_I2C_CLK. */
#define GPIO_EAST0_20     SNR_GPIO_DEF(GPIO_COMM_EAST0, 0x14) /**< PMU_I2C_DATA. */
#define GPIO_EAST0_21     SNR_GPIO_DEF(GPIO_COMM_EAST0, 0x15) /**< PECI_SMB_CLK. */
#define GPIO_EAST0_22     SNR_GPIO_DEF(GPIO_COMM_EAST0, 0x16) /**< PECI_SMB_ALRT_N. */
#define GPIO_EMMC_0       SNR_GPIO_DEF(GPIO_COMM_EMMC, 0x0)   /**< EMMC_CMD. */
#define GPIO_EMMC_1       SNR_GPIO_DEF(GPIO_COMM_EMMC, 0x1)   /**< EMMC_STROBE. */
#define GPIO_EMMC_2       SNR_GPIO_DEF(GPIO_COMM_EMMC, 0x2)   /**< EMMC_CLK. */
#define GPIO_EMMC_3       SNR_GPIO_DEF(GPIO_COMM_EMMC, 0x3)   /**< EMMC_D0. */
#define GPIO_EMMC_4       SNR_GPIO_DEF(GPIO_COMM_EMMC, 0x4)   /**< EMMC_D1. */
#define GPIO_EMMC_5       SNR_GPIO_DEF(GPIO_COMM_EMMC, 0x5)   /**< EMMC_D2. */
#define GPIO_EMMC_6       SNR_GPIO_DEF(GPIO_COMM_EMMC, 0x6)   /**< EMMC_D3. */
#define GPIO_EMMC_7       SNR_GPIO_DEF(GPIO_COMM_EMMC, 0x7)   /**< EMMC_D4. */
#define GPIO_EMMC_8       SNR_GPIO_DEF(GPIO_COMM_EMMC, 0x8)   /**< EMMC_D5. */
#define GPIO_EMMC_9       SNR_GPIO_DEF(GPIO_COMM_EMMC, 0x9)   /**< EMMC_D6. */
#define GPIO_EMMC_10      SNR_GPIO_DEF(GPIO_COMM_EMMC, 0xa)   /**< EMMC_D7. */

void gpio_configure_snr_pads(struct snr_pad_config *gpio, size_t num);

#endif // _SOC_SNOWRIDGE_GPIO_SNR_H_
