/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <ec/acpi/ec.h>
#include <stdint.h>
#include <gpio.h>
#include "ec.h"

#define CRATER_EC_CMD	0x666
#define CRATER_EC_DATA	0x662

#define ENABLE_M2_SSD1  1
#define EC_GPIO_0_ADDR		0xA0
#define   EC_EVAL_PWREN	BIT(0)

#define EC_GPIO_2_ADDR		0xA2
#define   EC2_EVAL_SLOT_PWREN	BIT(5)
#define   EC2_EVAL_19V_EN	BIT(2)
#define   EC2_DT_PWREN		BIT(6)

#define EC_GPIO_3_ADDR		0xA3
#define   EC3_WLAN_RST_AUX	BIT(5)
#define   EC3_WWAN_RST_AUX	BIT(4)
#define   EC3_SD_RST_AUX	BIT(3)
#define   EC3_DT_RST		BIT(2)
#define   EC3_LOM_RST_AUX	BIT(1)
#define   EC3_EVAL_RST_AUX	BIT(0)
#define   EC3_TBT_RST		BIT(7)

#define EC_GPIO_4_ADDR      0xA4
#define   EC4_TBT_PWREN		BIT(0)

#define EC_GPIO_7_ADDR		0xA7
#define   EC7_SSD_HDD_SW	BIT(5)
#define   EC7_ODD_SSD_SW	BIT(4)
#define   EC7_CAM1_EN		BIT(0)

#define EC_GPIO_8_ADDR		0xA8
#define     EC8_ODD_PWR_EN	BIT(0)
#define     EC8_HDD_PWR_EN	BIT(1)
#define     EC8_M2SSD_PWREN	BIT(5)
#define     EC8_WL_RADIO	BIT(6)
#define     EC8_BT_RADIO	BIT(7)

#define EC_GPIO_9_ADDR		0xA9
#define    EC9_MUX2_S1             BIT(7)
#define    EC9_MUX2_S0             BIT(6)
#define    EC9_MUX1_S1             BIT(5)
#define    EC9_MUX1_S0             BIT(4)
#define    EC9_MUX0_S1             BIT(3)
#define    EC9_MUX0_S0             BIT(2)
#define    EC9_SMBUS0_DEV_MUX_SW   BIT(1)
#define    EC9_SMBUS1_DEV_MUX_SW   BIT(0)

#define EC_GPIO_A_ADDR		0xAA
#define   ECA_MUX2_S0		BIT(7)
#define   ECA_MUX2_S1		BIT(6)
#define   ECA_MUX1_S0		BIT(5)
#define   ECA_MUX1_S1		BIT(4)
#define   ECA_MUX0_S0		BIT(3)
#define   ECA_MUX0_S1		BIT(2)
#define   ECA_SMBUS1_EN		BIT(1)
#define   ECA_SMBUS0_EN		BIT(0)

#define EC_GPIO_B_ADDR		0xAB
#define   ECB_TPNL_EN		BIT(2)
#define   ECB_TPNL_PWR_EN	BIT(0)

#define EC_GPIO_C_ADDR		0xAC
#define   ECC_TPNL_BUF_EN	BIT(6)
#define   ECC_TPAD_BUF_EN	BIT(5)
#define   ECC_NFC_BUF_EN	BIT(4)

#define EC_GPIO_D_ADDR		0xAD
#define   ECD_TPNL_PWR_EN	BIT(7)
#define   ECD_TPNL_EN		BIT(6)
#define   ECD_SSD1_PWR_EN	BIT(5)
#define   ECD_FPR_PWR_EN	BIT(3)
#define   ECD_FPR_OFF_N		BIT(2)
#define   ECD_FPR_LOCK_N	BIT(1)
#define   ECD_TPAD_DISABLE_N	BIT(0)

#define EC_GPIO_E_ADDR		0xAE
#define   ECE_LOM_PWR_EN	BIT(7)
#define   ECE_SSD0_PWR_EN	BIT(6)
#define   ECE_SD_PWR_EN		BIT(5)
#define   ECE_WLAN_PWR_EN	BIT(4)
#define   ECE_WWAN_PWR_EN	BIT(3)
#define   ECE_CAM_PWR_EN	BIT(2)
#define   ECE_FPR_N_GBE_SEL	BIT(1)
#define   ECE_BT_N_TPNL_SEL	BIT(0)

#define EC_GPIO_F_ADDR		0xAF
#define   ECF_CAM_FW_WP_N	BIT(7)
#define   ECF_I2C_MUX_OE_N	BIT(4)
#define   ECF_WLAN0_N_WWAN1_SW	BIT(1)
#define   ECF_WWAN0_N_WLAN1_SW	BIT(0)

