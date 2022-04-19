ifeq ($(CONFIG_CRB_TPM),y)
all-y += tis.c tpm.c
endif
