## SPDX-License-Identifier: GPL-2.0-or-later

LIB_SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/soc/intel/jasperlake/spd/lp4x/$(f))
