## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_QUALCOMM_BASE),y)

decompressor-y += decompressor.c
decompressor-y += mmu.c
decompressor-y += ../common/timer.c
all-y += ../common/timer.c
all-y += ../common/gpio.c
all-y += ../common/clock.c
all-y += clock.c
all-y += ../common/spi.c
all-y += ../common/qspi.c
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
romstage-y += ../common/shrm_load_reset.c
romstage-y += cpucp_load_reset.c
romstage-y += ../common/qclib.c
romstage-y += ../common/mmu.c
romstage-y += ../common/watchdog.c
romstage-y += qclib.c
romstage-y += mmu.c
romstage-y += ../common/aop_load_reset.c
romstage-$(CONFIG_DRIVERS_UART) += ../common/qupv3_uart.c
romstage-y += ../common/spmi.c

################################################################################
ramstage-y += soc.c
ramstage-y += cbmem.c
ramstage-y += ../common/mmu.c
ramstage-$(CONFIG_DRIVERS_UART) += ../common/qupv3_uart.c
ramstage-$(CONFIG_PCI) += ../common/pcie_common.c
ramstage-y += ../common/spmi.c
ramstage-$(CONFIG_PCI) += pcie.c
ramstage-y += cpucp_load_reset.c

################################################################################

CPPFLAGS_common += -Isrc/soc/qualcomm/x1p42100/include
CPPFLAGS_common += -Isrc/soc/qualcomm/common/include

################################################################################
# look for QC blobs if QC SoC blobs are only available in upstream else ignore
ifeq ($(CONFIG_QC_BLOBS_UPSTREAM),y)
ifeq ($(CONFIG_USE_QC_BLOBS),y)
# TODO: Upload X1P42100 SoC blobs
X1P42100_BLOB := $(top)/3rdparty/qc_blobs/x1p42100

ifeq ($(CONFIG_SOC_QUALCOMM_HAMOA),y)
DTB_DCB_BLOB_PATH := hamoa
else
DTB_DCB_BLOB_PATH := x1p42100
endif

ifeq ($(CONFIG_QC_SDI_ENABLE),y)
BL31_MAKEARGS += QTI_SDI_BUILD=1
BL31_MAKEARGS += QTISECLIB_PATH=$(X1P42100_BLOB)/qtiseclib/libqtisec_dbg.a
else
BL31_MAKEARGS += QTISECLIB_PATH=$(X1P42100_BLOB)/qtiseclib/libqtisec.a
endif # CONFIG_QC_SDI_ENABLE

