## SPDX-License-Identifier: GPL-2.0-only

romstage-$(CONFIG_CONSOLE_NE2K) += ne2k.c
ramstage-$(CONFIG_CONSOLE_NE2K) += ne2k.c
ramstage-$(CONFIG_REALTEK_8168_RESET) += r8168.c
ramstage-$(CONFIG_ATHEROS_ATL1E_SETMAC) += atl1e.c

ifneq ($(CONFIG_REALTEK_8168_MACADDRESS),"")
$(obj)/rt8168-macaddress: $(DOTCONFIG)
		echo "    Creating a file holding the rt8168 macaddress"
		printf %s $(CONFIG_REALTEK_8168_MACADDRESS) > $@

cbfs-files-$(CONFIG_REALTEK_8168_RESET) += rt8168-macaddress
rt8168-macaddress-file := $(obj)/rt8168-macaddress
rt8168-macaddress-type := raw
endif

ifneq ($(CONFIG_ATHEROS_ATL1E_MACADDRESS),"")
$(obj)/atl1e-macaddress: $(DOTCONFIG)
	echo "    Creating a file holding the atl1e macaddress"
	printf %s $(CONFIG_ATHEROS_ATL1E_MACADDRESS) > $@

cbfs-files-$(CONFIG_ATHEROS_ATL1E_SETMAC) += atl1e-macaddress
atl1e-macaddress-file := $(obj)/atl1e-macaddress
atl1e-macaddress-type := raw
endif
