## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_EC_LIBREM_EC),y)

all-y += ../../system76/ec/system76_ec.c
smm-$(CONFIG_DEBUG_SMI) += ../../system76/ec/system76_ec.c

endif
