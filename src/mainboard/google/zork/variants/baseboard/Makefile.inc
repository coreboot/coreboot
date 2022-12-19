# SPDX-License-Identifier: GPL-2.0-or-later

all-y += helpers.c

verstage-y += tpm_tis.c
romstage-y += tpm_tis.c
ramstage-y += tpm_tis.c
ramstage-y += ramstage_common.c

# Add OEM ID table
ifeq ($(CONFIG_USE_OEM_BIN),y)
cbfs-files-y += oem.bin
oem.bin-file := $(call strip_quotes,$(CONFIG_OEM_BIN_FILE))
oem.bin-type := raw
endif #($(CONFIG_USE_OEM_BIN),y)
