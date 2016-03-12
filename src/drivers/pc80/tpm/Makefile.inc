ifeq ($(CONFIG_ARCH_X86),y)

verstage-$(CONFIG_LPC_TPM) += tpm.c
romstage-$(CONFIG_LPC_TPM) += tpm.c
ramstage-$(CONFIG_LPC_TPM) += tpm.c
romstage-$(CONFIG_LPC_TPM) += romstage.c

endif
