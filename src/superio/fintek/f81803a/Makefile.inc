##
## This file is part of the coreboot project.
##
## Copyright (C) 2011 Advanced Micro Devices, Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

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
