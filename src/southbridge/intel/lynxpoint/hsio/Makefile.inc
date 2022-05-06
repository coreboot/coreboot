## SPDX-License-Identifier: GPL-2.0-or-later

romstage-y += common.c
ifeq ($(CONFIG_INTEL_LYNXPOINT_LP),y)
romstage-y += lpt_lp_bx.c
else
romstage-y += lpt_h_cx.c
endif
