# SPDX-License-Identifier: GPL-2.0-or-later

ifeq ($(CONFIG_BOOTBLOCK_CONSOLE),y)
bootblock-$(CONFIG_SUPERIO_FINTEK_F81803A) += ../common/early_serial.c
endif

## Notice: For fan control at romstage, HWM must be initialized before
## the API is called. Ramstage can use devicetree to initialize it.

romstage-$(CONFIG_SUPERIO_FINTEK_F81803A) += ../common/early_serial.c
romstage-$(CONFIG_SUPERIO_FINTEK_FAN_CONTROL) += fan_control.c
romstage-$(CONFIG_SUPERIO_FINTEK_FAN_API_CALL) += ../common/fan_api_call.c

ramstage-$(CONFIG_SUPERIO_FINTEK_F81803A) += superio.c
ramstage-$(CONFIG_SUPERIO_FINTEK_FAN_CONTROL) += fan_control.c
ramstage-$(CONFIG_SUPERIO_FINTEK_FAN_API_CALL) += ../common/fan_api_call.c