#define EC_GPIO_G_ADDR	0xB0
#define   ECG_IR_LED_PWR_EN	BIT(7)
#define   ECG_U0_WLAN_HDR_SEL	BIT(6)
#define   ECG_DT_SSD1_MUX_OFF	BIT(5)
#define   ECG_WLAN_WWAN_MUX_OFF	BIT(4)

#define ECRAM_BOARDID_OFFSET 0x93
#define CRATER_REVB    0x42

static void configure_ec_gpio(void)
{
	uint8_t tmp;

	tmp = ec_read(EC_GPIO_2_ADDR);
	printk(BIOS_SPEW, "A2: Write reg [0x%02x] = 0x%02x\n", EC_GPIO_2_ADDR, tmp);
	if (CONFIG(ENABLE_EVAL_CARD)) {
		tmp |= EC2_EVAL_SLOT_PWREN;
		if (CONFIG(ENABLE_EVAL_19V)) {
			tmp |= EC2_EVAL_19V_EN;
		} else {
			tmp &= ~EC2_EVAL_19V_EN;
		}
	} else {
		tmp &= ~EC2_EVAL_SLOT_PWREN;
		tmp &= ~EC2_EVAL_19V_EN;
	}
	printk(BIOS_SPEW, "Write reg [0x%02x] = 0x%02x\n", EC_GPIO_2_ADDR, tmp);
	ec_write(EC_GPIO_2_ADDR, tmp);

	tmp = ec_read(EC_GPIO_7_ADDR);
	printk(BIOS_SPEW, "A7: Write reg [0x%02x] = 0x%02x\n", EC_GPIO_7_ADDR, tmp);
	if (CONFIG(ENABLE_M2_SSD1)) {
		tmp |= (EC7_ODD_SSD_SW | EC7_SSD_HDD_SW);
	} else {
		tmp &= ~(EC7_ODD_SSD_SW | EC7_SSD_HDD_SW);
	}
	printk(BIOS_SPEW, "Write reg [0x%02x] = 0x%02x\n", EC_GPIO_7_ADDR, tmp);
	ec_write(EC_GPIO_7_ADDR, tmp);

	tmp = ec_read(EC_GPIO_8_ADDR);
	printk(BIOS_SPEW, "A8: Write reg [0x%02x] = 0x%02x\n", EC_GPIO_8_ADDR, tmp);
	if (CONFIG(ENABLE_M2_SSD1)) {
		tmp |= EC8_M2SSD_PWREN;
	} else {
		tmp &= ~EC8_M2SSD_PWREN;
	}
	printk(BIOS_SPEW, "Write reg [0x%02x] = 0x%02x\n", EC_GPIO_8_ADDR, tmp);
	ec_write(EC_GPIO_8_ADDR, tmp);

	/* eDP Touch panel  */
	if(CONFIG(ENABLE_EDP)) {
		tmp = ec_read(EC_GPIO_9_ADDR);
		tmp &= ~(EC9_MUX0_S1);
		tmp |= EC9_MUX0_S0;
		printk(BIOS_SPEW, "Write reg [0x%02x] = 0x%02x\n", EC_GPIO_9_ADDR, tmp);
		ec_write(EC_GPIO_9_ADDR, tmp);

		tmp  = ec_read(EC_GPIO_B_ADDR);
		tmp |= ECB_TPNL_EN | ECB_TPNL_EN;
		printk(BIOS_SPEW, "Write reg [0x%02x] = 0x%02x\n", EC_GPIO_B_ADDR, tmp);
		ec_write(EC_GPIO_B_ADDR, tmp);
	}
}

static const struct soc_amd_gpio RevA_gpio_set_stage_ram[] = {
	/* PCIE x8 SLOT*/
	PAD_GPO(GPIO_4, HIGH),
};

static const struct soc_amd_gpio RevB_gpio_set_stage_ram[] = {
	PAD_GPI(GPIO_4, PULL_UP),
	PAD_GPO(GPIO_12, LOW),
};

void crater_boardrevision(void)
{
	uint8_t BoardRev;
	BoardRev = ec_read(ECRAM_BOARDID_OFFSET + 0x3);

	if (BoardRev == CRATER_REVB)
		gpio_configure_pads(RevB_gpio_set_stage_ram, ARRAY_SIZE(RevB_gpio_set_stage_ram));
	else
		gpio_configure_pads(RevA_gpio_set_stage_ram, ARRAY_SIZE(RevA_gpio_set_stage_ram));
}

void crater_ec_init(void)
{
	ec_set_ports(CRATER_EC_CMD, CRATER_EC_DATA);
	crater_boardrevision();
	configure_ec_gpio();
}
