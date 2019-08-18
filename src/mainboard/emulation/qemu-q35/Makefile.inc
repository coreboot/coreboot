bootblock-y += bootblock.c

romstage-y += ../qemu-i440fx/fw_cfg.c
romstage-y += ../qemu-i440fx/memmap.c

postcar-y += ../qemu-i440fx/fw_cfg.c
postcar-y += ../qemu-i440fx/memmap.c
postcar-y += ../qemu-i440fx/exit_car.S

ramstage-y += ../qemu-i440fx/fw_cfg.c
ramstage-y += ../qemu-i440fx/memmap.c
ramstage-y += ../qemu-i440fx/northbridge.c
