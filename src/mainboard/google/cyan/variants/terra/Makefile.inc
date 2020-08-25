## SPDX-License-Identifier: GPL-2.0-only

romstage-y += romstage.c
romstage-y += spd_util.c

ramstage-y += gpio.c
ramstage-y += ramstage.c

SPD_SOURCES =  samsung_dimm_K4E8E304EE-EGCE
SPD_SOURCES += samsung_dimm_K4E8E324EB-EGCF
SPD_SOURCES += micron_2GiB_dimm_EDF8132A3MA-GD-F-R
SPD_SOURCES += micron_2GiB_dimm_MT52L256M32D1PF-107WT
