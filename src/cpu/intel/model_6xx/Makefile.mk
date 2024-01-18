## SPDX-License-Identifier: GPL-2.0-only

ramstage-y += model_6xx_init.c

cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-06-*)
