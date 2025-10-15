## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_BOARD_STARLABS_STARFIGHTER_RPL),y)
SPD_SOURCES = 16gb # 13
SPD_SOURCES += 32gb # 0
SPD_SOURCES += 64gb # 8
else
SPD_SOURCES = 32gb-mtl # 13
SPD_SOURCES += 64gb-mtl # 0
endif
