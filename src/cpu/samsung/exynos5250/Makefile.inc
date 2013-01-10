# Run an intermediate step when producing coreboot.rom
# that adds additional components to the final firmware
# image outside of CBFS
INTERMEDIATE += exynos5250_add_bl1

romstage-y += clock.c
romstage-y += clock_init.c
romstage-y += exynos_cache.c
romstage-y += lowlevel_init.S
romstage-y += lowlevel_init_c.c
romstage-y += pinmux.c
romstage-y += power.c
romstage-y += soc.c
romstage-y += uart.c

#ramstage-y += clock.c
#ramstage-y += clock_init.c
#ramstage-y += power.c
#ramstage-y += uart.c
#ramstage-y += pinmux.c
##ramstage-y += tzpc_init.c
ramstage-y += clock.c
ramstage-y += clock_init.c
ramstage-y += exynos_cache.c
ramstage-y += lowlevel_init_c.c
ramstage-y += pinmux.c
ramstage-y += power.c
ramstage-y += soc.c
ramstage-y += uart.c

#ramstage-$(CONFIG_EXYNOS_ACE_SHA) += ace_sha.c
#ramstage-$(CONFIG_SATA_AHCI) += sata.c
ramstage-$(CONFIG_SPL_BUILD) += lowlevel_init_c.c
ramstage-$(CONFIG_SPL_BUILD) += dmc_common.c
ramstage-$(CONFIG_SPL_BUILD) += dmc_init_ddr3.c

exynos5250_add_bl1: $(obj)/coreboot.pre
	printf "    DD         Adding Samsung Exynos5250 BL1\n"
	dd if=3rdparty/cpu/samsung/exynos5250/E5250.nbl1.bin \
		of=$(obj)/coreboot.pre conv=notrunc >/dev/null 2>&1