################################################################################
ifeq ($(CONFIG_QC_SDI_ENABLE),y)
QCSDI_FILE := $(X1P42100_BLOB)/boot/QcSdi.elf
QCSDI_CBFS := $(CONFIG_CBFS_PREFIX)/qcsdi
$(QCSDI_CBFS)-file := $(QCSDI_FILE)
$(QCSDI_CBFS)-type := stage
$(QCSDI_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(QCSDI_CBFS)
endif

################################################################################
QC_SEC_FILE := $(X1P42100_BLOB)/qc_sec/qc_sec.mbn
TME_SEQ_FILE := $(X1P42100_BLOB)/tme/sequencer_ram.elf
TME_FW_FILE := $(X1P42100_BLOB)/tme/signed_firmware_soc_view.elf

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.elf
	@util/qualcomm/createxbl.py --mbn_version 7 -f $(objcbfs)/bootblock.raw.elf \
		-o $(objcbfs)/bootblock.mbn \
		-a 64 -c 64
	@util/qualcomm/create_multielf.py -f $(TME_SEQ_FILE),$(TME_FW_FILE),$(QC_SEC_FILE),$(objcbfs)/bootblock.mbn \
		-o $(objcbfs)/merged_bb.melf
	@printf "\nqgpt.py 4K sector size\n"
	@util/qualcomm/qgpt.py $(objcbfs)/merged_bb.melf \
		$(objcbfs)/bootblock.bin

################################################################################
QCLIB_FILE := $(X1P42100_BLOB)/boot/QcLib.elf
QCLIB_CBFS := $(CONFIG_CBFS_PREFIX)/qclib
$(QCLIB_CBFS)-file := $(QCLIB_FILE)
$(QCLIB_CBFS)-type := stage
$(QCLIB_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
cbfs-files-y += $(QCLIB_CBFS)

################################################################################
DCB_FILE := $(X1P42100_BLOB)/boot/$(DTB_DCB_BLOB_PATH)/dcb.bin
DCB_CBFS := $(CONFIG_CBFS_PREFIX)/dcb
$(DCB_CBFS)-file := $(DCB_FILE)
$(DCB_CBFS)-type := raw
$(DCB_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(DCB_CBFS)

################################################################################
DTB_FILE := $(X1P42100_BLOB)/boot/$(DTB_DCB_BLOB_PATH)/pre-ddr.dtb
DTB_CBFS := $(CONFIG_CBFS_PREFIX)/dtb
$(DTB_CBFS)-file := $(DTB_FILE)
$(DTB_CBFS)-type := raw
$(DTB_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(DTB_CBFS)

################################################################################
CPR_FILE := $(X1P42100_BLOB)/boot/$(DTB_DCB_BLOB_PATH)/cpr.bin
CPR_CBFS := $(CONFIG_CBFS_PREFIX)/cpr
$(CPR_CBFS)-file := $(CPR_FILE)
$(CPR_CBFS)-type := raw
$(CPR_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
cbfs-files-y += $(CPR_CBFS)

################################################################################
UART_FW_FILE := $(X1P42100_BLOB)/qup_fw/uart_fw.bin
UART_FW_CBFS := $(CONFIG_CBFS_PREFIX)/uart_fw
$(UART_FW_CBFS)-file := $(UART_FW_FILE)
$(UART_FW_CBFS)-type := raw
$(UART_FW_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
cbfs-files-y += $(UART_FW_CBFS)

################################################################################
SPI_FW_FILE := $(X1P42100_BLOB)/qup_fw/spi_fw.bin
SPI_FW_CBFS := $(CONFIG_CBFS_PREFIX)/spi_fw
$(SPI_FW_CBFS)-file := $(SPI_FW_FILE)
$(SPI_FW_CBFS)-type := raw
$(SPI_FW_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
cbfs-files-y += $(SPI_FW_CBFS)

################################################################################
I2C_FW_FILE := $(X1P42100_BLOB)/qup_fw/i2c_fw.bin
I2C_FW_CBFS := $(CONFIG_CBFS_PREFIX)/i2c_fw
$(I2C_FW_CBFS)-file := $(I2C_FW_FILE)
$(I2C_FW_CBFS)-type := raw
$(I2C_FW_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
cbfs-files-y += $(I2C_FW_CBFS)

################################################################################
AOP_FILE := $(X1P42100_BLOB)/aop/aop.mbn
AOP_CBFS := $(CONFIG_CBFS_PREFIX)/aop
$(AOP_CBFS)-file := $(AOP_FILE)
$(AOP_CBFS)-type := payload
$(AOP_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(AOP_CBFS)

################################################################################
# Rule to create aop_meta from aop.mbn
# This rule depends on aop.mbn built and the extractor script existing.
$(obj)/mainboard/$(MAINBOARDDIR)/aop_meta: $(X1P42100_BLOB)/aop/aop.mbn util/qualcomm/elf_segment_extractor.py
	@echo "Extracting ELF headers and hash table segment from $< to $@"
	@util/qualcomm/elf_segment_extractor.py --eh --pht --hashtable $< $@

AOP_META_FILE := $(obj)/mainboard/$(MAINBOARDDIR)/aop_meta
AOP_META_CBFS := $(CONFIG_CBFS_PREFIX)/aop_meta
$(AOP_META_CBFS)-file := $(AOP_META_FILE)
$(AOP_META_CBFS)-type := raw
$(AOP_META_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(AOP_META_CBFS)

################################################################################
AOP_CFG_FILE := $(X1P42100_BLOB)/aop/aop_devcfg.mbn
AOP_CFG_CBFS := $(CONFIG_CBFS_PREFIX)/aop_cfg
$(AOP_CFG_CBFS)-file := $(AOP_CFG_FILE)
$(AOP_CFG_CBFS)-type := payload
$(AOP_CFG_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(AOP_CFG_CBFS)

################################################################################
# Rule to create aop_meta from aop_devcfg.mbn
# This rule depends on aop_devcfg.mbn built and the extractor script existing.
$(obj)/mainboard/$(MAINBOARDDIR)/aop_devcfg_meta: $(X1P42100_BLOB)/aop/aop_devcfg.mbn util/qualcomm/elf_segment_extractor.py
	@echo "Extracting ELF headers and hash table segment from $< to $@"
	@util/qualcomm/elf_segment_extractor.py --eh --pht --hashtable $< $@

AOP_DEVCFG_META_FILE := $(obj)/mainboard/$(MAINBOARDDIR)/aop_devcfg_meta
AOP_DEVCFG_META_CBFS := $(CONFIG_CBFS_PREFIX)/aop_devcfg_meta
$(AOP_DEVCFG_META_CBFS)-file := $(AOP_DEVCFG_META_FILE)
$(AOP_DEVCFG_META_CBFS)-type := raw
$(AOP_DEVCFG_META_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(AOP_DEVCFG_META_CBFS)

################################################################################
CPUCP_FILE := $(X1P42100_BLOB)/cpucp/cpucp.elf
CPUCP_CBFS := $(CONFIG_CBFS_PREFIX)/cpucp
$(CPUCP_CBFS)-file := $(CPUCP_FILE)
$(CPUCP_CBFS)-type := payload
$(CPUCP_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(CPUCP_CBFS)

################################################################################
# Rule to create cpucp_meta from cpucp.elf
# This rule depends on cpucp.elf being built and the extractor script existing.
$(obj)/mainboard/$(MAINBOARDDIR)/cpucp_meta: $(X1P42100_BLOB)/cpucp/cpucp.elf util/qualcomm/elf_segment_extractor.py
	@echo "Extracting ELF headers and hash table segment from $< to $@"
	@util/qualcomm/elf_segment_extractor.py --eh --pht --hashtable $< $@

CPUCP_META_FILE := $(obj)/mainboard/$(MAINBOARDDIR)/cpucp_meta
CPUCP_META_CBFS := $(CONFIG_CBFS_PREFIX)/cpucp_meta
$(CPUCP_META_CBFS)-file := $(CPUCP_META_FILE)
$(CPUCP_META_CBFS)-type := raw
$(CPUCP_META_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(CPUCP_META_CBFS)

################################################################################
CPUCP_DTBS_FILE := $(X1P42100_BLOB)/cpucp/cpucp_dtbs.elf
CPUCP_DTBS_CBFS := $(CONFIG_CBFS_PREFIX)/cpucp_dtbs
$(CPUCP_DTBS_CBFS)-file := $(CPUCP_DTBS_FILE)
$(CPUCP_DTBS_CBFS)-type := payload
$(CPUCP_DTBS_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(CPUCP_DTBS_CBFS)

################################################################################
SHRM_FILE := $(X1P42100_BLOB)/shrm/shrm.elf
SHRM_CBFS := $(CONFIG_CBFS_PREFIX)/shrm
$(SHRM_CBFS)-file := $(SHRM_FILE)
$(SHRM_CBFS)-type := payload
$(SHRM_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
cbfs-files-y += $(SHRM_CBFS)

################################################################################
# Rule to create shrm_meta from shrm.elf
# This rule depends on shrm.elf being built and the extractor script existing.
$(obj)/mainboard/$(MAINBOARDDIR)/shrm_meta: $(X1P42100_BLOB)/shrm/shrm.elf util/qualcomm/elf_segment_extractor.py
	@echo "Extracting ELF headers and hash table segment from $< to $@"
	@util/qualcomm/elf_segment_extractor.py --eh --pht --hashtable $< $@

SHRM_META_FILE := $(obj)/mainboard/$(MAINBOARDDIR)/shrm_meta
SHRM_META_CBFS := $(CONFIG_CBFS_PREFIX)/shrm_meta
$(SHRM_META_CBFS)-file := $(SHRM_META_FILE)
$(SHRM_META_CBFS)-type := raw
$(SHRM_META_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
cbfs-files-y += $(SHRM_META_CBFS)

################################################################################
GSI_FW_FILE := $(X1P42100_BLOB)/qup_fw/gsi_fw.bin
GSI_FW_CBFS := $(CONFIG_CBFS_PREFIX)/gsi_fw
$(GSI_FW_CBFS)-file := $(GSI_FW_FILE)
$(GSI_FW_CBFS)-type := raw
$(GSI_FW_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(GSI_FW_CBFS)

endif # ifeq ($(CONFIG_USE_QC_BLOBS),y)

endif # ifeq ($(CONFIG_QC_BLOBS_UPSTREAM),y)
endif # ifeq ($(CONFIG_SOC_QUALCOMM_BASE),y)
