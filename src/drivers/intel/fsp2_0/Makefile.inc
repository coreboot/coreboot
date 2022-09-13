# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_PLATFORM_USES_FSP2_0),y)

bootblock-$(CONFIG_FSP_CAR) += fspt_report.c

romstage-y += debug.c
romstage-$(CONFIG_FSP_USES_CB_DEBUG_EVENT_HANDLER) += fsp_debug_event.c
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
ramstage-$(CONFIG_FSP_USES_CB_DEBUG_EVENT_HANDLER) += fsp_debug_event.c
ramstage-$(CONFIG_USE_INTEL_FSP_MP_INIT) += fsp_mpinit.c
ramstage-$(CONFIG_DISPLAY_FSP_TIMESTAMPS) += fsp_timestamp.c
ramstage-$(CONFIG_RUN_FSP_GOP) += graphics.c
ramstage-y += hand_off_block.c
ramstage-$(CONFIG_DISPLAY_FSP_HEADER) += header_display.c
ramstage-$(CONFIG_DISPLAY_HOBS) += hob_display.c
ramstage-$(CONFIG_VERIFY_HOBS) += hob_verify.c
ramstage-y += notify.c
ramstage-y += silicon_init.c
ramstage-$(CONFIG_DISPLAY_UPD_DATA) += upd_display.c
ramstage-y += util.c
ramstage-$(CONFIG_CACHE_MRC_SETTINGS) += save_mrc_data.c
ramstage-$(CONFIG_MMA) += mma_core.c

ifneq ($(CONFIG_NO_FSP_TEMP_RAM_EXIT),y)
postcar-$(CONFIG_FSP_CAR) += temp_ram_exit.c
endif
postcar-$(CONFIG_FSP_CAR) += util.c
postcar-$(CONFIG_DISPLAY_FSP_HEADER) += header_display.c
postcar-y += hand_off_block.c

CPPFLAGS_common += -I$(src)/drivers/intel/fsp2_0/include

FSP_T_CBFS = $(call strip_quotes,$(CONFIG_FSP_T_CBFS))
FSP_M_CBFS = $(call strip_quotes,$(CONFIG_FSP_M_CBFS))
FSP_S_CBFS = $(call strip_quotes,$(CONFIG_FSP_S_CBFS))

# Add FSP blobs into cbfs. SoC code may supply  additional options with
# -options, e.g --xip or -b
ifeq ($(CONFIG_ADD_FSP_BINARIES)$(CONFIG_FSP_CAR),yy)
cbfs-files-y += $(FSP_T_CBFS)
$(FSP_T_CBFS)-file := $(call strip_quotes,$(CONFIG_FSP_T_FILE))
$(FSP_T_CBFS)-type := fsp
ifeq ($(CONFIG_FSP_T_XIP),y)
$(FSP_T_CBFS)-options := --xip $(TXTIBB)
$(FSP_T_CBFS)-position = $(CONFIG_FSP_T_LOCATION)
endif # CONFIG_FSP_T_XIP
endif # CONFIG_ADD_FSP_BINARIES && CONFIG_FSP_CAR

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
ifneq ($(CONFIG_FSP_ALIGNMENT_FSP_M),)
$(FSP_M_CBFS)-align := $(CONFIG_FSP_ALIGNMENT_FSP_M)
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
ifneq ($(CONFIG_FSP_ALIGNMENT_FSP_S),)
$(FSP_S_CBFS)-align := $(CONFIG_FSP_ALIGNMENT_FSP_S)
endif

ifeq ($(CONFIG_FSP_FULL_FD),y)
$(obj)/Fsp_M.fd: $(call strip_quotes,$(CONFIG_FSP_FD_PATH)) $(DOTCONFIG)
	python 3rdparty/fsp/Tools/SplitFspBin.py split -f $(CONFIG_FSP_FD_PATH) -o "$(obj)" -n "Fsp.fd"

$(obj)/Fsp_S.fd: $(call strip_quotes,$(CONFIG_FSP_FD_PATH)) $(obj)/Fsp_M.fd
	true

$(obj)/Fsp_T.fd: $(call strip_quotes,$(CONFIG_FSP_FD_PATH)) $(obj)/Fsp_M.fd
	true
endif

# Add logo to the cbfs image
cbfs-files-$(CONFIG_BMP_LOGO) += logo.bmp
logo.bmp-file := $(call strip_quotes,$(CONFIG_FSP2_0_LOGO_FILE_NAME))
logo.bmp-type := raw
logo.bmp-compression := LZMA

ifneq ($(call strip_quotes,$(CONFIG_FSP_HEADER_PATH)),)
CPPFLAGS_common+=-I$(CONFIG_FSP_HEADER_PATH)
endif

# check if the FSP files that are supposed to be added are specified
ifeq ($(CONFIG_ADD_FSP_BINARIES),y)
ifeq ($(CONFIG_FSP_CAR),y)
ifeq ($(call strip_quotes,$(CONFIG_FSP_T_FILE)),)
$(error No FSP-T binary file specified.)
endif # CONFIG_FSP_T_FILE
endif # CONFIG_FSP_CAR
ifeq ($(call strip_quotes,$(CONFIG_FSP_M_FILE)),)
$(error No FSP-M binary file specified.)
endif # CONFIG_FSP_M_FILE
ifeq ($(call strip_quotes,$(CONFIG_FSP_S_FILE)),)
$(error No FSP-S binary file specified.)
endif # CONFIG_FSP_S_FILE
else # CONFIG_ADD_FSP_BINARIES
build_complete:: warn_no_fsp_binaries
endif # CONFIG_ADD_FSP_BINARIES

PHONY+=warn_no_fsp_binaries
warn_no_fsp_binaries:
	printf "\n\t** WARNING **\n"
	printf "ADD_FSP_BINARIES isn't selected even though this SoC relies on the FSP.\n"
	printf "The resulting image won't contain the FSP binaries and will not boot unless\n"
	printf "they are added later.\n"

subdirs-y += ppi

endif
