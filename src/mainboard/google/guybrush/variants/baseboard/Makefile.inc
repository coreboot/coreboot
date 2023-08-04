## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += gpio.c
bootblock-y += helpers.c

romstage-y += helpers.c
romstage-y += gpio.c

ramstage-y += gpio.c
ramstage-y += helpers.c

verstage-y += gpio.c

smm-y += gpio.c
