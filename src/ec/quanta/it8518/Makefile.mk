## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_EC_QUANTA_IT8518),y)

romstage-y += ec.c
ramstage-y += ec.c
smm-y += ec.c
smm-y += ../../../lib/delay.c

endif
