# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += gpio.c
bootblock-y += variant.c

romstage-y += gpio.c
romstage-y += variant.c

ramstage-y += gpio.c
ramstage-y += variant.c

verstage-y += gpio.c

subdirs-y += ./memory
