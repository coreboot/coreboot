## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += variants/$(VARIANT_DIR)

bootblock-y += bootblock.c

romstage-y += romstage.c

ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
ramstage-y += mainboard.c

ifeq ($(CONFIG_ADD_APCB_SOURCES),y)
APCB_SOURCES = $(call strip_quotes, $(CONFIG_APCB_SOURCES_PATH))/APCB_CZN_D4_Updatable.bin
APCB_SOURCES_68 = $(call strip_quotes,$(CONFIG_APCB_SOURCES_PATH))/APCB_CZN_D4_Updatable_68.bin
APCB_SOURCES_RECOVERY = $(call strip_quotes,$(CONFIG_APCB_SOURCES_PATH))/APCB_CZN_D4_DefaultRecovery.bin
endif
