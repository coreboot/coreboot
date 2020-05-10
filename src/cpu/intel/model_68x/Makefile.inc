## SPDX-License-Identifier: GPL-2.0-or-later

ramstage-y += model_68x_init.c
subdirs-y += ../../x86/name

cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-08-*)
