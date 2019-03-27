## SPDX-License-Identifier: GPL-2.0-or-later
## This file is part of the coreboot project.

bootblock-y += com_init.c

ramstage-y += gpio.c
ramstage-y += irqroute.c
ramstage-y += ramstage.c
ramstage-y += spi_vscc.c
