
ramstage-$(CONFIG_I2C_TPM) += tis.c
romstage-$(CONFIG_I2C_TPM) += tis.c
verstage-$(CONFIG_I2C_TPM) += tis.c
bootblock-$(CONFIG_I2C_TPM) += tis.c

ramstage-$(CONFIG_I2C_TPM_GENERIC) += tpm.c
romstage-$(CONFIG_I2C_TPM_GENERIC) += tpm.c
verstage-$(CONFIG_I2C_TPM_GENERIC) += tpm.c
bootblock-$(CONFIG_I2C_TPM_GENERIC) += tpm.c

ramstage-$(CONFIG_I2C_TPM_CR50) += cr50.c
romstage-$(CONFIG_I2C_TPM_CR50) += cr50.c
verstage-$(CONFIG_I2C_TPM_CR50) += cr50.c
bootblock-$(CONFIG_I2C_TPM_CR50) += cr50.c

ramstage-$(CONFIG_DRIVER_I2C_TPM_ACPI) += chip.c
