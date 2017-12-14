subdirs-$(CONFIG_TPM_CR50) += tss/vendor/cr50

## TSS

ifeq ($(CONFIG_TPM1),y)

ramstage-y += tss/tcg-1.2/tss.c
romstage-y += tss/tcg-1.2/tss.c

verstage-$(CONFIG_VBOOT) += tss/tcg-1.2/tss.c
postcar-$(CONFIG_VBOOT) += tss/tcg-1.2/tss.c

## TSPI

ramstage-y += tspi/tspi.c tspi/log.c
romstage-y += tspi/tspi.c tspi/log.c

verstage-$(CONFIG_VBOOT) += tspi/tspi.c tspi/log.c
postcar-$(CONFIG_VBOOT) += tspi/tspi.c tspi/log.c

endif # CONFIG_TPM1

ifeq ($(CONFIG_TPM2),y)

ramstage-y += tss/tcg-2.0/tss_marshaling.c
ramstage-y += tss/tcg-2.0/tss.c

romstage-y += tss/tcg-2.0/tss_marshaling.c
romstage-y += tss/tcg-2.0/tss.c

verstage-$(CONFIG_VBOOT) += tss/tcg-2.0/tss_marshaling.c
verstage-$(CONFIG_VBOOT) += tss/tcg-2.0/tss.c

postcar-$(CONFIG_VBOOT) += tss/tcg-2.0/tss_marshaling.c
postcar-$(CONFIG_VBOOT) += tss/tcg-2.0/tss.c

## TSPI

ramstage-y += tspi/tspi.c tspi/log.c
romstage-y += tspi/tspi.c tspi/log.c

verstage-$(CONFIG_VBOOT) += tspi/tspi.c tspi/log.c
postcar-$(CONFIG_VBOOT) += tspi/tspi.c tspi/log.c

endif # CONFIG_TPM2
