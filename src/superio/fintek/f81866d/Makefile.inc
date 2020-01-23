# SPDX-License-Identifier: GPL-2.0-or-later
# This file is part of the coreboot project.

ramstage-$(CONFIG_SUPERIO_FINTEK_F81866D) += f81866d_hwm.c f81866d_uart.c
ramstage-$(CONFIG_SUPERIO_FINTEK_F81866D) += superio.c
