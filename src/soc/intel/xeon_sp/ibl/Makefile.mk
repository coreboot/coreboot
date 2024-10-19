## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += soc_gpio.c soc_pch.c
romstage-y += soc_gpio.c soc_pmutil.c soc_pch.c
ramstage-y += lockdown.c soc_gpio.c soc_pch.c soc_pmutil.c

CPPFLAGS_common += -I$(src)/soc/intel/xeon_sp/ibl/include
