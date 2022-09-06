ifeq ($(CONFIG_SOC_MEDIATEK_MT8183),y)

bootblock-y += bootblock.c
bootblock-y += ../common/auxadc.c
bootblock-y += ../common/gpio.c gpio.c
bootblock-y += ../common/pll.c pll.c
bootblock-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
bootblock-y += mt8183.c
bootblock-y += ../common/i2c.c i2c.c
bootblock-y += ../common/timer.c
bootblock-y += ../common/uart.c
bootblock-y += ../common/wdt.c

decompressor-y += decompressor.c
decompressor-y += ../common/mmu_operations.c
decompressor-y += ../common/timer.c

verstage-y += ../common/auxadc.c
verstage-y += ../common/gpio.c gpio.c
verstage-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
verstage-y += mt8183.c
verstage-y += ../common/i2c.c i2c.c
verstage-y += ../common/timer.c
verstage-y += ../common/uart.c
verstage-y += ../common/wdt.c

romstage-y += ../common/auxadc.c
romstage-y += ../common/cbmem.c emi.c
romstage-y += dramc_init_setting.c
romstage-y += dramc_param.c
romstage-y += dramc_pi_basic_api.c
romstage-y += dramc_pi_calibration_api.c
romstage-y += memory.c
romstage-$(CONFIG_MEMORY_TEST) += ../common/memory_test.c
romstage-y += mt8183.c
romstage-y += ../common/gpio.c gpio.c
romstage-y += ../common/mmu_operations.c mmu_operations.c
romstage-y += ../common/pll.c pll.c
romstage-y += ../common/pmic_wrap.c pmic_wrap.c mt6358.c
romstage-y += ../common/rtc.c ../common/rtc_osc_init.c rtc.c
romstage-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
romstage-y += ../common/i2c.c i2c.c
romstage-y += ../common/timer.c
romstage-y += ../common/uart.c
romstage-y += ../common/wdt.c

ramstage-$(CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE) += ../common/bl31.c
ramstage-y += emi.c
ramstage-y += ../common/auxadc.c
ramstage-y += ../common/ddp.c ddp.c
ramstage-y += ../common/dsi.c ../common/mtk_mipi_dphy.c
ramstage-y += ../common/gpio.c gpio.c
ramstage-y += ../common/i2c.c i2c.c
ramstage-y += ../common/mcu.c
ramstage-y += ../common/mmu_operations.c mmu_operations.c
ramstage-y += ../common/mtcmos.c mtcmos.c
ramstage-y += ../common/pmic_wrap.c
ramstage-y += ../common/rtc.c ../common/rtc_osc_init.c rtc.c
ramstage-y += soc.c
ramstage-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
ramstage-y += spm.c
ramstage-y += ../common/sspm.c
ramstage-y += ../common/timer.c
ramstage-y += ../common/uart.c
ramstage-y += ../common/usb.c
ramstage-y += ../common/wdt.c
ramstage-y += md_ctrl.c

MT8183_BLOB_DIR := 3rdparty/blobs/soc/mediatek/mt8183

cbfs-files-y += pcm_allinone_lp4_3200.bin
pcm_allinone_lp4_3200.bin-file := $(MT8183_BLOB_DIR)/pcm_allinone_lp4_3200.bin
pcm_allinone_lp4_3200.bin-type := raw
pcm_allinone_lp4_3200.bin-compression := $(CBFS_COMPRESS_FLAG)

cbfs-files-y += pcm_allinone_lp4_3733.bin
pcm_allinone_lp4_3733.bin-file := $(MT8183_BLOB_DIR)/pcm_allinone_lp4_3733.bin
pcm_allinone_lp4_3733.bin-type := raw
pcm_allinone_lp4_3733.bin-compression := $(CBFS_COMPRESS_FLAG)

cbfs-files-y += sspm.bin
sspm.bin-file := $(MT8183_BLOB_DIR)/sspm.bin
sspm.bin-type := raw
sspm.bin-compression := $(CBFS_COMPRESS_FLAG)

DRAM_CBFS := $(CONFIG_CBFS_PREFIX)/dram
$(DRAM_CBFS)-file := $(MT8183_BLOB_DIR)/dram.elf
$(DRAM_CBFS)-type := stage
$(DRAM_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
ifneq ($(wildcard $($(DRAM_CBFS)-file)),)
	cbfs-files-y += $(DRAM_CBFS)
endif

BL31_MAKEARGS += PLAT=mt8183

CPPFLAGS_common += -Isrc/soc/mediatek/mt8183/include
CPPFLAGS_common += -Isrc/soc/mediatek/common/include

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	./util/mtkheader/gen-bl-img.py mt8183 emmc $< $@

endif
