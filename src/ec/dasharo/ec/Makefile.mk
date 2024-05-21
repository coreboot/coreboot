# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_EC_DASHARO_EC),y)

all-y += dasharo_ec.c
all-y += buttons.c
smm-$(CONFIG_DEBUG_SMI) += dasharo_ec.c

endif
