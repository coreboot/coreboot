# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += gpio.c
romstage-y += gpio.c
ramstage-y += gpio.c
verstage-y += gpio.c

bootblock-y += variant.c
romstage-y += variant.c
ramstage-y += variant.c

ramstage-y += ramstage.c

subdirs-y += ./memory
