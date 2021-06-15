# SPDX-License-Identifier: GPL-2.0-only

romstage-y += raminit.c

# We don't ship that, but booting without it is bound to fail
cbfs-files-$(CONFIG_HAVE_MRC) += mrc.bin
mrc.bin-file := $(call strip_quotes,$(CONFIG_MRC_FILE))
mrc.bin-position := 0xfffa0000
mrc.bin-type := mrc
