## SPDX-License-Identifier: GPL-2.0-only

all-y += die.c
smm-y += die.c
bootblock-$(CONFIG_ENABLE_EC_UART1) += com_init.c
