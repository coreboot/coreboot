ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_S3),y)

romstage-y += s3_resume.c
ramstage-y += s3_resume.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_S3
