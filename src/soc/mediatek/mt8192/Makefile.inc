ifeq ($(CONFIG_SOC_MEDIATEK_MT8192),y)

bootblock-y += bootblock.c
bootblock-y += flash_controller.c
bootblock-y += ../common/gpio.c gpio.c
bootblock-y += ../common/mmu_operations.c
bootblock-y += ../common/pll.c pll.c
bootblock-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
bootblock-y += ../common/timer.c
bootblock-y += ../common/uart.c
bootblock-y += ../common/wdt.c

verstage-y += flash_controller.c
verstage-y += ../common/gpio.c gpio.c
verstage-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
verstage-y += ../common/timer.c
verstage-y += ../common/uart.c

romstage-y += ../common/cbmem.c
romstage-y += emi.c
romstage-y += flash_controller.c
romstage-y += ../common/gpio.c gpio.c
romstage-y += ../common/mmu_operations.c
romstage-y += memory.c dramc_param.c ../common/memory_test.c
romstage-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
romstage-y += ../common/timer.c
romstage-y += ../common/uart.c

ramstage-y += flash_controller.c
ramstage-y += ../common/gpio.c gpio.c
ramstage-y += emi.c
ramstage-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
ramstage-y += ../common/mtcmos.c mtcmos.c
ramstage-y += soc.c
ramstage-y += ../common/timer.c
ramstage-y += ../common/uart.c
ramstage-y += ../common/usb.c usb.c

CPPFLAGS_common += -Isrc/soc/mediatek/mt8192/include
CPPFLAGS_common += -Isrc/soc/mediatek/common/include

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	./util/mtkheader/gen-bl-img.py mt8183 sf $< $@

endif
