ifeq ($(CONFIG_EC_GOOGLE_WILCO),y)

ramstage-y += chip.c commands.c mailbox.c

endif
