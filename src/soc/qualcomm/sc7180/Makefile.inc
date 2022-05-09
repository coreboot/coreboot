
ifeq ($(CONFIG_SOC_QUALCOMM_SC7180),y)

decompressor-y += decompressor.c
decompressor-y += mmu.c
decompressor-y += ../common/timer.c
all-y += ../common/timer.c
all-y += ../common/gpio.c
all-y += ../common/spi.c
all-$(CONFIG_SC7180_QSPI) += ../common/qspi.c
all-y += ../common/qupv3_i2c.c
all-y += ../common/qupv3_spi.c
all-y += clock.c
all-y += ../common/clock.c
all-y += qcom_qup_se.c
all-y += ../common/qupv3_config.c
all-y += ../common/qup_se_handler.c

################################################################################
bootblock-y += bootblock.c
bootblock-y += mmu.c
bootblock-$(CONFIG_DRIVERS_UART) += ../common/uart_bitbang.c
################################################################################
verstage-$(CONFIG_DRIVERS_UART) += ../common/qupv3_uart.c

################################################################################
romstage-y += cbmem.c
romstage-y += watchdog.c
romstage-y += ../common/qclib.c
romstage-y += qclib.c
romstage-y += ../common/mmu.c
romstage-y += mmu.c
romstage-y += ../common/usb/usb.c
romstage-y += carve_out.c
romstage-$(CONFIG_DRIVERS_UART) += ../common/qupv3_uart.c

################################################################################
ramstage-y += soc.c
ramstage-y += carve_out.c
ramstage-y += ../common/aop_load_reset.c
ramstage-$(CONFIG_DRIVERS_UART) += ../common/qupv3_uart.c
ramstage-y += ../common/usb/usb.c
ramstage-y += ../common/usb/qusb_phy.c
ramstage-y += ../common/usb/qmpv3_usb_phy.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += display/dsi_phy_pll.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += display/dsi_phy.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += display/dsi.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += display/mdss.c

################################################################################

CPPFLAGS_common += -Isrc/soc/qualcomm/sc7180/include
CPPFLAGS_common += -Isrc/soc/qualcomm/common/include

BL31_MAKEARGS += PLAT=sc7180

SC7180_BLOB := $(top)/3rdparty/qc_blobs/sc7180

ifeq ($(CONFIG_QC_SDI_ENABLE),y)
BL31_MAKEARGS += QTI_SDI_BUILD=1
BL31_MAKEARGS += QTISECLIB_PATH=$(SC7180_BLOB)/qtiseclib/libqtisec_dbg.a
else
BL31_MAKEARGS += QTISECLIB_PATH=$(SC7180_BLOB)/qtiseclib/libqtisec.a
endif # CONFIG_QC_SDI_ENABLE

################################################################################
ifeq ($(CONFIG_QC_SDI_ENABLE),y)
QCSDI_FILE := $(SC7180_BLOB)/boot/QcSdi.elf
QCSDI_CBFS := $(CONFIG_CBFS_PREFIX)/qcsdi
$(QCSDI_CBFS)-file := $(QCSDI_FILE)
$(QCSDI_CBFS)-type := stage
$(QCSDI_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(QCSDI_CBFS)
endif

################################################################################
QCLIB_FILE := $(SC7180_BLOB)/boot/QcLib.elf
QCLIB_CBFS := $(CONFIG_CBFS_PREFIX)/qclib
$(QCLIB_CBFS)-file := $(QCLIB_FILE)
$(QCLIB_CBFS)-type := stage
$(QCLIB_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
cbfs-files-y += $(QCLIB_CBFS)

################################################################################
PMIC_FILE := $(SC7180_BLOB)/boot/Pmic.bin
PMIC_CBFS := $(CONFIG_CBFS_PREFIX)/pmiccfg
$(PMIC_CBFS)-file := $(PMIC_FILE)
$(PMIC_CBFS)-type := raw
$(PMIC_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(PMIC_CBFS)

################################################################################
DCB_FILE := $(SC7180_BLOB)/boot/dcb.bin
DCB_CBFS := $(CONFIG_CBFS_PREFIX)/dcb
$(DCB_CBFS)-file := $(DCB_FILE)
$(DCB_CBFS)-type := raw
$(DCB_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(DCB_CBFS)

################################################################################
DCB_LONGSYS1P8_FILE := $(SC7180_BLOB)/boot/dcb_slow.bin
DCB_LONGSYS1P8_CBFS := $(CONFIG_CBFS_PREFIX)/dcb_longsys1p8
$(DCB_LONGSYS1P8_CBFS)-file := $(DCB_LONGSYS1P8_FILE)
$(DCB_LONGSYS1P8_CBFS)-type := raw
$(DCB_LONGSYS1P8_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(DCB_LONGSYS1P8_CBFS)

################################################################################
AOP_FILE := $(SC7180_BLOB)/aop/aop.mbn
AOP_CBFS := $(CONFIG_CBFS_PREFIX)/aop
$(AOP_CBFS)-file := $(AOP_FILE)
$(AOP_CBFS)-type := payload
$(AOP_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(AOP_CBFS)

################################################################################
UART_FW_FILE := $(SC7180_BLOB)/qup_fw/uart_fw.bin
UART_FW_CBFS := $(CONFIG_CBFS_PREFIX)/uart_fw
$(UART_FW_CBFS)-file := $(UART_FW_FILE)
$(UART_FW_CBFS)-type := raw
$(UART_FW_CBFS)-compression := none
cbfs-files-y += $(UART_FW_CBFS)

################################################################################
SPI_FW_FILE := $(SC7180_BLOB)/qup_fw/spi_fw.bin
SPI_FW_CBFS := $(CONFIG_CBFS_PREFIX)/spi_fw
$(SPI_FW_CBFS)-file := $(SPI_FW_FILE)
$(SPI_FW_CBFS)-type := raw
$(SPI_FW_CBFS)-compression := none
cbfs-files-y += $(SPI_FW_CBFS)

################################################################################
I2C_FW_FILE := $(SC7180_BLOB)/qup_fw/i2c_fw.bin
I2C_FW_CBFS := $(CONFIG_CBFS_PREFIX)/i2c_fw
$(I2C_FW_CBFS)-file := $(I2C_FW_FILE)
$(I2C_FW_CBFS)-type := raw
$(I2C_FW_CBFS)-compression := none
cbfs-files-y += $(I2C_FW_CBFS)

################################################################################
QC_SEC_FILE := $(SC7180_BLOB)/qc_sec/qc_sec.mbn
$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.elf
	@util/qualcomm/createxbl.py --mbn_version 6 -f $(objcbfs)/bootblock.raw.elf \
		-x $(QC_SEC_FILE) -o $(objcbfs)/merged_bb_qcsec.mbn \
		-a 64 -d 64 -c 64
	@printf "\nqgpt.py 4K sector size\n"
	@util/qualcomm/qgpt.py $(objcbfs)/merged_bb_qcsec.mbn \
		$(objcbfs)/bootblock.bin

endif
