## SPDX-License-Identifier: GPL-2.0-only

# Order matters for SPD sources. The following indices
# define the SPD data to use.
# 0b000 - 4GiB total - 2 x 2GiB Micron MT41K256M16HA-125:E 1600MHz
# 0b001 - 4GiB total - 2 x 2GiB Hynix  H5TC4G63AFR-PBA 1600MHz
# 0b010 - 2GiB total - 2 x 1GiB Micron MT41K128M16JT-125:K 1600MHz
# 0b011 - 2GiB total - 2 x 1GiB Hynix  H5TC2G63FFR-PBA 1600MHz
# 0b100 - 2GiB total - 1 x 2GiB Micron MT41K256M16HA-125:E 1600MHz
# 0b101 - 2GiB total - 1 x 2GiB Hynix  H5TC4G63AFR-PBA 1600MHz
# 0b110 - 4GiB total - 2 x 2GiB Samsung K4B4G1646Q-HYK0 1600MHz
# 0b111 - 2GiB total - 1 x 2GiB Samsung K4B4G1646Q-HYK0 1600MHz
SPD_SOURCES = micron_2GiB_dimm_MT41K256M16HA-125
SPD_SOURCES += hynix_2GiB_dimm_H5TC4G63AFR-PBA
SPD_SOURCES += micron_1GiB_dimm_MT41K128M16JT-125
SPD_SOURCES += hynix_1GiB_dimm_H5TC2G63FFR-PBA
SPD_SOURCES += micron_2GiB_dimm_MT41K256M16HA-125
SPD_SOURCES += hynix_2GiB_dimm_H5TC4G63AFR-PBA
SPD_SOURCES += samsung_2GiB_dimm_K4B4G1646Q-HYK0
SPD_SOURCES += samsung_2GiB_dimm_K4B4G1646Q-HYK0
