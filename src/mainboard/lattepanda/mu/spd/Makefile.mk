## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_BOARD_LATTEPANDA_MU_MEMORY_8GB),y)
SPD_SOURCES = mu_lp5_8gb
else
SPD_SOURCES = mu_lp5_16gb
endif
