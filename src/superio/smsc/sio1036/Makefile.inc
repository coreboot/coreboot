# SPDX-License-Identifier: GPL-2.0-only

bootblock-$(CONFIG_SUPERIO_SMSC_SIO1036) += sio1036_early_init.c
romstage-$(CONFIG_SUPERIO_SMSC_SIO1036) += sio1036_early_init.c
ramstage-$(CONFIG_SUPERIO_SMSC_SIO1036) += superio.c
