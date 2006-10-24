#ifndef _SUPERIO_NSC_PC97307
#define _SUPERIO_NSC_PC97307

#ifndef PNP_INDEX_REG
#define PNP_INDEX_REG   0x15C
#endif
#ifndef PNP_DATA_REG
#define PNP_DATA_REG    0x15D
#endif

extern struct chip_operations superio_nsc_pc97307_ops;

#include <pc80/keyboard.h>
#include <uart8250.h>

struct superio_nsc_pc97307_config {
	struct uart8250 com1, com2;
	struct pc_keyboard keyboard;
};
#endif /* _SUPERIO_NSC_PC97307 */
