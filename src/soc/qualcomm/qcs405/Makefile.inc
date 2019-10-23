
ifeq ($(CONFIG_SOC_QUALCOMM_QCS405),y)

################################################################################
bootblock-y += bootblock.c
bootblock-y += timer.c
bootblock-y += spi.c
bootblock-y += mmu.c
bootblock-y += gpio.c
bootblock-y += clock.c
bootblock-y += i2c.c
bootblock-y += qup.c
bootblock-y += blsp.c
bootblock-$(CONFIG_DRIVERS_UART) += uart.c

################################################################################
verstage-y += timer.c
verstage-y += spi.c
verstage-y += gpio.c
verstage-y += clock.c
verstage-y += i2c.c
verstage-y += qup.c
verstage-y += blsp.c
verstage-$(CONFIG_DRIVERS_UART) += uart.c

################################################################################
romstage-y += timer.c
romstage-y += spi.c
romstage-y += cbmem.c
romstage-y += gpio.c
romstage-y += clock.c
romstage-y += usb.c
romstage-$(CONFIG_DRIVERS_UART) += uart.c
romstage-y += i2c.c
romstage-y += qup.c
romstage-y += blsp.c

################################################################################
ramstage-y += soc.c
ramstage-y += timer.c
ramstage-y += spi.c
ramstage-y += gpio.c
ramstage-y += clock.c
ramstage-y += i2c.c
ramstage-y += qup.c
ramstage-y += blsp.c
ramstage-y += usb.c
ramstage-$(CONFIG_DRIVERS_UART) += uart.c

################################################################################

CPPFLAGS_common += -Isrc/soc/qualcomm/qcs405/include

endif
