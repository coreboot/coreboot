ramstage-$(CONFIG_TPM_INIT_RAMSTAGE) += tpm.c

ifeq ($(CONFIG_TPM_PPI),y)
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += ppi.c
else
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += ppi_stub.c
endif

bootblock-$(CONFIG_TPM_CR50) += cr50.c
verstage-$(CONFIG_TPM_CR50) += cr50.c
romstage-$(CONFIG_TPM_CR50) += cr50.c
ramstage-$(CONFIG_TPM_CR50) += cr50.c
postcar-$(CONFIG_TPM_CR50) += cr50.c
