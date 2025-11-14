## SPDX-License-Identifier: GPL-2.0-or-later
##

romstage-y += spd.c

ifneq ($(SPD_SOURCES),)
LIB_SPD_DEPS := $(SPD_SOURCES)
endif
