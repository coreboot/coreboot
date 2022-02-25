ifeq ($(CONFIG_SOC_QUALCOMM_SC7280),y)

decompressor-y += decompressor.c
decompressor-y += mmu.c
decompressor-y += ../common/timer.c
all-y += ../common/timer.c
all-y += ../common/gpio.c
all-y += ../common/clock.c
all-y += clock.c
all-y += ../common/spi.c
all-$(CONFIG_SC7280_QSPI) += ../common/qspi.c
all-y += ../common/qupv3_config.c
all-y += qcom_qup_se.c
all-y += ../common/qup_se_handler.c
all-y += ../common/qupv3_spi.c
all-y += ../common/qupv3_i2c.c
all-y += ../common/qupv3_spi.c

################################################################################
bootblock-y += bootblock.c
bootblock-y += mmu.c
bootblock-$(CONFIG_DRIVERS_UART) += ../common/uart_bitbang.c

################################################################################
verstage-$(CONFIG_DRIVERS_UART) += ../common/qupv3_uart.c

################################################################################
romstage-y += cbmem.c
romstage-y += shrm_load_reset.c
romstage-y += cpucp_load_reset.c
romstage-y += ../common/qclib.c
romstage-y += ../common/mmu.c
romstage-y += ../common/watchdog.c
romstage-y += mmu.c
romstage-y += ../common/usb/usb.c
romstage-y += carve_out.c
romstage-$(CONFIG_DRIVERS_UART) += ../common/qupv3_uart.c

################################################################################
ramstage-y += soc.c
ramstage-y += carve_out.c
ramstage-y += cbmem.c
ramstage-$(CONFIG_DRIVERS_UART) += ../common/qupv3_uart.c
ramstage-y += ../common/usb/usb.c
ramstage-y += ../common/usb/snps_usb_phy.c
ramstage-y += ../common/usb/qmpv4_usb_phy.c
ramstage-y += ../common/aop_load_reset.c
ramstage-y += cpucp_load_reset.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += display/edp_aux.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += display/edp_ctrl.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += display/edp_phy_7nm.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += ../common/display/mdss.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += display/disp.c
ramstage-$(CONFIG_PCI) += ../common/pcie_common.c
ramstage-$(CONFIG_PCI) += pcie.c

################################################################################

CPPFLAGS_common += -Isrc/soc/qualcomm/sc7280/include
CPPFLAGS_common += -Isrc/soc/qualcomm/common/include

SC7280_BLOB := $(top)/3rdparty/qc_blobs/sc7280

################################################################################
BL31_MAKEARGS += PLAT=sc7280

ifeq ($(CONFIG_QC_SDI_ENABLE),y)
BL31_MAKEARGS += QTI_SDI_BUILD=1
BL31_MAKEARGS += QTISECLIB_PATH=$(SC7280_BLOB)/qtiseclib/libqtisec_dbg.a
else
BL31_MAKEARGS += QTISECLIB_PATH=$(SC7280_BLOB)/qtiseclib/libqtisec.a
endif # CONFIG_QC_SDI_ENABLE

