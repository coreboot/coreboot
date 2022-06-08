ifeq ($(CONFIG_SOC_MEDIATEK_MT8186),y)

# for bootblock, verstage, romstage, ramstage
all-y += ../common/flash_controller.c
all-y += ../common/gpio.c gpio.c
all-y += ../common/i2c.c i2c.c
all-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
all-y += ../common/timer.c ../common/timer_prepare.c
all-y += ../common/uart.c

bootblock-y += bootblock.c
bootblock-y += ../common/eint_event.c
bootblock-y += gic.c
bootblock-y += ../common/mmu_operations.c
bootblock-y += ../common/pll.c pll.c
bootblock-y += ../common/tracker.c ../common/tracker_v1.c
bootblock-y += ../common/wdt.c ../common/wdt_req.c wdt.c

romstage-y += ../common/cbmem.c
romstage-y += ../common/dram_init.c
romstage-y += ../common/dramc_param.c
romstage-y += emi.c
romstage-y += ../common/memory.c
romstage-y += ../common/memory_test.c
romstage-y += ../common/mmu_operations.c ../common/mmu_cmops.c
romstage-y += ../common/pll.c pll.c
romstage-y += ../common/pmic_wrap.c pmic_wrap.c pmif.c mt6366.c
romstage-y += ../common/rtc.c ../common/rtc_osc_init.c rtc.c

ramstage-y += ../common/auxadc.c
ramstage-y += ../common/ddp.c ddp.c
ramstage-y += devapc.c
ramstage-y += ../common/dfd.c
ramstage-y += ../common/dsi.c ../common/mtk_mipi_dphy.c
ramstage-y += emi.c
ramstage-y += ../common/mcu.c
ramstage-y += ../common/mmu_operations.c ../common/mmu_cmops.c
ramstage-$(CONFIG_COMMONLIB_STORAGE_MMC) += ../common/msdc.c msdc.c
ramstage-y += ../common/mtcmos.c mtcmos.c
ramstage-y += ../common/pmic_wrap.c pmic_wrap.c pmif.c mt6366.c
ramstage-y += ../common/rtc.c ../common/rtc_osc_init.c rtc.c
ramstage-y += soc.c
ramstage-y += ../common/spm.c spm.c
ramstage-y += ../common/sspm.c
ramstage-y += ../common/usb.c usb.c

CPPFLAGS_common += -Isrc/soc/mediatek/mt8186/include
CPPFLAGS_common += -Isrc/soc/mediatek/common/include

BL31_MAKEARGS += PLAT=mt8186

MT8186_BLOB_DIR := 3rdparty/blobs/soc/mediatek/mt8186

mcu-firmware-files := \
	$(CONFIG_SSPM_FIRMWARE) \
	$(CONFIG_SPM_FIRMWARE)

$(foreach fw, $(call strip_quotes,$(mcu-firmware-files)), \
	  $(eval $(fw)-file := $(MT8186_BLOB_DIR)/$(fw)) \
	  $(eval $(fw)-type := raw) \
	  $(eval $(fw)-compression := LZ4) \
	  $(if $(wildcard $($(fw)-file)), $(eval cbfs-files-y += $(fw)), ) \
)

DRAM_CBFS := $(CONFIG_CBFS_PREFIX)/dram
$(DRAM_CBFS)-file := $(MT8186_BLOB_DIR)/dram.elf
$(DRAM_CBFS)-type := stage
$(DRAM_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
ifneq ($(wildcard $($(DRAM_CBFS)-file)),)
	cbfs-files-y += $(DRAM_CBFS)
endif

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	./util/mtkheader/gen-bl-img.py mt8183 sf $< $@

endif
