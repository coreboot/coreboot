# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_PLATFORM_USES_FSP2_0),y)

bootblock-$(CONFIG_FSP_CAR) += fspt_report.c

romstage-y += debug.c
romstage-y += hand_off_block.c
romstage-$(CONFIG_DISPLAY_FSP_HEADER) += header_display.c
romstage-$(CONFIG_DISPLAY_HOBS) += hob_display.c
romstage-$(CONFIG_DISPLAY_UPD_DATA) += upd_display.c
romstage-$(CONFIG_VERIFY_HOBS) += hob_verify.c
romstage-y += util.c
romstage-y += memory_init.c
romstage-$(CONFIG_MMA) += mma_core.c
romstage-y += cbmem.c

ramstage-y += debug.c
ramstage-$(CONFIG_USE_INTEL_FSP_MP_INIT) += fsp_mpinit.c
ramstage-$(CONFIG_RUN_FSP_GOP) += graphics.c
ramstage-y += hand_off_block.c
ramstage-$(CONFIG_DISPLAY_FSP_HEADER) += header_display.c
ramstage-$(CONFIG_DISPLAY_HOBS) += hob_display.c
ramstage-$(CONFIG_VERIFY_HOBS) += hob_verify.c
ramstage-$(CONFIG_FSP2_0_DISPLAY_LOGO) += logo.c
ramstage-y += notify.c
ramstage-y += silicon_init.c
ramstage-$(CONFIG_DISPLAY_UPD_DATA) += upd_display.c
ramstage-y += util.c
ramstage-$(CONFIG_MMA) += mma_core.c

postcar-$(CONFIG_FSP_CAR) += temp_ram_exit.c
postcar-$(CONFIG_FSP_CAR) += util.c
postcar-$(CONFIG_DISPLAY_FSP_HEADER) += header_display.c
postcar-y += hand_off_block.c

CPPFLAGS_common += -I$(src)/drivers/intel/fsp2_0/include

FSP_T_CBFS = $(call strip_quotes,$(CONFIG_FSP_T_CBFS))
FSP_M_CBFS = $(call strip_quotes,$(CONFIG_FSP_M_CBFS))
FSP_S_CBFS = $(call strip_quotes,$(CONFIG_FSP_S_CBFS))

# Add FSP blobs into cbfs. SoC code may supply  additional options with
# -options, e.g --xip or -b
cbfs-files-$(CONFIG_FSP_CAR) += $(FSP_T_CBFS)
$(FSP_T_CBFS)-file := $(call strip_quotes,$(CONFIG_FSP_T_FILE))
$(FSP_T_CBFS)-type := fsp
ifeq ($(CONFIG_FSP_T_XIP),y)
$(FSP_T_CBFS)-options := --xip $(TXTIBB)
endif

cbfs-files-$(CONFIG_ADD_FSP_BINARIES) += $(FSP_M_CBFS)
$(FSP_M_CBFS)-file := $(call strip_quotes,$(CONFIG_FSP_M_FILE))
$(FSP_M_CBFS)-type := fsp
ifeq ($(CONFIG_FSP_M_XIP),y)
$(FSP_M_CBFS)-options := --xip $(TXTIBB)
endif
ifeq ($(CONFIG_FSP_COMPRESS_FSP_M_LZMA),y)
$(FSP_M_CBFS)-compression := LZMA
endif
ifeq ($(CONFIG_FSP_COMPRESS_FSP_M_LZ4),y)
$(FSP_M_CBFS)-compression := LZ4
endif

cbfs-files-$(CONFIG_ADD_FSP_BINARIES) += $(FSP_S_CBFS)
$(FSP_S_CBFS)-file := $(call strip_quotes,$(CONFIG_FSP_S_FILE))
$(FSP_S_CBFS)-type := fsp
ifeq ($(CONFIG_FSP_COMPRESS_FSP_S_LZMA),y)
$(FSP_S_CBFS)-compression := LZMA
endif
ifeq ($(CONFIG_FSP_COMPRESS_FSP_S_LZ4),y)
$(FSP_S_CBFS)-compression := LZ4
endif

ifeq ($(CONFIG_FSP_FULL_FD),y)
$(obj)/Fsp_M.fd: $(call strip_quotes,$(CONFIG_FSP_FD_PATH)) $(DOTCONFIG)
	python2 3rdparty/fsp/Tools/SplitFspBin.py split -f $(CONFIG_FSP_FD_PATH) -o "$(obj)" -n "Fsp.fd"

$(obj)/Fsp_S.fd: $(call strip_quotes,$(CONFIG_FSP_FD_PATH)) $(obj)/Fsp_M.fd
	true

$(obj)/Fsp_T.fd: $(call strip_quotes,$(CONFIG_FSP_FD_PATH)) $(obj)/Fsp_M.fd
	true
endif

# Add logo to the cbfs image
cbfs-files-$(CONFIG_FSP2_0_DISPLAY_LOGO) += logo.bmp
logo.bmp-file := $(call strip_quotes,$(CONFIG_FSP2_0_LOGO_FILE_NAME))
logo.bmp-type := raw
logo.bmp-compression := LZMA

ifneq ($(call strip_quotes,$(CONFIG_FSP_HEADER_PATH)),)
CPPFLAGS_common+=-I$(CONFIG_FSP_HEADER_PATH)
endif

# Include PPI directory of CONFIG_FSP_PEIM_TO_PEIM_INTERFACE is enable
subdirs-$(CONFIG_FSP_PEIM_TO_PEIM_INTERFACE) += ppi

endif
