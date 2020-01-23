# SPDX-License-Identifier: GPL-2.0-or-later
# This file is part of the coreboot project.

bootblock-$(CONFIG_SUPERIO_SMSC_LPC47M10X) += early_serial.c
romstage-$(CONFIG_SUPERIO_SMSC_LPC47M10X) += early_serial.c
ramstage-$(CONFIG_SUPERIO_SMSC_LPC47M10X) += superio.c
