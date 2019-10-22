
ifeq ($(CONFIG_SOC_QUALCOMM_SC7180),y)

################################################################################
bootblock-y += bootblock.c
bootblock-y += mmu.c
bootblock-y += timer.c
bootblock-y += spi.c
bootblock-y += gpio.c
bootblock-$(CONFIG_DRIVERS_UART) += uart_bitbang.c
bootblock-y += clock.c
bootblock-$(CONFIG_SC7180_QSPI) += qspi.c

################################################################################
verstage-y += timer.c
verstage-y += spi.c
verstage-y += gpio.c
verstage-$(CONFIG_DRIVERS_UART) += uart_bitbang.c
verstage-y += clock.c
verstage-$(CONFIG_SC7180_QSPI) += qspi.c

################################################################################
romstage-y += cbmem.c
romstage-y += timer.c
romstage-y += ../common/qclib.c
romstage-y += qclib.c
romstage-y += ../common/mmu.c
romstage-y += mmu.c
romstage-y += usb.c
romstage-y += spi.c
romstage-y += gpio.c
romstage-$(CONFIG_DRIVERS_UART) += uart_bitbang.c
romstage-y += clock.c
romstage-$(CONFIG_SC7180_QSPI) += qspi.c

################################################################################
ramstage-y += soc.c
ramstage-y += timer.c
ramstage-y += spi.c
ramstage-y += gpio.c
ramstage-$(CONFIG_DRIVERS_UART) += uart_bitbang.c
ramstage-y += clock.c
ramstage-$(CONFIG_SC7180_QSPI) += qspi.c
ramstage-y += aop_load_reset.c
ramstage-y += usb.c

################################################################################

CPPFLAGS_common += -Isrc/soc/qualcomm/sc7180/include
CPPFLAGS_common += -Isrc/soc/qualcomm/common/include

################################################################################

endif
