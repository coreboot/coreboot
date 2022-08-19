## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_MEDIATEK_MT8173),y)

bootblock-y += bootblock.c
bootblock-$(CONFIG_SPI_FLASH) += ../common/flash_controller.c
bootblock-y += ../common/i2c.c i2c.c
bootblock-y += ../common/pll.c pll.c
bootblock-y += ../common/spi.c spi.c
bootblock-y += ../common/timer.c
bootblock-y += timer.c

bootblock-y += ../common/uart.c

bootblock-y += ../common/gpio.c gpio.c gpio_init.c
bootblock-y +=  ../common/pmic_wrap.c pmic_wrap.c mt6391.c
bootblock-y += ../common/wdt.c ../common/reset.c
bootblock-y += ../common/mmu_operations.c mmu_operations.c

################################################################################

verstage-y += ../common/i2c.c i2c.c
verstage-y += ../common/spi.c spi.c

verstage-y += ../common/uart.c

verstage-y += ../common/timer.c
verstage-y += timer.c
verstage-y += ../common/wdt.c ../common/reset.c
verstage-$(CONFIG_SPI_FLASH) += ../common/flash_controller.c
verstage-y += ../common/gpio.c gpio.c

################################################################################

romstage-$(CONFIG_SPI_FLASH) += ../common/flash_controller.c
romstage-y += ../common/pll.c pll.c
romstage-y += ../common/timer.c
romstage-y += timer.c
romstage-y += ../common/i2c.c i2c.c

romstage-y += ../common/uart.c
romstage-y += ../common/cbmem.c
romstage-y += ../common/gpio.c gpio.c
romstage-y += ../common/spi.c spi.c
romstage-y += ../common/pmic_wrap.c pmic_wrap.c mt6391.c
romstage-y += memory.c
romstage-y += emi.c dramc_pi_basic_api.c dramc_pi_calibration_api.c
romstage-$(CONFIG_MEMORY_TEST) += ../common/memory_test.c
romstage-y += ../common/wdt.c ../common/reset.c
romstage-y += ../common/mmu_operations.c mmu_operations.c
romstage-y += ../common/rtc.c rtc.c

################################################################################

ramstage-y += emi.c
ramstage-y += ../common/spi.c spi.c
ramstage-$(CONFIG_SPI_FLASH) += ../common/flash_controller.c
ramstage-y += soc.c ../common/mtcmos.c
ramstage-y += ../common/timer.c
ramstage-y += timer.c
ramstage-y += ../common/uart.c
ramstage-y += ../common/i2c.c i2c.c
ramstage-y += ../common/pmic_wrap.c pmic_wrap.c mt6391.c
ramstage-y += mt6311.c
ramstage-y += da9212.c
ramstage-y += ../common/gpio.c gpio.c
ramstage-y += ../common/wdt.c ../common/reset.c
ramstage-y += ../common/pll.c pll.c
ramstage-y += ../common/rtc.c rtc.c

ramstage-y += ../common/usb.c usb.c

ramstage-y += ../common/ddp.c ddp.c
ramstage-y += ../common/dsi.c dsi.c

BL31_MAKEARGS += PLAT=mt8173

################################################################################

# Generate the actual coreboot bootblock code
$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	./util/mtkheader/gen-bl-img.py mt8173 sf $< $@

CPPFLAGS_common += -Isrc/soc/mediatek/mt8173/include
CPPFLAGS_common += -Isrc/soc/mediatek/common/include

endif
