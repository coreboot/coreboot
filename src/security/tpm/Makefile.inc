verstage-$(CONFIG_TPM) += tcg-1.2/tss.c
verstage-$(CONFIG_TPM2) += tcg-2.0/tss_marshaling.c
verstage-$(CONFIG_TPM2) += tcg-2.0/tss.c

ifeq ($(CONFIG_VBOOT_SEPARATE_VERSTAGE),y)
romstage-$(CONFIG_TPM) += tcg-1.2/tss.c
romstage-$(CONFIG_TPM2) += tcg-2.0/tss_marshaling.c
romstage-$(CONFIG_TPM2) += tcg-2.0/tss.c
endif # CONFIG_VBOOT_SEPARATE_VERSTAGE

ramstage-$(CONFIG_TPM2) += tcg-2.0/tss_marshaling.c
ramstage-$(CONFIG_TPM2) += tcg-2.0/tss.c
