## SPDX-License-Identifier: GPL-2.0-only

# Add a handler for BCT config files
$(call add-special-class,bct-cfg)
bct-cfg-handler= $(eval $(obj)/generated/bct.cfg: $(1)$(2))

$(obj)/generated/bct.cfg:
	@printf "    CAT        $(subst $(obj)/,,$(@))\n"
	cat $^ > $@

subdirs-y += bct

bootblock-y += bootblock.c
bootblock-y += pmic.c
bootblock-y += reset.c

verstage-y += chromeos.c
verstage-y += reset.c

romstage-y += reset.c
romstage-y += romstage.c
romstage-y += chromeos.c
romstage-y += sdram_configs.c

ramstage-y += boardid.c
ramstage-y += mainboard.c
ramstage-y += reset.c
ramstage-y += chromeos.c
ramstage-y += sdram_configs.c
