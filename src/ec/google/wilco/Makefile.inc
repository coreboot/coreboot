ifeq ($(CONFIG_EC_GOOGLE_WILCO),y)

bootblock-y += bootblock.c
ramstage-y += chip.c commands.c mailbox.c
smm-y += commands.c mailbox.c smihandler.c

endif
