ifeq ($(CONFIG_TPM),y)

ramstage-$(CONFIG_TPM_INIT_RAMSTAGE) += tpm.c

ifeq ($(CONFIG_TPM_PPI),y)
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += ppi.c
else
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += ppi_stub.c
endif

all-$(CONFIG_TPM_GOOGLE) += cr50.c

endif
