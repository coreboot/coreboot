bootblock-y += mode_switch.S
ifneq ($(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK),y)
verstage-y += mode_switch.S
endif
romstage-y += mode_switch.S
postcar-y += mode_switch.S
ramstage-y += mode_switch.S
