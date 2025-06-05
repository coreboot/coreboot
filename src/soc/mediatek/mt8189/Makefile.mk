## SPDX-License-Identifier: GPL-2.0-only OR MIT

ifeq ($(CONFIG_SOC_MEDIATEK_MT8189),y)

all-y += ../common/flash_controller.c
all-y += ../common/gpio.c ../common/gpio_op.c ../common/gpio_eint_v2.c gpio.c gpio_eint.c
all-y += ../common/i2c.c ../common/i2c_common.c i2c.c
all-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
all-y += ../common/timer_prepare.c timer.c
all-y += ../common/uart.c

bootblock-y += bootblock.c
bootblock-y += efuse.c
bootblock-y += ../common/mmu_operations.c
bootblock-y += ../common/mtcmos.c mtcmos.c
bootblock-y += ../common/pll.c pll.c
bootblock-y += ../common/wdt.c ../common/wdt_req.c wdt.c

romstage-y += ../common/cbmem.c
romstage-y += ../common/cpu_id.c ../common/cpu_segment_id.c
romstage-y += ../common/dram_init.c
romstage-y += ../common/dramc_param.c
romstage-y += dvfs.c
romstage-y += ../common/emi.c
romstage-y += ../common/memory.c ../common/memory_test.c
romstage-y += ../common/mmu_operations.c ../common/mmu_cmops.c
romstage-y += ../common/mt6315.c mt6315.c
romstage-y += ../common/mt6359p.c mt6359p.c
romstage-y += ../common/pll.c pll.c
romstage-y += ../common/pmif.c ../common/pmif_clk.c ../common/pmif_init.c pmif_clk.c
romstage-y += ../common/pmif_spi.c pmif_spi.c
romstage-y += ../common/pmif_spmi.c pmif_spmi.c
romstage-y += ../common/rtc.c ../common/rtc_osc_init.c ../common/rtc_mt6359p.c

ramstage-y += ../common/auxadc.c
ramstage-$(CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE) += ../common/bl31.c
ramstage-y += ../common/dpm.c ../common/dpm_v2.c
ramstage-y += ../common/dramc_info.c
ramstage-y += ../common/emi.c
ramstage-y += ../common/mcu.c mcupm.c
ramstage-y += ../common/memory.c
ramstage-y += ../common/mmu_operations.c ../common/mmu_cmops.c
ramstage-$(CONFIG_COMMONLIB_STORAGE_MMC) += ../common/msdc.c msdc.c
ramstage-y += ../common/mt6315.c mt6315.c
ramstage-y += ../common/mt6359p.c mt6359p.c
ramstage-y += ../common/mtcmos.c mtcmos.c
ramstage-y += ../common/mtk_fsp.c
ramstage-y += ../common/pmif.c ../common/pmif_clk.c ../common/pmif_init.c pmif_clk.c
ramstage-y += ../common/pmif_spi.c pmif_spi.c
ramstage-y += ../common/pmif_spmi.c pmif_spmi.c
ramstage-y += ../common/rtc.c ../common/rtc_mt6359p.c ../common/rtc_osc_init.c
ramstage-y += soc.c
ramstage-y += ../common/spm.c ../common/spm_v2.c spm.c
ramstage-y += ../common/sspm.c ../common/sspm_sram.c
ramstage-y += ../common/usb.c ../common/usb_secondary.c usb.c

BL31_MAKEARGS += PLAT=mt8189

CPPFLAGS_common += -Isrc/soc/mediatek/mt8189/include
CPPFLAGS_common += -Isrc/soc/mediatek/common/include

MT8189_BLOB_DIR := 3rdparty/blobs/soc/mediatek/mt8189

firmware-files := \
	$(CONFIG_DPM_DM_FIRMWARE) \
	$(CONFIG_DPM_PM_FIRMWARE) \
	$(CONFIG_MCUPM_FIRMWARE) \
	$(CONFIG_SPM_FIRMWARE) \
	$(CONFIG_SSPM_FIRMWARE)

$(foreach fw, $(call strip_quotes,$(firmware-files)), \
	$(eval $(fw)-file := $(MT8189_BLOB_DIR)/$(fw)) \
	$(eval $(fw)-type := raw) \
	$(eval $(fw)-compression := LZ4) \
	$(if $(wildcard $($(fw)-file)), $(eval cbfs-files-y += $(fw)), ) \
)

DRAM_CBFS := $(CONFIG_CBFS_PREFIX)/dram
$(DRAM_CBFS)-file := $(MT8189_BLOB_DIR)/dram.elf
$(DRAM_CBFS)-type := stage
$(DRAM_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
ifneq ($(wildcard $($(DRAM_CBFS)-file)),)
	cbfs-files-y += $(DRAM_CBFS)
endif
$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	./util/mediatek/gen-bl-img.py mt8189 sf $< $@

endif

FSP_CBFS := $(CONFIG_CBFS_PREFIX)/mtk_fsp_ramstage
$(FSP_CBFS)-file := $(MT8189_BLOB_DIR)/mtk_fsp_ramstage.elf
$(FSP_CBFS)-type := stage
$(FSP_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
ifneq ($(wildcard $($(FSP_CBFS)-file)),)
	cbfs-files-y += $(FSP_CBFS)
endif
