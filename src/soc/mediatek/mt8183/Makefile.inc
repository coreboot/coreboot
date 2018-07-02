ifeq ($(CONFIG_SOC_MEDIATEK_MT8183),y)

bootblock-y += bootblock.c
bootblock-y += ../common/gpio.c gpio.c
bootblock-y += ../common/pll.c pll.c
bootblock-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
bootblock-y += ../common/timer.c
ifeq ($(CONFIG_BOOTBLOCK_CONSOLE),y)
bootblock-$(CONFIG_DRIVERS_UART) += ../common/uart.c
endif
bootblock-y += ../common/wdt.c

decompressor-y += decompressor.c
decompressor-y += ../common/mmu_operations.c
decompressor-y += ../common/timer.c

verstage-y += ../common/gpio.c gpio.c
verstage-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
verstage-y += ../common/timer.c
verstage-$(CONFIG_DRIVERS_UART) += ../common/uart.c
verstage-y += ../common/wdt.c

romstage-y += emi.c
romstage-y += ../common/gpio.c gpio.c
romstage-y += ../common/mmu_operations.c mmu_operations.c
romstage-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
romstage-y += ../common/timer.c
romstage-$(CONFIG_DRIVERS_UART) += ../common/uart.c
romstage-y += ../common/wdt.c

ramstage-y += ../common/cbmem.c emi.c
ramstage-y += ../common/gpio.c gpio.c
ramstage-y += ../common/mmu_operations.c mmu_operations.c
ramstage-y += ../common/mtcmos.c mtcmos.c
ramstage-y += soc.c
ramstage-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
ramstage-y += ../common/timer.c
ramstage-$(CONFIG_DRIVERS_UART) += ../common/uart.c
ramstage-y += ../common/wdt.c

CPPFLAGS_common += -Isrc/soc/mediatek/mt8183/include
CPPFLAGS_common += -Isrc/soc/mediatek/common/include

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	./util/mtkheader/gen-bl-img.py mt8183 emmc $< $@

endif
