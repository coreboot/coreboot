#
# Copyright (C) 2014 Steve Shenton <sgsit@libreboot.org>
#                    Leah Rowe <info@minifree.org>
# Copyright (C) 2017 Damien Zammit <damien@zamaudio.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

#
# Datasheets:
#
# http://www.intel.co.uk/content/dam/doc/application-note/i-o-controller-hub-9m-82567lf-lm-v-nvm-map-appl-note.pdf
# https://communities.intel.com/community/wired/blog/2010/10/14/how-to-basic-eeprom-checksums

# The datasheet says that this spec covers the following pci ids:
# 8086:10F5 - Intel 82567LM gigabit ethernet controller
# 8086:10BF - Intel 82567LF gigabit ethernet controller
# 8086:10CB - Intel 82567V gigabit ethernet controller

# GbE SPEC for ICH9M (82567LM/LF/V)
{
	"macaddress"[6]		: 8,
	"ba_reserved1_0"	: 8,
	"ba_reserved1_1"	: 3,
	"ba_ibootagent"		: 1,
	"ba_reserved2"		: 4,
	"reserved04"		: 16,
	"version05"		: 16,
	"reserved06"		: 16,
	"reserved07"		: 16,
	"pbalow"		: 16,
	"pbahigh"		: 16,
	"pci_loadvid"		: 1,
	"pci_loadssid"		: 1,
	"pci_reserved1"		: 1,
	"pci_reserved2"		: 3,
	"pci_pmen"		: 1,
	"pci_auxpwr"		: 1,
	"pci_reserved3"		: 4,
	"pci_reserved4"		: 4,
	"ssdid"			: 16,
	"ssvid"			: 16,
	"did"			: 16,
	"vid"			: 16,
	"devrevid"		: 16,
	"lanpwr_d3pwr"		: 5,
	"lanpwr_reserved"	: 3,
	"lanpwr_d0pwr"		: 8,
	"reserved11"		: 16,
	"reserved12"		: 16,
	"sh_reserved1"		: 3,
	"sh_force_halfduplex"	: 1,
	"sh_force_lowspeed"	: 1,
	"sh_reserved2_0"	: 3,
	"sh_reserved2_1"	: 1,
	"sh_phy_enpwrdown"	: 1,
	"sh_reserved3"		: 1,
	"sh_reserved4"		: 3,
	"sh_sign"		: 2,
	"cw1_extcfgptr"		: 12,
	"cw1_oemload"		: 1,
	"cw1_reserved1"		: 1,
	"cw1_reserved2"		: 1,
	"cw1_reserved3"		: 1,
	"cw2_reserved"		: 8,
	"cw2_extphylen"		: 8,
	"extcfg16"		: 16,
	"l1_led1mode"		: 4,
	"l1_reserved1"		: 1,
	"l1_led1fastblink"	: 1,
	"l1_led1invert"		: 1,
	"l1_led1blinks"		: 1,
	"l1_reserved2"		: 1,
	"l1_lplu_all"		: 1,
	"l1_lplu_non_d0a"	: 1,
	"l1_gbedis_non_d0a"	: 1,
	"l1_reserved3"		: 2,
	"l1_gbedis"		: 1,
	"l1_reserved4"		: 1,
	"l02_led0mode"		: 4,
	"l02_reserved1"		: 1,
	"l02_led0fastblink"	: 1,
	"l02_led0invert"	: 1,
	"l02_led0blinks"	: 1,
	"l02_led2mode"		: 4,
	"l02_reserved2"		: 1,
	"l02_led2fastblink"	: 1,
	"l02_led2invert"	: 1,
	"l02_led2blinks"	: 1,
	"reserved19"		: 16,
	"reserved1a"		: 16,
	"reserved1b"		: 16,
	"reserved1c"		: 16,
	"reserved1d"		: 16,
	"_82567lm"		: 16,
	"_82567lf"		: 16,
	"reserved20"		: 16,
	"_82567v"		: 16,
	"reserved22_"[14]	: 16,
	"pxe30_protocolsel"	: 2,
	"pxe30_reserved1"	: 1,
	"pxe30_defbootsel"	: 2,
	"pxe30_reserved2"	: 1,
	"pxe30_ctrlsprompt"	: 2,
	"pxe30_dispsetup"	: 1,
	"pxe30_reserved3"	: 1,
	"pxe30_forcespeed"	: 2,
	"pxe30_forcefullduplex"	: 1,
	"pxe30_reserved4"	: 1,
	"pxe30_efipresent"	: 1,
	"pxe30_pxeabsent"	: 1,
	"pxe31_disablemenu"	: 1,
	"pxe31_disabletitle"	: 1,
	"pxe31_disableprotsel"	: 1,
	"pxe31_disablebootorder": 1,
	"pxe31_disablelegacywak": 1,
	"pxe31_disableflash_pro": 1,
	"pxe31_reserved1"	: 2,
	"pxe31_ibootagentmode"	: 3,
	"pxe31_reserved2"	: 3,
	"pxe31_signature"	: 2,
	"pxe32_buildnum"	: 8,
	"pxe32_minorversion"	: 4,
	"pxe32_majorversion"	: 4,
	"pxe33_basecodeabsent"	: 1,
	"pxe33_undipresent"	: 1,
	"pxe33_reserved1"	: 1,
	"pxe33_efiundipresent"	: 1,
	"pxe33_reserved2_0"	: 4,
	"pxe33_reserved2_1"	: 6,
	"pxe33_signature"	: 2,
	"pxe_padding"[11]	: 16,
	"checksum_gbe"		: 16,
	"padding"[0xf80]	: 8
}
