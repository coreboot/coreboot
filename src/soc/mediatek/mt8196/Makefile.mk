## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_MEDIATEK_MT8196),y)

all-y += ../common/flash_controller.c
all-y += ../common/gpio.c ../common/gpio_op.c gpio.c
all-y += ../common/gpio_eint_v2.c gpio_eint.c
all-y += ../common/i2c.c ../common/i2c_common.c i2c.c
all-y += ../common/pll.c pll.c
all-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
all-y += ../common/timer_prepare_v2.c timer.c
all-y += ../common/uart.c

bootblock-y += booker.c
bootblock-y += ../common/bootblock.c bootblock.c
bootblock-y += cksys.c
bootblock-y += ../common/early_init.c
bootblock-y += ../common/eint_event.c eint_event_info.c
bootblock-y += ../common/lastbus_v2.c lastbus.c
bootblock-y += mminfra.c
bootblock-y += ../common/mmu_operations.c
bootblock-y += mtcmos.c
bootblock-$(CONFIG_PCI) += ../common/pcie.c pcie.c
bootblock-y += ../common/tracker_v3.c tracker.c
bootblock-y += ../common/wdt.c ../common/wdt_req.c wdt.c

romstage-y += ../common/cbmem.c
romstage-y += clkbuf_ctl.c
romstage-y += ../common/dram_init.c
romstage-y += ../common/dramc_param.c
romstage-y += dvfs.c
romstage-y += dvfsrc.c
romstage-$(CONFIG_PCI) += ../common/early_init.c ../common/pcie.c
romstage-y += ../common/emi.c
romstage-y += irq2axi.c
romstage-y += l2c_ops.c
romstage-y += modem_power_ctrl.c
romstage-y += ../common/memory.c ../common/memory_type.c
romstage-y += ../common/memory_test.c
romstage-y += ../common/mmu_operations.c
romstage-y += ../common/mt6316.c mt6316.c
romstage-y += ../common/mt6363.c mt6363.c
romstage-y += ../common/mt6373.c mt6373.c
romstage-y += ../common/mt6685.c mt6685.c
romstage-y += mt6685_rtc.c
romstage-y += ../common/mtk_fsp.c
romstage-y += pwrsel.c
romstage-y += ../common/pmif_clk.c pmif_clk.c
romstage-y += ../common/pmif.c pmif_init.c
romstage-y += ../common/rtc.c ../common/rtc_osc_init.c
romstage-y += ../common/pmif_spmi_v2.c pmif_spmi.c
romstage-y += ../common/srclken_rc.c srclken_rc.c
romstage-y += ../common/thermal.c thermal.c
romstage-y += ../common/thermal_sram.c thermal_sram.c

ramstage-$(CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE) += ../common/bl31.c
ramstage-y += dcc.c
ramstage-y += ddp.c
ramstage-y += ../common/display.c
ramstage-y += ../common/dpm.c
ramstage-y += ../common/dpm_v2.c
ramstage-y += ../common/dp/dp_intf_v2.c
ramstage-y += ../common/dp/dptx_common.c ../common/dp/dptx_v2.c dptx.c
ramstage-y += ../common/dp/dptx_hal_common.c ../common/dp/dptx_hal_v2.c dptx_hal.c
ramstage-y += ../common/dramc_info.c
ramstage-y += ../common/early_init.c
ramstage-y += ../common/emi.c
ramstage-y += gpueb.c
ramstage-y += l2c_ops.c
ramstage-y += ../common/mcu.c mcupm.c
ramstage-y += ../common/mmu_operations.c
ramstage-$(CONFIG_PCI) += ../common/pcie.c pcie.c
ramstage-$(CONFIG_COMMONLIB_STORAGE_MMC) += msdc.c
ramstage-y += ../common/mt6363.c mt6363.c
ramstage-y += ../common/mt6363_sdmadc.c
ramstage-y += ../common/mt6373.c mt6373.c
ramstage-y += ../common/mt6685.c mt6685.c
ramstage-y += mt6685_rtc.c
ramstage-y += mtcmos.c
ramstage-y += ../common/mtk_fsp.c
ramstage-y += ../common/pi_image.c
ramstage-y += soc.c
ramstage-y += ../common/spm.c ../common/spm_v2.c spm.c
ramstage-y += ../common/sspm.c ../common/sspm_sram.c
ramstage-y += ../common/pmif_clk.c pmif_clk.c
ramstage-y += ../common/pmif.c pmif_init.c
ramstage-y += ../common/pmif_spmi_v2.c pmif_spmi.c
ramstage-y += ../common/rtc.c ../common/rtc_osc_init.c
ramstage-y += ../common/usb.c usb.c

