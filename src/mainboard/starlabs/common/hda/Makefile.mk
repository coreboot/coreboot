# SPDX-License-Identifier: GPL-2.0-only

ramstage-y += runtime.c
ramstage-$(CONFIG_HAVE_HDA_DMIC) += dmic.c
