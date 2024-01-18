# SPDX-License-Identifier: GPL-2.0-or-later
bootblock-y += gpio.c

romstage-y += gpio.c

ramstage-y += gpio.c
ramstage-$(CONFIG_FW_CONFIG) += variant.c
