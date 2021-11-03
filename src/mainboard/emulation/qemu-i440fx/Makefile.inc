romstage-y += memmap.c

postcar-y += memmap.c
postcar-y += exit_car.S

ramstage-y += memmap.c
ramstage-y += northbridge.c

all-y += fw_cfg.c
all-y += bootmode.c
