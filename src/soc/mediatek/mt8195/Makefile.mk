## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_MEDIATEK_MT8195),y)

# for bootblock, verstage, romstage, ramstage
all-y += ../common/flash_controller.c
all-y += ../common/gpio_eint_v1.c ../common/gpio.c ../common/gpio_op.c gpio.c
all-y += ../common/i2c.c i2c.c
all-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
all-y += ../common/timer.c ../common/timer_prepare.c
all-y += ../common/uart.c

bootblock-y += bootblock.c
bootblock-y += ../common/early_init.c
bootblock-y += ../common/eint_event.c
bootblock-y += ../common/mmu_operations.c
bootblock-$(CONFIG_PCI) += ../common/pcie.c pcie.c
bootblock-y += ../common/pll.c pll.c
bootblock-y += ../common/tracker.c ../common/tracker_v2.c
bootblock-y += ../common/wdt.c ../common/wdt_req.c wdt.c

romstage-y += ../common/cbmem.c
romstage-y += ../common/clkbuf.c
romstage-y += ../common/dram_init.c
romstage-y += ../common/dramc_param.c
romstage-y += emi.c
romstage-y += ../common/l2c_ops.c
romstage-y += ../common/memory.c
romstage-y += ../common/memory_test.c
romstage-y += ../common/mmu_operations.c ../common/mmu_cmops.c
romstage-y += ../common/pll.c pll.c
romstage-y += scp.c
romstage-y += ../common/pmif.c ../common/pmif_clk.c ../common/pmif_init.c pmif_clk.c
romstage-y += ../common/pmif_spi.c pmif_spi.c
romstage-y += ../common/pmif_spmi.c pmif_spmi.c
romstage-y += ../common/mt6315.c mt6315.c
romstage-y += ../common/mt6359p.c mt6359p.c
romstage-y += mt6360.c
romstage-y += mt6691.c
romstage-$(CONFIG_PCI) += ../common/early_init.c ../common/pcie.c
romstage-y += ../common/rtc.c ../common/rtc_osc_init.c ../common/rtc_mt6359p.c

ramstage-y += apusys.c
ramstage-y += apusys_devapc.c
ramstage-y += ../common/auxadc.c
ramstage-$(CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE) += ../common/bl31.c
ramstage-y += ../common/early_init.c
ramstage-y += ../common/ddp.c ddp.c
ramstage-y += ../common/devapc.c devapc.c
ramstage-y += ../common/dfd.c
ramstage-y += ../common/display.c
ramstage-y += ../common/dpm_v1.c
ramstage-$(CONFIG_DPM_FOUR_CHANNEL) += ../common/dpm_4ch.c
ramstage-y += ../common/dp/dptx_common.c ../common/dp/dptx_hal_common.c
ramstage-y += ../common/dp/dp_intf.c ../common/dp/dptx.c ../common/dp/dptx_hal.c dp_intf.c
ramstage-y += emi.c
ramstage-y += hdmi.c
ramstage-y += ../common/l2c_ops.c
ramstage-y += ../common/mcu.c
ramstage-y += ../common/mcupm.c
ramstage-y += ../common/mmu_operations.c ../common/mmu_cmops.c
ramstage-$(CONFIG_COMMONLIB_STORAGE_MMC) += ../common/msdc.c msdc.c
ramstage-y += mt6360.c
ramstage-y += ../common/mtcmos.c mtcmos.c
ramstage-$(CONFIG_PCI) += ../common/pcie.c pcie.c
ramstage-y += ../common/pll.c pll.c
ramstage-y += ../common/pmif.c ../common/pmif_init.c
ramstage-y += ../common/rtc.c ../common/rtc_mt6359p.c
ramstage-y += soc.c
ramstage-y += ../common/spm.c ../common/spm_v1.c spm.c
ramstage-y += ../common/sspm.c
ramstage-y += ../common/ufs.c
ramstage-y += ../common/usb.c usb.c

CPPFLAGS_common += -Isrc/soc/mediatek/mt8195/include
CPPFLAGS_common += -Isrc/soc/mediatek/common/dp/include
CPPFLAGS_common += -Isrc/soc/mediatek/common/include
CPPFLAGS_common += -Isrc/vendorcode/mediatek/mt8195/include

BL31_MAKEARGS += PLAT=mt8195

MT8195_BLOB_DIR := 3rdparty/blobs/soc/mediatek/mt8195

mcu-firmware-files := \
	$(CONFIG_DPM_DM_FIRMWARE) \
	$(CONFIG_DPM_PM_FIRMWARE) \
	$(CONFIG_MCUPM_FIRMWARE) \
	$(CONFIG_SSPM_FIRMWARE) \
	$(CONFIG_SPM_FIRMWARE)

$(foreach fw, $(call strip_quotes,$(mcu-firmware-files)), \
	  $(eval $(fw)-file := $(MT8195_BLOB_DIR)/$(fw)) \
	  $(eval $(fw)-type := raw) \
	  $(eval $(fw)-compression := LZ4) \
	  $(eval cbfs-files-y += $(fw)) \
)

DRAM_CBFS := $(CONFIG_CBFS_PREFIX)/dram
$(DRAM_CBFS)-file := $(MT8195_BLOB_DIR)/dram.elf
$(DRAM_CBFS)-type := stage
$(DRAM_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
cbfs-files-y += $(DRAM_CBFS)

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	./util/mtkheader/gen-bl-img.py mt8183 sf $< $@

endif
