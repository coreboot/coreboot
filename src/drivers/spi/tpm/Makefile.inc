verstage-$(CONFIG_SPI_TPM) += tis.c tpm.c
romstage-$(CONFIG_SPI_TPM) += tis.c tpm.c
ramstage-$(CONFIG_SPI_TPM) += tis.c tpm.c

ifneq ($(CONFIG_CHROMEOS),y)
bootblock-$(CONFIG_SPI_TPM) += tis.c tpm.c
romstage-$(CONFIG_SPI_TPM) += tis.c tpm.c
ramstage-$(CONFIG_SPI_TPM) += tis.c tpm.c
endif
