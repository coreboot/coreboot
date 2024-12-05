## SPDX-License-Identifier: GPL-2.0-only OR MIT

ifeq ($(CONFIG_SOC_MEDIATEK_MT8189),y)

all-y += ../common/flash_controller.c
all-y += ../common/gpio.c ../common/gpio_op.c ../common/gpio_eint_v2.c gpio.c gpio_eint.c
all-$(CONFIG_SPI_FLASH) += spi.c
all-y += ../common/timer_prepare.c timer.c
all-y += ../common/uart.c

bootblock-y += bootblock.c
bootblock-y += ../common/mmu_operations.c
bootblock-y += ../common/wdt.c ../common/wdt_req.c wdt.c

romstage-y += ../common/cbmem.c
romstage-y += ../common/dram_init.c
romstage-y += ../common/dramc_param.c
romstage-y += ../common/emi.c
romstage-y += ../common/memory.c ../common/memory_test.c
romstage-y += ../common/mmu_operations.c ../common/mmu_cmops.c

ramstage-$(CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE) += ../common/bl31.c
ramstage-y += ../common/dramc_info.c
ramstage-y += ../common/emi.c
ramstage-y += ../common/memory.c
ramstage-y += ../common/mmu_operations.c ../common/mmu_cmops.c
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
