## SPDX-License-Identifier: GPL-2.0-or-later
## This file is part of the coreboot project.

ifneq ($(CONFIG_BOOTMEDIA_LOCK_NONE),y)
ramstage-y += lockdown.c
endif
