## SPDX-License-Identifier: GPL-2.0-only OR MIT

ifeq ($(CONFIG_SOC_MEDIATEK_MT8189),y)

all-y += ../common/flash_controller.c
all-y += ../common/gpio.c ../common/gpio_op.c ../common/gpio_eint_v2.c gpio.c gpio_eint.c
all-y += ../common/i2c.c ../common/i2c_common.c i2c.c
all-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
all-y += ../common/timer_prepare.c timer.c
all-y += ../common/uart.c

bootblock-y += bootblock.c
bootblock-y += ../common/mmu_operations.c
bootblock-y += ../common/mtcmos.c mtcmos.c
bootblock-y += ../common/pll.c pll.c
bootblock-y += ../common/wdt.c ../common/wdt_req.c wdt.c

romstage-y += ../common/cbmem.c
romstage-y += ../common/dram_init.c
romstage-y += ../common/dramc_param.c
romstage-y += ../common/emi.c
romstage-y += ../common/memory.c ../common/memory_test.c
romstage-y += ../common/mmu_operations.c ../common/mmu_cmops.c
romstage-y += ../common/mt6359p.c mt6359p.c
romstage-y += ../common/pll.c pll.c
romstage-y += ../common/pmif.c ../common/pmif_clk.c ../common/pmif_init.c pmif_clk.c
romstage-y += ../common/pmif_spi.c pmif_spi.c
romstage-y += ../common/pmif_spmi.c pmif_spmi.c

ramstage-$(CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE) += ../common/bl31.c
ramstage-y += ../common/dramc_info.c
ramstage-y += ../common/emi.c
ramstage-y += ../common/memory.c
ramstage-y += ../common/mmu_operations.c ../common/mmu_cmops.c
ramstage-y += ../common/mt6359p.c mt6359p.c
ramstage-y += ../common/mtcmos.c mtcmos.c
ramstage-y += ../common/pmif.c ../common/pmif_clk.c ../common/pmif_init.c pmif_clk.c
ramstage-y += ../common/pmif_spi.c pmif_spi.c
ramstage-y += ../common/pmif_spmi.c pmif_spmi.c
ramstage-y += soc.c
ramstage-y += ../common/usb.c usb.c

BL31_MAKEARGS += PLAT=mt8189

CPPFLAGS_common += -Isrc/soc/mediatek/mt8189/include
CPPFLAGS_common += -Isrc/soc/mediatek/common/include

MT8189_BLOB_DIR := 3rdparty/blobs/soc/mediatek/mt8189

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
