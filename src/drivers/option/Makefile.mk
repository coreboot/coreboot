# SPDX-License-Identifier: GPL-2.0-only

ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c

all-$(CONFIG_USE_CBFS_FILE_OPTION_BACKEND) += cbfs_file_option.c
