ifeq ($(CONFIG_SOC_QUALCOMM_SC7280),y)

all-y += ../common/timer.c
all-y += spi.c

################################################################################
bootblock-y += bootblock.c
bootblock-y += mmu.c

################################################################################
romstage-y += cbmem.c
romstage-y += ../common/qclib.c
romstage-y += ../common/mmu.c
romstage-y += mmu.c

################################################################################
ramstage-y += soc.c
ramstage-y += cbmem.c

################################################################################

CPPFLAGS_common += -Isrc/soc/qualcomm/sc7280/include
CPPFLAGS_common += -Isrc/soc/qualcomm/common/include

SC7280_BLOB := $(top)/3rdparty/qc_blobs/sc7180

################################################################################

QC_SEC_FILE := $(SC7280_BLOB)/qc_sec/qc_sec.mbn
$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.elf
	@util/qualcomm/createxbl.py --mbn_version 6 -f $(objcbfs)/bootblock.raw.elf \
		-x $(QC_SEC_FILE) -o $(objcbfs)/merged_bb_qcsec.mbn \
		-a 64 -d 64 -c 64
	@printf "\nqgpt.py 4K sector size\n"
	@util/qualcomm/qgpt.py $(objcbfs)/merged_bb_qcsec.mbn \
		$(objcbfs)/bootblock.bin

endif
