#ifndef _SUPERIO_VIA_VT1211
#define _SUPERIO_VIA_VT1211

#include <uart8250.h>

extern struct chip_operations superio_via_vt1211_ops;

struct superio_via_vt1211_config {
	struct uart8250 com1, com2;
};

#endif /* _SUPERIO_VIA_VT1211 */
