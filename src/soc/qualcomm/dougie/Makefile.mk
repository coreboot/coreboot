## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_QUALCOMM_DOUGIE),y)

decompressor-y += decompressor.c
decompressor-y += mmu.c
decompressor-y += ../common/timer.c
all-y += ../common/timer.c
all-y += ../common/gpio.c
all-y += ../common/clock.c
all-y += clock.c
all-y += ../common/qupv3_config.c
all-y += qcom_qup_se.c
all-y += ../common/qup_se_handler.c
all-y += ../common/qupv3_spi.c
all-y += ../common/qupv3_i2c.c

################################################################################
bootblock-y += bootblock.c
bootblock-y += mmu.c

################################################################################
romstage-y += cbmem.c

################################################################################
ramstage-y += soc.c
ramstage-y += cbmem.c

################################################################################

CPPFLAGS_common += -Isrc/soc/qualcomm/dougie/include
CPPFLAGS_common += -Isrc/soc/qualcomm/common/include

################################################################################
ifeq ($(CONFIG_QC_BLOBS_UPSTREAM),y)
ifeq ($(CONFIG_USE_QC_BLOBS),y)
DOUGIE_BLOB := $(top)/3rdparty/qc_blobs/dougie

################################################################################
UART_FW_FILE := $(DOUGIE_BLOB)/qup_fw/uart_fw.bin
UART_FW_CBFS := $(CONFIG_CBFS_PREFIX)/uart_fw
$(UART_FW_CBFS)-file := $(UART_FW_FILE)
$(UART_FW_CBFS)-type := raw
$(UART_FW_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
cbfs-files-y += $(UART_FW_CBFS)

################################################################################
SPI_FW_FILE := $(DOUGIE_BLOB)/qup_fw/spi_fw.bin
SPI_FW_CBFS := $(CONFIG_CBFS_PREFIX)/spi_fw
$(SPI_FW_CBFS)-file := $(SPI_FW_FILE)
$(SPI_FW_CBFS)-type := raw
$(SPI_FW_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
cbfs-files-y += $(SPI_FW_CBFS)

################################################################################
I2C_FW_FILE := $(DOUGIE_BLOB)/qup_fw/i2c_fw.bin
I2C_FW_CBFS := $(CONFIG_CBFS_PREFIX)/i2c_fw
$(I2C_FW_CBFS)-file := $(I2C_FW_FILE)
$(I2C_FW_CBFS)-type := raw
$(I2C_FW_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
cbfs-files-y += $(I2C_FW_CBFS)

endif # ifeq ($(CONFIG_USE_QC_BLOBS),y)
endif # ifeq ($(CONFIG_QC_BLOBS_UPSTREAM),y)

endif
