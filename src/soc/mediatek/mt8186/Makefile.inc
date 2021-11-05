ifeq ($(CONFIG_SOC_MEDIATEK_MT8186),y)

bootblock-y += ../common/auxadc.c
bootblock-y += bootblock.c
bootblock-y += ../common/eint_event.c
bootblock-y += ../common/flash_controller.c
bootblock-y += gic.c
bootblock-y += ../common/gpio.c gpio.c
bootblock-y += ../common/mmu_operations.c
bootblock-y += ../common/pll.c pll.c
bootblock-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
bootblock-y += ../common/timer.c timer.c
bootblock-y += ../common/uart.c
bootblock-y += ../common/wdt.c wdt.c

verstage-y += ../common/auxadc.c
verstage-y += ../common/flash_controller.c
verstage-y += ../common/gpio.c gpio.c
verstage-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
verstage-y += ../common/timer.c timer.c
verstage-y += ../common/uart.c
verstage-y += ../common/wdt.c wdt.c

romstage-y += ../common/auxadc.c
romstage-y += ../common/cbmem.c
romstage-y += emi.c
romstage-y += ../common/flash_controller.c
romstage-y += ../common/gpio.c gpio.c
romstage-y += ../common/mmu_operations.c ../common/mmu_cmops.c
romstage-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
romstage-y += ../common/timer.c timer.c
romstage-y += ../common/uart.c
romstage-y += ../common/wdt.c wdt.c
romstage-y += ../common/pmic_wrap.c pmic_wrap.c mt6366.c

ramstage-y += ../common/auxadc.c
ramstage-y += emi.c
ramstage-y += ../common/flash_controller.c
ramstage-y += ../common/gpio.c gpio.c
ramstage-y += ../common/mmu_operations.c ../common/mmu_cmops.c
ramstage-$(CONFIG_COMMONLIB_STORAGE_MMC) += ../common/msdc.c msdc.c
ramstage-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
ramstage-y += soc.c
ramstage-y += ../common/timer.c timer.c
ramstage-y += ../common/uart.c
ramstage-y += ../common/usb.c usb.c
ramstage-y += ../common/wdt.c wdt.c
ramstage-y += ../common/pmic_wrap.c pmic_wrap.c mt6366.c

CPPFLAGS_common += -Isrc/soc/mediatek/mt8186/include
CPPFLAGS_common += -Isrc/soc/mediatek/common/include

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	./util/mtkheader/gen-bl-img.py mt8183 sf $< $@

endif
