## SPDX-License-Identifier: GPL-2.0-only

romstage-y += chromeos.c
ramstage-y += chromeos.c
verstage-$(CONFIG_VBOOT_SEPARATE_VERSTAGE) += chromeos.c

smm-y += mainboard_smi.c
