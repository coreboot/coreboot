## SPDX-License-Identifier: GPL-2.0-only

# Order matters for SPD sources. The following indices
# define the SPD data to use.
# 0b000 - 2GiB total - 1 x 2GiB Samsung K4B4G1646Q-HYK0 1600MHz
# 0b001 - 2GiB total - 1 x 2GiB Hynix  H5TC4G63AFR-PBA 1600MHz
# 0b010 - 4GiB total - 2 x 2GiB Samsung K4B4G1646Q-HYK0 1600MHz
# 0b011 - 4GiB total - 2 x 2GiB Hynix  H5TC4G63AFR-PBA 1600MHz
SPD_SOURCES  = samsung_2GiB_dimm_K4B4G1646Q-HYK0
SPD_SOURCES += hynix_2GiB_dimm_H5TC4G63AFR-PBA
SPD_SOURCES += samsung_2GiB_dimm_K4B4G1646Q-HYK0
SPD_SOURCES += hynix_2GiB_dimm_H5TC4G63AFR-PBA
