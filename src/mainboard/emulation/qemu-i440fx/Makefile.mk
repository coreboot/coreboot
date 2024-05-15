## SPDX-License-Identifier: GPL-2.0-only

romstage-y += memmap.c

postcar-y += memmap.c
postcar-y += exit_car.S

ramstage-y += memmap.c
ramstage-y += northbridge.c
ramstage-y += rom_media.c

all-y += fw_cfg.c
all-y += bootmode.c
