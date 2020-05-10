# SPDX-License-Identifier: GPL-2.0-only

## include generic fintek pre-ram stage driver
romstage-$(CONFIG_SUPERIO_ASPEED_COMMON_PRE_RAM) += common/early_serial.c
bootblock-$(CONFIG_SUPERIO_ASPEED_COMMON_PRE_RAM) += common/early_serial.c
bootblock-$(CONFIG_SUPERIO_ASPEED_COMMON_PRE_RAM) += common/early_config.c


subdirs-y += ast2400
subdirs-y += common
