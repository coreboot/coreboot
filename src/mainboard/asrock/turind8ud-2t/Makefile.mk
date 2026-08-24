## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c

ifeq ($(CONFIG_BUILD_WITH_DEBUG_APCB),y)
APCB_SOURCES_RECOVERY = $(src)/mainboard/$(MAINBOARDDIR)/data_rec_debug.apcb
else
ifeq ($(CONFIG_AMD_SEV_SNP_ENABLE),y)
APCB_SOURCES_RECOVERY = $(src)/mainboard/$(MAINBOARDDIR)/data_rec_snp.apcb
else
APCB_SOURCES_RECOVERY = $(src)/mainboard/$(MAINBOARDDIR)/data_rec.apcb
endif # CONFIG_AMD_SEV_SNP_ENABLE
endif # CONFIG_BUILD_WITH_DEBUG_APCB
APCB_SOURCES_RECOVERY1 = $(src)/mainboard/$(MAINBOARDDIR)/data_rec1.apcb
APCB_SOURCES_RECOVERY2 = $(src)/mainboard/$(MAINBOARDDIR)/data_rec2.apcb
