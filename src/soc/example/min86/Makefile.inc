## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_EXAMPLE_MIN86),y)

bootblock-y	+= cache_as_ram.S
bootblock-y	+= ../../../cpu/intel/car/bootblock.c

postcar-y	+= exit_car.S

romstage-y	+= romstage.c

ramstage-y	+= chip.c
ramstage-y	+= timer.c

endif
