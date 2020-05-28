## SPDX-License-Identifier: GPL-2.0-only

subdirs-$(CONFIG_CHROMEOS) += chromeos

ramstage-$(CONFIG_GOOGLE_SMBIOS_MAINBOARD_VERSION) += smbios.c
