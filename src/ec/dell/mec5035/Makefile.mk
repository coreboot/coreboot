## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_EC_DELL_MEC5035),y)

bootblock-y += mec5035.c
romstage-y += mec5035.c
ramstage-y += mec5035.c

endif
