## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += gpio.c

romstage-y += gpio.c
romstage-y += memory.c

ramstage-y += gpio.c
ramstage-y += mainboard.c

verstage-y += gpio.c

smm-y += gpio.c
