/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <ec/acpi/ec.h>
#include <stdint.h>
#include "ec.h"

#define BIRMAN_EC_CMD	0x666
#define BIRMAN_EC_DATA	0x662

#define EC_GPIO_0_ADDR		0xA0
#define EC0_EVAL_PRSNTn		BIT(2)

#define EC_GPIO_1_ADDR		0xA1
#define   EC1_EVAL_PWREN	BIT(1)

#define EC_GPIO_2_ADDR		0xA2
#define   EC2_EVAL_SLOT_PWREN	BIT(5)
#define   EC2_EVAL_19V_EN	BIT(2)

#define EC_GPIO_3_ADDR		0xA3
#define   EC3_WLAN_RST_AUX	BIT(5)
#define   EC3_WWAN_RST_AUX	BIT(4)
#define   EC3_SD_RST_AUX	BIT(3)
#define   EC3_DT_RST_AUX	BIT(2)
#define   EC3_LOM_RST_AUX	BIT(1)
#define   EC3_EVAL_RST_AUX	BIT(0)

#define EC_GPIO_6_ADDR		0xA6
#define   EC6_TPNL_BUF_EN	BIT(1)
#define   EC6_TPAD_BUF_EN	BIT(0)

#define EC_GPIO_7_ADDR		0xA7
#define   EC7_WWAN_PWR_OFF_N	BIT(7)
#define   EC7_BT_RADIO_DIS	BIT(2)
#define   EC7_WL_RADIO_EN	BIT(0)

#define EC_GPIO_8_ADDR		0xA8
#define   EC8_ADAPTER_OFF	BIT(5)
#define   EC8_EVAL_SMBUS1_N_SW	BIT(3)
#define   EC8_MP2_SEL		BIT(2)

#define EC_GPIO_9_ADDR		0xA9
#define   EC9_CAM0_PWR_EN	BIT(7)
#define   EC9_CAM1_PWR_EN	BIT(6)
#define   EC9_WWAN_RST_N	BIT(5)
#define   EC9_SSD1_PWREN	BIT(2)
#define   EC9_TPM_PWR_EN	BIT(1)
#define   EC9_TPM_S0I3_N	BIT(0)

#define EC_GPIO_A_ADDR		0xAA
#define   ECA_MUX2_S0		BIT(7)
#define   ECA_MUX2_S1		BIT(6)
#define   ECA_MUX1_S0		BIT(5)
#define   ECA_MUX1_S1		BIT(4)
#define   ECA_MUX0_S0		BIT(3)
#define   ECA_MUX0_S1		BIT(2)
#define   ECA_SMBUS1_EN		BIT(1)
#define   ECA_SMBUS0_EN		BIT(0)

#define EC_GPIO_D_ADDR		0xAD
#define   ECD_TPNL_PWR_EN	BIT(7)
#define   ECD_TPNL_EN		BIT(6)
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
#define   ECE_FPR_N_GBE_SEL	BIT(1) //USB5_MUX_FPR#_GBE_SEL
#define   ECE_BT_N_TPNL_SEL	BIT(0) //USB6_MUX_BT#_TPNL_SEL

#define EC_GPIO_F_ADDR		0xAF
#define   ECF_SD_MAIN_PWR_EN	BIT(6)
#define   ECF_SDE_N_WLAN1_SW	BIT(1)
#define   ECF_GBE_N_WWAN1_SW	BIT(0)

#define EC_GPIO_G_ADDR	0xB0
#define   ECG_IR_LED_PWR_EN	BIT(7)
#define   ECG_U0_WLAN_HDR_SEL	BIT(6)
#define   ECG_NFC_BUF_EN	BIT(5)
#define   ECG_WLAN_WWAN_MUX_OFF	BIT(4)

