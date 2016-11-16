#
# Copyright (C) 2014 Steve Shenton <sgsit@libreboot.org>
# Copyright (C) 2014, 2015 Leah Rowe <info@minifree.org>
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
# Info on flash descriptor (page 845 onwards):
#
# http://www.intel.co.uk/content/dam/doc/datasheet/io-controller-hub-9-datasheet.pdf

# Flash Descriptor SPEC for GM45/ICH9M
{
	# Signature for descriptor mode
	"fd_signature"		: 32,

	# Flash map registers
	"flmap0_fcba"		: 8,
	"flmap0_nc"		: 2,
	"flmap0_reserved0"	: 6,
	"flmap0_frba"		: 8,
	"flmap0_nr"		: 3,
	"flmap0_reserved1"	: 5,
	"flmap1_fmba"		: 8,
	"flmap1_nm"		: 3,
	"flmap1_reserved"	: 5,
	"flmap1_fisba"		: 8,
	"flmap1_isl"		: 8,
	"flmap2_fmsba"		: 8,
	"flmap2_msl"		: 8,
	"flmap2_reserved"	: 16,

	# Component section
	"flcomp_density1"	: 3,
	"flcomp_density2"	: 3,
	"flcomp_reserved0"	: 2,
	"flcomp_reserved1"	: 8,
	"flcomp_reserved2"	: 1,
	"flcomp_readclockfreq"	: 3,
	"flcomp_fastreadsupp"	: 1,
	"flcomp_fastreadfreq"	: 3,
	"flcomp_w_eraseclkfreq"	: 3,
	"flcomp_r_statclkfreq"	: 3,
	"flcomp_reserved3"	: 2,
	"flill"			: 32,
	"flbp"			: 32,
	"comp_padding"[36]	: 8,

	# Region section
	"flreg0_base"		: 13,
	"flreg0_reserved0"	: 3,
	"flreg0_limit"		: 13,
	"flreg0_reserved1"	: 3,
	"flreg1_base"		: 13,
	"flreg1_reserved0"	: 3,
	"flreg1_limit"		: 13,
	"flreg1_reserved1"	: 3,
	"flreg2_base"		: 13,
	"flreg2_reserved0"	: 3,
	"flreg2_limit"		: 13,
	"flreg2_reserved1"	: 3,
	"flreg3_base"		: 13,
	"flreg3_reserved0"	: 3,
	"flreg3_limit"		: 13,
	"flreg3_reserved1"	: 3,
	"flreg4_base"		: 13,
	"flreg4_reserved0"	: 3,
	"flreg4_limit"		: 13,
	"flreg4_reserved1"	: 3,
	"flreg_padding"[12]	: 8,

	# Master access section

	# 1: Host CPU/BIOS
	"flmstr1_requesterid"	: 16,
	"flmstr1_r_fd"		: 1,
	"flmstr1_r_bios"	: 1,
	"flmstr1_r_me"		: 1,
	"flmstr1_r_gbe"		: 1,
	"flmstr1_r_pd"		: 1,
	"flmstr1_r_reserved"	: 3,
	"flmstr1_w_fd"		: 1,
	"flmstr1_w_bios"	: 1,
	"flmstr1_w_me"		: 1,
	"flmstr1_w_gbe"		: 1,
	"flmstr1_w_pd"		: 1,
	"flmstr1_w_reserved"	: 3,

	# 2: ME
	"flmstr2_requesterid"	: 16,
	"flmstr2_r_fd"		: 1,
	"flmstr2_r_bios"	: 1,
	"flmstr2_r_me"		: 1,
	"flmstr2_r_gbe"		: 1,
	"flmstr2_r_pd"		: 1,
	"flmstr2_r_reserved"	: 3,
	"flmstr2_w_fd"		: 1,
	"flmstr2_w_bios"	: 1,
	"flmstr2_w_me"		: 1,
	"flmstr2_w_gbe"		: 1,
	"flmstr2_w_pd"		: 1,
	"flmstr2_w_reserved"	: 3,

	# 3: GbE
	"flmstr3_requesterid"	: 16,
	"flmstr3_r_fd"		: 1,
	"flmstr3_r_bios"	: 1,
	"flmstr3_r_me"		: 1,
	"flmstr3_r_gbe"		: 1,
	"flmstr3_r_pd"		: 1,
	"flmstr3_r_reserved"	: 3,
	"flmstr3_w_fd"		: 1,
	"flmstr3_w_bios"	: 1,
	"flmstr3_w_me"		: 1,
	"flmstr3_w_gbe"		: 1,
	"flmstr3_w_pd"		: 1,
	"flmstr3_w_reserved"	: 3,

	"flmstr_padding"[148]	: 8,

	# ICHSTRAP0
	"ich0_medisable"	: 1,
	"ich0_reserved0"	: 6,
	"ich0_tcomode"		: 1,
	"ich0_mesmbusaddr"	: 7,
	"ich0_bmcmode"		: 1,
	"ich0_trippointsel"	: 1,
	"ich0_reserved1"	: 2,
	"ich0_integratedgbe"	: 1,
	"ich0_lanphy"		: 1,
	"ich0_reserved2"	: 3,
	"ich0_dmireqiddisable"	: 1,
	"ich0_me2smbusaddr"	: 7,

	# ICHSTRAP1
	"ich1_dynclk_nmlink"	: 1,
	"ich1_dynclk_smlink"	: 1,
	"ich1_dynclk_mesmbus"	: 1,
	"ich1_dynclk_sst"	: 1,
	"ich1_reserved0"	: 4,
	"ich1_nmlink_npostreqs"	: 1,
	"ich1_reserved1"	: 7,
	"ich1_reserved2"	: 16,

	"ichstrap_padding"[248]	: 8,

	# MCHSTRAP0
	"mch0_medisable"	: 1,
	"mch0_mebootfromflash"	: 1,
	"mch0_tpmdisable"	: 1,
	"mch0_reserved0"	: 3,
	"mch0_spifingerprinton"	: 1,
	# Alternate disable - allows ME to perform chipset
	# init functions but disables FW apps such as AMT
	"mch0_mealtdisable"	: 1,
	"mch0_reserved1"	: 8,
	"mch0_reserved2"	: 16,

	"mchstrap_padding"[3292]: 8,

	# ME VSCC Table
	"mevscc_jid0"		: 32,
	"mevscc_vscc0"		: 32,
	"mevscc_jid1"		: 32,
	"mevscc_vscc1"		: 32,
	"mevscc_jid2"		: 32,
	"mevscc_vscc2"		: 32,
	"mevscc_padding"[4]	: 8,

	# Descriptor Map 2 Record
	"mevscc_tablebase"	: 8,
	"mevscc_tablelength"	: 8,
	"mevscc_reserved"	: 16,

	# OEM section
	"oem_magic"[8]		: 8,
	"oem_padding"[248]	: 8
}
