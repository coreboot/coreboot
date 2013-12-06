/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
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
#ifndef AZALIA_H_
#define AZALIA_H_

/*
 * The tables found in this file are derived from the Intel High Definition
 * Audio Specification Revision 1.0a, published 17 June 2010
 */

/*
 * Page 177: Default Pin Configuration
 */

enum AzaliaPinCfgPortConnectivity {
	AZALIA_PINCFG_PORT_JACK                = 0b00,
	AZALIA_PINCFG_PORT_NC                  = 0b01,
	AZALIA_PINCFG_PORT_FIXED               = 0b10,
	AZALIA_PINCFG_PORT_MULTIPLE            = 0b11,
};

enum AzaliaPinCfgLocationGross {
	AZALIA_PINCFG_LOCATION_EXTERNAL        = 0x00,
	AZALIA_PINCFG_LOCATION_INTERNAL        = 0x10,
	AZALIA_PINCFG_LOCATION_EXT_CHASSIS     = 0x20,
	AZALIA_PINCFG_LOCATION_OTHER           = 0x30,
};

enum AzaliaPinCfgLocationFine {
	AZALIA_PINCFG_LOCATION_NOT_APPLICABLE  = 0x00,
	AZALIA_PINCFG_LOCATION_REAR            = 0x01,
	AZALIA_PINCFG_LOCATION_FRONT           = 0x02,
	AZALIA_PINCFG_LOCATION_LEFT            = 0x03,
	AZALIA_PINCFG_LOCATION_RIGHT           = 0x04,
	AZALIA_PINCFG_LOCATION_TOP             = 0x05,
	AZALIA_PINCFG_LOCATION_BOTTOM          = 0x06,
};

enum AzaliaPinCfgLocationSpecial {
	AZALIA_PINCFG_LOCATION_REAR_PANEL      = 0x07,
	AZALIA_PINCFG_LOCATION_DRIVE_BAY       = 0x08,
	AZALIA_PINCFG_LOCATION_RISER_CARD      = 0x17,
	AZALIA_PINCFG_LOCATION_DIGITAL_DISPLAY = 0x18,
	AZALIA_PINCFG_LOCATION_ATAPI           = 0x19,
	AZALIA_PINCFG_LOCATION_INSIDE_LID      = 0x37,
	AZALIA_PINCFG_LOCATION_OUTSIDE_LID     = 0x38,
};

enum AzaliaPinCfgDefaultDevice {
	AZALIA_PINCFG_DEVICE_LINEOUT           = 0x0,
	AZALIA_PINCFG_DEVICE_SPEAKER           = 0x1,
	AZALIA_PINCFG_DEVICE_HP_OUT            = 0x2,
	AZALIA_PINCFG_DEVICE_CD                = 0x3,
	AZALIA_PINCFG_DEVICE_SPDIF_OUT         = 0x4,
	AZALIA_PINCFG_DEVICE_DIGITAL_OUT       = 0x5,
	AZALIA_PINCFG_DEVICE_MODEM_LINE        = 0x6,
	AZALIA_PINCFG_DEVICE_MODEM_HANDSET     = 0x7,
	AZALIA_PINCFG_DEVICE_LINEIN            = 0x8,
	AZALIA_PINCFG_DEVICE_AUX               = 0x9,
	AZALIA_PINCFG_DEVICE_MICROPHONE        = 0xA,
	AZALIA_PINCFG_DEVICE_TELEPHONY         = 0xB,
	AZALIA_PINCFG_DEVICE_SPDIF_IN          = 0xC,
	AZALIA_PINCFG_DEVICE_DIGITAL_IN        = 0xD,
	AZALIA_PINCFG_DEVICE_OTHER             = 0xF,
};

enum AzaliaPinCfgConnectionType {
	AZALIA_PINCFG_CONN_UNKNOWN             = 0x0,
	AZALIA_PINCFG_CONN_MINI_HEADPHONE_JACK = 0x1,
	AZALIA_PINCFG_CONN_STEREO_PHONE_JACK   = 0x2,
	AZALIA_PINCFG_CONN_INTERNAL_ATAPI      = 0x3,
	AZALIA_PINCFG_CONN_RCA                 = 0x4,
	AZALIA_PINCFG_CONN_OPTICAL             = 0x5,
	AZALIA_PINCFG_CONN_OTHER_DIGITAL       = 0x6,
	AZALIA_PINCFG_CONN_OTHER_ANALOG        = 0x7,
	AZALIA_PINCFG_CONN_DIN_PLUG            = 0x8,
	AZALIA_PINCFG_CONN_XLR                 = 0x9,
	AZALIA_PINCFG_CONN_MODEM_RJ11          = 0xA,
	AZALIA_PINCFG_CONN_COMBINATION         = 0xB,
	AZALIA_PINCFG_CONN_OTHER               = 0xF,
};

enum AzaliaPinCfgColor {
	AZALIA_PINCFG_COLOR_UNKNOWN            = 0x0,
	AZALIA_PINCFG_COLOR_BLACK              = 0x1,
	AZALIA_PINCFG_COLOR_GREY               = 0x2,
	AZALIA_PINCFG_COLOR_BLUE               = 0x3,
	AZALIA_PINCFG_COLOR_GREEN              = 0x4,
	AZALIA_PINCFG_COLOR_RED                = 0x5,
	AZALIA_PINCFG_COLOR_ORANGE             = 0x6,
	AZALIA_PINCFG_COLOR_YELLOW             = 0x7,
	AZALIA_PINCFG_COLOR_PURPLE             = 0x8,
	AZALIA_PINCFG_COLOR_PINK               = 0x9,
	AZALIA_PINCFG_COLOR_WHITE              = 0xE,
	AZALIA_PINCFG_COLOR_OTHER              = 0xF,
};

enum AzaliaPinCfgMisc {
	AZALIA_PINCFG_MISC_IGNORE_PRESENCE     = 0x1,
};

union AzaliaPinConfiguration {
	unsigned int value;
	struct __attribute__((aligned(4),packed)) {
		enum AzaliaPinCfgPortConnectivity   port:2;
		unsigned char                       location:6;
		enum AzaliaPinCfgDefaultDevice      device:4;
		enum AzaliaPinCfgConnectionType     connection:4;
		enum AzaliaPinCfgColor              color:4;
		unsigned char                       misc:4;
		unsigned char                       association:4;
		unsigned char                       sequence:4;
	};
};

#endif /* AZALIA_H_ */
