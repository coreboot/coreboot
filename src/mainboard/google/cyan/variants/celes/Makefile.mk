## SPDX-License-Identifier: GPL-2.0-only

romstage-y += spd_util.c

ramstage-y += gpio.c
ramstage-y += ramstage.c

SPD_SOURCES = samsung_dimm_K4E8E304EE-EGCE    # Index0
SPD_SOURCES += samsung_dimm_K4E8E324EB-EGCF   # Index1
