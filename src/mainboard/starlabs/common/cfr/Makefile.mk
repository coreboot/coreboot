# SPDX-License-Identifier: GPL-2.0-only

CPPFLAGS_common += -I$(src)/mainboard/starlabs/common/cfr
ramstage-y += cfr.c
