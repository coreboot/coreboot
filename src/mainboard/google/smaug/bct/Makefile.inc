##
## This file is part of the coreboot project.
##
## Copyright 2015 Google Inc.
## Copyright (c) 2015, NVIDIA CORPORATION.  All rights reserved.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

bct-cfg-$(CONFIG_SMAUG_BCT_CFG_EMMC) += emmc.cfg
bct-cfg-$(CONFIG_SMAUG_BCT_CFG_SPI) += spi.cfg
bct-cfg-y += odmdata.cfg
bct-cfg-y += jtag.cfg
#NOTE: When full LPDDR4 SDRAM config is done in romstage, remove this
bct-cfg-$(CONFIG_BOOTROM_SDRAM_INIT) += sdram-samsung-204.cfg

# Note when SDRAM config (sdram-*.cfg) files are changed, we have to regenerate
# the include files (sdram-*.inc) by running "./cfg2inc.sh sdram-*.cfg".
# TODO(hungte) Change cfg2inc.sh to NVIDIA's official tool in cbootimage.
