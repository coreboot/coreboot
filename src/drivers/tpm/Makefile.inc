ramstage-$(CONFIG_TPM_INIT) += tpm.c

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += ppi_stub.c
