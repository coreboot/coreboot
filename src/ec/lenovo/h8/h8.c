/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <ec/acpi/ec.h>
#include <kconfig.h>
#include <stdlib.h>
#include <string.h>
#include <smbios.h>
#include <pc80/mc146818rtc.h>
#include <pc80/keyboard.h>

#include "h8.h"
#include "chip.h"

static void h8_bluetooth_enable(int on)
{
	if (on)
		ec_set_bit(0x3a, 4);
	else
		ec_clr_bit(0x3a, 4);
}

void h8_trackpoint_enable(int on)
{
	ec_write(H8_TRACKPOINT_CTRL,
		 on ? H8_TRACKPOINT_ON : H8_TRACKPOINT_OFF);

}

/* Controls radio-off pin in WLAN MiniPCIe slot.  */
void h8_wlan_enable(int on)
{
	if (on)
		ec_set_bit(0x3a, 5);
	else
		ec_clr_bit(0x3a, 5);
}

/* Controls radio-off pin in WWAN MiniPCIe slot.  */
static void h8_wwan_enable(int on)
{
	if (on)
		ec_set_bit(0x3a, 6);
	else
		ec_clr_bit(0x3a, 6);
}

/* Controls radio-off pin in UWB MiniPCIe slot.  */
static void h8_uwb_enable(int on)
{
	if (on)
		ec_set_bit(0x31, 2);
	else
		ec_clr_bit(0x31, 2);
}

static void h8_fn_ctrl_swap(int on)
{
	if (on)
		ec_set_bit(0xce, 4);
	else
		ec_clr_bit(0xce, 4);
}

static void h8_sticky_fn(int on)
{
	if (on)
		ec_set_bit(0x0, 3);
	else
		ec_clr_bit(0x0, 3);
}

static void h8_log_ec_version(void)
{
	char ecfw[17];
	u8 len;
	u16 fwvh, fwvl;

	len = h8_build_id_and_function_spec_version(ecfw, sizeof ecfw - 1);
	ecfw[len] = 0;

	fwvh = ec_read(0xe9);
	fwvl = ec_read(0xe8);

	printk(BIOS_INFO, "EC Firmware ID %s, Version %d.%d%d%c\n", ecfw,
	       fwvh >> 4, fwvh & 0x0f, fwvl >> 4, 0x41 + (fwvl & 0xf));
}

void h8_set_audio_mute(int mute)
{
	if (mute)
		ec_set_bit(0x3a, 0);
	else
		ec_clr_bit(0x3a, 0);
}

void h8_enable_event(int event)
{
	if (event < 0 || event > 127)
		return;

	ec_set_bit(0x10 + (event >> 3), event & 7);
}

void h8_disable_event(int event)
{
	if (event < 0 || event > 127)
		return;

	ec_clr_bit(0x10 + (event >> 3), event & 7);

}

void h8_usb_power_enable(int onoff)
{
	if (onoff)
		ec_set_bit(0x3b, 4);
	else
		ec_clr_bit(0x3b, 4);
}

int h8_ultrabay_device_present(void)
{
	return ec_read(H8_STATUS1) & 0x5 ? 0 : 1;
}

u8 h8_build_id_and_function_spec_version(char *buf, u8 buf_len)
{
	u8 i, c;
	char str[16 + 1]; /* 16 ASCII chars + \0 */

	/* Build ID */
	for (i = 0; i < 8; i++) {
		c = ec_read(0xf0 + i);
		if (c < 0x20 || c > 0x7f) {
		  i = snprintf(str, sizeof (str), "*INVALID");
			break;
		}
		str[i] = c;
	}

	/* EC firmware function specification version */
	i += snprintf(str + i, sizeof (str) - i, "-%u.%u", ec_read(0xef), ec_read(0xeb));

	i = MIN(buf_len, i);
	memcpy(buf, str, i);

	return i;
}

static void h8_smbios_strings(struct device *dev, struct smbios_type11 *t)
{
	char tpec[] = "IBM ThinkPad Embedded Controller -[                 ]-";

	h8_build_id_and_function_spec_version(tpec + 35, 17);

	t->count = smbios_add_string(t->eos, tpec);
}

static void h8_init(device_t dev)
{
	pc_keyboard_init();
}

