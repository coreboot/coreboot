## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += gpio.c

romstage-y += memory.c
romstage-y += gpio.c

ramstage-y += gpio.c
ramstage-y += nhlt.c
ramstage-y += cbi_ssfc.c

smm-y += gpio.c
