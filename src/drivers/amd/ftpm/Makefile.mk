## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_AMD_CRB_FTPM),y)
# Currently depends on SOC_AMD_COMMON_BLOCK_PSP_SMI, that means
# fTPM is only accepting commands after SMM has been set up.
ramstage-y += tis.c tpm.c
endif
