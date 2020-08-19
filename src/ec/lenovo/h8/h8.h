/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_LENOVO_H8_H
#define EC_LENOVO_H8_H

#include <stdint.h>
#include <device/device.h>

enum usb_always_on {
	UAO_OFF = 0,
	UAO_AC_AND_BATTERY = 1,
	UAO_AC_ONLY = 2
};

void h8_trackpoint_enable(int on);
void h8_wlan_enable(int on);
void h8_set_audio_mute(int on);
void h8_usb_always_on_enable(enum usb_always_on on);
void h8_usb_power_enable(int on);
void h8_enable_event(int event);
void h8_disable_event(int event);
int h8_ultrabay_device_present(void);
u8 h8_build_id_and_function_spec_version(char *buf, u8 buf_len);
void h8_usb_always_on(void);

int h8_get_fn_key(void);
int h8_get_sense_ready(void);

void h8_bluetooth_enable(int on);
bool h8_bluetooth_nv_enable(void);
bool h8_has_bdc(const struct device *dev);

void h8_wwan_enable(int on);
bool h8_wwan_nv_enable(void);
bool h8_has_wwan(const struct device *dev);

void h8_ssdt_generator(const struct device *dev);
/*
 * boards needing specific h8-related inits could override it
 */
void h8_mb_init(void);

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

#define H8_LED_CONTROL			0x0c
#define H8_LED_CONTROL_OFF		0x00
#define H8_LED_CONTROL_ON		0x80
#define H8_LED_CONTROL_PULSE		0xa0	/* Some models, power LED only*/
#define H8_LED_CONTROL_BLINK		0xc0

#define H8_LED_CONTROL_POWER_LED	0x00
#define H8_LED_CONTROL_BAT0_LED		0x01
#define H8_LED_CONTROL_BAT1_LED		0x02
#define H8_LED_CONTROL_UBAY_LED		0x04
#define H8_LED_CONTROL_SUSPEND_LED	0x07
#define H8_LED_CONTROL_DOCK_LED1	0x08
#define H8_LED_CONTROL_DOCK_LED2	0x09
#define H8_LED_CONTROL_ACDC_LED		0x0c
#define H8_LED_CONTROL_MUTE_LED		0x0e

#define H8_USB_ALWAYS_ON		0x0d
#define H8_USB_ALWAYS_ON_ENABLE		0x01
#define H8_USB_ALWAYS_ON_AC_ONLY	0x0c

#define H8_FAN_CONTROL			0x2f
#define H8_FAN_CONTROL_AUTO		0x80

#define H8_VOLUME_CONTROL		0x30
#define H8_VOLUME_CONTROL_MUTE		0x40
/* Embedded controller events */
#define H8_EVENT_FN_F1			0x10
#define H8_EVENT_FN_F2			0x11
#define H8_EVENT_FN_F3			0x12
#define H8_EVENT_FN_F4			0x13
#define H8_EVENT_FN_HOME		0x14
#define H8_EVENT_FN_END			0x15
#define H8_EVENT_FN_F7			0x16
#define H8_EVENT_FN_F8			0x17
#define H8_EVENT_FN_F9			0x18
#define H8_EVENT_FN_THINKVANTAGE	0x19
#define H8_EVENT_FN_F11			0x1a
#define H8_EVENT_FN_F12			0x1b
#define H8_EVENT_FN_1			0x1c
#define H8_EVENT_FN_2			0x1d
#define H8_EVENT_FN_PGUP		0x1f

#define H8_EVENT_AC_ON			0x26
#define H8_EVENT_AC_OFF			0x27

#define H8_EVENT_PWRSW_PRESS		0x28
#define H8_EVENT_PWRSW_RELEASE		0x29

#define H8_EVENT_LIDSW_CLOSE		0x2a
#define H8_EVENT_LIDSW_PUSH		0x2b

#define H8_EVENT_UBAY_UNLOCK		0x2c
#define H8_EVENT_UBAY_LOCK		0x2d

#define H8_EVENT_KEYPRESS		0x33

#define H8_EVENT_FN_PRESS		0x39

#define H8_STATUS0			0x46
#define H8_STATUS0_FN_KEY_DOWN		0x01
#define H8_STATUS1			0x47
#define H8_STATUS2			0x48
#define H8_STATUS3			0x49

#define H8_EVENT_BAT0			0x4a
#define H8_EVENT_BAT0_STATE		0x4b

#define H8_EVENT_BAT1			0x4c
#define H8_EVENT_BAT1_STATE		0x4d

#define H8_EVENT_FN_F5			0x64
#define H8_EVENT_FN_F6			0x65

#endif /* EC_LENOVO_H8_H */
