## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += early_init.c

romstage-y += early_init.c
romstage-y += gpio.c

ramstage-y += cstates.c
