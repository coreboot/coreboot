## SPDX-License-Identifier: GPL-2.0-only

bootblock-y	+= ../car/cache_as_ram.S
bootblock-y	+= ../../intel/car/bootblock.c

postcar-y	+= ../car/exit_car.S
