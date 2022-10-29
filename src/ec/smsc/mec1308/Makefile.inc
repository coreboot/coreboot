## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_EC_SMSC_MEC1308),y)

ramstage-y += ec.c
smm-y += ec.c

endif