BL31_MAKEARGS += PLAT=mt8196

CPPFLAGS_common += -Isrc/soc/mediatek/mt8196/include
CPPFLAGS_common += -Isrc/soc/mediatek/common/include
CPPFLAGS_common += -Isrc/soc/mediatek/common/dp/include

MT8196_BLOB_DIR := 3rdparty/blobs/soc/mediatek/mt8196

BL31_LIB := $(top)/$(MT8196_BLOB_DIR)/libbl31.a

ifneq ($(wildcard $(BL31_LIB)),)
BL31_MAKEARGS += MTKLIB_PATH=$(BL31_LIB)
endif

PI_IMG := $(MT8196_BLOB_DIR)/$(call strip_quotes,$(CONFIG_PI_IMG_FIRMWARE))

.PHONY: check_pi_img
check_pi_img: $(PI_IMG)
	./util/mediatek/check-pi-img.py $<

# Make sure check_pi_img is always run.
$(obj)/coreboot.pre: | check_pi_img

mcu-firmware-files := \
	$(CONFIG_DPM_DM_FIRMWARE) \
	$(CONFIG_DPM_PM_FIRMWARE) \
	$(CONFIG_GPUEB_FIRMWARE) \
	$(CONFIG_MCUPM_FIRMWARE) \
	$(CONFIG_PI_IMG_FIRMWARE) \
	$(CONFIG_SSPM_FIRMWARE) \
	$(CONFIG_SPM_FIRMWARE)

$(foreach fw, $(call strip_quotes,$(mcu-firmware-files)), \
	$(eval $(fw)-file := $(MT8196_BLOB_DIR)/$(fw)) \
	$(eval $(fw)-type := raw) \
	$(eval $(fw)-compression := LZ4) \
	$(if $(wildcard $($(fw)-file)), $(eval cbfs-files-y += $(fw)), ) \
)

DRAM_CBFS := $(CONFIG_CBFS_PREFIX)/dram
$(DRAM_CBFS)-file := $(MT8196_BLOB_DIR)/dram.elf
$(DRAM_CBFS)-type := stage
$(DRAM_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
cbfs-files-y += $(DRAM_CBFS)

FSP_ROMSTAGE_CBFS := $(CONFIG_CBFS_PREFIX)/mtk_fsp_romstage
$(FSP_ROMSTAGE_CBFS)-file := $(MT8196_BLOB_DIR)/mtk_fsp_romstage.elf
$(FSP_ROMSTAGE_CBFS)-type := stage
$(FSP_ROMSTAGE_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
cbfs-files-y += $(FSP_ROMSTAGE_CBFS)

FSP_RAMSTAGE_CBFS := $(CONFIG_CBFS_PREFIX)/mtk_fsp_ramstage
$(FSP_RAMSTAGE_CBFS)-file := $(MT8196_BLOB_DIR)/mtk_fsp_ramstage.elf
$(FSP_RAMSTAGE_CBFS)-type := stage
$(FSP_RAMSTAGE_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(FSP_RAMSTAGE_CBFS)

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	./util/mediatek/gen-bl-img.py mt8196 sf $< $@

endif
