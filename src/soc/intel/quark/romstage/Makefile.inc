# SPDX-License-Identifier: GPL-2.0-only

romstage-y += car.c
romstage-$(CONFIG_DISPLAY_UPD_DATA) += debug.c
romstage-y += fsp_params.c
romstage-y += pcie.c
romstage-y += report_platform.c
romstage-y += romstage.c
romstage-y += ../../../../cpu/intel/car/romstage.c
