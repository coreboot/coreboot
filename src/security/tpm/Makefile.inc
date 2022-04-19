subdirs-$(CONFIG_TPM_GOOGLE) += tss/vendor/cr50

## TSS

ifeq ($(CONFIG_TPM1),y)

ramstage-y += tss/tcg-1.2/tss.c
romstage-y += tss/tcg-1.2/tss.c
bootblock-y += tss/tcg-1.2/tss.c
verstage-y += tss/tcg-1.2/tss.c
postcar-y += tss/tcg-1.2/tss.c

## TSPI

ramstage-y += tspi/tspi.c
romstage-y += tspi/tspi.c
bootblock-y += tspi/tspi.c
verstage-y += tspi/tspi.c
postcar-y += tspi/tspi.c

endif # CONFIG_TPM1

ifeq ($(CONFIG_TPM2),y)

ramstage-y += tss/tcg-2.0/tss_marshaling.c
ramstage-y += tss/tcg-2.0/tss.c

romstage-y += tss/tcg-2.0/tss_marshaling.c
romstage-y += tss/tcg-2.0/tss.c

verstage-$(CONFIG_VBOOT) += tss/tcg-2.0/tss_marshaling.c
verstage-$(CONFIG_VBOOT) += tss/tcg-2.0/tss.c

postcar-y += tss/tcg-2.0/tss_marshaling.c
postcar-y += tss/tcg-2.0/tss.c

bootblock-y += tss/tcg-2.0/tss_marshaling.c
bootblock-y += tss/tcg-2.0/tss.c

## TSPI

ramstage-y += tspi/tspi.c
romstage-y += tspi/tspi.c
bootblock-y += tspi/tspi.c
verstage-$(CONFIG_VBOOT) += tspi/tspi.c
postcar-y += tspi/tspi.c

endif # CONFIG_TPM2

ifeq ($(CONFIG_TPM_MEASURED_BOOT),y)

bootblock-y += tspi/crtm.c
verstage-y += tspi/crtm.c
romstage-y += tspi/crtm.c
ramstage-y += tspi/crtm.c
postcar-y += tspi/crtm.c

ramstage-y += tspi/log.c
romstage-y += tspi/log.c
verstage-y += tspi/log.c
postcar-y += tspi/log.c
bootblock-y += tspi/log.c

endif # CONFIG_TPM_MEASURED_BOOT
