## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c

all-y += die.c
smm-y += die.c

ifneq ($(wildcard $(src)/mainboard/$(MAINBOARDDIR)/data.apcb),)
ifeq ($(CONFIG_BUILD_WITH_DEBUG_APCB),y)
APCB_SOURCES = $(src)/mainboard/$(MAINBOARDDIR)/data_debug.apcb
APCB_SOURCES1 = $(src)/mainboard/$(MAINBOARDDIR)/data1_debug.apcb
else
APCB_SOURCES = $(src)/mainboard/$(MAINBOARDDIR)/data.apcb
APCB_SOURCES1 = $(src)/mainboard/$(MAINBOARDDIR)/data1.apcb
endif
APCB_SOURCES_RECOVERY = $(src)/mainboard/$(MAINBOARDDIR)/data_rec.apcb
APCB_SOURCES_RECOVERY1 = $(src)/mainboard/$(MAINBOARDDIR)/data_rec1.apcb
APCB_SOURCES_RECOVERY2 = $(src)/mainboard/$(MAINBOARDDIR)/data_rec2.apcb
else
show_notices:: warn_no_apcb
endif
