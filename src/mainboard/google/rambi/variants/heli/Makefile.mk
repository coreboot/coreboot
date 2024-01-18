## SPDX-License-Identifier: GPL-2.0-only

# Order matters for SPD sources. The following indices
# define the SPD data to use.
# RAM_ID	Vendor	Vendor_PN	Freq	Size	Total_size	channel
# 0b0011	Hynix	H5TC4G63AFR-PBA	1600MHZ	4Gb	2GB	single-channel
# 0b0100	Hynix	H5TC4G63CFR-PBA	1600MHZ	4Gb	2GB	single-channel
# 0b0101	Samsung	K4B4G1646Q-HYK0	1600MHZ	4Gb	2GB	single-channel
# 0b0110	Hynix	H5TC4G63CFR-PBA	1600MHZ	4Gb	4GB	dual-channel
# 0b0111	Samsung	K4B4G1646Q-HYK0	1600MHZ	4Gb	4GB	dual-channel
SPD_SOURCES = empty					# 0b0000
SPD_SOURCES += empty					# 0b0001
SPD_SOURCES += empty					# 0b0010
SPD_SOURCES += hynix_2GiB_dimm_H5TC4G63AFR-PBA		# 0b0011
SPD_SOURCES += hynix_2GiB_dimm_H5TC4G63CFR-PBA		# 0b0100
SPD_SOURCES += samsung_2GiB_dimm_K4B4G1646Q-HYK0	# 0b0101
SPD_SOURCES += hynix_2GiB_dimm_H5TC4G63CFR-PBA		# 0b0110
SPD_SOURCES += samsung_2GiB_dimm_K4B4G1646Q-HYK0	# 0b0111
SPD_SOURCES += empty					# 0b1000
SPD_SOURCES += empty					# 0b1001
SPD_SOURCES += empty					# 0b1010
SPD_SOURCES += empty					# 0b1011
SPD_SOURCES += empty					# 0b1100
SPD_SOURCES += empty					# 0b1101
SPD_SOURCES += empty					# 0b1110
SPD_SOURCES += empty					# 0b1111
