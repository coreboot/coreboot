# SPDX-License-Identifier: GPL-2.0-or-later
# This file is part of the coreboot project.

bootblock-$(CONFIG_SUPERIO_SMSC_LPC47N207) += early_serial.c
romstage-$(CONFIG_SUPERIO_SMSC_LPC47N207) += early_serial.c
