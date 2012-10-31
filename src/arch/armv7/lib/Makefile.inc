#romstage-y += start.S

ramstage-y += _ashldi3.S
ramstage-y += _ashrdi3.S
ramstage-y += _divsi3.S
ramstage-y += _lshrdi3.S
ramstage-y += _modsi3.S
ramstage-y += _udivsi3.S
ramstage-y += _umodsi3.S

ramstage-y += c_start.S
ramstage-y += coreboot_table.c
ramstage-y += div.c
ramstage-y += div0.c
ramstage-y += div64.S
ramstage-y += hang_spl.c
ramstage-y += interrupts.c
ramstage-y += memcpy.S
ramstage-y += memset.S
ramstage-y += printk_init.c
ramstage-y += reset.c
ramstage-y += tables.c

ramstage-$(CONFIG_CHROMEOS) += _uldivmod.S

romstage-$(CONFIG_EABI_COMPAT) += eabi_compat.c
ramstage-$(CONFIG_EABI_COMPAT) += eabi_compat.c

# FIXME: is cache_v7 used by the integrated cache controller on the A15?
# romstage-y += cache_v7.c
