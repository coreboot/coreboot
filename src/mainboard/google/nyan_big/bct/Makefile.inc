## SPDX-License-Identifier: GPL-2.0-only

bct-cfg-$(CONFIG_NYAN_BIG_BCT_CFG_EMMC) += emmc.cfg
bct-cfg-$(CONFIG_NYAN_BIG_BCT_CFG_SPI) += spi.cfg
bct-cfg-y += odmdata.cfg

# Note when SDRAM config (sdram-*.cfg) files are changed, we have to regenerate
# the include files (sdram-*.inc). See ../../nyan/bct/Makefile.inc for more
# information.
