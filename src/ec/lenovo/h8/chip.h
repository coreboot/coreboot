/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_LENOVO_H8EC_CHIP_H
#define EC_LENOVO_H8EC_CHIP_H

struct ec_lenovo_h8_config {
	u8 config0;
	u8 config1;
	u8 config2;
	u8 config3;

	u8 beepmask0;
	u8 beepmask1;

	u8 event0_enable;
	u8 event1_enable;
	u8 event2_enable;
	u8 event3_enable;
	u8 event4_enable;
	u8 event5_enable;
	u8 event6_enable;
	u8 event7_enable;
	u8 event8_enable;
	u8 event9_enable;
	u8 eventa_enable;
	u8 eventb_enable;
	u8 eventc_enable;
	u8 eventd_enable;
	u8 evente_enable;
	u8 eventf_enable;

	u8 has_keyboard_backlight;
	u8 has_power_management_beeps;
	u8 has_uwb;

	u8 bdc_gpio_num;
	u8 bdc_gpio_lvl;

	u8 wwan_gpio_num;
	u8 wwan_gpio_lvl;
};

#endif /* EC_LENOVO_H8EC_CHIP_H */
