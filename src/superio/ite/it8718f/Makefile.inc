# SPDX-License-Identifier: GPL-2.0-or-later
# This file is part of the coreboot project.

bootblock-$(CONFIG_SUPERIO_ITE_IT8718F) += early_serial.c
romstage-$(CONFIG_SUPERIO_ITE_IT8718F) += early_serial.c
ramstage-$(CONFIG_SUPERIO_ITE_IT8718F) += superio.c
