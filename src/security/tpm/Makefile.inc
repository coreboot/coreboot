verstage-$(CONFIG_TPM) += tcg-1.2/tlcl.c
verstage-$(CONFIG_TPM2) += tcg-2.0/tpm2_marshaling.c
verstage-$(CONFIG_TPM2) += tcg-2.0/tpm2_tlcl.c

ifeq ($(CONFIG_VBOOT_SEPARATE_VERSTAGE),y)
romstage-$(CONFIG_TPM) += tcg-1.2/tlcl.c
romstage-$(CONFIG_TPM2) += tcg-2.0/tpm2_marshaling.c
romstage-$(CONFIG_TPM2) += tcg-2.0/tpm2_tlcl.c
endif # CONFIG_VBOOT_SEPARATE_VERSTAGE

ramstage-$(CONFIG_TPM2) += tcg-2.0/tpm2_marshaling.c
ramstage-$(CONFIG_TPM2) += tcg-2.0/tpm2_tlcl.c
