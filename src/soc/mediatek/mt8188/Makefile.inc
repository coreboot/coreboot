ifeq ($(CONFIG_SOC_MEDIATEK_MT8188),y)

all-y += ../common/flash_controller.c
all-y += ../common/gpio.c ../common/gpio_op.c gpio.c
all-y += ../common/i2c.c i2c.c
all-y += ../common/pll.c pll.c
all-$(CONFIG_SPI_FLASH) += spi.c
all-y += ../common/timer.c ../common/timer_prepare.c
all-y += ../common/uart.c

bootblock-y += bootblock.c
bootblock-y += ../common/mmu_operations.c
bootblock-y += ../common/wdt.c ../common/wdt_req.c wdt.c

romstage-y += ../common/cbmem.c
romstage-y += emi.c
romstage-y += ../common/mmu_operations.c ../common/mmu_cmops.c
romstage-y += ../common/mt6315.c mt6315.c
romstage-y += ../common/mt6359p.c mt6359p.c
romstage-y += ../common/pmif.c ../common/pmif_clk.c pmif_clk.c
romstage-y += ../common/pmif_spi.c pmif_spi.c
romstage-y += ../common/pmif_spmi.c pmif_spmi.c

ramstage-y += emi.c
ramstage-y += ../common/mmu_operations.c ../common/mmu_cmops.c
ramstage-$(CONFIG_COMMONLIB_STORAGE_MMC) += ../common/msdc.c msdc.c
ramstage-y += ../common/mt6359p.c mt6359p.c
ramstage-y += ../common/pmif.c ../common/pmif_clk.c pmif_clk.c
ramstage-y += ../common/pmif_spi.c pmif_spi.c
ramstage-y += soc.c
ramstage-y += ../common/usb.c usb.c

CPPFLAGS_common += -Isrc/soc/mediatek/mt8188/include
CPPFLAGS_common += -Isrc/soc/mediatek/common/include

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	./util/mtkheader/gen-bl-img.py mt8183 sf $< $@

endif
