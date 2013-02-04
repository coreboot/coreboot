# Run an intermediate step when producing coreboot.rom
# that adds additional components to the final firmware
# image outside of CBFS
#INTERMEDIATE += exynos5250_add_bl1

# Clock init is done in bootblock to support UART output for
# debugging. We may add a Kconfig option to disable clock init
# in the bootblock and try moving it entirely into romstage.
bootblock-y += clock_init.c
bootblock-y += clock.c

romstage-y += clock.c
romstage-y += clock_init.c
romstage-y += exynos_cache.c
romstage-y += pinmux.c
romstage-y += power.c
romstage-y += soc.c
romstage-y += uart.c
romstage-y += dmc_common.c
romstage-y += dmc_init_ddr3.c

#ramstage-y += clock.c
#ramstage-y += clock_init.c
#ramstage-y += power.c
#ramstage-y += uart.c
#ramstage-y += pinmux.c
##ramstage-y += tzpc_init.c
ramstage-y += clock.c
ramstage-y += clock_init.c
ramstage-y += exynos_cache.c
ramstage-y += pinmux.c
ramstage-y += power.c
ramstage-y += soc.c
ramstage-y += uart.c

#ramstage-$(CONFIG_EXYNOS_ACE_SHA) += ace_sha.c
#ramstage-$(CONFIG_SATA_AHCI) += sata.c

exynos5250_add_bl1: $(obj)/coreboot.pre
	printf "    DD         Adding Samsung Exynos5250 BL1\n"
	dd if=3rdparty/cpu/samsung/exynos5250/E5250.nbl1.bin \
		of=$(obj)/coreboot.pre conv=notrunc >/dev/null 2>&1
