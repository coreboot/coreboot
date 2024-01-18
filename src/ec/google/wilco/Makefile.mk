## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_EC_GOOGLE_WILCO),y)

bootblock-y += bootblock.c commands.c mailbox.c
verstage-y += commands.c mailbox.c
romstage-y += commands.c mailbox.c romstage.c boardid.c
ramstage-y += chip.c commands.c mailbox.c boardid.c
smm-y += commands.c mailbox.c smihandler.c boardid.c

endif
