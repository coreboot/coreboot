# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/OemCustomize.c
bootblock-y += gpio.c

romstage-y += BiosCallOuts.c
romstage-y += bootblock/OemCustomize.c
romstage-y += OemCustomize.c

ramstage-y += BiosCallOuts.c
ramstage-y += gpio.c
ramstage-y += OemCustomize.c
ramstage-$(CONFIG_SUPERIO_FINTEK_FAN_API_CALL) += fan_init.c
