ifeq ($(CONFIG_SOC_MEDIATEK_MT8192),y)

bootblock-y += ../common/auxadc.c
bootblock-y += bootblock.c
bootblock-y += flash_controller.c
bootblock-y += ../common/gpio.c gpio.c
bootblock-y += ../common/i2c.c i2c.c
bootblock-y += ../common/mmu_operations.c
bootblock-y += ../common/pll.c pll.c
bootblock-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
bootblock-y += ../common/timer.c
bootblock-y += ../common/uart.c
bootblock-y += ../common/wdt.c
bootblock-y += pmif.c pmif_clk.c pmif_spi.c pmif_spmi.c
bootblock-y += ../common/rtc.c ../common/rtc_osc_init.c rtc.c
bootblock-y += mt6315.c
bootblock-y += mt6359p.c

verstage-y += ../common/auxadc.c
verstage-y += flash_controller.c
verstage-y += ../common/gpio.c gpio.c
verstage-y += ../common/i2c.c i2c.c
verstage-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
verstage-y += ../common/timer.c
verstage-y += ../common/uart.c

romstage-y += ../common/auxadc.c
romstage-y += ../common/cbmem.c
romstage-y += dramc_pi_main.c dramc_pi_basic_api.c dramc_pi_calibration_api.c dramc_utility.c
romstage-y += emi.c
romstage-y += flash_controller.c
romstage-y += ../common/gpio.c gpio.c
romstage-y += ../common/i2c.c i2c.c
romstage-y += ../common/mmu_operations.c mmu_operations.c
romstage-y += memory.c dramc_param.c ../common/memory_test.c
romstage-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
romstage-y += ../common/timer.c
romstage-y += ../common/uart.c
romstage-y += pmif.c pmif_clk.c pmif_spi.c pmif_spmi.c
romstage-y += mt6359p.c

ramstage-y += ../common/auxadc.c
ramstage-y += ../common/ddp.c ddp.c
ramstage-y += devapc.c
ramstage-y += dpm.c
ramstage-y += ../common/dsi.c ../common/mtk_mipi_dphy.c
ramstage-y += flash_controller.c
ramstage-y += ../common/gpio.c gpio.c
ramstage-y += ../common/i2c.c i2c.c
ramstage-y += emi.c
ramstage-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
ramstage-y += ../common/mcu.c
ramstage-y += mcupm.c
ramstage-y += ../common/mmu_operations.c mmu_operations.c
ramstage-y += ../common/mtcmos.c mtcmos.c
ramstage-y += soc.c
ramstage-y += spm.c
ramstage-y += sspm.c
ramstage-y += ../common/timer.c
ramstage-y += ../common/uart.c
ramstage-y += ufs.c
ramstage-y += ../common/usb.c usb.c

MT8192_BLOB_DIR := 3rdparty/blobs/soc/mediatek/mt8192

mcu-firmware-files := \
	$(CONFIG_DPM_DM_FIRMWARE) \
	$(CONFIG_DPM_PM_FIRMWARE) \
	$(CONFIG_MCUPM_FIRMWARE) \
	$(CONFIG_SSPM_FIRMWARE) \
	$(CONFIG_SPM_FIRMWARE)

$(foreach fw, $(call strip_quotes,$(mcu-firmware-files)), \
	  $(eval $(fw)-file := $(MT8192_BLOB_DIR)/$(fw)) \
	  $(eval $(fw)-type := raw) \
	  $(eval $(fw)-compression := $(CBFS_COMPRESS_FLAG)) \
	  $(if $(wildcard $($(fw)-file)), $(eval cbfs-files-y += $(fw)), ) \
)

DRAM_CBFS := $(CONFIG_CBFS_PREFIX)/dram
$(DRAM_CBFS)-file := $(MT8192_BLOB_DIR)/dram.elf
$(DRAM_CBFS)-type := stage
$(DRAM_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
ifneq ($(wildcard $($(DRAM_CBFS)-file)),)
        cbfs-files-y += $(DRAM_CBFS)
endif

BL31_MAKEARGS += PLAT=mt8192

CPPFLAGS_common += -Isrc/soc/mediatek/mt8192/include
CPPFLAGS_common += -Isrc/soc/mediatek/common/include

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	./util/mtkheader/gen-bl-img.py mt8183 sf $< $@

endif
