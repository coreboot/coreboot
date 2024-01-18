## SPDX-License-Identifier: GPL-2.0-only

bct-cfg-$(CONFIG_NYAN_BCT_CFG_EMMC) += emmc.cfg
bct-cfg-$(CONFIG_NYAN_BCT_CFG_SPI) += spi.cfg
bct-cfg-y += odmdata.cfg

# Note when SDRAM config (sdram-*.cfg) files are changed, we have to regenerate
# the include files (sdram-*.inc) by running "./cfg2inc.sh sdram-*.cfg".
# TODO(hungte) Change cfg2inc.sh to NVIDIA's official tool in cbootimage.
