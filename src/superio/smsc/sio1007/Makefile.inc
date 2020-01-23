# SPDX-License-Identifier: GPL-2.0-only
# This file is part of the coreboot project.

bootblock-$(CONFIG_SUPERIO_SMSC_SIO1007) += early_serial.c
romstage-$(CONFIG_SUPERIO_SMSC_SIO1007) += early_serial.c
