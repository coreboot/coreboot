# SPDX-License-Identifier: GPL-2.0-only
# This file is part of the coreboot project.

bootblock-$(CONFIG_SUPERIO_SMSC_LPC47M15X) += early_serial.c
romstage-$(CONFIG_SUPERIO_SMSC_LPC47M15X) += early_serial.c
ramstage-$(CONFIG_SUPERIO_SMSC_LPC47M15X) += superio.c
