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

#ifndef EC_LENOVO_H8_H
#define EC_LENOVO_H8_H

void h8_trackpoint_enable(int on);
void h8_wlan_enable(int on);
void h8_set_audio_mute(int on);
void h8_usb_power_enable(int on);
void h8_enable_event(int event);
void h8_disable_event(int event);
int h8_ultrabay_device_present(void);

/* EC registers */
#define H8_CONFIG0 0x00
#define H8_CONFIG0_EVENTS_ENABLE	0x02
#define H8_CONFIG0_HOTKEY_ENABLE	0x04
#define H8_CONFIG0_SMM_H8_ENABLE	0x20
#define H8_CONFIG0_TC_ENABLE		0x80

#define H8_CONFIG1 0x01
#define H8_CONFIG1_BKLT_LID		0x01
#define H8_CONFIG1_UBAY_PWR		0x20

#define H8_CONFIG2 0x02
#define H8_CONFIG2_USB_DOCK			0x01
#define H8_CONFIG2_DOCK_SPEAKER_MUTE		0x02
#define H8_CONFIG2_DOCK_SPEAKER_MUTE_POL	0x04

#define H8_CONFIG3 0x03

#define H8_SOUND_ENABLE0		0x04
#define H8_SOUND_ENABLE1		0x05
#define H8_SOUND_REG			0x06
#define H8_SOUND_REPEAT			0x07

#define H8_TRACKPOINT_CTRL		0x0B
#define H8_TRACKPOINT_AUTO		0x01
#define H8_TRACKPOINT_OFF		0x02
#define H8_TRACKPOINT_ON		0x03

#define H8_LED_CONTROL		0x0c
#define H8_LED_CONTROL_OFF		0x00
#define H8_LED_CONTROL_ON		0x80
#define H8_LED_CONTROL_BLINK		0xc0

#define H8_LED_CONTROL_POWER_LED	0x00
#define H8_LED_CONTROL_BAT0_LED	0x01
#define H8_LED_CONTROL_BAT1_LED	0x02
#define H8_LED_CONTROL_UBAY_LED	0x04
#define H8_LED_CONTROL_SUSPEND_LED	0x07
#define H8_LED_CONTROL_DOCK_LED1	0x08
#define H8_LED_CONTROL_DOCK_LED2	0x09

#define H8_FAN_CONTROL			0x2f
#define H8_FAN_CONTROL_AUTO		0x80

#define H8_VOLUME_CONTROL		0x30
#define H8_VOLUME_CONTROL_MUTE		0x40
/* Embedded controller events */
#define H8_EVENT_FN_F1	0x10
#define H8_EVENT_FN_F2	0x11
#define H8_EVENT_FN_F3	0x12
#define H8_EVENT_FN_F4	0x13
#define H8_EVENT_FN_HOME	0x14
#define H8_EVENT_FN_END	0x15
#define H8_EVENT_FN_F7	0x16
#define H8_EVENT_FN_F8	0x17
#define H8_EVENT_FN_F9	0x18
#define H8_EVENT_FN_THINKVANTAGE	0x19
#define H8_EVENT_FN_F11	0x1a
#define H8_EVENT_FN_F12	0x1b
#define H8_EVENT_FN_1		0x1c
#define H8_EVENT_FN_2		0x1d
#define H8_EVENT_FN_PGUP	0x1f

#define H8_EVENT_AC_ON	0x26
#define H8_EVENT_AC_OFF	0x27

#define H8_EVENT_PWRSW_PRESS	0x28
#define H8_EVENT_PWRSW_RELEASE 0x29

#define H8_EVENT_LIDSW_CLOSE	0x2a
#define H8_EVENT_LIDSW_PUSH	0x2b

#define H8_EVENT_UBAY_UNLOCK	0x2c
#define H8_EVENT_UBAY_LOCK	0x2d

#define H8_EVENT_KEYPRESS	0x33

#define H8_EVENT_FN_PRESS	0x39

#define H8_STATUS0		0x46
#define H8_STATUS1		0x47
#define H8_STATUS2		0x48

#define H8_EVENT_BAT0		0x4a
#define H8_EVENT_BAT0_STATE	0x4b

#define H8_EVENT_BAT1		0x4c
#define H8_EVENT_BAT1_STATE	0x4d

#define H8_EVENT_FN_F5 0x64
#define H8_EVENT_FN_F6 0x65
#endif
