ifeq ($(CONFIG_TPM)$(CONFIG_SPI_TPM),yy)
all-y += tis.c tpm.c
endif
