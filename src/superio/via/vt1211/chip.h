#ifndef _SUPERIO_VIA_VT1211
#define _SUPERIO_VIA_VT1211

extern struct chip_operations superio_via_vt1211_control;

struct superio_via_vt1211_config {
	/* PCI function enables */
	/* i.e. so that pci scan bus will find them. */
	/* I am putting in IDE as an example but obviously this needs
	 * to be more complete!
	 */
	/* enables of functions of devices */
	int enable_com_ports;
	int enable_fdc;
	int enable_lpt;
	int enable_hwmon;
};

#endif /* _SUPERIO_VIA_VT1211 */