static void configure_ec_gpio(void)
{
	uint8_t tmp;

	tmp = ec_read(EC_GPIO_1_ADDR);
	if (CONFIG(ENABLE_EVAL_CARD)) {
		tmp |= EC1_EVAL_PWREN;
	} else {
		tmp &= ~EC1_EVAL_PWREN;
	}
	printk(BIOS_SPEW, "Write reg [0x%02x] = 0x%02x\n", EC_GPIO_1_ADDR, tmp);
	ec_write(EC_GPIO_1_ADDR, tmp);

	tmp = ec_read(EC_GPIO_2_ADDR);
	if (CONFIG(ENABLE_EVAL_CARD)) {
		tmp |= EC2_EVAL_SLOT_PWREN;
		if (CONFIG(ENABLE_EVAL_19V)) {
			uint8_t eval_present = !(ec_read(EC_GPIO_0_ADDR) & EC0_EVAL_PRSNTn);
			if (eval_present) {
				tmp |= EC2_EVAL_19V_EN;
			}
		} else {
			tmp &= ~EC2_EVAL_19V_EN;
		}
	} else {
		tmp &= ~EC2_EVAL_SLOT_PWREN;
		tmp &= ~EC2_EVAL_19V_EN;
	}
	printk(BIOS_SPEW, "Write reg [0x%02x] = 0x%02x\n", EC_GPIO_2_ADDR, tmp);
	ec_write(EC_GPIO_2_ADDR, tmp);

	tmp = ec_read(EC_GPIO_3_ADDR);
	tmp |= EC3_WLAN_RST_AUX | EC3_WWAN_RST_AUX | EC3_SD_RST_AUX;
	tmp |= EC3_DT_RST_AUX | EC3_LOM_RST_AUX | EC3_EVAL_RST_AUX;
	printk(BIOS_SPEW, "Write reg [0x%02x] = 0x%02x\n", EC_GPIO_3_ADDR, tmp);
	ec_write(EC_GPIO_3_ADDR, tmp);

	tmp = ec_read(EC_GPIO_6_ADDR);
	tmp |= EC6_TPNL_BUF_EN | EC6_TPAD_BUF_EN;
	printk(BIOS_SPEW, "Write reg [0x%02x] = 0x%02x\n", EC_GPIO_6_ADDR, tmp);
	ec_write(EC_GPIO_6_ADDR, tmp);

	tmp = ec_read(EC_GPIO_7_ADDR);
	tmp |= EC7_WWAN_PWR_OFF_N;
	tmp &= ~(EC7_BT_RADIO_DIS | EC7_BT_RADIO_DIS);
	printk(BIOS_SPEW, "Write reg [0x%02x] = 0x%02x\n", EC_GPIO_7_ADDR, tmp);
	ec_write(EC_GPIO_7_ADDR, tmp);

	tmp = ec_read(EC_GPIO_8_ADDR);
	tmp |= EC8_MP2_SEL;
	printk(BIOS_SPEW, "Write reg [0x%02x] = 0x%02x\n", EC_GPIO_8_ADDR, tmp);
	ec_write(EC_GPIO_8_ADDR, tmp);

	tmp = ec_read(EC_GPIO_9_ADDR);
	tmp |= EC9_CAM0_PWR_EN | EC9_CAM1_PWR_EN | EC9_TPM_PWR_EN;

	if (CONFIG(DISABLE_WWAN_GBE_BIRMANPLUS)) {
		tmp &= ~EC9_WWAN_RST_N;
	} else {
		tmp |= EC9_WWAN_RST_N;
	}

	if (CONFIG(ENABLE_SSD1_BIRMANPLUS)) {
		tmp |= EC9_SSD1_PWREN;
	} else {
		tmp &= ~EC9_SSD1_PWREN;
	}

	printk(BIOS_SPEW, "Write reg [0x%02x] = 0x%02x\n", EC_GPIO_9_ADDR, tmp);
	ec_write(EC_GPIO_9_ADDR, tmp);

	tmp = ec_read(EC_GPIO_A_ADDR);
	tmp = ECA_MUX1_S0 | ECA_SMBUS1_EN | ECA_SMBUS0_EN;
	// Touch Panel under I2C0 bus (default)
	tmp &= ~(ECA_MUX0_S1 | ECA_MUX0_S0);
	// Touch Pad Under I2C1 bus (default)
	tmp |= ECA_MUX1_S0;
	tmp &= ~ECA_MUX1_S1;
	// NFC under I2C0 bus (default)
	tmp &= ~(ECA_MUX2_S0 | ECA_MUX2_S1);
	printk(BIOS_SPEW, "Write reg [0x%02x] = 0x%02x\n", EC_GPIO_A_ADDR, tmp);
	ec_write(EC_GPIO_A_ADDR, tmp);

	tmp = ec_read(EC_GPIO_D_ADDR);
	tmp |= ECD_TPNL_PWR_EN | ECD_TPNL_EN | ECD_TPAD_DISABLE_N;
	printk(BIOS_SPEW, "Write reg [0x%02x] = 0x%02x\n", EC_GPIO_D_ADDR, tmp);
	ec_write(EC_GPIO_D_ADDR, tmp);

	tmp = ec_read(EC_GPIO_E_ADDR);
	tmp |= ECE_SSD0_PWR_EN | ECE_CAM_PWR_EN | ECE_FPR_N_GBE_SEL;
	tmp &= ~ECE_BT_N_TPNL_SEL;

	if (CONFIG(ENABLE_SDCARD_BIRMANPLUS)) {
		tmp |= ECE_SD_PWR_EN;
	} else {
		tmp &= ~ECE_SD_PWR_EN;
	}

	if (CONFIG(ENABLE_GBE_BIRMANPLUS)) {
		tmp |= ECE_LOM_PWR_EN;
	} else {
		tmp &= ~ECE_LOM_PWR_EN;
	}

	if (CONFIG(DISABLE_WWAN_GBE_BIRMANPLUS)) {	// no WWAN, turn off WWAN power
		tmp &= ~ECE_WWAN_PWR_EN;
	} else {
		tmp |= ECE_WWAN_PWR_EN;
	}

	if (CONFIG(DISABLE_WLAN_SD_BIRMANPLUS)) {	// no WLAN, turn off WLAN power
		tmp &= ~ECE_WLAN_PWR_EN;
	} else {
		tmp |= ECE_WLAN_PWR_EN;
	}

	printk(BIOS_SPEW, "Write reg [0x%02x] = 0x%02x\n", EC_GPIO_E_ADDR, tmp);
	ec_write(EC_GPIO_E_ADDR, tmp);

	tmp = ec_read(EC_GPIO_F_ADDR);

	if (CONFIG(ENABLE_SDCARD_BIRMANPLUS)) {
		tmp |= ECF_SD_MAIN_PWR_EN;
	} else {
		tmp &= ~ECF_SD_MAIN_PWR_EN;
	}

	if (CONFIG(ENABLE_WLAN02_BIRMANPLUS)) {
		tmp &= ~ECF_GBE_N_WWAN1_SW;  // Enable x2 WLAN, SD disable
	} else {
		tmp |= ECF_GBE_N_WWAN1_SW;  // Default  GBE x1 WWAN x1
	}

	if (CONFIG(ENABLE_WWAN02_BIRMANPLUS)) {
		tmp &= ~ECF_SDE_N_WLAN1_SW;  // Enable x2 WWAN, GBE disable
	} else {
		tmp |= ECF_SDE_N_WLAN1_SW; // Default SDCard x1 WLAN x1
	}

	printk(BIOS_SPEW, "Write reg [0x%02x] = 0x%02x\n", EC_GPIO_F_ADDR, tmp);
	ec_write(EC_GPIO_F_ADDR, tmp);

	tmp = ec_read(EC_GPIO_G_ADDR);
	tmp &= ~ECG_WLAN_WWAN_MUX_OFF; // Keep low to avoid GPP13, GPP14 not work.
	tmp |= ECG_IR_LED_PWR_EN | ECG_U0_WLAN_HDR_SEL | ECG_NFC_BUF_EN;
	printk(BIOS_SPEW, "Write reg [0x%02x] = 0x%02x\n", EC_GPIO_G_ADDR, tmp);
	ec_write(EC_GPIO_G_ADDR, tmp);
}

void birman_ec_init(void)
{
	ec_set_ports(BIRMAN_EC_CMD, BIRMAN_EC_DATA);
	configure_ec_gpio();
}
