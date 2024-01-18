## SPDX-License-Identifier: GPL-2.0-only

all-$(CONFIG_SMMSTORE) += store.c

ramstage-$(CONFIG_SMMSTORE_V2) += ramstage.c

smm-$(CONFIG_SMMSTORE) += store.c smi.c
