ramstage-$(CONFIG_TPM_INIT) += tpm.c

ifeq ($(CONFIG_TPM_PPI),y)
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += ppi.c
else
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += ppi_stub.c
endif
