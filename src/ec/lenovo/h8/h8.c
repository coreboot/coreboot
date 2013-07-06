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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <ec/acpi/ec.h>
#include <device/pnp.h>
#include <stdlib.h>
#include "h8.h"
#include "chip.h"
#include <pc80/mc146818rtc.h>

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

void h8_wlan_enable(int on)
{
	if (on)
		ec_set_bit(0x3a, 5);
	else
		ec_clr_bit(0x3a, 5);
}

static void h8_log_ec_version(void)
{
	unsigned char ecfw[9], c;
	u16 fwvh, fwvl;
	int i;

	for(i = 0; i < 8; i++) {
		c = ec_read(0xf0 + i);
		if (c < 0x20 || c > 0x7f)
			break;
		ecfw[i] = c;
	}
	ecfw[i] = '\0';

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

static void h8_enable(device_t dev)
{
	struct ec_lenovo_h8_config *conf = dev->chip_info;
	u8 val, tmp;

	h8_log_ec_version();

	ec_write(H8_CONFIG0, conf->config0);
	ec_write(H8_CONFIG1, conf->config1);
	ec_write(H8_CONFIG2, conf->config2);
	ec_write(H8_CONFIG3, conf->config3);

	ec_write(H8_SOUND_ENABLE0, conf->beepmask0);
	ec_write(H8_SOUND_ENABLE1, conf->beepmask1);
	ec_write(H8_SOUND_REPEAT, 0x00);

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
	h8_wlan_enable(conf->wlan_enable);
	h8_trackpoint_enable(conf->trackpoint_enable);
	h8_usb_power_enable(1);

	if (!get_option(&val, "volume"))
		ec_write(H8_VOLUME_CONTROL, val);


	if (!get_option(&val, "bluetooth"))
		h8_bluetooth_enable(val);

	if (!get_option(&val, "first_battery")) {
		tmp = ec_read(H8_CONFIG3);
		tmp &= ~(1 << 4);
		tmp |= (val & 1)<< 4;
		ec_write(H8_CONFIG3, tmp);
	}
	h8_set_audio_mute(0);
}

struct chip_operations ec_lenovo_h8_ops = {
	CHIP_NAME("Lenovo H8 EC")
	.enable_dev = h8_enable
};
