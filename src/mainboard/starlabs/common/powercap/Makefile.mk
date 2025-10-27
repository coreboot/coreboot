# SPDX-License-Identifier: GPL-2.0-only

ramstage-y += powercap.c

CPPFLAGS_common += -I$(src)/mainboard/starlabs/common/powercap
