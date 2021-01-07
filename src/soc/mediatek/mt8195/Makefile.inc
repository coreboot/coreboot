ifeq ($(CONFIG_SOC_MEDIATEK_MT8195),y)

bootblock-y += bootblock.c
bootblock-y += ../common/mmu_operations.c
bootblock-$(CONFIG_SPI_FLASH) += spi.c
bootblock-y += ../common/timer.c
bootblock-y += ../common/uart.c

verstage-$(CONFIG_SPI_FLASH) += spi.c
verstage-y += ../common/timer.c
verstage-y += ../common/uart.c

romstage-y += ../common/cbmem.c
romstage-y += emi.c
romstage-$(CONFIG_SPI_FLASH) += spi.c
romstage-y += ../common/timer.c
romstage-y += ../common/uart.c

ramstage-y += emi.c
ramstage-$(CONFIG_SPI_FLASH) += spi.c
ramstage-y += soc.c
ramstage-y += ../common/timer.c
ramstage-y += ../common/uart.c

CPPFLAGS_common += -Isrc/soc/mediatek/mt8195/include
CPPFLAGS_common += -Isrc/soc/mediatek/common/include

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	./util/mtkheader/gen-bl-img.py mt8183 sf $< $@

endif
