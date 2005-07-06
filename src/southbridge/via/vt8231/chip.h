#ifndef _SOUTHBRIDGE_VIA_VT8231
#define _SOUTHBRIDGE_VIA_VT8231

extern struct chip_operations southbridge_via_vt8231_ops;

struct southbridge_via_vt8231_config {
	/* enables of Non-PCI devices */
	int enable_native_ide;
	int enable_com_ports;
	int enable_keyboard;
};

#endif /* _SOUTHBRIDGE_VIA_VT8231 */
