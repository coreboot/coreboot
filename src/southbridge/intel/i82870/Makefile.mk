## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_I82870),y)

ramstage-y += ioapic.c
ramstage-y += pcibridge.c

endif
