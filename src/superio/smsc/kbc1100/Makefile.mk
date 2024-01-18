# SPDX-License-Identifier: GPL-2.0-only

bootblock-$(CONFIG_SUPERIO_SMSC_KBC1100) += early_init.c
romstage-$(CONFIG_SUPERIO_SMSC_KBC1100) += early_init.c
ramstage-$(CONFIG_SUPERIO_SMSC_KBC1100) += superio.c
