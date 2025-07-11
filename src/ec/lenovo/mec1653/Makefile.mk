## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_EC_LENOVO_MEC1653),y)

bootblock-y += mec1653.c
bootblock-$(CONFIG_MEC1653_ENABLE_UART) += debug.c
bootblock-$(CONFIG_MEC1653_ENABLE_UART) += uart.c

endif
