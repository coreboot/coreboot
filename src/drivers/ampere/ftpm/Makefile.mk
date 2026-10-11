## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_AMPERE_CRB_FTPM),y)
ramstage-y += tpm.c tis.c
endif
