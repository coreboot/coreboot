ifeq ($(CONFIG_ARCH_X86),y)

verstage-$(CONFIG_LPC_TPM) += tis.c
romstage-$(CONFIG_LPC_TPM) += tis.c
ramstage-$(CONFIG_LPC_TPM) += tis.c
romstage-$(CONFIG_LPC_TPM) += romstage.c

endif