struct device_operations h8_dev_ops = {
	.get_smbios_strings = h8_smbios_strings,
	.init = h8_init,
};

static void h8_enable(struct device *dev)
{
	struct ec_lenovo_h8_config *conf = dev->chip_info;
	u8 val, tmp;
	u8 beepmask0, beepmask1, config1;

	dev->ops = &h8_dev_ops;

	h8_log_ec_version();

	ec_write(H8_CONFIG0, conf->config0);
	config1 = conf->config1;

	if (conf->has_keyboard_backlight) {
		if (get_option(&val, "backlight") != CB_SUCCESS)
			val = 0; /* Both backlights.  */
		config1 = (config1 & 0xf3) | ((val & 0x3) << 2);
	}
	ec_write(H8_CONFIG1, config1);
	ec_write(H8_CONFIG2, conf->config2);
	ec_write(H8_CONFIG3, conf->config3);

	beepmask0 = conf->beepmask0;
	beepmask1 = conf->beepmask1;

	if (conf->has_power_management_beeps
	    && get_option(&val, "power_management_beeps") == CB_SUCCESS
	    && val == 0) {
		beepmask0 = 0x00;
		beepmask1 = 0x00;
	}

	if (conf->has_power_management_beeps) {
		if (get_option(&val, "low_battery_beep") != CB_SUCCESS)
			val = 1;
		if (val)
			beepmask0 |= 2;
		else
			beepmask0 &= ~2;
	}

	ec_write(H8_SOUND_ENABLE0, beepmask0);
	ec_write(H8_SOUND_ENABLE1, beepmask1);

	ec_write(H8_SOUND_REPEAT, 0x00);

	/* silence sounds in queue */
	ec_write(H8_SOUND_REG, 0x00);

	ec_write(0x10, conf->event0_enable);
	ec_write(0x11, conf->event1_enable);
	ec_write(0x12, conf->event2_enable);
	ec_write(0x13, conf->event3_enable);
	ec_write(0x14, conf->event4_enable);
	ec_write(0x15, conf->event5_enable);
	ec_write(0x16, conf->event6_enable);
	ec_write(0x17, conf->event7_enable);
	ec_write(0x18, conf->event8_enable);
	ec_write(0x19, conf->event9_enable);
	ec_write(0x1a, conf->eventa_enable);
	ec_write(0x1b, conf->eventb_enable);
	ec_write(0x1c, conf->eventc_enable);
	ec_write(0x1d, conf->eventd_enable);
	ec_write(0x1e, conf->evente_enable);
	ec_write(0x1f, conf->eventf_enable);

	ec_write(H8_FAN_CONTROL, H8_FAN_CONTROL_AUTO);

	if (get_option(&val, "wlan") != CB_SUCCESS)
		val = 1;
	h8_wlan_enable(val);

	h8_trackpoint_enable(1);
	h8_usb_power_enable(1);

	if (get_option(&val, "volume") == CB_SUCCESS)
		ec_write(H8_VOLUME_CONTROL, val);

	if (get_option(&val, "bluetooth") != CB_SUCCESS)
		val = 1;
	h8_bluetooth_enable(val);

	if (get_option(&val, "wwan") != CB_SUCCESS)
		val = 1;

	h8_wwan_enable(val);

	if (conf->has_uwb) {
		if (get_option(&val, "uwb") != CB_SUCCESS)
			val = 1;

		h8_uwb_enable(val);
	}

	if (get_option(&val, "fn_ctrl_swap") != CB_SUCCESS)
		val = 0;
	h8_fn_ctrl_swap(val);

	if (get_option(&val, "sticky_fn") != CB_SUCCESS)
		val = 0;
	h8_sticky_fn(val);

	if (get_option(&val, "first_battery") != CB_SUCCESS)
		val = 1;

	tmp = ec_read(H8_CONFIG3);
	tmp &= ~(1 << 4);
	tmp |= (val & 1) << 4;
	ec_write(H8_CONFIG3, tmp);
	h8_set_audio_mute(0);

#if !IS_ENABLED(CONFIG_H8_DOCK_EARLY_INIT)
	h8_mainboard_init_dock ();
#endif
}

struct chip_operations ec_lenovo_h8_ops = {
	CHIP_NAME("Lenovo H8 EC")
	.enable_dev = h8_enable,
};
