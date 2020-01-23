# SPDX-License-Identifier: GPL-2.0-or-later
# This file is part of the coreboot project.

bootblock-$(CONFIG_SUPERIO_FINTEK_F81216H) += early_serial.c
romstage-$(CONFIG_SUPERIO_FINTEK_F81216H) += early_serial.c
ramstage-$(CONFIG_SUPERIO_FINTEK_F81216H) += superio.c
