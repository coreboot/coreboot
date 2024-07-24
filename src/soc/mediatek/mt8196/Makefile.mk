## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_MEDIATEK_MT8196),y)

all-y += ../common/flash_controller.c
all-y += ../common/gpio.c ../common/gpio_op.c gpio.c gpio_eint.c
all-y += ../common/i2c.c i2c.c
all-y += ../common/pll.c pll.c
all-$(CONFIG_SPI_FLASH) += spi.c
all-y += timer.c timer_prepare.c
all-y += ../common/uart.c

bootblock-y += bootblock.c
bootblock-y += ../common/bootblock.c bootblock.c
bootblock-y += ../common/early_init.c
bootblock-y += ../common/mmu_operations.c
bootblock-$(CONFIG_PCI) += ../common/pcie.c pcie.c
bootblock-y += ../common/wdt.c ../common/wdt_req.c wdt.c

romstage-y += ../common/cbmem.c
romstage-$(CONFIG_PCI) += ../common/early_init.c ../common/pcie.c
romstage-y += emi.c
romstage-y += irq2axi.c
romstage-y += l2c_ops.c
romstage-y += ../common/mmu_operations.c ../common/mmu_cmops.c

ramstage-y += ../common/early_init.c
ramstage-y += emi.c
ramstage-y += l2c_ops.c
ramstage-y += ../common/mmu_operations.c ../common/mmu_cmops.c
ramstage-$(CONFIG_PCI) += ../common/pcie.c pcie.c
ramstage-y += soc.c
ramstage-y += ../common/usb.c usb.c

CPPFLAGS_common += -Isrc/soc/mediatek/mt8196/include
CPPFLAGS_common += -Isrc/soc/mediatek/common/include

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	./util/mtkheader/gen-bl-img.py mt8196 sf $< $@

endif
