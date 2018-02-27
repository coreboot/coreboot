ramstage-$(CONFIG_DRIVER_TIS_DEFAULT) += tis.c
romstage-$(CONFIG_DRIVER_TIS_DEFAULT) += tis.c
verstage-$(CONFIG_DRIVER_TIS_DEFAULT) += tis.c
bootblock-$(CONFIG_DRIVER_TIS_DEFAULT) += tis.c

ramstage-$(CONFIG_MAINBOARD_HAS_I2C_TPM_ATMEL) += tis_atmel.c
romstage-$(CONFIG_MAINBOARD_HAS_I2C_TPM_ATMEL) += tis_atmel.c
verstage-$(CONFIG_MAINBOARD_HAS_I2C_TPM_ATMEL) += tis_atmel.c
bootblock-$(CONFIG_MAINBOARD_HAS_I2C_TPM_ATMEL) += tis_atmel.c

ramstage-$(CONFIG_MAINBOARD_HAS_I2C_TPM_GENERIC) += tpm.c
romstage-$(CONFIG_MAINBOARD_HAS_I2C_TPM_GENERIC) += tpm.c
verstage-$(CONFIG_MAINBOARD_HAS_I2C_TPM_GENERIC) += tpm.c
bootblock-$(CONFIG_MAINBOARD_HAS_I2C_TPM_GENERIC) += tpm.c

ramstage-$(CONFIG_MAINBOARD_HAS_I2C_TPM_CR50) += cr50.c
romstage-$(CONFIG_MAINBOARD_HAS_I2C_TPM_CR50) += cr50.c
verstage-$(CONFIG_MAINBOARD_HAS_I2C_TPM_CR50) += cr50.c
bootblock-$(CONFIG_MAINBOARD_HAS_I2C_TPM_CR50) += cr50.c

ramstage-$(CONFIG_DRIVER_I2C_TPM_ACPI) += chip.c
