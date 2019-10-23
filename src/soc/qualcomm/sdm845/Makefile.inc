
ifeq ($(CONFIG_SOC_QUALCOMM_SDM845),y)

################################################################################
bootblock-y += bootblock.c
bootblock-y += spi.c
bootblock-y += mmu.c
bootblock-y += timer.c
bootblock-y += gpio.c
bootblock-y += clock.c
bootblock-$(CONFIG_SDM845_QSPI) += qspi.c

################################################################################
verstage-y += spi.c
verstage-y += timer.c
verstage-y += gpio.c
verstage-y += clock.c
verstage-$(CONFIG_SDM845_QSPI) += qspi.c

################################################################################
romstage-y += spi.c
romstage-y += cbmem.c
romstage-y += timer.c
romstage-y += gpio.c
romstage-y += clock.c
romstage-$(CONFIG_SDM845_QSPI) += qspi.c
romstage-y += usb.c
romstage-y += ../common/qclib.c
romstage-y += qclib.c
romstage-y += ../common/mmu.c
romstage-y += mmu.c

################################################################################
ramstage-y += soc.c
ramstage-y += spi.c
ramstage-y += timer.c
ramstage-y += gpio.c
ramstage-y += clock.c
ramstage-$(CONFIG_SDM845_QSPI) += qspi.c
ramstage-y += usb.c
ramstage-y += gpio.c
ramstage-y += clock.c
ramstage-y += aop_load_reset.c

################################################################################

CPPFLAGS_common += -Isrc/soc/qualcomm/sdm845/include
CPPFLAGS_common += -Isrc/soc/qualcomm/common/include

endif
