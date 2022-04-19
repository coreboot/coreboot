ifeq ($(CONFIG_MEMORY_MAPPED_TPM),y)
all-y += tis.c
endif
