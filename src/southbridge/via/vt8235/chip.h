#ifndef _SOUTHBRIDGE_VIA_VT8235
#define _SOUTHBRIDGE_VIA_VT8235

struct southbridge_via_vt8235_config {
	/* PCI function enables */
	/* i.e. so that pci scan bus will find them. */
	/* I am putting in IDE as an example but obviously this needs
	 * to be more complete!
	 */
	int enable_ide;
	/* enables of functions of devices */
	int enable_usb;
	int enable_native_ide;
	int enable_com_ports;
	int enable_keyboard;
	int enable_nvram;
};

#endif /* _SOUTHBRIDGE_VIA_VT8235 */
