ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_S3),y)

romstage-$(CONFIG_HAVE_ACPI_RESUME) += s3_resume.c
ramstage-$(CONFIG_HAVE_ACPI_RESUME) += s3_resume.c

endif
