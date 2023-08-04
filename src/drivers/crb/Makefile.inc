## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_CRB_TPM),y)
all-y += tis.c tpm.c
endif
