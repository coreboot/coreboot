## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_CONSOLE_AMD_SIMNOW),y)
all-y += simnow_console.c

smm-$(CONFIG_DEBUG_SMI) += simnow_console.c
endif
