## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(SPD_SOURCES),)
	SPD_DEPS := $(error SPD_SOURCES is not set. Variant must provide this)
endif
