## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOUTHBRIDGE_TI_PCI7420),y)

ramstage-y += cardbus.c
ramstage-y += firewire.c

endif
