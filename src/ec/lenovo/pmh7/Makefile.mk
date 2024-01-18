## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_EC_LENOVO_PMH7),y)

bootblock-y += pmh7.c
ramstage-y += pmh7.c
smm-y += pmh7.c
romstage-y += pmh7.c

endif
