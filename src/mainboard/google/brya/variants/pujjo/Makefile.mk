# SPDX-License-Identifier: GPL-2.0-only
bootblock-y += gpio.c

romstage-y += gpio.c

ramstage-$(CONFIG_FW_CONFIG) += fw_config.c
ramstage-y += variant.c
ramstage-y += gpio.c

$(call add_vbt_to_cbfs, vbt-pujjo1e.bin, data-pujjo1e.vbt)
