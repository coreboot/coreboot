## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += soc_pch.c soc_gpio.c
romstage-y += soc_pmutil.c soc_gpio.c
ramstage-y += soc_pmutil.c soc_pch.c soc_gpio.c

CPPFLAGS_common += -I$(src)/soc/intel/xeon_sp/lbg/include
