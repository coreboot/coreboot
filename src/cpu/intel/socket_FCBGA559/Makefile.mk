## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += ../model_106cx
subdirs-y += ../microcode
subdirs-y += ../speedstep

bootblock-y += ../car/bootblock.c
bootblock-y += ../car/non-evict/cache_as_ram.S

postcar-y += ../car/non-evict/exit_car.S

romstage-y += ../car/romstage.c
