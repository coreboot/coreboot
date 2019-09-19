
ifeq ($(CONFIG_SOC_QUALCOMM_SC7180),y)

################################################################################
bootblock-y += bootblock.c
bootblock-y += mmu.c
bootblock-y += timer.c
bootblock-y += spi.c
bootblock-y += qupv3_spi.c
bootblock-y += gpio.c
bootblock-y += qupv3_i2c.c
bootblock-$(CONFIG_DRIVERS_UART) += uart_bitbang.c
bootblock-y += clock.c
bootblock-$(CONFIG_SC7180_QSPI) += qspi.c
bootblock-y += qupv3_config.c
bootblock-y += qcom_qup_se.c

################################################################################
verstage-y += timer.c
verstage-y += spi.c
verstage-y += qupv3_spi.c
verstage-y += gpio.c
verstage-y += qupv3_i2c.c
verstage-y += clock.c
verstage-$(CONFIG_SC7180_QSPI) += qspi.c
verstage-y += qcom_qup_se.c
verstage-y += qupv3_config.c
verstage-$(CONFIG_DRIVERS_UART) += qupv3_uart.c

################################################################################
romstage-y += cbmem.c
romstage-y += timer.c
romstage-y += ../common/qclib.c
romstage-y += qclib.c
romstage-y += ../common/mmu.c
romstage-y += mmu.c
romstage-y += usb.c
romstage-y += spi.c
romstage-y += qupv3_spi.c
romstage-y += gpio.c
romstage-y += qupv3_i2c.c
romstage-y += clock.c
romstage-$(CONFIG_SC7180_QSPI) += qspi.c
romstage-y += qcom_qup_se.c
romstage-y += qupv3_config.c
romstage-$(CONFIG_DRIVERS_UART) += qupv3_uart.c

################################################################################
ramstage-y += soc.c
ramstage-y += timer.c
ramstage-y += spi.c
ramstage-y += qupv3_spi.c
ramstage-y += gpio.c
ramstage-y += qupv3_i2c.c
ramstage-y += clock.c
ramstage-$(CONFIG_SC7180_QSPI) += qspi.c
ramstage-y += aop_load_reset.c
ramstage-y += usb.c
ramstage-y += qupv3_config.c
ramstage-y += qcom_qup_se.c
ramstage-$(CONFIG_DRIVERS_UART) += qupv3_uart.c

################################################################################

CPPFLAGS_common += -Isrc/soc/qualcomm/sc7180/include
CPPFLAGS_common += -Isrc/soc/qualcomm/common/include

################################################################################

endif
