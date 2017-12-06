ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_S3),y)

ramstage-$(CONFIG_HAVE_ACPI_RESUME) += spi.c

endif
