## SPDX-License-Identifier: GPL-2.0-only

bct-cfg-y += spi.cfg
bct-cfg-y += odmdata.cfg
bct-cfg-y += jtag.cfg
#NOTE: When full LPDDR4 SDRAM config is done in romstage, remove this
bct-cfg-$(CONFIG_BOOTROM_SDRAM_INIT) += sdram-nano-4GB-204.cfg

# Note when SDRAM config (sdram-*.cfg) files are changed, we have to regenerate
# the include files (sdram-*.inc) by running "./cfg2inc.sh sdram-*.cfg".
