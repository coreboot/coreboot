## SPDX-License-Identifier: GPL-2.0-or-later
##

ifneq ($(SPD_SOURCES),)
ifeq ($(SPD_SOURCE_PATH),)
SPD_SOURCE_PATH := src/soc/intel/tigerlake/spd
endif

LIB_SPD_DEPS := $(foreach f, $(SPD_SOURCES), $(SPD_SOURCE_PATH)/$(f))

endif