################################################################################
ifeq ($(CONFIG_QC_SDI_ENABLE),y)
QCSDI_FILE := $(SC7280_BLOB)/boot/QcSdi.elf
QCSDI_CBFS := $(CONFIG_CBFS_PREFIX)/qcsdi
$(QCSDI_CBFS)-file := $(QCSDI_FILE)
$(QCSDI_CBFS)-type := stage
$(QCSDI_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(QCSDI_CBFS)
endif

################################################################################
QC_SEC_FILE := $(SC7280_BLOB)/qc_sec/qc_sec.mbn
$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.elf
	@util/qualcomm/createxbl.py --mbn_version 6 -f $(objcbfs)/bootblock.raw.elf \
		-x $(QC_SEC_FILE) -o $(objcbfs)/merged_bb_qcsec.mbn \
		-a 64 -d 64 -c 64
	@printf "\nqgpt.py 4K sector size\n"
	@util/qualcomm/qgpt.py $(objcbfs)/merged_bb_qcsec.mbn \
		$(objcbfs)/bootblock.bin

################################################################################
QCLIB_FILE := $(SC7280_BLOB)/boot/QcLib.elf
QCLIB_CBFS := $(CONFIG_CBFS_PREFIX)/qclib
$(QCLIB_CBFS)-file := $(QCLIB_FILE)
$(QCLIB_CBFS)-type := stage
$(QCLIB_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
cbfs-files-y += $(QCLIB_CBFS)

################################################################################
PMIC_FILE := $(SC7280_BLOB)/boot/Pmic.bin
PMIC_CBFS := $(CONFIG_CBFS_PREFIX)/pmiccfg
$(PMIC_CBFS)-file := $(PMIC_FILE)
$(PMIC_CBFS)-type := raw
$(PMIC_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(PMIC_CBFS)

################################################################################
DCB_FILE := $(SC7280_BLOB)/boot/dcb.bin
DCB_CBFS := $(CONFIG_CBFS_PREFIX)/dcb
$(DCB_CBFS)-file := $(DCB_FILE)
$(DCB_CBFS)-type := raw
$(DCB_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(DCB_CBFS)

################################################################################
UART_FW_FILE := $(SC7280_BLOB)/qup_fw/uart_fw.bin
UART_FW_CBFS := $(CONFIG_CBFS_PREFIX)/uart_fw
$(UART_FW_CBFS)-file := $(UART_FW_FILE)
$(UART_FW_CBFS)-type := raw
$(UART_FW_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
cbfs-files-y += $(UART_FW_CBFS)

################################################################################
SPI_FW_FILE := $(SC7280_BLOB)/qup_fw/spi_fw.bin
SPI_FW_CBFS := $(CONFIG_CBFS_PREFIX)/spi_fw
$(SPI_FW_CBFS)-file := $(SPI_FW_FILE)
$(SPI_FW_CBFS)-type := raw
$(SPI_FW_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
cbfs-files-y += $(SPI_FW_CBFS)

################################################################################
I2C_FW_FILE := $(SC7280_BLOB)/qup_fw/i2c_fw.bin
I2C_FW_CBFS := $(CONFIG_CBFS_PREFIX)/i2c_fw
$(I2C_FW_CBFS)-file := $(I2C_FW_FILE)
$(I2C_FW_CBFS)-type := raw
$(I2C_FW_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
cbfs-files-y += $(I2C_FW_CBFS)

################################################################################
AOP_FILE := $(SC7280_BLOB)/aop/aop.mbn
AOP_CBFS := $(CONFIG_CBFS_PREFIX)/aop
$(AOP_CBFS)-file := $(AOP_FILE)
$(AOP_CBFS)-type := payload
$(AOP_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(AOP_CBFS)

################################################################################
CPUCP_FILE := $(SC7280_BLOB)/cpucp/cpucp.elf
CPUCP_CBFS := $(CONFIG_CBFS_PREFIX)/cpucp
$(CPUCP_CBFS)-file := $(CPUCP_FILE)
$(CPUCP_CBFS)-type := payload
$(CPUCP_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(CPUCP_CBFS)

################################################################################
SHRM_FILE := $(SC7280_BLOB)/shrm/shrm.elf
SHRM_CBFS := $(CONFIG_CBFS_PREFIX)/shrm
$(SHRM_CBFS)-file := $(SHRM_FILE)
$(SHRM_CBFS)-type := payload
$(SHRM_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
cbfs-files-y += $(SHRM_CBFS)

################################################################################
GSI_FW_FILE := $(SC7280_BLOB)/qup_fw/gsi_fw.bin
GSI_FW_CBFS := $(CONFIG_CBFS_PREFIX)/gsi_fw
$(GSI_FW_CBFS)-file := $(GSI_FW_FILE)
$(GSI_FW_CBFS)-type := raw
$(GSI_FW_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(GSI_FW_CBFS)

endif
