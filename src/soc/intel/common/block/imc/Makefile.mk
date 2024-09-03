## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_INTEL_COMMON_BLOCK_IMC),y)
romstage-y += imc.c
romstage-y += spd_access.c
endif
