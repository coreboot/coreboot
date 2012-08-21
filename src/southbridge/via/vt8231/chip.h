#ifndef _SOUTHBRIDGE_VIA_VT8231
#define _SOUTHBRIDGE_VIA_VT8231

struct southbridge_via_vt8231_config {
	/* enables of Non-PCI devices */
	int enable_native_ide;
	int enable_com_ports;
	int enable_keyboard;
	/* currently not parsed but needed by densitron dpx114 */
	int enable_usb;
	int enable_nvram;
};

#endif /* _SOUTHBRIDGE_VIA_VT8231 */
