
ifeq ($(CONFIG_SOC_QUALCOMM_SC7180),y)

################################################################################
bootblock-y += bootblock.c
bootblock-y += mmu.c
bootblock-y += timer.c
bootblock-y += spi.c
bootblock-y += qupv3_spi.c
bootblock-y += gpio.c
bootblock-y += qupv3_i2c.c
bootblock-$(CONFIG_DRIVERS_UART) += uart_bitbang.c
bootblock-y += clock.c
bootblock-$(CONFIG_SC7180_QSPI) += qspi.c
bootblock-y += qupv3_config.c
bootblock-y += qcom_qup_se.c

################################################################################
verstage-y += timer.c
verstage-y += spi.c
verstage-y += qupv3_spi.c
verstage-y += gpio.c
verstage-y += qupv3_i2c.c
verstage-y += clock.c
verstage-$(CONFIG_SC7180_QSPI) += qspi.c
verstage-y += qcom_qup_se.c
verstage-y += qupv3_config.c
verstage-$(CONFIG_DRIVERS_UART) += qupv3_uart.c

################################################################################
romstage-y += cbmem.c
romstage-y += timer.c
romstage-y += ../common/qclib.c
romstage-y += qclib.c
romstage-y += ../common/mmu.c
romstage-y += mmu.c
romstage-y += usb.c
romstage-y += spi.c
romstage-y += qupv3_spi.c
romstage-y += gpio.c
romstage-y += qupv3_i2c.c
romstage-y += clock.c
romstage-$(CONFIG_SC7180_QSPI) += qspi.c
romstage-y += qcom_qup_se.c
romstage-y += qupv3_config.c
romstage-$(CONFIG_DRIVERS_UART) += qupv3_uart.c

################################################################################
ramstage-y += soc.c
ramstage-y += timer.c
ramstage-y += spi.c
ramstage-y += qupv3_spi.c
ramstage-y += gpio.c
ramstage-y += qupv3_i2c.c
ramstage-y += clock.c
ramstage-$(CONFIG_SC7180_QSPI) += qspi.c
ramstage-y += aop_load_reset.c
ramstage-y += usb.c
ramstage-y += qupv3_config.c
ramstage-y += qcom_qup_se.c
ramstage-$(CONFIG_DRIVERS_UART) += qupv3_uart.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += display/dsi_phy_pll.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += display/dsi_phy.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += display/dsi.c

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
