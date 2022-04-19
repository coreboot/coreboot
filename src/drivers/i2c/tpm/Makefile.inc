ifeq ($(CONFIG_TPM)$(CONFIG_I2C_TPM),yy)

all-$(CONFIG_DRIVER_TIS_DEFAULT) += tis.c

ifeq ($(CONFIG_TPM_ATMEL),y)
all-y += tis_atmel.c
else ifeq ($(CONFIG_TPM_GOOGLE),y)
all-y += cr50.c
else
all-y += tpm.c
endif

endif

ramstage-$(CONFIG_DRIVER_I2C_TPM_ACPI) += chip.c
