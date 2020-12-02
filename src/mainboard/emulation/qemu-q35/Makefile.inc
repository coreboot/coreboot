bootblock-y += bootblock.c

romstage-y += ../qemu-i440fx/fw_cfg.c
romstage-y += ../qemu-i440fx/memmap.c
romstage-y += memmap.c

postcar-y += ../qemu-i440fx/fw_cfg.c
postcar-y += ../qemu-i440fx/memmap.c
postcar-y += ../qemu-i440fx/exit_car.S
postcar-y += memmap.c

ramstage-y += ../qemu-i440fx/fw_cfg.c
ramstage-y += ../qemu-i440fx/memmap.c
ramstage-y += ../qemu-i440fx/northbridge.c
ramstage-y += memmap.c

verstage-$(CONFIG_CHROMEOS) += chromeos.c
verstage-$(CONFIG_CHROMEOS) += ../qemu-i440fx/fw_cfg.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += smi.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += memmap.c
