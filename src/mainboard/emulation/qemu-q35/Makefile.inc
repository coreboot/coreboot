ramstage-y += ../qemu-i440fx/northbridge.c
ramstage-y += ../qemu-i440fx/memory.c
ramstage-y += ../qemu-i440fx/fw_cfg.c
romstage-y += ../qemu-i440fx/memory.c
romstage-y += ../qemu-i440fx/fw_cfg.c

postcar-y += ../qemu-i440fx/memory.c
postcar-y += ../qemu-i440fx/exit_car.S
postcar-y += ../qemu-i440fx/fw_cfg.c

bootblock-y += bootblock.c
