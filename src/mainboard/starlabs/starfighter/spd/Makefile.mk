## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_BOARD_STARLABS_STARFIGHTER_RPL),y)
# RPL memory configs (selected by straps) with runtime speed selection.
# CBFS index order is (per config): 5500, 6400, 7500.
SPD_SOURCES = 16gb-5500
SPD_SOURCES += 16gb-6400
SPD_SOURCES += 16gb-7500
SPD_SOURCES += 32gb-5500
SPD_SOURCES += 32gb-6400
SPD_SOURCES += 32gb-7500
SPD_SOURCES += 64gb-5500
SPD_SOURCES += 64gb-6400
SPD_SOURCES += 64gb-7500
else
# MTL memory configs (selected by straps) with runtime speed selection.
# CBFS index order is (per config): 5500, 6400, 7500.
SPD_SOURCES = 32gb-mtl-5500
SPD_SOURCES += 32gb-mtl-6400
SPD_SOURCES += 32gb-mtl-7500
SPD_SOURCES += 64gb-mtl-5500
SPD_SOURCES += 64gb-mtl-6400
SPD_SOURCES += 64gb-mtl-7500
endif
