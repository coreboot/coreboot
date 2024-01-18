# SPDX-License-Identifier: GPL-2.0-only

bootblock-$(CONFIG_SUPERIO_SMSC_LPC47N227) += early_serial.c
romstage-$(CONFIG_SUPERIO_SMSC_LPC47N227) += early_serial.c
ramstage-$(CONFIG_SUPERIO_SMSC_LPC47N227) += superio.c
